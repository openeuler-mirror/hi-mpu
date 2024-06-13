// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * peci module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <linux/clkdev.h>
#else
#include <asm/clkdev.h>
#endif

#include <asm/uaccess.h>
#include <asm/io.h>

#include "peci_csr_addr_define.h"
#include "comm_knl_api.h"
#include "peci_usr_api.h"
#include "peci.h"

static peci_mgr_s *g_peci_mgr = NULL;

/* Configure the unidirectional or bidirectional mode, 0 is the dual-wire mode, 1 is the single-wire mode. */
static u32 g_is_single = 1;
module_param(g_is_single, uint, S_IRUGO);

static s32 peci_err_type_check(u_peci_ctrl peci_ctrl)
{
    u32 reg_value;

    if (peci_ctrl.bits.cfg_err || peci_ctrl.bits.tbit_err) {
        if (peci_ctrl.bits.tbit_err) {
            LOG(LOG_ERROR, "peci tBIT-A < tBIT-M error.");
        }

        /* If the write length is less than 3, an error interrupt is reported. */
        reg_value = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_WR_LENGTH_OFFSET_ADDR);
        if (reg_value < PECI_MIN_WR_LEN) {
            LOG(LOG_ERROR, "peci write lenth error. wr_lenth = %u", reg_value);
        }

        /* An error interrupt is reported when the read length is 0. */
        reg_value = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_RD_LENGTH_OFFSET_ADDR);
        if (reg_value == 0) {
            LOG(LOG_ERROR, "peci read lenth error. rd_lenth = 0");
        }

        /* An error interrupt is reported when the initial negotiated rate is not between 12 and 12500. */
        reg_value = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_HOST_SPEED_OFFSET_ADDR);
        if (reg_value < PECI_MIN_SPEED || reg_value > PECI_MAX_SPEED) {
            LOG(LOG_ERROR, "peci set initial rate error. rate = %u", reg_value);
        }

        reg_value = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_NEGO_SPEED_OFFSET_ADDR);
        if (reg_value < PECI_MIN_SPEED || reg_value > PECI_MAX_SPEED) {
            LOG(LOG_ERROR, "peci communication rate error. rate = %u", reg_value);
        }

        return -ECOMM;
    }

    return 0;
}

static void peci_irq_enable(bool enable)
{
    u_peci_int_en peci_int_en;

    peci_int_en.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_INT_EN_OFFSET_ADDR);
    peci_int_en.bits.normal_irq_en = enable;
    peci_int_en.bits.cfg_err_irq_en = enable;
    peci_int_en.bits.tbit_err_irq_en = enable;
    peci_int_en.bits.peci_irq_en = enable;
    iowrite32(peci_int_en.val, g_peci_mgr->peci_map_addr + PECI_CSR_INT_EN_OFFSET_ADDR);

    return;
}

static void peci_trans_abort(void)
{
    u_peci_ctrl peci_ctrl;

    peci_ctrl.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);
    peci_ctrl.bits.abort = 0x1;
    iowrite32(peci_ctrl.val, g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);

    return;
}

static s32 peci_bus_ctrl_bit_check(u32 mask)
{
    u32 i;
    u_peci_ctrl peci_ctrl;

    /* Check whether the value of the PECI control register matches the mask. Wait for a maximum of 1s.. */
    for (i = 0; i < PECI_MAX_LOOP_NUM; i++) {
        peci_ctrl.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);
        if (peci_ctrl.val & mask) {
            return 0;
        }

        msleep(10);
    }

    return -ETIMEDOUT;
}

static s32 peci_check_trans_status(void)
{
    s32 ret;
    u_peci_ctrl peci_ctrl;
    u_peci_ctrl clear_int;

    clear_int.val = 0;

    /* Check whether the transmission is complete. */
    ret = peci_bus_ctrl_bit_check(PECI_INT_STATE_MASK);
    if (ret) {
        LOG(LOG_ERROR, "peci trans timeout ");

        peci_trans_abort();
        return -ETIMEDOUT;
    }

    peci_ctrl.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);

    clear_int.bits.int0 = 0x1;
    iowrite32(clear_int.val, g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);

    if (peci_err_type_check(peci_ctrl)) {
        peci_trans_abort();

        LOG(LOG_ERROR, "peci trans fail(0x%x).", peci_ctrl.val);
        return -EFAULT;
    }

    return 0;
}

static s32 peci_trans_data(PECI_CMD_S *peci_cmd)
{
    u32 i;
    s32 ret;
    u32 *data_ptr = NULL;
    u8 rd_data[PECI_MAX_DATA_NUM];
    u_peci_ctrl peci_ctrl;

    if ((peci_cmd->reg_wr_len > PECI_MAX_DATA_NUM) || (peci_cmd->reg_rd_len > PECI_MAX_DATA_NUM) ||
        (peci_cmd->reg_wr_len == 0) || (peci_cmd->reg_rd_len == 0)) {
        LOG(LOG_ERROR, "peci wr_len(0x%x) or rd_len(0x%x) is error.", peci_cmd->reg_wr_len, peci_cmd->reg_rd_len);
        return -EINVAL;
    }

    ret = peci_bus_ctrl_bit_check(PECI_BUS_READY_MASK);
    if (ret) {
        LOG(LOG_ERROR, "peci bus is busy!");

        peci_trans_abort();
        return -EBUSY;
    }

    iowrite32(peci_cmd->reg_wr_len, g_peci_mgr->peci_map_addr + PECI_CSR_WR_LENGTH_OFFSET_ADDR);
    iowrite32(peci_cmd->reg_rd_len, g_peci_mgr->peci_map_addr + PECI_CSR_RD_LENGTH_OFFSET_ADDR);

    data_ptr = (u32 *)&peci_cmd->wr_data[0];
    for (i = 0; i < peci_cmd->reg_wr_len; i += sizeof(u32)) {
        iowrite32(*data_ptr, g_peci_mgr->peci_map_addr + (PECI_CSR_WR_DAT0_OFFSET_ADDR + i));
        data_ptr++;
    }

    peci_ctrl.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);
    peci_ctrl.bits.start = 0x1;
    iowrite32(peci_ctrl.val, g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);

    ret = peci_check_trans_status();
    if (ret) {
        LOG(LOG_ERROR, "peci trans data error(%d)!", ret);
        return ret;
    }

    /* If the transfer succeeds, the data is read from the register. */
    data_ptr = (u32 *)&rd_data[0];
    for (i = 0; i < peci_cmd->reg_rd_len; i += sizeof(u32)) {
        *data_ptr = ioread32(g_peci_mgr->peci_map_addr + (PECI_CSR_RD_DAT0_OFFSET_ADDR + i));
        data_ptr++;
    }

    /* The byte sequence needs to be changed based on the chip interface. */
    for (i = 0; i < peci_cmd->reg_rd_len; i++) {
        peci_cmd->rd_data[i] = rd_data[(peci_cmd->reg_rd_len - i) - 1];
    }

    return 0;
}

static void peci_hw_init(void)
{
    u_peci_ctrl peci_ctrl;
    u_host_speed host_speed;

    host_speed.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_HOST_SPEED_OFFSET_ADDR);
    host_speed.bits.typ_num = PECI_SPEED;
    iowrite32(host_speed.val, g_peci_mgr->peci_map_addr + PECI_CSR_HOST_SPEED_OFFSET_ADDR);

    peci_ctrl.val = ioread32(g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);
    peci_ctrl.bits.int0 = 0x1;
    iowrite32(peci_ctrl.val, g_peci_mgr->peci_map_addr + PECI_CSR_CTRL_OFFSET_ADDR);

    peci_irq_enable(0x0);

    return;
}

static s32 peci_cmd_trans_data(u32 cmd, unsigned long arg)
{
    s32 ret;
    PECI_CMD_S peci_cmd;

    if (copy_from_user(&peci_cmd, (PECI_CMD_S *)(uintptr_t)arg, sizeof(PECI_CMD_S))) {
        LOG(LOG_ERROR, "peci cmd(0x%x) copy_from_user failed.", cmd);
        return -EFAULT;
    }

    ret = peci_trans_data(&peci_cmd);
    if (ret) {
        LOG(LOG_ERROR, "peci_trans_data err(0x%x).", ret);
        return ret;
    }

    if (copy_to_user((PECI_CMD_S *)(uintptr_t)arg, &peci_cmd, sizeof(PECI_CMD_S))) {
        LOG(LOG_ERROR, "peci cmd(0x%x) copy_to_user failed", cmd);
        return -EFAULT;
    }

    return 0;
}

static long peci_ioctl(struct file *file, u32 cmd, unsigned long arg)
{
    long ret;

    /* Command word validity check */
    ret = comm_ioc_parm_check(cmd, (void __user *)arg, PECI_IOC_MAGIC, PECI_CMD_MAX_NR);
    if (ret) {
        LOG(LOG_ERROR, "peci cmd(0x%x) ioc parm check failed(0x%x).", cmd, ret);
        return ret;
    }

    ret = down_interruptible(&g_peci_mgr->peci_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "peci cmd(0x%x) down_interruptible err(0x%x).", cmd, ret);
        return ret;
    }

    switch (cmd) {
        /* Message Transfer Command */
        case PECI_CMD_TRANS:
            ret = peci_cmd_trans_data(cmd, arg);
            break;

        /* Message Termination Command */
        case PECI_CMD_ABORT:
            peci_trans_abort();
            break;

        default:
            LOG(LOG_ERROR, "peci cmd(0x%x) isn't support.", cmd);
            ret = -EFAULT;
    }

    up(&g_peci_mgr->peci_access_sem);

    return ret;
}

static const struct file_operations g_peci_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = peci_ioctl
};

static s32 peci_probe(struct platform_device *pdev)
{
    s32 ret;
    u32 dts_reg_base_index;
    struct device_node *node = pdev->dev.of_node;

    g_peci_mgr = (peci_mgr_s *)kzalloc(sizeof(peci_mgr_s), GFP_KERNEL);
    if (g_peci_mgr == NULL) {
        LOG(LOG_ERROR, "g_peci_mgr kmalloc failed!");
        return -ENOMEM;
    }

    ret = comm_init_dev(&g_peci_mgr->peci_dev, &g_peci_fops, PECI_DEV_NAME);
    if (ret) {
        LOG(LOG_ERROR, "peci_dev comm_init_dev failed!");
        goto DEV_INIT_ERR;
    }

    /* Obtains the start address sequence number of a register in the DTS. */
    dts_reg_base_index = g_is_single ? 0 : 1;

    LOG(LOG_PRINT, "peci is %u mode!", g_is_single);

    /* Mapping between physical addresses and virtual addresses */
    g_peci_mgr->peci_map_addr = of_iomap(node, dts_reg_base_index);
    if (g_peci_mgr->peci_map_addr == NULL) {
        LOG(LOG_ERROR, "peci_map_addr ioremap error!");
        goto IOMAP_ERR;
    }

    /* Initialize the PECI hardware. */
    peci_hw_init();

    init_mutex(&g_peci_mgr->peci_access_sem);

    LOG(LOG_ERROR, "peci init ok!");

    return 0;

IOMAP_ERR:
    comm_cleanup_dev(&g_peci_mgr->peci_dev);

DEV_INIT_ERR:
    kfree(g_peci_mgr);
    g_peci_mgr = NULL;

    return -1;
}

static s32 peci_remove(struct platform_device *pdev)
{
    if (g_peci_mgr) {
        iounmap(g_peci_mgr->peci_map_addr);
        g_peci_mgr->peci_map_addr = NULL;

        if (&g_peci_mgr->peci_dev != NULL) {
            comm_cleanup_dev(&g_peci_mgr->peci_dev);
        }

        kfree(g_peci_mgr);
        g_peci_mgr = NULL;
    }

    return 0;
}

static const struct of_device_id g_peci_match[] = {
    { .compatible = "hisilicon, hi1711-peci" }, {}
};

static struct platform_driver g_peci_driver = {
    .driver = {
        .name = DEV_PECI_NAME,
        .of_match_table = g_peci_match,
    },

    .probe = peci_probe,
    .remove = peci_remove,
};
module_platform_driver(g_peci_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("peci driver");