// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Public module host_subsys external interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/spinlock_types.h>
#include <linux/io.h>

#include "host_subsys.h"

#define HOST_SUBSYS_REG_BASE_ADDR   0x8b20000
#define HOST_SUBSYS_REG_SIZE        0x1000

struct host_subsys_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

struct host_subsys_mgr g_hostsubsys_mgr = { 0 };

/**
 * @brief: initialize host sybsys resource
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 host_subsys_init(void)
{
    if (atomic_read(&g_hostsubsys_mgr.init_flag)) {
        return 0;
    }

    g_hostsubsys_mgr.virt_base = (unsigned long)ioremap(HOST_SUBSYS_REG_BASE_ADDR, HOST_SUBSYS_REG_SIZE);
    if (!g_hostsubsys_mgr.virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_hostsubsys_mgr.lock);
    atomic_set(&g_hostsubsys_mgr.init_flag, 1);

    return 0;
}

/**
 * @brief: release host_subsys resources.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 host_subsys_exit(void)
{
    if (atomic_read(&g_hostsubsys_mgr.init_flag)) {
        atomic_set(&g_hostsubsys_mgr.init_flag, 0);
        iounmap((void *)g_hostsubsys_mgr.virt_base);
    }

    return 0;
}

/**
 * @brief: atomic read operation for host_subsys registers.
 * @param: reg_offset: register offset.
 *         reg_val: the read val.
 * @retval: 0 is success, other is failure.
 */
s32 host_subsys_reg_read(u32 reg_offset, u32 *reg_val)
{
    unsigned long irqflags = 0;

    if (reg_val == NULL) {
        return -EINVAL;
    }

    if (atomic_read(&g_hostsubsys_mgr.init_flag) == 0 || g_hostsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_hostsubsys_mgr.lock, irqflags);
    *reg_val = readl((void *)(g_hostsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_hostsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(host_subsys_reg_read);

/**
 * @brief: atomic write operation for host_subsys registers.
 * @param: reg_offset: register offset.
 *         reg_val: the read val.
 * @retval: 0 is success, other is failure.
 */
s32 host_subsys_reg_write(u32 reg_offset, u32 reg_val)
{
    unsigned long irqflags = 0;

    if (atomic_read(&g_hostsubsys_mgr.init_flag) == 0 || g_hostsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_hostsubsys_mgr.lock, irqflags);
    writel(reg_val, (void *)(g_hostsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_hostsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(host_subsys_reg_write);