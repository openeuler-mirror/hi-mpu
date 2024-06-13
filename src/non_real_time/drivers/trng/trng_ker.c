// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * trng driver interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/memory.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/io.h>   /* ioremap */
#include <linux/slab.h> /* kmalloc */
#include <linux/fs.h>   /* struct file_operations */
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>

#include "trng.h"
#include "lp_subsys.h"

struct device_node *g_trng_node = NULL;
void __iomem *g_lp_trng_addr = NULL;
u32 g_trng_irq_num = 0;
cdev_st *g_trng_dev = NULL;

s32 trng_rst_req(void)
{
    u32 reg_val = 0;
    u32 time_out = TRNG_COM_ST_TMOUT;

    /* Resetting the TRNG */
    lp_subsys_reg_write(LPSC_RESET_REQ2, LPSC_TRNG_BIT);

    /* To query the TRNG reset status */
    (void)lp_subsys_reg_read(LPSC_RESET_ST2, &reg_val);
    while ((reg_val & LPSC_TRNG_BIT) != LPSC_TRNG_BIT) {
        if (time_out-- == 0) {
            LOG(LOG_ERROR, "[T] wait trng reset time_out. st:0x%08x", reg_val);
            return -EAGAIN;
        }
        msleep(1);
        (void)lp_subsys_reg_read(LPSC_RESET_ST2, &reg_val);
    }

    return 0;
}

s32 trng_rst_dreq(void)
{
    u32 reg_val = 0;
    u32 time_out = TRNG_COM_ST_TMOUT;

    /* Deassert reset on the TRNG. */
    lp_subsys_reg_write(LPSC_RESET_DREQ2, LPSC_TRNG_BIT);

    /* To query the TRNG reset status */
    (void)lp_subsys_reg_read(LPSC_RESET_ST2, &reg_val);
    while ((reg_val & LPSC_TRNG_BIT) != 0) {
        if (time_out-- == 0) {
            LOG(LOG_ERROR, "[T]  wait trng dreset time_out. st:0x%08x", reg_val);
            return -EAGAIN;
        }
        msleep(1);
        (void)lp_subsys_reg_read(LPSC_RESET_ST2, &reg_val);
    }

    return 0;
}

s32 trng_get_random_user(u32 *data, u32 random_nums)
{
    u32 time_out;
    u32 temp_data[RANDOM_GRP_CNT]; /* A group of random numbers contains four words.，128bit */
    u32 counts;
    u32 remain;
    u32 cp_size;
    u32 i;

    /* Random numbers are grouped by 128 bits (four u32s). */
    counts = random_nums >> 2; // >> 2
    remain = random_nums - (counts << 2); // << 2

    for (i = 0; i <= counts; i++) {
        time_out = TRNG_RANDOM_TMOUT;
        do {
            temp_data[RANDOM_0] = readl(g_lp_trng_addr + LP_TRNG_RAN_DATA0_REG); /* random 0 */
            temp_data[RANDOM_1] = readl(g_lp_trng_addr + LP_TRNG_RAN_DATA1_REG); /* random 1 */
            temp_data[RANDOM_2] = readl(g_lp_trng_addr + LP_TRNG_RAN_DATA2_REG); /* random 2 */
            temp_data[RANDOM_3] = readl(g_lp_trng_addr + LP_TRNG_RAN_DATA3_REG); /* random 3 */
            msleep(1);
        } while ((!temp_data[RANDOM_0] || !temp_data[RANDOM_1] || !temp_data[RANDOM_2] || !temp_data[RANDOM_3])
                 && --time_out);

        if (time_out == 0) {
            LOG(LOG_ERROR, "[T] lp trng get random timeout!");
            return -1;
        }

        cp_size = (i == counts) ? (remain * TRNG_WORD_BYTE_CNT) : (RANDOM_GRP_CNT * TRNG_WORD_BYTE_CNT);
        (void)memcpy_s((data + (i * TRNG_WORD_BYTE_CNT)), cp_size, temp_data, cp_size);
    }

    return 0;
}

EXPORT_SYMBOL(trng_get_random_user);

s32 trng_get_random_inside(u32 *data, u32 random_nums)
{
    s32 ret;
    u_trng_status status_value;
    u32 temp_data[RANDOM_GRP_CNT];
    u32 time_out;
    u32 counts;
    u32 remain;
    u32 cp_size;
    u32 i;

    counts = random_nums >> 2; // >> 2
    remain = random_nums - (counts << 2); // << 2

    for (i = 0; i <= counts; i++) {
        /* clear ready int, value readed from while cycle */
        writel(TRNG_INT_READY, g_lp_trng_addr + LP_TRNG_INTACK_REG);

        time_out = TRNG_RANDOM_TMOUT;
        status_value.val = readl(g_lp_trng_addr + LP_TRNG_STATUS_REG);
        while (status_value.bits.ready != 1) {
            if (time_out-- == 0) {
                LOG(LOG_ERROR, "[T] get random data wait TRNG_STATUS ready time_out.");
                return -1;
            }
            msleep(1);
            status_value.val = readl(g_lp_trng_addr + LP_TRNG_STATUS_REG);
        }

        temp_data[RANDOM_0] = readl(g_lp_trng_addr + LP_TRNG_OUTPUT_0_REG);
        temp_data[RANDOM_1] = readl(g_lp_trng_addr + LP_TRNG_OUTPUT_1_REG);
        temp_data[RANDOM_2] = readl(g_lp_trng_addr + LP_TRNG_OUTPUT_2_REG);
        temp_data[RANDOM_3] = readl(g_lp_trng_addr + LP_TRNG_OUTPUT_3_REG);

        cp_size = (i == counts) ? (remain * TRNG_WORD_BYTE_CNT) : (RANDOM_GRP_CNT * TRNG_WORD_BYTE_CNT);
        ret = memcpy_s((data + (i * TRNG_WORD_BYTE_CNT)), cp_size, temp_data, cp_size);
        if (ret) {
            LOG(LOG_INFO, "[T] carry random out fail. ret:%d", ret);
            return ret;
        }
    }

    return 0;
}

void trng_int_init(void)
{
    u32 value;

    /* clear int & pending int, only clear error int status in */
    value = readl(g_lp_trng_addr + LP_TRNG_STATUS_REG);
    value &= (TRNG_INT_SHUTDOWN_OFLO | TRNG_INT_RUN_FAIL | TRNG_INT_LRUN_FAIL | TRNG_INT_POKER_FAIL
              | TRNG_INT_MONOBIT_FAIL | TRNG_INT_STUCK_NRBG | TRNG_INT_REPCNT_FAIL | TRNG_INT_APROP_FAIL
              | TRNG_INT_STUCK_OUT | TRNG_INT_NOISE_FAIL);
    writel(value, g_lp_trng_addr + LP_TRNG_INTACK_REG);

    /* enable int mask */
    value = readl(g_lp_trng_addr + LP_TRNG_CONTROL_REG);
    /* enable all error int mask */
    value |= (TRNG_INT_SHUTDOWN_OFLO | TRNG_INT_RUN_FAIL | TRNG_INT_LRUN_FAIL | TRNG_INT_POKER_FAIL
              | TRNG_INT_MONOBIT_FAIL | TRNG_INT_STUCK_NRBG | TRNG_INT_REPCNT_FAIL | TRNG_INT_APROP_FAIL
              | TRNG_INT_STUCK_OUT | TRNG_INT_NOISE_FAIL);
    /* disable ready mask */
    value &= ~TRNG_INT_READY;
    writel(value, g_lp_trng_addr + LP_TRNG_CONTROL_REG);

    return;
}

/* Software initialization of hardware private interfaces */
void trng_sw_init_with_hw_interface(void)
{
    u32 value;

    writel(0x10008, g_lp_trng_addr + LP_TRNG_CONFIG_REG);
    writel(0x200ff, g_lp_trng_addr + LP_TRNG_ALARMCNT_REG);
    writel(0xff, g_lp_trng_addr + LP_TRNG_FROENABLE_REG);

    trng_int_init();
    value = readl(g_lp_trng_addr + LP_TRNG_CONTROL_REG);
    value |= (TRNG_DRBG_ENABLE | TRNG_TRNG_ENABLE);
    writel(value, g_lp_trng_addr + LP_TRNG_CONTROL_REG);

    value = readl(g_lp_trng_addr + LP_TRNG_CONTROL_REG);
    value |= (TRNG_DATA_BLOCKS | TRNG_REQUEST_DATA);
    writel(value, g_lp_trng_addr + LP_TRNG_CONTROL_REG);

    value = readl(g_lp_trng_addr + LP_TRNG_CTRL0_REG);
    value |= (TRNG_AUTO_RESEED_ENABLE | TRNG_RSA_INTFS_DISABLE | TRNG_MODE);
    value &= ~TRNG_USR_INTFS_DISABLE;
    writel(value, g_lp_trng_addr + LP_TRNG_CTRL0_REG);

    return;
}

s32 trng_init(void)
{
    s32 ret;

    /* trng reset */
    ret = trng_rst_req();
    if (ret) {
        return ret;
    }

    /* trng dreset */
    ret = trng_rst_dreq();
    if (ret) {
        return ret;
    }

    trng_sw_init_with_hw_interface();
    LOG(LOG_INFO, "[T] sw init hw intf success.");

    return 0;
}

void trng_dfx_reg_dump(void)
{
    LOG(LOG_ERROR, "TRNG_CONTROL:      0x%08x", readl(g_lp_trng_addr + LP_TRNG_CONTROL_REG));
    LOG(LOG_ERROR, "TRNG_CONFIG:       0x%08x", readl(g_lp_trng_addr + LP_TRNG_CONFIG_REG));
    LOG(LOG_ERROR, "TRNG_ALARMMASK:    0x%08x", readl(g_lp_trng_addr + LP_TRNG_ALARMMASK_REG));
    LOG(LOG_ERROR, "TRNG_ALARMSTOP:    0x%08x", readl(g_lp_trng_addr + LP_TRNG_ALARMSTOP_REG));
    LOG(LOG_ERROR, "TRNG_COUNT:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_COUNT_REG));
    LOG(LOG_ERROR, "TRNG_RUN_CNT:      0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_CNT_REG));
    LOG(LOG_ERROR, "TRNG_RUN_1:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_1_REG));
    LOG(LOG_ERROR, "TRNG_RUN_2:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_2_REG));
    LOG(LOG_ERROR, "TRNG_RUN_3:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_3_REG));
    LOG(LOG_ERROR, "TRNG_RUN_4:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_4_REG));
    LOG(LOG_ERROR, "TRNG_RUN_5:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_5_REG));
    LOG(LOG_ERROR, "TRNG_RUN_6_UP:     0x%08x", readl(g_lp_trng_addr + LP_TRNG_RUN_6_UP_REG));
    LOG(LOG_ERROR, "TRNG_MONOBITCNT:   0x%08x", readl(g_lp_trng_addr + LP_TRNG_MONOBITCNT_REG));
    LOG(LOG_ERROR, "TRNG_POKER_3_0:    0x%08x", readl(g_lp_trng_addr + LP_TRNG_POKER_3_0_REG));
    LOG(LOG_ERROR, "TRNG_POKER_7_4:    0x%08x", readl(g_lp_trng_addr + LP_TRNG_POKER_7_4_REG));
    LOG(LOG_ERROR, "TRNG_POKER_B_8:    0x%08x", readl(g_lp_trng_addr + LP_TRNG_POKER_B_8_REG));
    LOG(LOG_ERROR, "TRNG_POKER_F_C:    0x%08x", readl(g_lp_trng_addr + LP_TRNG_POKER_F_C_REG));
    LOG(LOG_ERROR, "TRNG_BLOCKCNT:     0x%08x", readl(g_lp_trng_addr + LP_TRNG_BLOCKCNT_REG));
    LOG(LOG_ERROR, "TRNG_CTRL0:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_CTRL0_REG));
    LOG(LOG_ERROR, "TRNG_CTRL1:        0x%08x", readl(g_lp_trng_addr + LP_TRNG_CTRL1_REG));
    LOG(LOG_ERROR, "TRNG_FSM_ST:       0x%08x", readl(g_lp_trng_addr + LP_TRNG_FSM_ST_REG));
    LOG(LOG_ERROR, "RNG_PHY_SEED:      0x%08x", readl(g_lp_trng_addr + LP_RNG_PHY_SEED_REG));
    LOG(LOG_ERROR, "RNG_ERR:           0x%08x", readl(g_lp_trng_addr + LP_RNG_ERR_REG));

    return;
}

bool trng_check_trng_need_reset(u32 int_st)
{
    u32 i;
    u64 obj_time;
    u64 cur_time = jiffies;
    bool trng_rst_flag = false;
    static u32 trng_rst_err_cnt[TRNG_ERROR_INT_NUM] = { 0 };
    static u64 trng_rst_err_time_start[TRNG_ERROR_INT_NUM] = { 0 };
    u32 err_int_mask_list[TRNG_ERROR_INT_NUM] = {
        TRNG_INT_RUN_FAIL,
        TRNG_INT_LRUN_FAIL,
        TRNG_INT_POKER_FAIL,
        TRNG_INT_MONOBIT_FAIL,
        TRNG_INT_STUCK_NRBG,
        TRNG_INT_REPCNT_FAIL,
        TRNG_INT_APROP_FAIL,
    };

    for (i = 0; i < sizeof(err_int_mask_list) / sizeof(err_int_mask_list[0]); i++) {
        if (int_st & err_int_mask_list[i]) {
            trng_rst_err_cnt[i]++;
            trng_rst_err_time_start[i] = (trng_rst_err_time_start[i] == 0) ? cur_time : trng_rst_err_time_start[i];
            LOG(LOG_ERROR, "[T] normal error int occur. int mask list:0x%x int occur:0x%x cnt:%u", err_int_mask_list[0],
                int_st & err_int_mask_list[i], trng_rst_err_cnt[i]);
        }

        if (trng_rst_err_cnt[i] < TRNG_RST_ERR_CNT) {
            continue;
        }

        /* if some error occure over 100-times in 5 minutes, reset trng */
        obj_time = trng_rst_err_time_start[i] + msecs_to_jiffies(TRNG_RST_ERR_CHK_PERIOD);
        if (time_before((unsigned long)cur_time, (unsigned long)obj_time)) {
            LOG(LOG_EMERG, "[T] normal error int cnt come up to threshold(100 times). idx:%u cnt: %u",
                i, trng_rst_err_cnt[i]);
            trng_rst_flag = true;
        }

        /* if coount is over 100-times, re-start record time & count */
        trng_rst_err_cnt[i] = 0;
        trng_rst_err_time_start[i] = cur_time;
    }

    return trng_rst_flag;
}

static irqreturn_t trng_error_interrupt_handle(s32 irq, void *arg)
{
    s32 ret;
    u32 int_st;
    u32 fro_detune;
    u32 fro_detune_cfg;
    u32 fro_err_bit_map = 0;
    u32 alarm_stop;
    u32 alarm_mask;
    bool trng_rst_flag = false;

    /* just clear error source int */
    int_st = readl(g_lp_trng_addr + LP_TRNG_STATUS_REG);
    writel((int_st & TRNG_ERROR_INT), g_lp_trng_addr + LP_TRNG_INTACK_REG);

    if (int_st & TRNG_ERROR_INT) {
        LOG(LOG_ERROR, "[T] error int occur. int_val:0x%x", int_st);
        trng_dfx_reg_dump();
    }

    /* shutdown oflo no need to reset trng, just modulation fro frequency */
    if (int_st & TRNG_INT_SHUTDOWN_OFLO) {
        alarm_mask = readl(g_lp_trng_addr + LP_TRNG_ALARMMASK_REG);
        alarm_stop = readl(g_lp_trng_addr + LP_TRNG_ALARMSTOP_REG);
        writel(0, g_lp_trng_addr + LP_TRNG_ALARMMASK_REG);
        writel(0, g_lp_trng_addr + LP_TRNG_ALARMSTOP_REG);
        fro_err_bit_map = alarm_mask & alarm_stop; /* keep alarm event fro bitmap */
        LOG(LOG_ERROR, "[T] shutdown oflo error. alarm_mask:0x%x alarm_stop:0x%x", alarm_mask, alarm_stop);
    }

    /*
     * After noise_fail and stuck_out occur, the TRng should be restarted immediately.
     * This is because the masks of the two are enabled. In addition to reporting interrupts,
     * the TRng also considers that a fatal error occurs and disables the TRng.
     */
    if ((int_st & TRNG_INT_STUCK_OUT) || (int_st & TRNG_INT_NOISE_FAIL)) {
        LOG(LOG_ERROR, "[T] stuck out or noise fail occur. stuck_out:%u noise_fail:%u",
            int_st & TRNG_INT_STUCK_OUT, int_st & TRNG_INT_NOISE_FAIL);

        trng_rst_flag = true;
    }

    trng_rst_flag = trng_check_trng_need_reset(int_st) || trng_rst_flag;

    if (fro_err_bit_map) {
        fro_detune = readl(g_lp_trng_addr + LP_TRNG_FRODETUNE_REG);
        fro_detune_cfg = ~fro_detune;
        fro_detune_cfg &= fro_err_bit_map;
        fro_detune_cfg |= (fro_detune & ~fro_err_bit_map);

        LOG(LOG_ERROR, "[T] modulation fro. error occur, fro_detune_old:0x%x fre_detune_cfg:0x%x", fro_detune,
            fro_detune_cfg);
        writel(fro_detune_cfg, g_lp_trng_addr + LP_TRNG_FRODETUNE_REG);

        writel(0xff, g_lp_trng_addr + LP_TRNG_FROENABLE_REG);
    }

    /* if record 2bit ecc 5-times, reset trng */
    if (trng_rst_flag) {
        LOG(LOG_ERROR, "[T] reset trng. error occur, int_val: 0x%x", int_st);
        ret = trng_init();
        if (ret) {
            LOG(LOG_EMERG, "[T] trng re-init fail after 2bit ecc error. ret:%d", ret);
        }
    }

    return IRQ_HANDLED;
}

s32 trng_get_random(u32 cmd, u64 arg)
{
    s32 ret = 0;
    u32 randoms[RANDOM_GRP_NUM] = { 0 };
    u32 len;
    u32 *ptr = NULL;
    trng_rand_s info;

    (void)memset_s(&info, sizeof(trng_rand_s), 0, sizeof(trng_rand_s));

    ret = copy_from_user(&info, (void __user *)arg, sizeof(trng_rand_s));
    if (ret) {
        LOG(LOG_ERROR, "[T] copy random info from user fail!");
        return -EAGAIN;
    }

    if (!info.rand_data || info.cnt > RANDOM_MAX_CNT) {
        LOG(LOG_ERROR, "[T] random info error. cnt:%u", info.cnt);
        return -EINVAL;
    }

    ptr = info.rand_data;
    len = info.cnt;

    while (len >= RANDOM_GRP_NUM) {
        ret = trng_get_random_user(randoms, RANDOM_GRP_NUM);
        if (ret) {
            return ret;
        }

        ret = copy_to_user(ptr, randoms, TRNG_WORD_BYTE_CNT * RANDOM_GRP_NUM);
        if (ret) {
            LOG(LOG_ERROR, "[T] copy random to user fail. len:%u.", len);
            return ret;
        }

        len -= RANDOM_GRP_NUM;
        ptr += RANDOM_GRP_NUM;
    }

    if (len) {
        ret = trng_get_random_user(randoms, len);
        if (ret) {
            return ret;
        }

        ret = copy_to_user(ptr, randoms, TRNG_WORD_BYTE_CNT * len);
        if (ret) {
            LOG(LOG_ERROR, "[T] copy random to user fail. len:%u.", len);
        }
    }

    return ret;
}

s32 trng_reset(u32 cmd, unsigned long arg)
{
    s32 ret;

    ret = trng_rst_req();
    if (ret) {
        LOG(LOG_ERROR, "[T] trng reset error!");
        return -EAGAIN;
    }

    ret = trng_rst_dreq();
    if (ret) {
        LOG(LOG_ERROR, "[T] trng dreset error!");
        return -EAGAIN;
    }

    LOG(LOG_INFO, "[T] trng reset success!");

    return ret;
}

static long trng_ioctl(struct file *pfile, u32 cmd, unsigned long arg)
{
    s32 ret;

    if (_IOC_TYPE(cmd) != LP_TRNG_IOC_MAGIC) {
        LOG(LOG_ERROR, "[T] ioctl magic error.");
        return -ENOTTY;
    }

    switch (cmd) {
        case LP_TRNG_RANDOM:
            ret = trng_get_random(cmd, arg);
            break;
        case LP_TRNG_RESET:
            ret = trng_reset(cmd, arg);
            break;
        default:
            LOG(LOG_ERROR, "[T] ioctl cmd error, cmd:0x%x.", cmd);
            ret = -EINVAL;
    }

    return ret;
}

const struct file_operations g_trng_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = trng_ioctl,
};

static s32 trng_probe_get_vaddr(void)
{
    g_lp_trng_addr = of_iomap(g_trng_node, 0);
    if (g_lp_trng_addr == NULL) {
        LOG(LOG_ERROR, "[T] iocfg register ioremap error.");
        return -ENOMEM;
    }

    LOG(LOG_INFO, "[T] ioremap reg: %p", g_lp_trng_addr);

    return 0;
}

static s32 trng_probe_int_init(u32 *irq)
{
    s32 ret;
    u32 irq_num;

    irq_num = irq_of_parse_and_map(g_trng_node, 0);
    if (!irq_num) {
        LOG(LOG_ERROR, "[T] IRQ parse and map fail.");
        return -EPERM;
    }

    ret = request_irq(irq_num, trng_error_interrupt_handle, 0, "trng", NULL);
    if (ret) {
        LOG(LOG_ERROR, "[T] IRQ already in use. irq:%u", irq_num);
        return -EPERM;
    }

    *irq = irq_num;

    return 0;
}

static s32 trng_probe(struct platform_device *pdev)
{
    s32 ret;

    g_trng_node = pdev->dev.of_node;

    ret = trng_probe_get_vaddr();
    if (ret) {
        goto clean_dev;
    }

    g_trng_dev = (cdev_st *)kmalloc(sizeof(cdev_st), 1);
    if (g_trng_dev == NULL) {
        LOG(LOG_ERROR, "[T] trng dev kmalloc memory error!");
        ret = -ENOMEM;
        goto clean_dev;
    }

    (void)memset_s(g_trng_dev, sizeof(cdev_st), 0, sizeof(cdev_st));

    ret = comm_init_dev(g_trng_dev, &g_trng_fops, DEV_TRNG_NAME);
    if (ret) {
        LOG(LOG_ERROR, "[T] comm_init_dev error!");
        ret = -EFAULT;
        goto clean_dev;
    }

    ret = trng_init();
    if (ret) {
        LOG(LOG_ERROR, "[T] trng init fail. ret:%d", ret);
        ret = -EFAULT;
        goto clean_irq;
    }

    ret = trng_probe_int_init(&g_trng_irq_num);
    if (ret) {
        goto clean_irq;
    }

    LOG(LOG_INFO, "[T] trng drv insmod.");

    return 0;

clean_irq:
    if (g_trng_irq_num) {
        free_irq(g_trng_irq_num, NULL);
        g_trng_irq_num = 0;
    }

    comm_cleanup_dev(g_trng_dev);

clean_dev:
    if (g_trng_dev) {
        kfree(g_trng_dev);
        g_trng_dev = NULL;
    }

    if (g_lp_trng_addr) {
        iounmap((void __iomem *)g_lp_trng_addr);
        g_lp_trng_addr = NULL;
    }

    return ret;
}

static s32 trng_remove(struct platform_device *pdev)
{
    if (g_trng_dev) {
        comm_cleanup_dev(g_trng_dev);
        kfree(g_trng_dev);
        g_trng_dev = NULL;
    }

    if (g_lp_trng_addr) {
        iounmap((void __iomem *)g_lp_trng_addr);
        g_lp_trng_addr = NULL;
    }

    if (g_trng_irq_num) {
        free_irq(g_trng_irq_num, NULL);
        g_trng_irq_num = 0;
    }

    LOG(LOG_INFO, "[T] trng drv removed.");

    return 0;
}

static const struct of_device_id g_trng_of_match[] = {
    {
        .compatible = "hisilicon, hi1711-trng",
    },
    {},
};

static struct platform_driver g_trng_driver = {
    .probe = trng_probe,
    .remove = trng_remove,
    .driver = {
        .name = "hi1711-trng",
        .of_match_table = g_trng_of_match,
    },
};

module_platform_driver(g_trng_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("trng driver");