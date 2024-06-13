// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * msg secure m3 files
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm_knl_api.h"
#include "sysctrl_csr_addr_define.h"
#include "sysctrl.h"
#include "msg_scm3_user_api.h"
#include "msg_scm3.h"

/*
 * Queue view:
 * -------------
 * pi/ci/init_flag
 * -------------    MSG_Q_BASE + 3 * MSG_Q_SIZE
 * MSG_Q_SERVICE_M3      |
 * -------------    MSG_Q_BASE + 2 * MSG_Q_SIZE
 * MSG_Q_SECURE_M3       |
 * -------------    MSG_Q_BASE + 1 * MSG_Q_SIZE
 * MSG_Q_A55             |
 * -------------    MSG_Q_BASE + 0 * MSG_Q_SIZE
 */
#define MSG_Q_BASE_ADDR         0x87040000
#define MSG_Q_HEAD_RSV_SIZE     8192 /* reserve 8K for head info */
#define MSG_Q_DATA_ADDR         (MSG_Q_BASE_ADDR + MSG_Q_HEAD_RSV_SIZE)
#define MSG_Q_DATA_SIZE         (MSG_Q_DEPTH * MSG_DATA_MAX_SIZE)
#define msg_data_address(index) (MSG_Q_DATA_ADDR + (index) * MSG_DATA_MAX_SIZE)

#define MSG_Q_PI_SIZE        4
#define MSG_Q_CI_SIZE        4
#define MSG_Q_INIT_FLAG_SIZE 4
#define MSG_Q_CTL_SIZE       (MSG_Q_PI_SIZE + MSG_Q_CI_SIZE + MSG_Q_INIT_FLAG_SIZE)

#define MSG_Q_INIT_MAGIC     0x171F
#define MSG_SEND_RETRY_TIMES 100
#define MSG_RESP_CMD_MASK    0x80
#define MSG_WAIT_TIME_OUT    60000 /* Time out:60s */

#define msg_counter(format, args...) KBOX_MSG(KBOX_COUNTER, format, ##args)

typedef struct {
    void __iomem *queue_base;
    u32 queue_depth;
    void __iomem *pi_base;
    void __iomem *ci_base;
    void __iomem *init_flag_base;
    spinlock_t send_lock;
} msg_queue_info_s;

typedef struct {
    struct workqueue_struct *msg_wq;
    struct delayed_work msg_delayed_work;
} msg_work_queue_s;

typedef struct {
    cdev_st *msg_dev;
    s8 *msg_dev_name;
} msg_device_info_s;

typedef struct {
    msg_queue_info_s msg_queue_info[MSG_Q_TYPE_BUTT];
    u8 msg_buf_status[MSG_Q_DEPTH];                   /* msg buffer control queue index */
    spinlock_t msg_get_buf_lock;
    u32 msg_intercore_virq;                           /* secure M3 inter core communication interrupt */
    struct completion msg_completion[MSG_Q_DEPTH];    /* completion variable for messages in ring queue */
    volatile msg_comm_data_s *msg_user_addr[MSG_Q_DEPTH];
} msg_queue_ctrl_s;

static msg_queue_ctrl_s g_msg_que_ctrl = {
    .msg_intercore_virq = 0,
};

static msg_device_info_s g_msg_dev_info = {
    .msg_dev = NULL,
    .msg_dev_name = SEC_MODULE_DEVNAME,
};

static msg_work_queue_s g_msg_wq = {
    .msg_wq = NULL,
};

/* Table of CRC constants - implements x^16+x^12+x^5+1 */
static const u8 g_crc_table_lo[256] = { /* 256 for array size */
    0x0,  0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x81, 0x91, 0xA1, 0xB1, 0xC1, 0xD1, 0xE1, 0xF1,
    0x12,  0x2, 0x32, 0x22, 0x52, 0x42, 0x72, 0x62, 0x93, 0x83, 0xB3, 0xA3, 0xD3, 0xC3, 0xF3, 0xE3,
    0x24, 0x34,  0x4, 0x14, 0x64, 0x74, 0x44, 0x54, 0xA5, 0xB5, 0x85, 0x95, 0xE5, 0xF5, 0xC5, 0xD5,
    0x36, 0x26, 0x16,  0x6, 0x76, 0x66, 0x56, 0x46, 0xB7, 0xA7, 0x97, 0x87, 0xF7, 0xE7, 0xD7, 0xC7,
    0x48, 0x58, 0x68, 0x78,  0x8, 0x18, 0x28, 0x38, 0xC9, 0xD9, 0xE9, 0xF9, 0x89, 0x99, 0xA9, 0xB9,
    0x5A, 0x4A, 0x7A, 0x6A, 0x1A,  0xA, 0x3A, 0x2A, 0xDB, 0xCB, 0xFB, 0xEB, 0x9B, 0x8B, 0xBB, 0xAB,
    0x6C, 0x7C, 0x4C, 0x5C, 0x2C, 0x3C,  0xC, 0x1C, 0xED, 0xFD, 0xCD, 0xDD, 0xAD, 0xBD, 0x8D, 0x9D,
    0x7E, 0x6E, 0x5E, 0x4E, 0x3E, 0x2E, 0x1E,  0xE, 0xFF, 0xEF, 0xDF, 0xCF, 0xBF, 0xAF, 0x9F, 0x8F,
    0x91, 0x81, 0xB1, 0xA1, 0xD1, 0xC1, 0xF1, 0xE1, 0x10,  0x0, 0x30, 0x20, 0x50, 0x40, 0x70, 0x60,
    0x83, 0x93, 0xA3, 0xB3, 0xC3, 0xD3, 0xE3, 0xF3,  0x2, 0x12, 0x22, 0x32, 0x42, 0x52, 0x62, 0x72,
    0xB5, 0xA5, 0x95, 0x85, 0xF5, 0xE5, 0xD5, 0xC5, 0x34, 0x24, 0x14,  0x4, 0x74, 0x64, 0x54, 0x44,
    0xA7, 0xB7, 0x87, 0x97, 0xE7, 0xF7, 0xC7, 0xD7, 0x26, 0x36,  0x6, 0x16, 0x66, 0x76, 0x46, 0x56,
    0xD9, 0xC9, 0xF9, 0xE9, 0x99, 0x89, 0xB9, 0xA9, 0x58, 0x48, 0x78, 0x68, 0x18,  0x8, 0x38, 0x28,
    0xCB, 0xDB, 0xEB, 0xFB, 0x8B, 0x9B, 0xAB, 0xBB, 0x4A, 0x5A, 0x6A, 0x7A,  0xA, 0x1A, 0x2A, 0x3A,
    0xFD, 0xED, 0xDD, 0xCD, 0xBD, 0xAD, 0x9D, 0x8D, 0x7C, 0x6C, 0x5C, 0x4C, 0x3C, 0x2C, 0x1C,  0xC,
    0xEF, 0xFF, 0xCF, 0xDF, 0xAF, 0xBF, 0x8F, 0x9F, 0x6E, 0x7E, 0x4E, 0x5E, 0x2E, 0x3E,  0xE, 0x1E
};

static const u8 g_crc_table_hi[256] = { /* 256 for array size */
    0x0,  0x21, 0x42, 0x63, 0x84, 0xA5, 0xC6, 0xE7,  0x8, 0x29, 0x4A, 0x6B, 0x8C, 0xAD, 0xCE, 0xEF,
    0x31, 0x10, 0x73, 0x52, 0xB5, 0x94, 0xF7, 0xD6, 0x39, 0x18, 0x7B, 0x5A, 0xBD, 0x9C, 0xFF, 0xDE,
    0x62, 0x43, 0x20,  0x1, 0xE6, 0xC7, 0xA4, 0x85, 0x6A, 0x4B, 0x28,  0x9, 0xEE, 0xCF, 0xAC, 0x8D,
    0x53, 0x72, 0x11, 0x30, 0xD7, 0xF6, 0x95, 0xB4, 0x5B, 0x7A, 0x19, 0x38, 0xDF, 0xFE, 0x9D, 0xBC,
    0xC4, 0xE5, 0x86, 0xA7, 0x40, 0x61,  0x2, 0x23, 0xCC, 0xED, 0x8E, 0xAF, 0x48, 0x69,  0xA, 0x2B,
    0xF5, 0xD4, 0xB7, 0x96, 0x71, 0x50, 0x33, 0x12, 0xFD, 0xDC, 0xBF, 0x9E, 0x79, 0x58, 0x3B, 0x1A,
    0xA6, 0x87, 0xE4, 0xC5, 0x22,  0x3, 0x60, 0x41, 0xAE, 0x8F, 0xEC, 0xCD, 0x2A,  0xB, 0x68, 0x49,
    0x97, 0xB6, 0xD5, 0xF4, 0x13, 0x32, 0x51, 0x70, 0x9F, 0xBE, 0xDD, 0xFC, 0x1B, 0x3A, 0x59, 0x78,
    0x88, 0xA9, 0xCA, 0xEB,  0xC, 0x2D, 0x4E, 0x6F, 0x80, 0xA1, 0xC2, 0xE3,  0x4, 0x25, 0x46, 0x67,
    0xB9, 0x98, 0xFB, 0xDA, 0x3D, 0x1C, 0x7F, 0x5E, 0xB1, 0x90, 0xF3, 0xD2, 0x35, 0x14, 0x77, 0x56,
    0xEA, 0xCB, 0xA8, 0x89, 0x6E, 0x4F, 0x2C,  0xD, 0xE2, 0xC3, 0xA0, 0x81, 0x66, 0x47, 0x24,  0x5,
    0xDB, 0xFA, 0x99, 0xB8, 0x5F, 0x7E, 0x1D, 0x3C, 0xD3, 0xF2, 0x91, 0xB0, 0x57, 0x76, 0x15, 0x34,
    0x4C, 0x6D,  0xE, 0x2F, 0xC8, 0xE9, 0x8A, 0xAB, 0x44, 0x65,  0x6, 0x27, 0xC0, 0xE1, 0x82, 0xA3,
    0x7D, 0x5C, 0x3F, 0x1E, 0xF9, 0xD8, 0xBB, 0x9A, 0x75, 0x54, 0x37, 0x16, 0xF1, 0xD0, 0xB3, 0x92,
    0x2E,  0xF, 0x6C, 0x4D, 0xAA, 0x8B, 0xE8, 0xC9, 0x26,  0x7, 0x64, 0x45, 0xA2, 0x83, 0xE0, 0xC1,
    0x1F, 0x3E, 0x5D, 0x7C, 0x9B, 0xBA, 0xD9, 0xF8, 0x17, 0x36, 0x55, 0x74, 0x93, 0xB2, 0xD1, 0xF0
};

void dal_cal_crc16(u8 *p_data, u16 length, u16 *crc)
{
    u8 uc_crc_hi = 0xFF; /* Initialization of CRC high byte */
    u8 uc_crc_lo = 0xFF; /* Initialization of CRC low byte */
    u8 uc_index;        /* Pointer to the CRC lookup table */

    if ((p_data == NULL) || (crc == NULL) || (length == 0)) {
        LOG(LOG_ERROR, "CRC-16: parameter error.");
        return;
    }

    while (length--) {
        uc_index = (u8)(uc_crc_hi ^ (*p_data++));
        /* Calculate CRC */
        uc_crc_hi = (u8)(uc_crc_lo ^ g_crc_table_lo[uc_index]);
        uc_crc_lo = g_crc_table_hi[uc_index];
    }
    *crc = (u16)(uc_crc_lo | ((u16)uc_crc_hi << 8)); /* uc_crc_hi take high 8 bits */
    return;
}

static irqreturn_t msg_interrupt_handle(int irq, void *arg)
{
    (void)queue_delayed_work(g_msg_wq.msg_wq, &g_msg_wq.msg_delayed_work, 0);

    /* clear secure M3 inter core interrupt */
    sysctrl_reg_write(SYSCTRL_CSR_SC_ASSERT_SOFT_INT_OFFSET_ADDR, 0);
    return IRQ_HANDLED;
}

static void msg_delaywork_handle(struct work_struct *work)
{
    u32 pi;
    u32 ci;
    u16 cksum;
    msg_comm_data_s *msg_buf = NULL;
    msg_queue_info_s *msg_q_info = NULL;

    msg_q_info = &(g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_A55]);

    rmb();
    pi = ioread32(msg_q_info->pi_base);
    ci = ioread32(msg_q_info->ci_base);
    while (pi != ci) {
        msg_buf = (msg_comm_data_s *)(((u64)(msg_q_info->queue_base)) + (ci * MSG_CMD_LEN));

        /* crc checksum */
        dal_cal_crc16((u8 *)msg_buf, MSG_CMD_LEN - MSG_CMD_CRC_LEN, &cksum);
        if (cksum != msg_buf->msg_crc) {
            LOG(LOG_ERROR, "checksum error, cal:0x%x, origin:0x%x", cksum, msg_buf->msg_crc);
            break;
        }

        g_msg_que_ctrl.msg_user_addr[msg_buf->msg_index] = msg_buf;

        /* wake up the waiting process */
        complete(&g_msg_que_ctrl.msg_completion[msg_buf->msg_index]);

        /* Update ci */
        ci = (ci + 1) % (msg_q_info->queue_depth);
        iowrite32(ci, msg_q_info->ci_base);
        wmb();

        pi = ioread32(msg_q_info->pi_base);
        ci = ioread32(msg_q_info->ci_base);
    }

    return;
}

static void msg_delaywork_init(void)
{
    g_msg_wq.msg_wq = create_singlethread_workqueue("msg_queue");
    INIT_DELAYED_WORK(&g_msg_wq.msg_delayed_work, msg_delaywork_handle);

    return;
}

s32 msg_cmd_send(u32 queue_type, const void *msg_buf)
{
    u32 pi;
    u32 ci;
    u32 init_magic;
    msg_comm_data_s *msg_addr = NULL;
    u16 cksum;
    unsigned long flag = 0;
    msg_queue_info_s *msg_q_info = NULL;
    s32 ret;

    if ((msg_buf == NULL) || (queue_type >= MSG_Q_TYPE_BUTT)) {
        LOG(LOG_ERROR, "msg_cmd_send parm: queue_type(0x%x) is inval!", queue_type);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_msg_que_ctrl.msg_queue_info[queue_type].send_lock, flag);

    msg_q_info = &(g_msg_que_ctrl.msg_queue_info[queue_type]);

    rmb();

    init_magic = ioread32(msg_q_info->init_flag_base);
    if (init_magic != MSG_Q_INIT_MAGIC) {
        LOG(LOG_ERROR, "msg_cmd_send queue_type(0x%x)'s magic(0x%x) isn't init!", queue_type, init_magic);
        spin_unlock_irqrestore(&g_msg_que_ctrl.msg_queue_info[queue_type].send_lock, flag);
        return -EFAULT;
    }

    pi = ioread32(msg_q_info->pi_base);
    ci = ioread32(msg_q_info->ci_base);
    /* The queue is full. */
    if (((pi + 1) % (msg_q_info->queue_depth)) == ci) {
        LOG(LOG_ERROR, "msg_cmd_send queue_type(0x%x) is full!", queue_type);
        spin_unlock_irqrestore(&g_msg_que_ctrl.msg_queue_info[queue_type].send_lock, flag);
        return -EFAULT;
    }

    /* Copying Data */
    msg_addr = (msg_comm_data_s *)(((u64)(msg_q_info->queue_base)) + (pi * MSG_CMD_LEN));
    ret = memcpy_s((void *)msg_addr, MSG_CMD_LEN, msg_buf, MSG_CMD_LEN);
    if (ret != 0) {
        LOG(LOG_ERROR, "memcpy_s error:%d", ret);
        return ret;
    }

    /* add crc checksum */
    dal_cal_crc16((u8 *)msg_addr, MSG_CMD_LEN - MSG_CMD_CRC_LEN, &cksum);
    msg_addr->msg_crc = cksum;

    /* Update pi */
    pi = (pi + 1) % (msg_q_info->queue_depth);
    iowrite32(pi, msg_q_info->pi_base);
    wmb();

    /* trigger inter core interrupt to M3 */
    sysctrl_reg_write(SYSCTRL_CSR_A55_ASSERT_SOFT_INT_OFFSET_ADDR, MSG_A55_M3_INT_TRIGGER);
    spin_unlock_irqrestore(&g_msg_que_ctrl.msg_queue_info[queue_type].send_lock, flag);

    return 0;
}

static s32 msg_comm_send_and_wait(msg_comm_data_s *msg)
{
    u32 time = 0;
    s32 ret;
    u16 completion_n;

    if (!msg) {
        LOG(LOG_ERROR, "Error: msg is NULL");
        return -EINVAL;
    }

    if (msg->msg_index >= MSG_Q_DEPTH) {
        LOG(LOG_ERROR, "Error: msg_index should range from 0 to 15");
        return -EINVAL;
    }

    completion_n = msg->msg_index;
    while (time < MSG_SEND_RETRY_TIMES) {
        ret = msg_cmd_send(MSG_Q_TYPE_SECURE_M3, (void *)msg);
        if (ret == 0) {
            break;
        }

        msleep(100); /* total wait 10s, 100 cycles, so sleep 100ms here */
        time++;
    }

    if (time >= MSG_SEND_RETRY_TIMES) {
        LOG(LOG_ERROR, "msg cmd send error:%d", ret);
        /* indicating return wrong msg from M3 */
        ret = memset_s((void *)msg, MSG_CMD_LEN, 0, MSG_CMD_LEN);
        if (ret != 0) {
            LOG(LOG_ERROR, "memset fail:%d", ret);
        }
        return -MSG_TX_TIME_OUT_ERROR;
    }

    ret = wait_for_completion_timeout(&g_msg_que_ctrl.msg_completion[completion_n],
                                      msecs_to_jiffies(MSG_WAIT_TIME_OUT));
    if (ret == 0) {
        ret = -MSG_RX_TIME_OUT_ERROR;
    } else {
        ret = 0;
    }

    return ret;
}

static s32 msg_scm3_open(struct inode *inode, struct file *file)
{
    return 0;
}

static s32 msg_scm3_release(struct inode *inode, struct file *file)
{
    return 0;
}

static void *msg_get_data_buffer(u16 *msg_index)
{
    unsigned long flag = 0;
    u16 index;
    void *addr = NULL;

    spin_lock_irqsave(&g_msg_que_ctrl.msg_get_buf_lock, flag);

    for (index = 0; index < MSG_Q_DEPTH; index++) {
        if (g_msg_que_ctrl.msg_buf_status[index] == 0) {
            g_msg_que_ctrl.msg_buf_status[index] = 1;
            break;
        }
    }

    spin_unlock_irqrestore(&g_msg_que_ctrl.msg_get_buf_lock, flag);

    if (index >= MSG_Q_DEPTH) {
        return NULL;
    } else {
        *msg_index = index;
        addr = g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_A55].queue_base + MSG_Q_HEAD_RSV_SIZE;
        addr += index * MSG_DATA_MAX_SIZE;
        return addr;
    }
}

static void msg_release_data_buffer(u16 index)
{
    unsigned long flag = 0;

    spin_lock_irqsave(&g_msg_que_ctrl.msg_get_buf_lock, flag);
    g_msg_que_ctrl.msg_buf_status[index] = 0;
    spin_unlock_irqrestore(&g_msg_que_ctrl.msg_get_buf_lock, flag);

    return;
}

static s32 msg_check_interal_error(void *msg)
{
    sec_common_msg_resp_s *user_data = NULL;

    user_data = (sec_common_msg_resp_s *)msg;
    if ((user_data->hd.cmd & MSG_RESP_CMD_MASK) != 0) {
        return 0;
    }

    LOG(LOG_ERROR, "msg comm internal error:%u", user_data->resp_code);
    return -MSG_COMM_INTERNAL_ERROR;
}

static s32 msg_check_ioctl_user_data(sec_module_cmd_s *user_cmd)
{
    if (user_cmd->s_len > MSG_DATA_MAX_SIZE) {
        LOG(LOG_ERROR, "tx len overflow 0x%x!", user_cmd->s_len);
        return -MSG_TX_LEN_OVERFLOW_ERROR;
    } else if (user_cmd->s_len < MSG_DATA_TX_MIN_SIZE) {
        LOG(LOG_ERROR, "tx len underflow 0x%x!", user_cmd->s_len);
        return -MSG_TX_LEN_UNDERFLOW_ERROR;
    } else if (user_cmd->r_len < MSG_DATA_RX_MIN_SIZE) {
        LOG(LOG_ERROR, "rx len underflow 0x%x!", user_cmd->r_len);
        return -MSG_RX_LEN_UNDERFLOW_ERROR;
    } else if (user_cmd->r_buff == NULL) {
        LOG(LOG_ERROR, "r_buffer NULL error");
        return -MSG_RX_BUFFER_NULL_ERROR;
    } else if (user_cmd->s_buff == NULL) {
        LOG(LOG_ERROR, "s_buffer NULL error");
        return -MSG_TX_BUFFER_NULL_ERROR;
    }

    return 0;
}

static s32 msg_sec_module_cmd_tx_rx(msg_comm_data_s *msg, u16 msg_index, void *msg_km_addr)
{
    s32 ret;

    ret = msg_comm_send_and_wait(msg);
    if (ret != 0) {
        LOG(LOG_ERROR, "msg_comm_send_and_wait error");
        return ret;
    }

    /* copy message to the corresponding process */
    (void)memcpy_s((void *)msg, MSG_CMD_LEN, (void *)g_msg_que_ctrl.msg_user_addr[msg_index], MSG_CMD_LEN);

    return msg_check_interal_error(msg_km_addr);
}

static s32 msg_sec_module_cmd_cpy_to_user(void *user_buf, sec_module_cmd_s *user_cmd, const void *data)
{
    s32 ret;

    if (user_cmd->r_len == 0) {
        LOG(LOG_ERROR, "user_cmd.rlen is zero");
        return -MSG_CP_TO_USER_CMD_ERROR;
    }

    ret = copy_to_user((void *)(user_cmd->r_buff), data, user_cmd->r_len);
    if (ret != 0) {
        LOG(LOG_ERROR, "msg copy_to_user user data error %d", ret);
        return -MSG_CP_TO_USER_CMD_ERROR;
    }

    ret = copy_to_user(user_buf, (void *)user_cmd, sizeof(sec_module_cmd_s));
    if (ret != 0) {
        LOG(LOG_ERROR, "msg copy_to_user user data structure error %d", ret);
        return -MSG_CP_TO_USER_DATA_ERROR;
    }

    return 0;
}

static s32 msg_sec_module_cmd_handle(unsigned long buf)
{
    msg_comm_data_s msg;
    sec_module_cmd_s user_cmd = { 0 };
    void *msg_km_addr = NULL;
    u16 msg_index;
    s32 ret;

    if (copy_from_user(&user_cmd, (void *)(uintptr_t)buf, sizeof(sec_module_cmd_s))) {
        LOG(LOG_ERROR, "msg cmd copy_from_user fail!");
        return -MSG_CP_FROM_USER_CMD_ERROR;
    }

    ret = msg_check_ioctl_user_data(&user_cmd);
    if (ret != 0) {
        return ret;
    }

    msg_km_addr = msg_get_data_buffer(&msg_index);
    if (msg_km_addr == NULL) {
        LOG(LOG_ERROR, "msg_get_data_buffer error");
        return -MSG_GET_DATA_BUFFER_ERROR;
    }

    if (copy_from_user(msg_km_addr, (void *)(user_cmd.s_buff), user_cmd.s_len)) {
        LOG(LOG_ERROR, "msg cmd copy_from_user fail!");
        msg_release_data_buffer(msg_index);
        return -MSG_CP_FROM_USER_DATA_ERROR;
    }

    msg.data_addr = msg_data_address(msg_index);
    msg.msg_index = msg_index;
    msg.data_len = user_cmd.s_len;

    ret = msg_sec_module_cmd_tx_rx(&msg, msg_index, msg_km_addr);
    if (ret != 0) {
        msg_release_data_buffer(msg_index);
        return ret;
    }

    if (msg.data_len > user_cmd.r_len) {
        LOG(LOG_ERROR, "msg len overflow 0x%x!", msg.data_len);
        msg_release_data_buffer(msg_index);
        return -MSG_RX_MSG_OVERFLOW_ERROR;
    }

    user_cmd.r_len = msg.data_len;
    ret = msg_sec_module_cmd_cpy_to_user((void *)(uintptr_t)buf, &user_cmd, msg_km_addr);

    msg_release_data_buffer(msg_index);

    return ret;
}

static long msg_scm3_ioctl(struct file *file, u32 cmd, unsigned long buf)
{
    s32 ret;

    switch (cmd) {
        case SEC_MODULE_CMD_TX_RX:
            ret = msg_sec_module_cmd_handle(buf);
            if (ret) {
                LOG(LOG_ERROR, "msg cmd(0x%x) handle error!", cmd);
                return ret;
            }
            break;
        default:
            LOG(LOG_ERROR, "msg cmd(0x%x) isn't support!", cmd);
            return -MSG_IO_CMD_NOT_SUPPORT;
    }

    return 0;
}

static const struct file_operations g_msg_scm3_fops = {
    .owner = THIS_MODULE,
    .open = msg_scm3_open,
    .release = msg_scm3_release,
    .unlocked_ioctl = msg_scm3_ioctl,
};

void msg_dfx_dump(void)
{
    u32 i;
    msg_comm_data_s *msg;
    void __iomem *data_addr;

    data_addr = g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_SECURE_M3].queue_base;
    if (data_addr == NULL) {
        msg_counter("msg queue base is NULL, no dump.\n");
        return;
    }

    for (i = 0; i < MSG_Q_DEPTH; i++) {
        msg = (msg_comm_data_s *)(data_addr + (i * MSG_CMD_LEN));
        msg_counter("msg index: %u, data addr: 0x%llx, data len: 0x%x, index: %u, crc: 0x%x.", \
                    i, msg->data_addr, msg->data_len, msg->msg_index, msg->msg_crc);
    }

    return;
}
EXPORT_SYMBOL(msg_dfx_dump);

static void msg_queue_ctrl_init(void __iomem *msg_q_io_addr)
{
    void __iomem *pici_io_addr = msg_q_io_addr + MSG_Q_TYPE_BUTT * MSG_Q_SIZE;
    u32 index;

    /* Initialize queue control information. */
    for (index = 0; index < MSG_Q_TYPE_BUTT; index++) {
        g_msg_que_ctrl.msg_queue_info[index].queue_depth = MSG_Q_DEPTH;
        g_msg_que_ctrl.msg_queue_info[index].queue_base = msg_q_io_addr + (index * MSG_Q_SIZE);
        g_msg_que_ctrl.msg_queue_info[index].pi_base = pici_io_addr + (index * MSG_Q_CTL_SIZE);
        g_msg_que_ctrl.msg_queue_info[index].ci_base = g_msg_que_ctrl.msg_queue_info[index].pi_base + MSG_Q_PI_SIZE;
        g_msg_que_ctrl.msg_queue_info[index].init_flag_base = g_msg_que_ctrl.msg_queue_info[index].ci_base +
                                                              MSG_Q_CI_SIZE;

        spin_lock_init(&g_msg_que_ctrl.msg_queue_info[index].send_lock);
        /* Initialize the queue parameters of the CPU. */
        if (index == MSG_Q_TYPE_A55) {
            iowrite32(0x0, g_msg_que_ctrl.msg_queue_info[index].pi_base);
            iowrite32(0x0, g_msg_que_ctrl.msg_queue_info[index].ci_base);
            iowrite32(MSG_Q_INIT_MAGIC, g_msg_que_ctrl.msg_queue_info[index].init_flag_base);
        }
    }

    return;
}

static s32 msg_scm3_probe(struct platform_device *pdev)
{
    s32 ret;
    u32 index;
    void __iomem *msg_q_io_addr;
    struct device_node *node = pdev->dev.of_node;

    g_msg_dev_info.msg_dev = (cdev_st *)kzalloc(sizeof(cdev_st), GFP_KERNEL);
    if (g_msg_dev_info.msg_dev == NULL) {
        LOG(LOG_ERROR, "sec_module kzalloc err!");
        return -ENOMEM;
    }

    ret = comm_init_dev(g_msg_dev_info.msg_dev, &g_msg_scm3_fops, g_msg_dev_info.msg_dev_name);
    if (ret != 0) {
        LOG(LOG_ERROR, "sec_module comm_init failed(%d)!", ret);
        goto dev_init_err;
    }

    msg_q_io_addr = ioremap_wc(MSG_Q_BASE_ADDR, MSG_Q_HEAD_RSV_SIZE + MSG_Q_DATA_SIZE);
    if (msg_q_io_addr == NULL) {
        LOG(LOG_ERROR, "fail to map msg queue addr(0x%x)!", MSG_Q_BASE_ADDR);
        goto msg_init_err;
    }

    /* init queue ctrl info */
    msg_queue_ctrl_init(msg_q_io_addr);

    /* init get buf lock */
    spin_lock_init(&g_msg_que_ctrl.msg_get_buf_lock);

    /* init completion variable and buffer status for all the messages */
    for (index = 0; index < MSG_Q_DEPTH; index++) {
        init_completion(&g_msg_que_ctrl.msg_completion[index]);
        g_msg_que_ctrl.msg_buf_status[index] = 0;
    }

    /* Initializing the A55 Message Processing Queue */
    msg_delaywork_init();

    /* clear secure M3 inter core interrupt */
    sysctrl_reg_write(SYSCTRL_CSR_SC_ASSERT_SOFT_INT_OFFSET_ADDR, 0);

    /* register msg secure M3 intercore interrupt */
    g_msg_que_ctrl.msg_intercore_virq = irq_of_parse_and_map(node, 0);
    if (!g_msg_que_ctrl.msg_intercore_virq) {
        LOG(LOG_ERROR, "g_msg_que_ctrl.msg_intercore_virq:failed to get irq");
        return -EINVAL;
    }

    ret = request_irq(g_msg_que_ctrl.msg_intercore_virq, msg_interrupt_handle, 0, "intercore_int",
                      NULL);
    if (ret != 0) {
        LOG(LOG_ERROR, "secure M3 inter core interrupt already in use!");
        goto req_irq_err;
    }

    return 0;
req_irq_err:
    iounmap(msg_q_io_addr);
    g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_A55].queue_base = NULL;

msg_init_err:
    comm_cleanup_dev(g_msg_dev_info.msg_dev);

dev_init_err:
    kfree(g_msg_dev_info.msg_dev);
    g_msg_dev_info.msg_dev = NULL;

    return ret;
}

static int msg_scm3_remove(struct platform_device *pdev)
{
    u32 index;

    /* The queue is marked as uninitialized. */
    for (index = 0; index < MSG_Q_TYPE_BUTT; index++) {
        if (MSG_Q_TYPE_A55 != index) {
            continue;
        }

        iowrite32(0x0, g_msg_que_ctrl.msg_queue_info[index].init_flag_base);
    }

    /* Delete the work_queue for message processing. */
    if (g_msg_wq.msg_wq != NULL) {
        cancel_delayed_work_sync(&g_msg_wq.msg_delayed_work);
        destroy_workqueue(g_msg_wq.msg_wq);
        g_msg_wq.msg_wq = NULL;
    }

    /* Cancel Address Mapping */
    if (g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_A55].queue_base != NULL) {
        iounmap(g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_A55].queue_base);
        g_msg_que_ctrl.msg_queue_info[MSG_Q_TYPE_A55].queue_base = NULL;
    }

    /* free secure m3 inter core interrupt */
    free_irq(g_msg_que_ctrl.msg_intercore_virq, NULL);

    if (g_msg_dev_info.msg_dev != NULL) {
        comm_cleanup_dev(g_msg_dev_info.msg_dev);
        kfree(g_msg_dev_info.msg_dev);
        g_msg_dev_info.msg_dev = NULL;
    }

    return 0;
}

static const struct of_device_id g_msg_scm3_match[] = {
    { .compatible = "hisilicon, hi1711-msg_scm3" }, {}
};

static struct platform_driver g_msg_scm3_driver = {
    .driver = {
        .name = SEC_MODULE_DEVNAME,
        .of_match_table = g_msg_scm3_match,
    },

    .probe = msg_scm3_probe,
    .remove = msg_scm3_remove,
};
module_platform_driver(g_msg_scm3_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("msg scm3 driver");