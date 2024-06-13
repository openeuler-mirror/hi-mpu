// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * dual boot startup information interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "dboot_ker.h"

#include "sysctrl_csr_addr_define.h"
#include "sysctrl.h"
#include "dboot_usr_api.h"

dboot_ctrl_s g_dboot_ctrl;

/* Obtaining the Dual-Boot Driver Status */
static inline s32 dboot_get_boot_status(u32 *boot_status)
{
    return sysctrl_reg_read(SYSCTRL_CSR_DBOOT_STATUS_REG_OFFSET_ADDR, boot_status);
}

/* Obtains the system reset information. (BMC domain, BMC non-security domain, HOST domain, and CHIP domain) */
static s32 dboot_get_rst_cnt(u32 *bmc_rst_cnt, u32 *bmc_unsec_rst_cnt,
                             u32 *host_rst_cnt, u32 *chip_rst_cnt)
{
    s32 ret;
    u32 reg_val = 0;

    ret = sysctrl_reg_read(SYSCTRL_CSR_BMC_RST_RECORD_OFFSET_ADDR, &reg_val);
    if (ret) {
        return ret;
    }
    *bmc_rst_cnt = (reg_val & BMC_RESET_COUNT_MASK) >> BMC_RESET_COUNT_OFFSET;

    ret = sysctrl_reg_read(SYSCTRL_CSR_BMC_UNSE_RST_RECORD_OFFSET_ADDR, &reg_val);
    if (ret) {
        return ret;
    }
    *bmc_unsec_rst_cnt = (reg_val & BMC_UNSEC_RESET_COUNT_MASK) >> BMC_UNSEC_RESET_COUNT_OFFSET;

    ret = sysctrl_reg_read(SYSCTRL_CSR_HS_RST_RECORD_OFFSET_ADDR, &reg_val);
    if (ret) {
        return ret;
    }
    *host_rst_cnt = (reg_val & HOST_RESET_COUNT_MASK) >> HOST_RESET_COUNT_OFFSET;

    ret = sysctrl_reg_read(SYSCTRL_CSR_INNER_CHIP_RST_RECORD_OFFSET_ADDR, &reg_val);
    if (ret) {
        return ret;
    }
    *chip_rst_cnt = (reg_val & CHIP_RESET_COUNT_MASK) >> CHIP_RESET_COUNT_OFFSET;

    return 0;
}

static s32 dboot_get_boot_strap_info(void __user *strap_info)
{
    s32 ret;
    u32 reg_val = 0;
    dboot_strap_info info = { 0 };

    ret = sysctrl_reg_read(SYSCTRL_CSR_STRAP_STATUS_REG_OFFSET_ADDR, &reg_val);
    if (ret) {
        return ret;
    }

    info.jtag_mode = ((reg_val & 0x100UL) >> 8UL);
    info.lb_boot_width = ((reg_val & 0x80UL) >> 7UL);
    info.lb_boot_offset = ((reg_val & 0x40UL) >> 6UL);
    info.sfc_addr_mode = ((reg_val & 0x20UL) >> 5UL);
    info.boot_size = ((reg_val & 0x10UL) >> 4UL);
    info.boot_dbios = ((reg_val & 0x08UL) >> 3UL);
    info.us_boot_core = ((reg_val & 0x04UL) >> 2UL);
    info.boot_mode = (reg_val & 0x3UL);

    if (copy_to_user(strap_info, &info, sizeof(dboot_strap_info))) {
        return -EFAULT;
    }

    return 0;
}

static s32 dboot_get_l0fw_ver(sec_fw_ver *ver)
{
    s32 ret;
    struct device_node *np = g_dboot_ctrl.np_l0fw_ver;
    const char *np_val = NULL;

    ret = of_property_read_string(np, "active", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l0fw active version failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l0fw_active_ver, sizeof(ver->l0fw_active_ver), np_val, SECFW_VER_LEN - 1);

    ret = of_property_read_string(np, "gold", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l0fw gold version failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l0fw_gold_ver, sizeof(ver->l0fw_gold_ver), np_val, SECFW_VER_LEN - 1);

    ret = of_property_read_string(np, "active_build_time", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l0fw active build time failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l0fw_active_build_time, sizeof(ver->l0fw_active_build_time), np_val, SECFW_BUILD_TIME_LEN - 1);

    ret = of_property_read_string(np, "gold_build_time", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l0fw gold build time failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l0fw_gold_build_time, sizeof(ver->l0fw_gold_build_time), np_val, SECFW_BUILD_TIME_LEN - 1);

    return 0;
}

static s32 dboot_get_l1fw_ver(sec_fw_ver *ver)
{
    s32 ret;
    struct device_node *np = g_dboot_ctrl.np_l1fw_ver;
    const char *np_val = NULL;

    ret = of_property_read_string(np, "active", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l1fw active version failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l1fw_active_ver, sizeof(ver->l1fw_active_ver), np_val, SECFW_VER_LEN - 1);

    ret = of_property_read_string(np, "gold", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l1fw gold version failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l1fw_gold_ver, sizeof(ver->l1fw_gold_ver), np_val, SECFW_VER_LEN - 1);

    ret = of_property_read_string(np, "active_build_time", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l1fw active build time failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l1fw_active_build_time, sizeof(ver->l1fw_active_build_time), np_val, SECFW_BUILD_TIME_LEN - 1);

    ret = of_property_read_string(np, "gold_build_time", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read l1fw gold build time failed\n");
        return ret;
    }
    (void)strncpy_s(ver->l1fw_gold_build_time, sizeof(ver->l1fw_gold_build_time), np_val, SECFW_BUILD_TIME_LEN - 1);

    return 0;
}

static s32 dboot_get_sec_fw_ver(void __user *sec_ver)
{
    s32 ret;
    sec_fw_ver ver = { 0 };

    if (sec_ver == NULL) {
        return -EINVAL;
    }

    ret = dboot_get_l0fw_ver(&ver);
    if (ret) {
        LOG(LOG_ERROR, "Get l0 version failed\n");
        return ret;
    }

    ret = dboot_get_l1fw_ver(&ver);
    if (ret) {
        LOG(LOG_ERROR, "Get l1 version failed\n");
        return ret;
    }

    ret = copy_to_user(sec_ver, &ver, sizeof(sec_fw_ver));
    if (ret) {
        LOG(LOG_ERROR, "Copy scfw version failed\n");
        return ret;
    }

    return 0;
}

/* Obtain mtd0 and mtd2, that is, the versions of uboot0 and uboot1. */
static s32 dboot_get_uboot_ver(unsigned long arg, u8 print)
{
    s32 ret;
    const char *np_val = NULL;
    uboot_ver uboot_ver_info;
    struct device_node *np = g_dboot_ctrl.np_uboot_ver;

    if (!np) {
        LOG(LOG_ERROR, "no device node for np_uboot_ver\n");
        return -ENODEV;
    }

    ret = of_property_read_string(np, "active", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read uboot active version failed\n");
        return ret;
    }
    (void)strncpy_s(uboot_ver_info.uboot0_ver, sizeof(uboot_ver_info.uboot0_ver), np_val, VER_DATA_LEN - 1);

    ret = of_property_read_string(np, "active_build_time", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read uboot active build time failed\n");
        return ret;
    }
    strncpy_s(uboot_ver_info.uboot0_build_time, sizeof(uboot_ver_info.uboot0_build_time), np_val, VER_DATA_LEN - 1);

    ret = of_property_read_string(np, "gold", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read uboot gold version failed\n");
        return ret;
    }
    (void)strncpy_s(uboot_ver_info.uboot1_ver, sizeof(uboot_ver_info.uboot1_ver), np_val, VER_DATA_LEN - 1);

    ret = of_property_read_string(np, "gold_build_time", &np_val);
    if (ret) {
        LOG(LOG_ERROR, "Read uboot gold build time failed\n");
        return ret;
    }
    strncpy_s(uboot_ver_info.uboot1_build_time, sizeof(uboot_ver_info.uboot1_build_time), np_val, VER_DATA_LEN - 1);

    if (print) {
        LOG(LOG_PRINT, "U-Boot0 Version: %s Built: %s", uboot_ver_info.uboot0_ver, uboot_ver_info.uboot0_build_time);
        LOG(LOG_PRINT, "U-Boot1 Version: %s Built: %s", uboot_ver_info.uboot1_ver, uboot_ver_info.uboot1_build_time);
    } else if (arg) {
        if (copy_to_user((void __user *)(uintptr_t)arg, &uboot_ver_info, sizeof(uboot_ver_info))) {
            LOG(LOG_ERROR, "copy_to_user err");
            return -EFAULT;
        }
    }

    return 0;
}

/* Handlen hook attached to the I/O layer */
s32 dboot_open(struct inode *pnode, struct file *pfile)
{
    if (g_dboot_ctrl.cdev != NULL) {
        pfile->private_data = g_dboot_ctrl.cdev;
    }

    return 0;
}

/* Close hook attached to the I/O layer */
s32 dboot_release(struct inode *pnode, struct file *pfile)
{
    pfile->private_data = NULL;
    return 0;
}

/* Ioctl hook attached to the I/O layer */
static long dboot_ioctl(struct file *filp, u32 cmd, unsigned long arg)
{
    s32 ret = 0;
    u32 dboot_status = 0;
    dboot_info_s dboot_info;

    /* Command word validity check */
    ret = comm_ioc_parm_check(cmd, (void __user *)arg, DBOOT_CMD_MAGIC, DBOOT_CMD_MAX_NR);
    if (ret) {
        LOG(LOG_ERROR, "dboot cmd(0x%x) ioc parm check failed(0x%x).\n", cmd, ret);
        return ret;
    }

    ret = down_interruptible(&g_dboot_ctrl.sema);
    if (ret) {
        LOG(LOG_ERROR, "call down_interruptible error!");
        return ret;
    }

    switch (cmd) {
        case DBOOT_CMD_GET_INFO:
            ret = dboot_get_boot_status(&dboot_status);
            if (ret) {
                LOG(LOG_ERROR, "ioctl: dboot_get_boot_status err %d\n", ret);
                break;
            }

            dboot_info.cur_no = dboot_status & 0x1; /* Obtains the current boot region. */
            dboot_info.boot0_cnt = (dboot_status >> BOOT_CNT0_SHIFT) & GET_BOOT_CNT_OPS;
            dboot_info.boot1_cnt = (dboot_status >> BOOT_CNT1_SHIFT) & GET_BOOT_CNT_OPS;
            if (copy_to_user((void *)arg, &dboot_info, sizeof(dboot_info))) {
                LOG(LOG_ERROR, "copy_to_user err");
                ret = -EFAULT;
            }
            break;

        case DBOOT_CMD_VER_INFO:
            ret = dboot_get_uboot_ver(arg, 0);
            break;

        case DBOOT_GET_STRAP_STATUS:
            ret = dboot_get_boot_strap_info((void *)arg);
            break;

        case DBOOT_GET_SEC_FW_VER:
            ret = dboot_get_sec_fw_ver((void *)arg);
            break;

        default:
            ret = -EINVAL;
            LOG(LOG_ERROR, "dboot CMD_UNKNOWN!\n");
            break;
    }

    up(&g_dboot_ctrl.sema);

    return ret;
}

const struct file_operations g_dboot_fops = {
    .owner = THIS_MODULE,
    .open = dboot_open,
    .release = dboot_release,
    .unlocked_ioctl = dboot_ioctl,
};

/* The dual-boot driver exits. */
void __exit dboot_exit(void)
{
    /* Character device clear */
    if (g_dboot_ctrl.cdev) {
        comm_cleanup_dev(g_dboot_ctrl.cdev);

        kfree(g_dboot_ctrl.cdev);
        g_dboot_ctrl.cdev = NULL;
    }

    return;
}

/* Dboot_probe subinterface */
static s32 dboot_probe_info(void)
{
    s32 ret;
    u32 val = 0;
    u32 bmc_rst_cnt = 0;
    u32 bmc_unsec_rst_cnt = 0;
    u32 host_rst_cnt = 0;
    u32 chip_rst_cnt = 0;

    ret = dboot_get_boot_status(&val);
    if (ret) {
        LOG(LOG_ERROR, "dboot: dboot_get_boot_status err(%d)!", ret);
        return ret;
    }
    g_dboot_ctrl.boot_area = (val & 0x1);

    ret = dboot_get_rst_cnt(&bmc_rst_cnt, &bmc_unsec_rst_cnt, &host_rst_cnt, &chip_rst_cnt);
    if (ret) {
        LOG(LOG_ERROR, "dboot: dboot_get_rst_cnt err(%d)!", ret);
        return ret;
    }

    /* Get uboot version device node */
    g_dboot_ctrl.np_uboot_ver = of_find_node_by_name(NULL, "uboot_ver");
    if (g_dboot_ctrl.np_uboot_ver == NULL) {
        LOG(LOG_ERROR, "dboot: get uboot device node failed\n");
        return -ENODEV;
    }

    /* Get uboot version device node */
    g_dboot_ctrl.np_l0fw_ver = of_find_node_by_name(NULL, "l0fw_ver");
    if (g_dboot_ctrl.np_l0fw_ver == NULL) {
        LOG(LOG_ERROR, "dboot: get l0fw device node failed\n");
        return -ENODEV;
    }

    /* Get l0&l1 version device node */
    g_dboot_ctrl.np_l1fw_ver = of_find_node_by_name(NULL, "l1fw_ver");
    if (g_dboot_ctrl.np_l1fw_ver == NULL) {
        LOG(LOG_ERROR, "dboot: get l1fw device node failed\n");
        return -ENODEV;
    }

    ret = dboot_get_uboot_ver(0, 0);
    if (ret) {
        LOG(LOG_ERROR, "dboot: dboot_get_uboot_ver err(%d)!", ret);
        return ret;
    }

    LOG(LOG_PRINT, "boot from uboot %u", g_dboot_ctrl.boot_area);
    LOG(LOG_PRINT, "BMC reset count: %u", bmc_rst_cnt);
    LOG(LOG_PRINT, "BMC unsec reset count: %u", bmc_unsec_rst_cnt);
    LOG(LOG_PRINT, "HOST reset count: %u", host_rst_cnt);
    LOG(LOG_PRINT, "CHIP reset count: %u", chip_rst_cnt);

    return 0;
}

/* Entry function of the dual-boot driver */
s32 __init dboot_probe(void)
{
    s32 ret;

    LOG(LOG_ERROR, "dboot probe start.");

    g_dboot_ctrl.cdev = (cdev_st *)kzalloc(sizeof(cdev_st), GFP_KERNEL);
    if (!g_dboot_ctrl.cdev) {
        LOG(LOG_ERROR, "dboot: dev kmalloc failed!");
        return -ENOMEM;
    }

    ret = comm_init_dev(g_dboot_ctrl.cdev, &g_dboot_fops, "dboot");
    if (ret) {
        LOG(LOG_ERROR, "dboot: comm_init_dev failed!");
        goto out_comm_init_dev;
    }

    sema_init(&g_dboot_ctrl.sema, 1);

    ret = dboot_probe_info();
    if (ret) {
        LOG(LOG_ERROR, "dboot: dboot_probe_info failed!");
        goto out_get_boot_status;
    }

    LOG(LOG_PRINT, "dboot init ok!");

    return 0;

out_get_boot_status:
    comm_cleanup_dev(g_dboot_ctrl.cdev);

out_comm_init_dev:
    kfree(g_dboot_ctrl.cdev);
    g_dboot_ctrl.cdev = NULL;

    return ret;
}
module_init(dboot_probe);
module_exit(dboot_exit);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("dboot driver");