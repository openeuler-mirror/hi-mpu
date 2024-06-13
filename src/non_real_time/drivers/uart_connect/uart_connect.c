// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Implemention of uart connect module.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/interrupt.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <linux/clkdev.h>
#else
#include <asm/clkdev.h>
#endif

#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "lp_subsys_csr_addr_define.h"
#include "bdinfo.h"
#include "uart_connect.h"

#define UART_CONNECTOR_MODULE_NAME        "[uart-connector]:"
#define UART_CONNECT_DEV_NAME_LEN         32
#define UART_CONNECT_UART_SEL_REG_RST_VAL 0x76543210
#define UART_CONNECT_PORT_SEL_REG_RST_VAL 0x76543210
#define UART_CONNECT_ENABLE_REG_RST_VAL   0xFFFF

typedef struct {
    cdev_st *cdev;
    char cdev_name[UART_CONNECT_DEV_NAME_LEN];
    enum host_baud_status host_status;
    void __iomem *host_uart_baudrate_reg;
    spinlock_t lock;
} hisi_uart_connect_t;

hisi_uart_connect_t g_uart_connector;

static int host_uart_max_baud_check(u32 *baud_tbl, s32 tbl_len)
{
    s32 i;
    u32 constant_baud;

    constant_baud = baud_tbl[0];
    for (i = 1; i < tbl_len; i++) {
        LOG(LOG_INFO, "host_uart_max_baud_check: baudlist[%d]:%u", i, baud_tbl[i]);
        if (baud_tbl[i] > constant_baud) {
            constant_baud = baud_tbl[i];
        }
    }

    if (constant_baud > HOST_UART_MAX_BAUD) {
        LOG(LOG_ERROR, "host_uart_max_baud_check: baudrate(%u) exceeds max baudrate %u",
            constant_baud, HOST_UART_MAX_BAUD);
        return -EINVAL;
    }

    return 0;
}

static int host_uart_find_match_baud(u32 hw_baud_rate, u32 *baud_tbl, s32 tbl_len, u32 *sw_baud_rate)
{
    s32 i;
    u32 baud_match_flag = 0;

    for (i = 0; i < tbl_len; i++) {
        if ((baud_tbl[i]) == 0) {
            LOG(LOG_ERROR, "target baudrate is 0");
            return -EINVAL;
        }
        /* The baud rate cannot be the same as the host uart baud rate. */
        if ((hw_baud_rate / baud_tbl[i]) == 1) {
            *sw_baud_rate = baud_tbl[i];
            baud_match_flag = 1;
            break;
        }
    }

    if (baud_match_flag == 0) {
        LOG(LOG_ERROR, "baudrate not found in user's list");
        return -EINVAL;
    }

    return 0;
}

s32 host_uart_get_info(unsigned long arg)
{
    u32 ret;
    u32 hw_baud_rate;
    u32 baud_reg_val;
    struct host_uart_baud_info baud_info = { 0 };
    u32 baud_tmp[HOST_UART_BAUD_LIST_LEN] = { 0 };

    /* Table of bit rates that may be used in user mode input */
    ret = copy_from_user((void *)&baud_info, (void *)arg, sizeof(baud_info));
    if (ret) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EFAULT;
    }

    if (baud_info.baud_constant_list == NULL) {
        LOG(LOG_ERROR, "baud_constant_list is null\n");
        return -EFAULT;
    }

    ret |= copy_from_user((u32 *)baud_tmp, (u32 * __user) baud_info.baud_constant_list,
                          sizeof(u32) * HOST_UART_BAUD_LIST_LEN);
    if (ret) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EFAULT;
    }

    /* UART channels are supported only by UART0 and UART1. */
    if (baud_info.host_uart_chan > HOST_UART_MAX_CHAN) {
        LOG(LOG_ERROR, "x86 uart channel[%u] > %u", baud_info.host_uart_chan, HOST_UART_MAX_CHAN);
        return -EINVAL;
    }

    /* Verify that no entry with baud rate greater than 115200 exists in the incoming table */
    ret = host_uart_max_baud_check(baud_tmp, HOST_UART_BAUD_LIST_LEN);
    if (ret) {
        return ret;
    }
    // register offset 4
    baud_reg_val = readl(g_uart_connector.host_uart_baudrate_reg + baud_info.host_uart_chan * 4);
    if (baud_reg_val == 0) {
        if (g_uart_connector.host_status == HOST_UART_WORKING) {
            LOG(LOG_ERROR, "host_uart_get_info: uart %u not work", baud_info.host_uart_chan);
            g_uart_connector.host_status = HOST_UART_IDLE;
        }
        return -EINVAL;
    }

    g_uart_connector.host_status = HOST_UART_WORKING;

    // mul 16
    hw_baud_rate = HOST_UART_CLK_RATE / (16 * (baud_reg_val / HOST_UART_CLK_RATIO));
    ret = host_uart_find_match_baud(hw_baud_rate, baud_tmp,
                                    HOST_UART_BAUD_LIST_LEN, &baud_info.uart_baud);
    if (ret) {
        return ret;
    }

    if (copy_to_user((void *)arg, &baud_info, sizeof(baud_info))) {
        LOG(LOG_ERROR, "host_uart_get_info: copy_to_user err");
        return -EFAULT;
    }

    return 0;
}

/* Register manipulations */
static inline s32 get_uart_sel_reg_val(u32 *reg_val)
{
    return lp_subsys_reg_read(UART_SEL_REG_OFFSET, reg_val);
}

static inline s32 get_port_sel_reg_val(u32 *reg_val)
{
    return lp_subsys_reg_read(PORT_SEL_REG_OFFSET, reg_val);
}

static inline s32 get_uart_port_enable_reg_val(u32 *reg_val)
{
    return lp_subsys_reg_read(UART_PORT_ENABLE_REG_OFFSET, reg_val);
}

static inline s32 set_uart_sel_reg_val(u32 reg_val)
{
    return lp_subsys_reg_write(UART_SEL_REG_OFFSET, reg_val);
}

static inline s32 set_port_sel_reg_val(u32 reg_val)
{
    return lp_subsys_reg_write(PORT_SEL_REG_OFFSET, reg_val);
}

static inline s32 set_uart_port_enable_reg_val(u32 reg_val)
{
    return lp_subsys_reg_write(UART_PORT_ENABLE_REG_OFFSET, reg_val);
}

static s32 get_uart_sel(u32 uart_no, u32 *val)
{
    s32 ret;
    u32 reg_val = 0;
    u32 reg_shift = uart_no * 4; /* every port or uart take 4 bit width in reg */
    u32 reg_mask = (0xFUL << reg_shift);

    ret = get_uart_sel_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    *val = (reg_val & reg_mask) >> reg_shift;

    return 0;
}

static s32 get_port_sel(u32 port_no, u32 *val)
{
    s32 ret;
    u32 reg_val = 0;
    u32 reg_shift = port_no * 4; /* every port or uart take 4 bit width in reg */
    u32 reg_mask = (0xFUL << reg_shift);

    ret = get_port_sel_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    *val = (reg_val & reg_mask) >> reg_shift;

    return 0;
}

static s32 set_uart_sel(u32 uart_no, u32 val)
{
    s32 ret;
    u32 reg_val = 0;
    u32 reg_shift = uart_no * 4; /* every port or uart take 4 bit width in reg */
    u32 reg_mask = (0xFUL << reg_shift);

    ret = get_uart_sel_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    reg_val &= ~reg_mask;
    reg_val |= ((val & 0xFUL) << reg_shift);

    ret = set_uart_sel_reg_val(reg_val);
    if (ret) {
        return 0;
    }

    return 0;
}

static s32 set_port_sel(u32 port_no, u32 val)
{
    s32 ret;
    u32 reg_val = 0;
    u32 reg_shift = port_no * 4; /* every port or uart take 4 bit width in reg */
    u32 reg_mask = (0xFUL << reg_shift);

    ret = get_port_sel_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    reg_val &= ~reg_mask;
    reg_val |= ((val & 0xFUL) << reg_shift);

    ret = set_port_sel_reg_val(reg_val);
    if (ret) {
        return 0;
    }

    return 0;
}

static s32 uart_connect(u32 uart_no)
{
    u32 reg_mask;

    if (uart_no >= UART_CONNECT_MAX_PORT_NUM) {
        return -EINVAL;
    }

    /* Bit0-7 represents the uart_en */
    reg_mask = (1UL << uart_no);
    lp_subsys_reg_set_bits(UART_PORT_ENABLE_REG_OFFSET, reg_mask, 1);

    return 0;
}

static s32 port_connect(u32 port_no)
{
    u32 reg_mask;

    if (port_no >= UART_CONNECT_MAX_PORT_NUM) {
        return -EINVAL;
    }

    /* Bit8-15 represents the port_en */
    reg_mask = (1UL << (port_no + UART_PORT_ID_OFF));
    (void)lp_subsys_reg_set_bits(UART_PORT_ENABLE_REG_OFFSET, reg_mask, 1);

    return 0;
}

static s32 uart_disconnect(u32 uart_no)
{
    u32 reg_mask;

    if (uart_no >= UART_CONNECT_MAX_PORT_NUM) {
        return -EINVAL;
    }

    /* Bit0-7 represents the uart_en */
    reg_mask = (1UL << uart_no);
    lp_subsys_reg_set_bits(UART_PORT_ENABLE_REG_OFFSET, reg_mask, 0);

    return 0;
}

static s32 port_disconnect(u32 port_no)
{
    u32 reg_mask;

    if (port_no >= UART_CONNECT_MAX_PORT_NUM) {
        return -EINVAL;
    }

    /* Bit8-15 represents the port_en */
    reg_mask = (1UL << (port_no + 8));
    (void)lp_subsys_reg_set_bits(UART_PORT_ENABLE_REG_OFFSET, reg_mask, 0);

    return 0;
}

#ifdef UNLINK_BY_LOOP
/* Port set to loop mode */
static s32 port_set_loop(u32 port_no)
{
    return set_port_sel(port_no, port_no + UART_PORT_ID_OFF);
}

/* Uart set to loop mode */
static s32 uart_set_loop(u32 uart_no)
{
    return set_uart_sel(uart_no, uart_no + UART_PORT_ID_OFF);
}
#endif

static s32 port_get_status(u32 port_no)
{
    s32 ret;
    u32 reg_val;

    ret = get_uart_port_enable_reg_val(&reg_val);
    if (ret) {
        return ret;
    }
    /* Bit8-15 represents the port_en */
    if ((1U << (port_no + 8)) & reg_val) {
        return STATUS_ENABLE;
    }

    return STATUS_DISABLE;
}

static s32 uart_get_status(u32 uart_no)
{
    s32 ret;
    u32 reg_val = 0;

    ret = get_uart_port_enable_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    if ((1U << uart_no) & reg_val) {
        return STATUS_ENABLE;
    }

    return STATUS_DISABLE;
}

static s32 port_set_enable(u32 port_no, u32 enable)
{
    s32 ret;
    u32 reg_val = 0;
    u32 mask;

    ret = get_uart_port_enable_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    /* Bit [15:8] represents port enable bits */
    mask = (1U << (port_no + UART_PORT_ID_OFF));
    if (enable) {
        reg_val |= mask;
    } else {
        reg_val &= ~mask;
    }

    return set_uart_port_enable_reg_val(reg_val);
}

static s32 uart_set_enable(u32 uart_no, u32 enable)
{
    s32 ret;
    u32 reg_val = 0;
    u32 mask;

    ret = get_uart_port_enable_reg_val(&reg_val);
    if (ret) {
        return ret;
    }

    mask = (1U << uart_no);
    /* Bit [7:0] represents uart enable bits */
    if (enable) {
        reg_val |= mask;
    } else {
        reg_val &= ~mask;
    }

    return set_uart_port_enable_reg_val(reg_val);
}

/* Unlink old connected uart or port of the uart specified by uart_no */
static s32 uart_unlink(u32 uart_no)
{
    s32 ret;
    u32 old_uart_sel;

    ret = get_uart_sel(uart_no, &old_uart_sel);
    if (ret) {
        return ret;
    }

    /*
     * If old_uart_sel < 8, uart is connected to port
     * If old_uart_sel >= 8, uart is connected to uart
     */
    if (old_uart_sel < UART_PORT_ID_OFF) {
        /* unlink old connected port, disable port */
        ret = port_set_enable(old_uart_sel, 0);
    } else {
        /* unlink old connected uart, disable uart */
        ret = uart_set_enable(old_uart_sel - UART_PORT_ID_OFF, 0);
    }

    return ret;
}

static s32 port_unlink(u32 port_no)
{
    s32 ret;
    u32 old_port_sel;

    ret = get_port_sel(port_no, &old_port_sel);
    if (ret) {
        return ret;
    }

    /*
    * If old_uart_sel < 8, port is connected to uart
    * If old_uart_sel >= 8, port is connected to port
    */
    if (old_port_sel < UART_PORT_ID_OFF) {
        /* unlink old connected uart, disable uart */
        ret = uart_set_enable(old_port_sel, 0);
    } else {
        /* unlink old connected port, disable port */
        ret = port_set_enable(old_port_sel - UART_PORT_ID_OFF, 0);
    }

    return ret;
}

static int uart_check_status(u32 uart_no)
{
    s32 ret;

    if (uart_no >= UART_CONNECT_MAX_UART_NUM) {
        LOG(LOG_ERROR, "uart_no(%u) out of range", uart_no);
        return -EINVAL;
    }

    if (uart_get_status(uart_no)) {
        ret = uart_disconnect(uart_no);
        if (ret) {
            return -EIO;
        }

        ret = uart_unlink(uart_no);
        if (ret) {
            return -EIO;
        }
    }

    return 0;
}

static int port_check_status(u32 port_no)
{
    s32 ret;

    if (port_no >= UART_CONNECT_MAX_PORT_NUM) {
        LOG(LOG_ERROR, "port_no(%u) out of range", port_no);
        return -EINVAL;
    }

    if (port_get_status(port_no)) {
        ret = port_disconnect(port_no);
        if (ret) {
            return -EIO;
        }

        ret = port_unlink(port_no);
        if (ret) {
            return -EIO;
        }
    }

    return 0;
}

static s32 uart2uart_connect(u32 src, u32 dst)
{
    s32 ret;

    /* 1.First check uart status both src and dst uart
       2.Find and unlink the original connected uart of uart src and dst
     */
    ret = uart_check_status(src);
    if (ret) {
        return ret;
    }

    ret = uart_check_status(dst);
    if (ret) {
        return ret;
    }

    ret = set_uart_sel(src, dst + UART_PORT_ID_OFF);  /* src_sel set to dst + 8 */
    ret += set_uart_sel(dst, src + UART_PORT_ID_OFF); /* dst_sel set to src + 8 */
    if (ret) {
        return -EIO;
    }

    /* Connect uart pins */
    ret = uart_connect(src);
    ret += uart_connect(dst);
    if (ret) {
        return -EIO;
    }

    return 0;
}

static s32 uart2port_connect(u32 src, u32 dst)
{
    s32 ret;

    /* 1.First check uart/port status both src and dst uart
       2.Find and unlink the original connected port/uart of uart src and dst
     */
    ret = uart_check_status(src);
    if (ret) {
        return ret;
    }

    ret = port_check_status(dst);
    if (ret) {
        return ret;
    }

    /* src_sel set to dst(port) */
    /* dst_sel set to src(uart) */
    ret = set_uart_sel(src, dst);
    ret += set_port_sel(dst, src);
    if (ret) {
        return -EIO;
    }

    /* connect both src and dst pins */
    ret = uart_connect(src);
    ret += port_connect(dst);
    if (ret) {
        return -EIO;
    }

    return 0;
}

static s32 port2port_connect(u32 src, u32 dst)
{
    s32 ret;

    /* 1.First check port status, both src and dst
       2.Find and unlink the original connected port of port src and dst
     */
    ret = port_check_status(src);
    if (ret) {
        return ret;
    }

    ret = port_check_status(dst);
    if (ret) {
        return ret;
    }

    ret = set_port_sel(src, dst + UART_PORT_ID_OFF);  /* src_sel set to dst(port) + 8 */
    ret += set_port_sel(dst, src + UART_PORT_ID_OFF); /* dst_sel set to src(port) + 8 */
    if (ret) {
        return -EIO;
    }

    /* connect both src and dst pins */
    ret = port_connect(src);
    ret += port_connect(dst);
    if (ret) {
        return -EIO;
    }

    return 0;
}

static void dump_uart_connect_info(u8 *str)
{
    u32 ctrl0, ctrl1, status;
    s32 ret;

    ret = get_port_sel_reg_val(&ctrl0);
    if (ret) {
        return;
    }

    ret = get_uart_sel_reg_val(&ctrl1);
    if (ret) {
        return;
    }

    ret = get_uart_port_enable_reg_val(&status);
    if (ret) {
        return;
    }

    LOG(LOG_PRINT, "%s uart connect info ctrl0: 0x%x, ctrl1: 0x%x, status: 0x%x", str, ctrl0, ctrl1, status);

    return;
}

static s32 set_uart_connect_cfg(struct uart_connect_cfg *cfg)
{
    s32 ret;

    dump_uart_connect_info("before config");
    LOG(LOG_PRINT, "mode:%u, src:%u, dst:%u", cfg->mode, cfg->src, cfg->dst);

    switch (cfg->mode) {
        case UART_2_UART:
            ret = uart2uart_connect(cfg->src, cfg->dst);
            break;
        case UART_2_PORT:
            ret = uart2port_connect(cfg->src, cfg->dst);
            break;
        case PORT_2_PORT:
            ret = port2port_connect(cfg->src, cfg->dst);
            break;
        case UART_DISCONNECT:
            ret = uart_disconnect(cfg->src);
            break;
        case PORT_DISCONNECT:
            ret = port_disconnect(cfg->src);
            break;
        default:
            ret = -EINVAL;
            break;
    }

    dump_uart_connect_info("after config");

    return ret;
}

static s32 uart_connect_reset(void)
{
    s32 ret;

    ret = set_uart_sel_reg_val(UART_CONNECT_UART_SEL_REG_RST_VAL);
    ret += set_port_sel_reg_val(UART_CONNECT_PORT_SEL_REG_RST_VAL);
    if (ret) {
        return -EIO;
    }

    /* Enable all port & uart */
    ret = set_uart_port_enable_reg_val(UART_CONNECT_ENABLE_REG_RST_VAL);
    if (ret) {
        return -EIO;
    }

    return 0;
}

static s32 uart_connect_open(struct inode *pnode, struct file *pfile)
{
    if (g_uart_connector.cdev != NULL) {
        pfile->private_data = g_uart_connector.cdev;
    }

    return 0;
}

static long uart_connect_ioctl(struct file *pfile, u32 cmd, unsigned long arg)
{
    s32 ret;
    struct uart_connect_cfg cfg = { 0 };
    u32 reg_val = 0;
    unsigned long irqflags = 0;

    switch (cmd) {
        case SET_UART_CONNECT:
            if (copy_from_user(&cfg, (void *)arg, sizeof(struct uart_connect_cfg))) {
                LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "set_uart_connect copy_from_user err");
                return -EFAULT;
            }

            spin_lock_irqsave(&g_uart_connector.lock, irqflags);
            ret = set_uart_connect_cfg(&cfg);
            spin_unlock_irqrestore(&g_uart_connector.lock, irqflags);
            break;
        case GET_UART_CONNECT_INFO:
            (void)get_uart_sel_reg_val(&reg_val);
            ret = copy_to_user((void *)arg, &reg_val, sizeof(reg_val));
            break;
        case GET_PORT_CONNECT_INFO:
            (void)get_port_sel_reg_val(&reg_val);
            ret = copy_to_user((void *)arg, &reg_val, sizeof(reg_val));
            break;
        case GET_HOST_BAUDRATE_INFO:
            ret = host_uart_get_info(arg);
            break;
        case RESET_UART_CONNECT:
            spin_lock_irqsave(&g_uart_connector.lock, irqflags);
            ret = uart_connect_reset();
            spin_unlock_irqrestore(&g_uart_connector.lock, irqflags);
            break;
        default:
            LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "ioctl: param cmd(0x%x) invalid\n", cmd);
            return -EINVAL;
    }

    /*
     * Since GET_HOST_BAUDRATE_INFO may be called repeatedly
     * before host initializes host uart
     * We don't print the error when cmd is GET_HOST_BAUDRATE_INFO
     */
    if (ret && (cmd != GET_HOST_BAUDRATE_INFO)) {
        LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "ioctl cmd(0x%x) error %d\n", cmd, ret);
    }

    return ret;
}

static s32 uart_connect_release(struct inode *pnode, struct file *pfile)
{
    return 0;
}

const struct file_operations g_uart_connect_fops = {
    .owner = THIS_MODULE,
    .open = uart_connect_open,
    .release = uart_connect_release,
    .unlocked_ioctl = uart_connect_ioctl
};

static s32 uart_espi_inused(void)
{
    s32 pch_proto = bdinfo_get_host_pchproto();

    return (pch_proto == BDINFO_HOST_PCHPROTO_ESPI) ? 1 : 0;
}

static s32 __init uart_connect_init(void)
{
    s32 ret;
    const char *cdev_name = NULL;

    /* Uart connector will use lp_subsys module,check if lp_subsys module is ready */
    if (!lp_subsys_ready()) {
        /* Since comm_drv.ko is inserted at first, this condition means that system has a critical error */
        LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "lp_subsys not initialized");
        return -ENODEV;
    }

    g_uart_connector.cdev = (cdev_st *)kzalloc(sizeof(cdev_st), GFP_KERNEL);
    if (g_uart_connector.cdev == NULL) {
        LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "Failed to alloc g_uart_connector.cdev");
        return -ENOMEM;
    }

    ret = strncpy_s(g_uart_connector.cdev_name, UART_CONNECT_DEV_NAME_LEN,
                    "uart_connect", UART_CONNECT_DEV_NAME_LEN - 1);
    if (ret) {
        LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "copy uart cdev_name error - %d", ret);
        goto ERR_COMM_INIT_DEV;
    }

    cdev_name = g_uart_connector.cdev_name;
    ret = comm_init_dev(g_uart_connector.cdev, &g_uart_connect_fops, cdev_name);
    if (ret) {
        LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "Failed to init dev %d", ret);
        goto ERR_COMM_INIT_DEV;
    }

    LOG(LOG_INFO, "Host_uart use %s!", uart_espi_inused() ? "eSPI" : "LPC");

    if (uart_espi_inused()) {
        g_uart_connector.host_uart_baudrate_reg = ioremap(HOST_UART_ESPI_BAUD_REG, HOST_UART_REG_SIZE);
    } else {
        g_uart_connector.host_uart_baudrate_reg = ioremap(HOST_UART_LPC_BAUD_REG, HOST_UART_REG_SIZE);
    }

    if (g_uart_connector.host_uart_baudrate_reg == NULL) {
        LOG(LOG_ERROR, UART_CONNECTOR_MODULE_NAME "Failed to init c80 baud reg access");
        goto ERR_REG_MAP;
    }

    spin_lock_init(&g_uart_connector.lock);
    LOG(LOG_INFO, UART_CONNECTOR_MODULE_NAME "uart_connect_init success");

    return 0;
ERR_REG_MAP:
    comm_cleanup_dev(g_uart_connector.cdev);
ERR_COMM_INIT_DEV:
    kfree(g_uart_connector.cdev);
    g_uart_connector.cdev = NULL;
    return ret;
}

static void __exit uart_connect_exit(void)
{
    if (g_uart_connector.host_uart_baudrate_reg) {
        iounmap ((void __iomem *)(g_uart_connector.host_uart_baudrate_reg));
    }

    if (g_uart_connector.cdev) {
        comm_cleanup_dev(g_uart_connector.cdev);
        kfree(g_uart_connector.cdev);
        g_uart_connector.cdev = NULL;
    }

    LOG(LOG_INFO, UART_CONNECTOR_MODULE_NAME "uart_connect_exit success!");

    return;
}

module_init(uart_connect_init);
module_exit(uart_connect_exit);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("uart connect driver");