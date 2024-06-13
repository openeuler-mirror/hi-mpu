// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module comm external interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm.h"
#include <linux/of_irq.h>
#include "sysctrl_csr_addr_define.h"
#include "sysctrl_csr_c_union_define.h"
#include "comm_knl_api.h"
#include "m_reset.h"
#include "lp_subsys.h"
#include "io_config.h"
#include "hp_subsys.h"
#include "sysctrl.h"
#include "host_subsys.h"
#include "a5x_subsys.h"
#include "bdinfo.h"
#include "usr_data.h"

#define COMM_BYTES_OF_DWORD 0x4

/* number of types of reset interrupts for each subsystem: rising edge & falling edge. */
#define SUBSYS_RST_INT_MAX_TYPE_COUNT 0x2

#define SUBSYS_RST_INT_TYPE_RAISE 0x0
#define SUBSYS_RST_INT_TYPE_FALL  0x1

#define IRQ_NAME_STRING_MAX_LEN 32

typedef struct {
    unsigned long arg;
    subsys_rst_callback pfn_raise;
    subsys_rst_callback pfn_fall;
    subsys_rst_callback pfn_delay;
} subsys_rst_module_callback_s;

typedef struct {
    spinlock_t rst_lock;  /* lock for handling reset domain interrupts. */
    u32 rst_raise_virq;  /* interrupt number for reset domain rising edge. */
    u32 rst_fall_virq;  /* interrupt number for reset domain falling edge. */

    /* names of interrupts for rising edge and falling edge. */
    s8 int_name[SUBSYS_RST_INT_MAX_TYPE_COUNT][IRQ_NAME_STRING_MAX_LEN];
    struct delayed_work rst_delayed_work;  /* delayed work for reset domain. */

    /* callback functions registered for each reset domain. */
    subsys_rst_module_callback_s rst_module_callback[SUBSYS_RST_MAX_CB_MODULES];
} subsys_rst_module_ops_s;

typedef struct {
    u32 cpll_dlock_virq;  /* interrupt number for cpll loss lock. */
    u32 pll0_dlock_virq;  /* interrupt number for pll0 loss lock. */
    u32 pll1_dlock_virq;  /* interrupt number for pll1 loss lock. */
    u32 pll2_dlock_virq;  /* interrupt number for pll2 loss lock. */
    u32 dac_cable_virq;  /* interrupt number of dac_calbe. */

    /* host reset domain handling functions registered for each module. */
    subsys_rst_module_ops_s rst_module_ops[SUBSYS_RST_MAX_COUNT];
} comm_private_info_s;

static cdev_st *g_comm_dev = NULL;
static const s8 *g_comm_dev_name = DEV_COMM_NAME;

static comm_private_info_s g_comm_private_info = { 0 };

static const struct module_init_map_tag {
    s32 (*module_init_fun)(void);
    s32 (*module_exit_fun)(void);
    char *error_info;
} g_module_init_map[] = {
    { lp_subsys_init,   lp_subsys_exit,     "lp_subsys_init failed!"   },
    { hp_subsys_init,   hp_subsys_exit,     "hp_subsys_init failed!"   },
    { sysctrl_init,     sysctrl_exit,       "sysctrl_init failed!"     },
    { host_subsys_init, host_subsys_exit,   "host_subsys_init failed!" },
    { a5x_init,         a5x_exit,           "a5x_init failed!"         },
    { io_config_init,   io_config_exit,     "io_config_init failed!"   },
    { bdinfo_cfg_init,  bdinfo_cfg_exit,    "bdinfo_cfg_init failed!"  },
    { m_reset_init,     m_reset_exit,       "m_reset_init failed!"     },
    { usr_data_init,    usr_data_exit,      "usr_data_init failed!"    },
};

/**
 * @brief: input parameter verification for char dev ioctl.
 * @param: cmd: ioctl command;
 *         arg: ioctl command parameters;
 *         ioc_magic: magic word after ioctl macro conversion;
 *         ioc_max_nr: Maximum value of ioctl number.
 * @retval：0 is success, other is failure.
 */
s32 comm_ioc_parm_check(u32 cmd, void __user *arg, u32 ioc_magic, u32 ioc_max_nr)
{
    if (_IOC_TYPE(cmd) != ioc_magic) {
        LOG(LOG_ERROR, "cmd(0x%x)'s magic isn't magic(0x%x).", cmd, ioc_magic);
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > ioc_max_nr) {
        LOG(LOG_ERROR, "cmd(0x%x)'s nr is larger than(0x%x).", cmd, ioc_max_nr);
        return -ENOTTY;
    }

    return 0;
}
EXPORT_SYMBOL(comm_ioc_parm_check);

/**
 * @brief: char dev rmmod function
 * @param: p_dev: pointer to the char dev structure.
 * @retval: void
 */
void comm_cleanup_dev(cdev_st *p_dev)
{
    dev_t devno = MKDEV(p_dev->dev_major, p_dev->dev_minor);

    device_destroy(p_dev->dev_class, devno);
    class_destroy(p_dev->dev_class);
    cdev_del(&p_dev->cdev);
    unregister_chrdev_region(devno, 1);
}
EXPORT_SYMBOL(comm_cleanup_dev);

/**
 * @brief: char dev init function
 * @param: pDev: pointer to the comm char dev structure.
 *         pfoprs: comm character device file operation structure pointer.
 *         pdevname: comm character device name.
 * @retval: void
 */
s32 comm_init_dev(cdev_st *p_dev, const struct file_operations *pfoprs, const s8 *pdevname)
{
    s32 ret;
    dev_t dev_num = 0;
    struct device *pdev_tmp = NULL;

    /*  register or dynamically allocate an unused device number. */
    if (p_dev->dev_major) {
        dev_num = MKDEV(p_dev->dev_major, p_dev->dev_minor);
        ret = register_chrdev_region(dev_num, 1, pdevname);
    } else {
        ret = alloc_chrdev_region(&dev_num, p_dev->dev_minor, 1, pdevname);
        p_dev->dev_major = MAJOR(dev_num);
    }

    if (ret < 0) {
        LOG(LOG_ERROR, "dev(%s) can't get major(0x%x)!", pdevname, p_dev->dev_major);
        return ret;
    }

    /* initialize device structure. */
    cdev_init(&p_dev->cdev, pfoprs);
    p_dev->cdev.owner = THIS_MODULE;

    /* add character device. */
    ret = cdev_add(&p_dev->cdev, dev_num, 1);
    if (ret) {
        LOG(LOG_ERROR, "dev(%s) cdev_add failed(0x%x)", pdevname, ret);
        goto cdev_add_fail;
    }

    /* create device class. */
    p_dev->dev_class = class_create(THIS_MODULE, pdevname);
    if (IS_ERR(p_dev->dev_class)) {
        ret = PTR_ERR(p_dev->dev_class);

        LOG(LOG_ERROR, "dev(%s) class_create failed(0x%x)", pdevname, ret);
        goto class_create_fail;
    }

    /* create device node. */
    pdev_tmp = device_create(p_dev->dev_class, NULL, MKDEV(p_dev->dev_major, p_dev->dev_minor), NULL, "%s", \
                             pdevname);
    if (IS_ERR(pdev_tmp)) {
        ret = PTR_ERR(pdev_tmp);

        LOG(LOG_ERROR, "dev(%s) device_create failed(0x%x)", pdevname, ret);
        goto device_create_fail;
    }

    return 0;

device_create_fail:
    class_destroy(p_dev->dev_class);

class_create_fail:
    cdev_del(&p_dev->cdev);

cdev_add_fail:
    unregister_chrdev_region(MKDEV(p_dev->dev_major, p_dev->dev_minor), 1);

    return ret;
}
EXPORT_SYMBOL(comm_init_dev);

/**
 * @brief: registering a host reset handling function.
 */
s32 subsys_rst_callback_register(u32 subsys, u32 module_id, subsys_rst_callbacks_s *handle, u64 arg)
{
    unsigned long flags;

    if ((subsys >= SUBSYS_RST_MAX_COUNT) || (module_id >= SUBSYS_RST_MAX_CB_MODULES) || !handle) {
        LOG(LOG_ERROR, "subsys(0x%x) module_id(0x%x) is inval!", subsys, module_id);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_comm_private_info.rst_module_ops[subsys].rst_lock, flags);

    g_comm_private_info.rst_module_ops[subsys].rst_module_callback[module_id].pfn_raise = handle->raise_handle;
    g_comm_private_info.rst_module_ops[subsys].rst_module_callback[module_id].pfn_fall = handle->fall_handle;
    g_comm_private_info.rst_module_ops[subsys].rst_module_callback[module_id].pfn_delay = handle->delay_handle;
    g_comm_private_info.rst_module_ops[subsys].rst_module_callback[module_id].arg = arg;

    spin_unlock_irqrestore(&g_comm_private_info.rst_module_ops[subsys].rst_lock, flags);

    return 0;
}
EXPORT_SYMBOL(subsys_rst_callback_register);

/**
 * @brief: callback function.
 * @param: works: used for adapting the kernel framework only.
 * @retval: void
 */
static void host_rst_delay_func(struct work_struct *works)
{
    s32 id;
    unsigned long arg = 0;
    unsigned long flags;
    subsys_rst_callback pfn_delay = NULL;

    spin_lock_irqsave(&g_comm_private_info.rst_module_ops[SUBSYS_RST_HOST].rst_lock, flags);

    for (id = 0; id < SUBSYS_RST_MAX_CB_MODULES; id++) {
        arg = g_comm_private_info.rst_module_ops[SUBSYS_RST_HOST].rst_module_callback[id].arg;
        pfn_delay = g_comm_private_info.rst_module_ops[SUBSYS_RST_HOST].rst_module_callback[id].pfn_delay;

        if (pfn_delay) {
            pfn_delay(arg);
        }
    }

    spin_unlock_irqrestore(&g_comm_private_info.rst_module_ops[SUBSYS_RST_HOST].rst_lock, flags);

    return;
}

/**
 * @brief: clear reset interrupt status for subsystem.
 * @param: rst_subsys:  reset subsystem.
 *         int_type: interrupt type is rising edge or falling edge.
 * @retval: 0 is success, other if failure.
 */
static void sysctrl_rst_int_clear(u32 rst_subsys, u32 int_type)
{
    s32 ret;
    u_sys_int_stauts sys_int_stauts;

    sys_int_stauts.val = 0;

    if (rst_subsys == SUBSYS_RST_HOST) {
        if (int_type == SUBSYS_RST_INT_TYPE_RAISE) {
            sys_int_stauts.bits.hs_rst_pos_st = 0x1;
        } else {
            sys_int_stauts.bits.hs_rst_int_st = 0x1;
        }
    }

    if (rst_subsys == SUBSYS_RST_PCIE0) {
        if (int_type == SUBSYS_RST_INT_TYPE_RAISE) {
            sys_int_stauts.bits.pcie0_rst_pos_st = 0x1;
        } else {
            sys_int_stauts.bits.pcie0_rst_int_st = 0x1;
        }
    }

    if (rst_subsys == SUBSYS_RST_PCIE1) {
        if (int_type == SUBSYS_RST_INT_TYPE_RAISE) {
            sys_int_stauts.bits.pcie1_rst_pos_st = 0x1;
        } else {
            sys_int_stauts.bits.pcie1_rst_int_st = 0x1;
        }
    }

    ret = sysctrl_reg_write(SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, sys_int_stauts.val);
    if (ret) {
        LOG(LOG_ERROR, "write sysctrl reg to clear int fail. subsys(%u) int_type(%u)", rst_subsys, int_type);
    }

    return;
}

static void sysctrl_rst_raise_irq_handle(u32 rst_subsys)
{
    u32 rst_module;
    unsigned long flags;
    unsigned long arg;

    spin_lock_irqsave(&g_comm_private_info.rst_module_ops[rst_subsys].rst_lock, flags);

    sysctrl_rst_int_clear(rst_subsys, SUBSYS_RST_INT_TYPE_RAISE);

    for (rst_module = 0; rst_module < SUBSYS_RST_MAX_CB_MODULES; rst_module++) {
        arg = g_comm_private_info.rst_module_ops[rst_subsys].rst_module_callback[rst_module].arg;
        if (g_comm_private_info.rst_module_ops[rst_subsys].rst_module_callback[rst_module].pfn_raise) {
            g_comm_private_info.rst_module_ops[rst_subsys].rst_module_callback[rst_module].pfn_raise(arg);
        }
    }

    (void)schedule_delayed_work(&g_comm_private_info.rst_module_ops[rst_subsys].rst_delayed_work, LPC_DELAY_TIME);

    spin_unlock_irqrestore(&g_comm_private_info.rst_module_ops[rst_subsys].rst_lock, flags);

    return;
}

static void sysctrl_rst_fall_irq_handle(u32 rst_subsys)
{
    u32 rst_module;
    unsigned long flags;
    unsigned long arg;

    spin_lock_irqsave(&g_comm_private_info.rst_module_ops[rst_subsys].rst_lock, flags);

    sysctrl_rst_int_clear(rst_subsys, SUBSYS_RST_INT_TYPE_FALL);

    for (rst_module = 0; rst_module < SUBSYS_RST_MAX_CB_MODULES; rst_module++) {
        arg = g_comm_private_info.rst_module_ops[rst_subsys].rst_module_callback[rst_module].arg;
        if (g_comm_private_info.rst_module_ops[rst_subsys].rst_module_callback[rst_module].pfn_fall) {
            g_comm_private_info.rst_module_ops[rst_subsys].rst_module_callback[rst_module].pfn_fall(arg);
        }
    }

    spin_unlock_irqrestore(&g_comm_private_info.rst_module_ops[rst_subsys].rst_lock, flags);

    return;
}

/**
 * @brief: reset interrupt handling function.
 * @param: irq: reset interrupt number.
 *         dev_id: parameters given during interrupt registration, not used.
 * @retval: IRQ_HANDLED
 */
static irqreturn_t sysctrl_rst_int_handle(s32 irq, void *dev_id)
{
    u32 rst_subsys;
    u_sys_int_stauts sys_int_stauts;

    (void)sysctrl_reg_read(SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, &sys_int_stauts.val);
    LOG(LOG_WARN, "Receive subsys_rst_irq(%d), int_reg(0x%x) int_sts(0x%x)!",
        irq, SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, sys_int_stauts.val);

    for (rst_subsys = 0; rst_subsys < SUBSYS_RST_MAX_COUNT; rst_subsys++) {
        if (g_comm_private_info.rst_module_ops[rst_subsys].rst_raise_virq == irq) {
            sysctrl_rst_raise_irq_handle(rst_subsys);
            break;
        } else if (g_comm_private_info.rst_module_ops[rst_subsys].rst_fall_virq == irq) {
            sysctrl_rst_fall_irq_handle(rst_subsys);
            break;
        }
    }

    if (rst_subsys >= SUBSYS_RST_MAX_COUNT) {
        LOG(LOG_ERROR, "comm rcv error int(%d)!", irq);
    }

    return IRQ_HANDLED;
}

/**
 * @brief: disable reset interrupt.
 * @param: void
 * @retval: 0 is success, other is failure
 */
static void sysctrl_rst_int_uinit(void)
{
    u32 i;
    u_sys_int_mask sys_int_mask;

    /* disable interrupt */
    sys_int_mask.val = 0;
    sys_int_mask.bits.hs_rst_int_mask = 0x1;
    sys_int_mask.bits.hs_rst_pos_mask = 0x1;
    (void)sysctrl_reg_set_bits(SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR, sys_int_mask.val, 0x1);

    /* remove interrupt handling function. */
    for (i = 0; i < SUBSYS_RST_MAX_COUNT; i++) {
        if (g_comm_private_info.rst_module_ops[i].rst_raise_virq) {
            free_irq(g_comm_private_info.rst_module_ops[i].rst_raise_virq, NULL);
            g_comm_private_info.rst_module_ops[i].rst_raise_virq = 0;
        }

        if (g_comm_private_info.rst_module_ops[i].rst_fall_virq) {
            free_irq(g_comm_private_info.rst_module_ops[i].rst_fall_virq, NULL);
            g_comm_private_info.rst_module_ops[i].rst_fall_virq = 0;
        }
    }

    return;
}

static s32 sysctrl_rst_subsys_int_init(u32 rst_subsys, struct device_node *node)
{
    s32 ret;
    u32 rst_type;
    u32 dts_int_index;
    u32 virq;
    s8 *int_name = NULL;

    spin_lock_init(&g_comm_private_info.rst_module_ops[rst_subsys].rst_lock);

    for (rst_type = 0; rst_type < SUBSYS_RST_INT_MAX_TYPE_COUNT; rst_type++) {
        /* get virq */
        dts_int_index = 5 + SUBSYS_RST_INT_MAX_TYPE_COUNT * rst_subsys + rst_type; /* base index offset is 5 */
        virq = irq_of_parse_and_map(node, dts_int_index);
        if (!virq) {
            LOG(LOG_ERROR, "irq_of_parse_and_map failed, dtb_index(0x%x)!", dts_int_index);
            return -EFAULT;
        }

        /* register interrupt handling function. */
        int_name = g_comm_private_info.rst_module_ops[rst_subsys].int_name[rst_type];
        (void)snprintf_s(int_name, IRQ_NAME_STRING_MAX_LEN, IRQ_NAME_STRING_MAX_LEN - 1,
                         "subsys_rst_int%u", dts_int_index);

        ret = request_irq(virq, sysctrl_rst_int_handle, 0, int_name, NULL);
        if (ret) {
            LOG(LOG_ERROR, "failed to request-irq rst, dtb_index(0x%x)!", dts_int_index);
            return -EFAULT;
        }

        /* configure interrupt number. */
        if (rst_type == SUBSYS_RST_INT_TYPE_RAISE) {
            g_comm_private_info.rst_module_ops[rst_subsys].rst_raise_virq = virq;
        } else {
            g_comm_private_info.rst_module_ops[rst_subsys].rst_fall_virq = virq;
        }
    }

    return 0;
}

/**
 * @brief: initialize reset interrupt.
 * @param: pdev: pointer to platform device structure.
 * @retval: 0 is succes, other is failure
 */
static s32 sysctrl_rst_int_init(struct platform_device *pdev)
{
    s32 ret;
    u32 rst_subsys;
    u_sys_int_mask sys_int_mask;
    u_sys_int_stauts sys_int_stauts;
    struct device_node *node = pdev->dev.of_node;

    /* disable interrupt */
    sys_int_mask.val = 0;
    sys_int_mask.bits.hs_rst_int_mask = 0x1;
    sys_int_mask.bits.hs_rst_pos_mask = 0x1;
    (void)sysctrl_reg_set_bits(SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR, sys_int_mask.val, 0x1);

    (void)memset_s((void *)&g_comm_private_info.rst_module_ops[0],
                   SUBSYS_RST_MAX_COUNT * sizeof(subsys_rst_module_ops_s), 0,
                   SUBSYS_RST_MAX_COUNT * sizeof(subsys_rst_module_ops_s));

    for (rst_subsys = 0; rst_subsys < SUBSYS_RST_MAX_COUNT; rst_subsys++) {
        /* currently only enable interrupt handling for host domain, pcie domain is handled by secure M3. */
        if (rst_subsys != SUBSYS_RST_HOST) {
            continue;
        }

        ret = sysctrl_rst_subsys_int_init(rst_subsys, node);
        if (ret) {
            return ret;
        }
    }

    INIT_DELAYED_WORK(&g_comm_private_info.rst_module_ops[SUBSYS_RST_HOST].rst_delayed_work, host_rst_delay_func);

    /* clear interrupt. */
    sys_int_stauts.val = 0;
    sys_int_stauts.bits.hs_rst_int_st = 0x1;
    sys_int_stauts.bits.hs_rst_pos_st = 0x1;
    (void)sysctrl_reg_write(SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, sys_int_stauts.val);

    /* enable interrupt */
    sys_int_mask.val = 0;
    sys_int_mask.bits.hs_rst_int_mask = 0x1;
    sys_int_mask.bits.hs_rst_pos_mask = 0x1;
    (void)sysctrl_reg_set_bits(SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR, sys_int_mask.val, 0x0);

    return 0;
}

/**
 * @brief: sysctrl interrupt handle function.
 * @param: irq: interrupt number.
 *         dev_id: parameters given during interrupt registration, not used
 * @retval: irqreturn_t
 */
static irqreturn_t sysctrl_int_handle(s32 irq, void *dev_id)
{
    u_sys_int_stauts sys_int_stauts;

    if ((g_comm_private_info.cpll_dlock_virq != irq) &&
        (g_comm_private_info.pll0_dlock_virq != irq) &&
        (g_comm_private_info.pll1_dlock_virq != irq) &&
        (g_comm_private_info.pll2_dlock_virq != irq) &&
        (g_comm_private_info.dac_cable_virq != irq)) {
        LOG(LOG_ERROR, "recv pll irq, but irq(%d) isn't in reg_virq(%u_%u_%u_%u_%u)!",
            irq,
            g_comm_private_info.cpll_dlock_virq,
            g_comm_private_info.pll0_dlock_virq,
            g_comm_private_info.pll1_dlock_virq,
            g_comm_private_info.pll2_dlock_virq,
            g_comm_private_info.dac_cable_virq);

        return IRQ_NONE;
    }

    /* get interrupt status. */
    (void)sysctrl_reg_read(SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, &sys_int_stauts.val);
    LOG(LOG_ERROR, "Receive pll dlock int, int_sts(0x%x)!", sys_int_stauts.val);

    /* clear interrupt. */
    sys_int_stauts.val &= 0xb800;
    (void)sysctrl_reg_write(SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, sys_int_stauts.val);

    return IRQ_HANDLED;
}

/**
 * @brief: sysctl interrupt clear function.
 * @param: pdev: pointer to platform device structure.
 * @retval: 0 is success, other is failure
 */
void sysctrl_int_uinit(void)
{
    u_sys_int_mask sys_int_mask;

    /* disable hardware interrupt. */
    sys_int_mask.val = 0;
    sys_int_mask.bits.cpll_dlock_mask = 0x1;
    sys_int_mask.bits.ppll0_dlock_mask = 0x1;
    sys_int_mask.bits.ppll1_dlock_mask = 0x1;
    sys_int_mask.bits.ppll2_dlock_mask = 0x1;
    sys_int_mask.bits.dac_cable_mask = 0x1;
    (void)sysctrl_reg_set_bits(SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR, sys_int_mask.val, 0x1);

    /* remove interrupt handling function. */
    if (g_comm_private_info.cpll_dlock_virq) {
        free_irq(g_comm_private_info.cpll_dlock_virq, NULL);
        g_comm_private_info.cpll_dlock_virq = 0;
    }

    if (g_comm_private_info.pll0_dlock_virq) {
        free_irq(g_comm_private_info.pll0_dlock_virq, NULL);
        g_comm_private_info.pll0_dlock_virq = 0;
    }

    if (g_comm_private_info.pll1_dlock_virq) {
        free_irq(g_comm_private_info.pll1_dlock_virq, NULL);
        g_comm_private_info.pll1_dlock_virq = 0;
    }

    if (g_comm_private_info.pll2_dlock_virq) {
        free_irq(g_comm_private_info.pll2_dlock_virq, NULL);
        g_comm_private_info.pll2_dlock_virq = 0;
    }

    if (g_comm_private_info.dac_cable_virq) {
        free_irq(g_comm_private_info.dac_cable_virq, NULL);
        g_comm_private_info.dac_cable_virq = 0;
    }

    return;
}

/**
 * @brief: sysctrl init
 * @param: pdev: pointer to platform device structure.
 * @retval: 0 is success, other is failure.
 */
static s32 sysctrl_int_init(struct platform_device *pdev)
{
    s32 ret;
    u32 i;
    struct virq_map_tag {
        u32 *virq;
        char *name;
    } virq_map[5] = { /* 5 virqs are { cpll, pll0, pll1, pll2, dac }, ref dts */
        { &g_comm_private_info.cpll_dlock_virq, "cpll dlock interrupt" },
        { &g_comm_private_info.pll0_dlock_virq, "pll0 dlock interrupt" },
        { &g_comm_private_info.pll1_dlock_virq, "pll1 dlock interrupt" },
        { &g_comm_private_info.pll2_dlock_virq, "pll2 dlock interrupt" },
        { &g_comm_private_info.dac_cable_virq, "dac dlock interrupt" },
    };
    u_sys_int_stauts sys_int_stauts;
    u_sys_int_mask sys_int_mask;
    struct device_node *node = pdev->dev.of_node;

    /* disable interrupt. */
    sys_int_mask.val = 0;
    sys_int_mask.bits.cpll_dlock_mask = 0x1;
    sys_int_mask.bits.ppll0_dlock_mask = 0x1;
    sys_int_mask.bits.ppll1_dlock_mask = 0x1;
    sys_int_mask.bits.ppll2_dlock_mask = 0x1;
    sys_int_mask.bits.dac_cable_mask = 0x1;
    (void)sysctrl_reg_set_bits(SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR, sys_int_mask.val, 0x1);

    /* register interrupt handling function. */
    for (i = 0; i < sizeof(virq_map) / sizeof(virq_map[0]); i++) {
        *(virq_map[i].virq) = irq_of_parse_and_map(node, i);
        ret = request_irq(*(virq_map[i].virq), sysctrl_int_handle, 0, virq_map[i].name, NULL);
        if (ret) {
            LOG(LOG_ERROR, "request irq(%u) dlock irq failed(%d)", i, ret);
            return -EFAULT;
        }
    }

    /* clear interrupt. */
    sys_int_stauts.val = 0;
    sys_int_stauts.bits.cpll_dlock_st = 0x1;
    sys_int_stauts.bits.ppll0_dlock_st = 0x1;
    sys_int_stauts.bits.ppll1_dlock_st = 0x1;
    sys_int_stauts.bits.dac_cable_st = 0x1;
    (void)sysctrl_reg_write(SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR, sys_int_stauts.val);

    /* enable interrupt. */
    sys_int_mask.val = 0;
    sys_int_mask.bits.cpll_dlock_mask = 0x1;
    sys_int_mask.bits.ppll0_dlock_mask = 0x1;
    sys_int_mask.bits.ppll1_dlock_mask = 0x1;
    sys_int_mask.bits.dac_cable_mask = 0x1;
    (void)sysctrl_reg_set_bits(SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR, sys_int_mask.val, 0x0);

    return 0;
}

/* unschedule workqueue. */
void cancel_rearming_delayed_workqueue(const struct workqueue_struct *wq, struct delayed_work *work)
{
    if (wq != NULL && work != NULL) {
        cancel_delayed_work_sync(work);
    }
}
EXPORT_SYMBOL(cancel_rearming_delayed_workqueue);

/**
 * @brief: device open function.
 * @param: inode: pointer to COMM device file INODE structure.
 *         file: pointer to COMM device file structure.
 * @retval: 0 is success, other is failure.
 */
static s32 comm_open(struct inode *inode, struct file *file)
{
    return 0;
}

/**
 * @brief: device release function.
 * @param: inode: pointer to COMM device file INODE structure.
 *         file: pointer to COMM device file structure.
 * @retval: 0 is success, other is failure.
 */
static s32 comm_release(struct inode *inode, struct file *file)
{
    return 0;
}

/**
 * @brief: device control function.
 * @param: file: pointer to COMM device file structure.
 *         cmd: ioctl command number.
 *         buf: ioctl command parameters.
 * @retval: 0 is success, other is failure.
 */
static long comm_ioctl(struct file *file, u32 cmd, unsigned long buf)
{
    s32 ret = 0;
    m_reset_s m_reset;
    u16 module_id = 0;

    switch (cmd) {
        /* chip module reset command. */
        case COMM_CMD_MOD_RESET:
            if (copy_from_user(&m_reset, (void *)buf, sizeof(m_reset_s))) {
                LOG(LOG_ERROR, "comm cmd(0x%x) copy_from_user fail!", cmd);
                return -EFAULT;
            }

            ret = m_reset_handle(m_reset.reg_offset, m_reset.mask);
            if (ret != 0) {
                LOG(LOG_ERROR, "comm cmd(0x%x) m_reset_handle fail!", cmd);
                return ret;
            }
            break;

        case COMM_CMD_IOCFG_CHECK:
            if (copy_from_user(&module_id, (void *)buf, sizeof(u16))) {
                LOG(LOG_ERROR, "comm iocfg_check copy fail");
                return -EFAULT;
            }

            /* If pins are configured to the write function, return 1, otherwise return 0 */
            if (!bdinfo_iocfg_check(module_id)) {
                LOG(LOG_ERROR, "comm iocfg_check fail");
                return -EINVAL;
            }
            break;

        case COMM_CMD_IOCFG_CONFIG:
            if (copy_from_user(&module_id, (void *)buf, sizeof(u16))) {
                LOG(LOG_ERROR, "comm iocfg_config copy fail");
                return -EFAULT;
            }

            ret = bdinfo_set_module_iocfg(module_id);
            break;

        default:
            LOG(LOG_ERROR, "comm cmd(0x%x) isn't support!", cmd);
            return -EINVAL;
    }

    return ret;
}

const struct file_operations g_comm_fops = {
    .owner = THIS_MODULE,
    .open = comm_open,
    .release = comm_release,
    .unlocked_ioctl = comm_ioctl,
};

/**
 * @brief: comm device driver、function entry.
 * @param: pdev: pointer to platform device structure.
 * @retval: 0 is success, other is failure.
 */
static int comm_probe(struct platform_device *pdev)
{
    int ret;
    u32 i;

    g_comm_dev = (cdev_st *)kzalloc(sizeof(cdev_st), GFP_KERNEL);
    if (!g_comm_dev) {
        LOG(LOG_ERROR, "comm_dev kzalloc err!");
        return -ENOMEM;
    }

    ret = comm_init_dev(g_comm_dev, &g_comm_fops, g_comm_dev_name);
    if (ret) {
        LOG(LOG_ERROR, "comm_init failed(0x%x)!", ret);
        goto dev_init_err;
    }

    for (i = 0; i < sizeof(g_module_init_map) / sizeof(g_module_init_map[0]); i++) {
        ret = g_module_init_map[i].module_init_fun();
        if (ret < 0) {
            LOG(LOG_ERROR, g_module_init_map[i].error_info);
            goto module_init_err;
        }
    }

    ret = sysctrl_rst_int_init(pdev);
    if (ret < 0) {
        LOG(LOG_ERROR, "sysctrl_rst_int_init failed!");
        goto module_init_err;
    }

    ret = sysctrl_int_init(pdev);
    if (ret < 0) {
        LOG(LOG_ERROR, "sysctrl_int_init failed!");
        goto module_init_err;
    }

    LOG(LOG_ERROR, "comm_probe success!");

    return 0;

module_init_err:
    sysctrl_int_uinit();
    sysctrl_rst_int_uinit();
    for (i = (sizeof(g_module_init_map) / sizeof(g_module_init_map[0])); i > 0; i--) {
        (void)g_module_init_map[i - 1].module_exit_fun(); /* if i defined as int type, pclint error */
    }
    comm_cleanup_dev(g_comm_dev);

dev_init_err:
    kfree(g_comm_dev);
    g_comm_dev = NULL;

    return ret;
}

/**
 * @brief: comm device driver, common function unload function.
 * @param: pdev: pointer to platform device structure.
 * @retval: 0
 */
static int comm_remove(struct platform_device *pdev)
{
    u32 i;

    sysctrl_int_uinit();
    sysctrl_rst_int_uinit();

    for (i = (sizeof(g_module_init_map) / sizeof(g_module_init_map[0])); i > 0; i--) {
        (void)g_module_init_map[i - 1].module_exit_fun(); /* if i defined as int type, pclint error */
    }

    if (g_comm_dev) {
        comm_cleanup_dev(g_comm_dev);

        kfree(g_comm_dev);
        g_comm_dev = NULL;
    }

    return 0;
}

static const struct of_device_id g_comm_match[] = {
    { .compatible = "hisilicon, hi1711-comm" }, {}
};

static struct platform_driver g_comm_driver = {
    .driver = {
        .name = DEV_COMM_NAME,
        .of_match_table = g_comm_match,
    },

    .probe = comm_probe,
    .remove = comm_remove,
};
module_platform_driver(g_comm_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("comm driver");
