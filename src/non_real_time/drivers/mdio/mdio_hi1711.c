// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * mdio module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/phy.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include "comm_knl_api.h"
#include "mdio_csr_addr_define.h"
#include "mdio_c_union_define.h"
#include "mdio_usr_api.h"
#include "mdio_hi1711.h"

static cdev_st *g_mdio_dev = NULL;

static void __iomem *mdio_base_addr[MDIO_MAX_DRV_NUM] = { 0 };

static u32 g_mdio_virq[MDIO_MAX_DRV_NUM] = { 0 };

static struct semaphore g_mdio_wr_sem[MDIO_MAX_DRV_NUM];

static s32 mdio_wait_ready(u32 mdio_id)
{
    u32 cnt = MAX_WAIT_CNT;
    u_mdio_command_reg mdio_cmd;

    while (cnt) {
        mdio_cmd.val = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_COMMAND_REG_OFFSET_ADDR);
        if (mdio_cmd.bits.mdio_start == 0) {
            break;
        }

        udelay(10); /* total wait 10 ms, 1000 cycles */
        cnt--;
    };

    if (cnt == 0) {
        return -ETIMEDOUT;
    }

    return 0;
}

static s32 mdio_check_sta(u32 mdio_id)
{
    s32 ret;
    u_mdio_sta_reg mdio_sta;

    ret = mdio_wait_ready(mdio_id);
    if (ret) {
        LOG(LOG_ERROR, "mdio(%u) wait ready timeout!", mdio_id);
        return ret;
    }

    mdio_sta.val = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_STA_REG_OFFSET_ADDR);
    if (mdio_sta.bits.mdio_sta) {
        LOG(LOG_ERROR, "mdio(%u) check sta is err!", mdio_id);
        return -EFAULT;
    }

    return 0;
}

static void mdio_address_config(u32 mdio_id, u32 reg_addr)
{
    u_mdio_addr_reg mdio_addr;

    mdio_addr.val = 0;
    mdio_addr.bits.mdio_address = reg_addr;
    iowrite32(mdio_addr.val, mdio_base_addr[mdio_id] + MDIO_CSR_ADDR_REG_OFFSET_ADDR);

    return;
}

static void mdio_wdata_config(u32 mdio_id, u32 value)
{
    u_mdio_wdata_reg mdio_wdata;

    mdio_wdata.val = 0;
    mdio_wdata.bits.mdio_wdata = value;
    iowrite32(mdio_wdata.val, mdio_base_addr[mdio_id] + MDIO_CSR_WDATA_REG_OFFSET_ADDR);

    return;
}

static void mdio_cmd_send(u32 mdio_id, u32 prt_addr, u32 dev_addr, u32 stype, u32 op_code)
{
    u_mdio_command_reg mdio_cmd;

    mdio_cmd.val = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_COMMAND_REG_OFFSET_ADDR);
    mdio_cmd.bits.mdio_st = stype;
    mdio_cmd.bits.mdio_start = 1;
    mdio_cmd.bits.mdio_op = op_code;
    mdio_cmd.bits.mdio_prtad = prt_addr;
    mdio_cmd.bits.mdio_devad = dev_addr;
    iowrite32(mdio_cmd.val, mdio_base_addr[mdio_id] + MDIO_CSR_COMMAND_REG_OFFSET_ADDR);

    return;
}

s32 mdio_read_22(u32 mdio_id, u32 phy_addr, u32 reg_addr, u32 *val)
{
    u32 rd_data;
    u32 retry_count = 1;
    u_mdio_sta_reg mdio_sta;
    int retry = 0;

    if (!val) {
        LOG(LOG_ERROR, "mdio read22 parm: val is NULL!");
        return -EINVAL;
    }

    if (mdio_id >= MDIO_MAX_DRV_NUM || phy_addr > MDIO_CLUSE22_MAX_PHY_ADDR ||
        reg_addr > MDIO_CLUSE22_MAX_PHY_REG_ADDR) {
        LOG(LOG_ERROR, "mdio read22 parm: mdio_id(%u) phy_addr(0x%x) reg_addr(0x%x) is invalid!",
            mdio_id, phy_addr, reg_addr);
        return -EINVAL;
    }

    if (down_interruptible(&g_mdio_wr_sem[mdio_id])) {
        LOG(LOG_ERROR, "mdio(%u) read22 down_interruptible failed!", mdio_id);
        return -EBUSY;
    }

    if (mdio_wait_ready(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) read22 is busy!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EBUSY;
    }

    do {
        retry = 0;
        mdio_cmd_send(mdio_id, phy_addr, reg_addr, MDIO_ST_22, MDIO_ST22_READ);

        if (mdio_check_sta(mdio_id)) {
            mdio_sta.val = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_STA_REG_OFFSET_ADDR);
            if ((retry_count != 0) && (mdio_sta.bits.mdio_sta)) {
                retry_count--;
                retry = 1;
            } else {
                LOG(LOG_ERROR, "mdio(%u) read22 check error!", mdio_id);
                up(&g_mdio_wr_sem[mdio_id]);
                return -EFAULT;
            }
        }
    } while (retry != 0);
    
    rd_data = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_RDATA_REG_OFFSET_ADDR);
    *val = rd_data & 0xffff;

    up(&g_mdio_wr_sem[mdio_id]);

    return 0;
}
EXPORT_SYMBOL(mdio_read_22);

s32 mdio_write_22(u32 mdio_id, u32 phy_addr, u32 reg_addr, u32 value)
{
    u_mdio_sta_reg mdio_sta;
    u32 retry_count = 1;
    int retry = 0;

    if (mdio_id >= MDIO_MAX_DRV_NUM || phy_addr > MDIO_CLUSE22_MAX_PHY_ADDR ||
        reg_addr > MDIO_CLUSE22_MAX_PHY_REG_ADDR || value > MDIO_MAX_WRITE_VALUE) {
        LOG(LOG_ERROR, "mdio write22 parm: mdio_id(%u) phy_addr(0x%x) reg_addr(0x%x) value(0x%x) is invalid!",
            mdio_id, phy_addr, reg_addr, value);
        return -EINVAL;
    }

    if (down_interruptible(&g_mdio_wr_sem[mdio_id])) {
        LOG(LOG_ERROR, "mdio(%u) write22 down_interruptible failed!", mdio_id);
        return -EBUSY;
    }

    if (mdio_wait_ready(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) write22 is busy!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EBUSY;
    }

    do {
        retry = 0;
        mdio_wdata_config(mdio_id, value);
        mdio_cmd_send(mdio_id, phy_addr, reg_addr, MDIO_ST_22, MDIO_ST22_WRITE);

        if (mdio_check_sta(mdio_id)) {
            mdio_sta.val = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_STA_REG_OFFSET_ADDR);
            if ((retry_count != 0) && (mdio_sta.bits.mdio_sta)) {
                retry_count--;
                retry = 1;
            } else {
                LOG(LOG_ERROR, "mdio(%u) write22 check error!", mdio_id);
                up(&g_mdio_wr_sem[mdio_id]);
                return -EFAULT;
            }
        }
    } while (retry != 0);

    up(&g_mdio_wr_sem[mdio_id]);

    return 0;
}
EXPORT_SYMBOL(mdio_write_22);

s32 mdio_read_45(u32 mdio_id, u32 prt_addr, u32 dev_addr, u32 reg_addr, u32 *value)
{
    u32 rd_data;

    if (!value) {
        LOG(LOG_ERROR, "mdio read45 parm: value is NULL!");
        return -EINVAL;
    }

    if (mdio_id >= MDIO_MAX_DRV_NUM || prt_addr > MDIO_CLUSE45_MAX_PHY_ADDR ||
        dev_addr > MDIO_CLUSE45_MAX_PHY_DEV_ADDR || reg_addr > MDIO_CLUSE45_MAX_PHY_REG_ADDR) {
        LOG(LOG_ERROR, "mdio read45 parm: mdio_id(%u) prt_addr(0x%x) dev_addr(0x%x) reg_addr(0x%x) is invalid",
            mdio_id, prt_addr, dev_addr, reg_addr);
        return -EINVAL;
    }

    if (down_interruptible(&g_mdio_wr_sem[mdio_id])) {
        LOG(LOG_ERROR, "mdio(%u) read45 down_interruptible failed!", mdio_id);
        return -EBUSY;
    }

    if (mdio_wait_ready(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) read45 is busy!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EBUSY;
    }

    mdio_address_config(mdio_id, reg_addr);
    mdio_cmd_send(mdio_id, prt_addr, dev_addr, MDIO_ST_45, MDIO_ST45_WRITE_ADDR);

    if (mdio_check_sta(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) read45 check err!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EFAULT;
    }

    mdio_cmd_send(mdio_id, prt_addr, dev_addr, MDIO_ST_45, MDIO_ST45_READ);
    if (mdio_check_sta(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) read45 check err!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EFAULT;
    }

    rd_data = ioread32(mdio_base_addr[mdio_id] + MDIO_CSR_RDATA_REG_OFFSET_ADDR);
    *value = rd_data & 0xffff;

    up(&g_mdio_wr_sem[mdio_id]);

    return 0;
}
EXPORT_SYMBOL(mdio_read_45);

s32 mdio_write_45(u32 mdio_id, u32 prt_addr, u32 dev_addr, u32 reg_addr, u32 value)
{
    if (mdio_id >= MDIO_MAX_DRV_NUM || prt_addr > MDIO_CLUSE45_MAX_PHY_ADDR || value > MDIO_MAX_WRITE_VALUE ||
        dev_addr > MDIO_CLUSE45_MAX_PHY_DEV_ADDR || reg_addr > MDIO_CLUSE45_MAX_PHY_REG_ADDR) {
        LOG(LOG_ERROR, "mdio write45 parm mdio_id(%u) prt_addr(0x%x) dev_addr(0x%x) reg_addr(0x%x) value(0x%x) invalid",
            mdio_id, prt_addr, dev_addr, reg_addr, value);
        return -EINVAL;
    }

    if (down_interruptible(&g_mdio_wr_sem[mdio_id])) {
        LOG(LOG_ERROR, "mdio(%u) write45 down_interruptible failed!", mdio_id);
        return -EBUSY;
    }

    if (mdio_wait_ready(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) write45 is busy!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EBUSY;
    }

    mdio_address_config(mdio_id, reg_addr);

    mdio_cmd_send(mdio_id, prt_addr, dev_addr, MDIO_ST_45, MDIO_ST45_WRITE_ADDR);

    if (mdio_check_sta(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) write45 check error!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EFAULT;
    }

    mdio_wdata_config(mdio_id, value);

    mdio_cmd_send(mdio_id, prt_addr, dev_addr, MDIO_ST_45, MDIO_ST45_WRITE_DATA);
    if (mdio_check_sta(mdio_id)) {
        LOG(LOG_ERROR, "mdio(%u) write45 check error!", mdio_id);

        up(&g_mdio_wr_sem[mdio_id]);
        return -EFAULT;
    }

    up(&g_mdio_wr_sem[mdio_id]);

    return 0;
}
EXPORT_SYMBOL(mdio_write_45);

static irqreturn_t mdio_isr(s32 irq, void *dev_id)
{
    u_mdio_auto_intr mdio_auto_intr;

    mdio_auto_intr.val = ioread32(mdio_base_addr[0] + MDIO_CSR_AUTO_INTR_OFFSET_ADDR);
    LOG(LOG_ERROR, "gmac(mask:%u) link is changed!", mdio_auto_intr.val);
    iowrite32(mdio_auto_intr.val, mdio_base_addr[0] + MDIO_CSR_AUTO_INTR_OFFSET_ADDR);

    return IRQ_HANDLED;
}

static s32 mdio_init(u32 mdio_id, u32 freq)
{
    u_mdio_command_reg mdio_command;

    if (mdio_id >= MDIO_MAX_DRV_NUM) {
        LOG(LOG_ERROR, "mdio init mdio_id(%u) is invalid!", mdio_id);
        return -EINVAL;
    }

    mdio_command.val = 0;
    mdio_command.bits.mdio_clk_sel_exp = ((freq >> 1) & 0x1);
    mdio_command.bits.mdio_clk_sel = (freq & 0x1);
    mdio_command.bits.mdio_auto_scan = 0;
    iowrite32(mdio_command.val, mdio_base_addr[mdio_id] + MDIO_CSR_COMMAND_REG_OFFSET_ADDR);

    return 0;
}

static s32 mdio_open(struct inode *inodes, struct file *filp)
{
    return 0;
}

static s32 mdio_release(struct inode *inodes, struct file *filp)
{
    return 0;
}

static s32 mdio_ioctl_read(unsigned long arg)
{
    s32 ret;
    u32 value = 0;
    mdio_ioctl_read_s ioctl_read;

    if (copy_from_user(&ioctl_read, (void __user *)(uintptr_t)arg, sizeof(mdio_ioctl_read_s))) {
        LOG(LOG_ERROR, "mdio read copy_from_user failed!");
        return -EFAULT;
    }

    ret = mdio_read_22(ioctl_read.mdio_id, ioctl_read.phy_addr, ioctl_read.reg_addr, &value);
    if (ret) {
        return ret;
    }

    if (copy_to_user((void __user *)(uintptr_t)ioctl_read.value, &value, sizeof(value))) {
        LOG(LOG_ERROR, "mdio read copy_to_user failed!");
        return -EFAULT;
    }

    return 0;
}

static s32 mdio_ioctl_write(unsigned long arg)
{
    mdio_ioctl_write_s ioctl_write;

    if (copy_from_user(&ioctl_write, (void __user *)(uintptr_t)arg, sizeof(mdio_ioctl_write_s))) {
        LOG(LOG_ERROR, "mdio write copy_from_user fail!");
        return -EFAULT;
    }

    return mdio_write_22(ioctl_write.mdio_id, ioctl_write.phy_addr, ioctl_write.reg_addr, ioctl_write.value);
}

static s32 mdio_ioctl_read45(unsigned long arg)
{
    s32 ret;
    mdio_rw45_s mdio_rw45;

    if (copy_from_user(&mdio_rw45, (void __user *)(uintptr_t)arg, sizeof(mdio_rw45_s))) {
        LOG(LOG_ERROR, "mdio read copy_from_user failed!");
        return -EFAULT;
    }

    ret = mdio_read_45(mdio_rw45.mdio_id, mdio_rw45.prt_addr, mdio_rw45.dev_addr, mdio_rw45.reg_addr, &mdio_rw45.value);
    if (ret) {
        return ret;
    }

    if (copy_to_user((void __user *)(uintptr_t)arg, &mdio_rw45, sizeof(mdio_rw45_s))) {
        LOG(LOG_ERROR, "mdio read copy_to_user failed!");
        return -EFAULT;
    }

    return 0;
}

static s32 mdio_ioctl_write45(unsigned long arg)
{
    mdio_rw45_s mdio_rw45;

    if (copy_from_user(&mdio_rw45, (void __user *)(uintptr_t)arg, sizeof(mdio_rw45_s))) {
        LOG(LOG_ERROR, "mdio write45 copy_from_user fail!");
        return -EFAULT;
    }

    return mdio_write_45(mdio_rw45.mdio_id, mdio_rw45.prt_addr,
                         mdio_rw45.dev_addr, mdio_rw45.reg_addr, mdio_rw45.value);
}

static long mdio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    s32 ret;

    switch (cmd) {
        case MDIO_CMD_READ:
            ret = mdio_ioctl_read(arg);
            break;

        case MDIO_CMD_WRITE:
            ret = mdio_ioctl_write(arg);
            break;

        case MDIO_CMD_READ45:
            ret = mdio_ioctl_read45(arg);
            break;

        case MDIO_CMD_WRITE45:
            ret = mdio_ioctl_write45(arg);
            break;

        default:
            LOG(LOG_ERROR, "mdio cmd(0x%x) isn't support!", _IOC_NR(cmd));
            ret = -EINVAL;
    }

    return ret;
}

const struct file_operations g_mdio_fops = {
    .owner = THIS_MODULE,
    .open = mdio_open,
    .release = mdio_release,
    .unlocked_ioctl = mdio_ioctl,
};

static s32 mdio_remove(struct platform_device *pdev)
{
    if (g_mdio_virq[0]) {
        free_irq(g_mdio_virq[0], NULL);
        g_mdio_virq[0] = 0;
    }

    if (mdio_base_addr[0]) {
        iounmap((void *)mdio_base_addr[0]);
        mdio_base_addr[0] = NULL;
    }

    if (g_mdio_dev) {
        comm_cleanup_dev(g_mdio_dev);
        kfree(g_mdio_dev);
        g_mdio_dev = NULL;
    }

    return 0;
}

static s32 mdio_probe(struct platform_device *pdev)
{
    s32 ret;
    struct device_node *node = pdev->dev.of_node;

    g_mdio_dev = (cdev_st *)kzalloc(sizeof(cdev_st), GFP_KERNEL);
    if (!g_mdio_dev) {
        LOG(LOG_ERROR, "alloc mdio dev failed!");
        return -ENOMEM;
    }

    ret = comm_init_dev(g_mdio_dev, &g_mdio_fops, DEV_MDIO_KER_NAME);
    if (ret) {
        LOG(LOG_ERROR, "mdio comm_init_dev failed!");
        goto dev_init_err;
    }

    sema_init(&g_mdio_wr_sem[0], 1);

    mdio_base_addr[0] = of_iomap(node, 0);
    if (!mdio_base_addr[0]) {
        LOG(LOG_ERROR, "mdio iomap failed!");
        goto iomap_err;
    }

    g_mdio_virq[0] = irq_of_parse_and_map(node, 0);
    if (g_mdio_virq[0] == 0) {
        LOG(LOG_ERROR, "irq_of_parse_and_map(0x%x) error", g_mdio_virq[0]);
        goto irq_err;
    }

    ret = request_irq(g_mdio_virq[0], mdio_isr, 0, "mdio", NULL);
    if (ret) {
        LOG(LOG_ERROR, "mdio request_irq(0x%x) failed(0x%x).\n", g_mdio_virq[0], ret);
        goto irq_err;
    }

    ret = mdio_init(0, MDIO_FREQUENCE_1);
    if (ret) {
        LOG(LOG_ERROR, "mdio init failed(0x%x).\n", ret);
        goto init_err;
    }

    return 0;

init_err:
    free_irq(g_mdio_virq[0], NULL);

irq_err:
    iounmap((void *)mdio_base_addr[0]);
    mdio_base_addr[0] = NULL;
    g_mdio_virq[0] = 0;

iomap_err:
    comm_cleanup_dev(g_mdio_dev);

dev_init_err:
    kfree(g_mdio_dev);
    g_mdio_dev = NULL;

    return -EIO;
}

static const struct of_device_id g_mdio_match[] = {
    { .compatible = "hisilicon, hi1711-mdio" }, {}
};

static struct platform_driver g_mdio_driver = {
    .driver = {
        .name = DEV_MDIO_KER_NAME,
        .of_match_table = g_mdio_match,
    },

    .probe = mdio_probe,
    .remove = mdio_remove,
};
module_platform_driver(g_mdio_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("mdio driver");