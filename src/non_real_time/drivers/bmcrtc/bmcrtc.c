// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * BMCRTC acquire clock interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm_knl_api.h"
#include "bmcrtc_usr_api.h"
#include "bmcrtc.h"

static bmcrtc_info_s *g_bmcrtc = NULL;

static s32 bmcrtc_open(struct inode *inodes, struct file *filp)
{
    return 0;
}

static s32 bmcrtc_release(struct inode *inodes, struct file *filp)
{
    return 0;
}

/**
 * @brief: RTC read operation is used to acquire time
 * @param: unsigned long arg: command word parameters
 * @retval: 0 is success, other is failure
 */
static s32 bmcrtc_read(unsigned long arg)
{
    unsigned long irq_flags = 0;
    bmcrtc_time_s cur_time;

    spin_lock_irqsave(&g_bmcrtc->lock, irq_flags);
    cur_time.nanoseconds = readl(g_bmcrtc->virt_base + RTC_NANO_SECOND_CFG);
    cur_time.seconds = readl(g_bmcrtc->virt_base + RTC_SECOND_CFG);
    spin_unlock_irqrestore(&g_bmcrtc->lock, irq_flags);

    if (copy_to_user((s32 __user *)arg, &cur_time, sizeof(bmcrtc_time_s))) {
        LOG(LOG_ERROR, "copy to user fail");
        return -EFAULT;
    }

    return 0;
}

/**
 * @brief: RTC write operation is used to modify time
 * @param: unsigned long arg: command word parameters
 * @retval: 0 is success, other is failure
 */
static s32 bmcrtc_write(unsigned long arg)
{
    bmcrtc_time_s cur_time;
    unsigned long irq_flags = 0;

    if (copy_from_user(&cur_time, (s32 __user *)arg, sizeof(bmcrtc_time_s))) {
        LOG(LOG_ERROR, "copy from user fail");
        return -EFAULT;
    }

    if (cur_time.nanoseconds >= TOP_LIMIT) {
        LOG(LOG_ERROR, "cur_time.nanoseconds must lower than 100000000");
        return -EINVAL;
    }
    spin_lock_irqsave(&g_bmcrtc->lock, irq_flags);
    writel(cur_time.seconds, g_bmcrtc->virt_base + RTC_SECOND_CFG);
    writel(cur_time.nanoseconds, g_bmcrtc->virt_base + RTC_NANO_SECOND_CFG);
    spin_unlock_irqrestore(&g_bmcrtc->lock, irq_flags);

    return 0;
}

/**
 * @brief: RTC enable or disable
 * @param: u32 flag: 0 is disable, other is enable
 * @retval: void
 */
static void bmcrtc_enable_disable(u32 flag)
{
    unsigned long irq_flags = 0;

    spin_lock_irqsave(&g_bmcrtc->lock, irq_flags);
    if (flag) {
        writel(RTC_ENABLE, g_bmcrtc->virt_base + RTC_SWITCH);
    } else {
        writel(RTC_DISABLE, g_bmcrtc->virt_base + RTC_SWITCH);
    }
    spin_unlock_irqrestore(&g_bmcrtc->lock, irq_flags);

    return;
}

static long bmcrtc_ioctl(struct file *files, u32 cmd, unsigned long arg)
{
    s32 ret;

    ret = comm_ioc_parm_check(cmd, (void __user *)arg, RTC_IOC_MAGIC, RTC_IOC_MAXNR);
    if (ret) {
        LOG(LOG_ERROR, "bmcrtc cmd(0x%x) ioc parm check failed!", cmd);
        return -EFAULT;
    }

    switch (cmd) {
        case RTC_IOCIG_ENABLE:
            bmcrtc_enable_disable(RTC_ENABLE);
            break;
        case RTC_IOCIG_DISABLE:
            bmcrtc_enable_disable(RTC_DISABLE);
            break;
        case RTC_GET_TIME:
            ret = bmcrtc_read(arg);
            break;
        case RTC_MOD_TIME:
            ret = bmcrtc_write(arg);
            break;
        default:
            LOG(LOG_ERROR, "cmd error");
            return -ENOTTY;
    }

    return ret;
}

static const struct file_operations g_bmcrtc_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = bmcrtc_ioctl,
    .open = bmcrtc_open,
    .release = bmcrtc_release,
};

s32 bmcrtc_of_probe(struct platform_device *pdev)
{
    s32 ret;
    struct resource *reg_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    if (reg_res == NULL) {
        return -EINVAL;
    }

    g_bmcrtc = devm_kzalloc(&pdev->dev, sizeof(bmcrtc_info_s), GFP_KERNEL);
    if (g_bmcrtc == NULL) {
        LOG(LOG_ERROR, "failed to allocate g_bmcrtc structure");
        return -ENOMEM;
    }

    g_bmcrtc->phys_base = reg_res->start;
    g_bmcrtc->virt_base = devm_ioremap(&pdev->dev, g_bmcrtc->phys_base, resource_size(reg_res));
    if (g_bmcrtc->virt_base == NULL) {
        LOG(LOG_ERROR, "failed to remap virtual address for bmcrtc");
        return -EFAULT;
    }

    ret = comm_init_dev(&g_bmcrtc->chrdev, &g_bmcrtc_fops, BMCRTC_DEV_NAME_KER);
    if (ret) {
        LOG(LOG_ERROR, "register_chrdev for g_bmcrtc failed(%d)", ret);
        return -ENODEV;
    }

    spin_lock_init(&g_bmcrtc->lock);
    platform_set_drvdata(pdev, g_bmcrtc);

    LOG(LOG_PRINT, "BMCRTC Device Registered.");

    return 0;
}

static s32 bmcrtc_of_remove(struct platform_device *pdev)
{
    if (g_bmcrtc) {
        comm_cleanup_dev(&g_bmcrtc->chrdev);
        platform_set_drvdata(pdev, NULL);
        g_bmcrtc = NULL;
    }

    return 0;
}

static const struct of_device_id g_bmcrtc_of_match[] = {
    { .compatible = "hisilicon,bmcrtc" }, {}
};

static struct platform_driver g_bmcrtc_driver = {
    .driver = {
        .name = BMCRTC_DEV_NAME_KER,
        .of_match_table = g_bmcrtc_of_match,
    },

    .probe = bmcrtc_of_probe,
    .remove = bmcrtc_of_remove,
};

module_platform_driver(g_bmcrtc_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Hisilicon GPIO controller driver code");