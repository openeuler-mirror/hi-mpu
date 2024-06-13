// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * a5x_subsys module interface
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
#include "a5x_subsys.h"

#define A5X_CSR_BASE_ADDR 0x14000000
#define A5X_REG_SIZE      0x1000

struct a5x_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

static struct a5x_mgr g_a5x_mgr = { 0 };

/**
 * @brief: a5x resource initialization
 * @param: void
 * @retval: 0 is success, other is failure
 */
s32 a5x_init(void)
{
    if (atomic_read(&g_a5x_mgr.init_flag)) {
        return 0;
    }

    g_a5x_mgr.virt_base = (unsigned long)ioremap(A5X_CSR_BASE_ADDR, A5X_REG_SIZE);
    if (!g_a5x_mgr.virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_a5x_mgr.lock);
    atomic_set(&g_a5x_mgr.init_flag, 1);
    return 0;
}

/**
 * @brief: a5x resource release
 * @param: void
 * @retval: 0 is success, other is failure
 */
s32 a5x_exit(void)
{
    if (atomic_read(&g_a5x_mgr.init_flag)) {
        atomic_set(&g_a5x_mgr.init_flag, 0);
        iounmap((void *)g_a5x_mgr.virt_base);
    }

    return 0;
}

/**
 * @brief: atomic read operation on the a5x register
 * @param: u32 reg_offset: register offset, u32 reg_val: the read value
 * @retval: 0 is success, other is failure
 */
s32 a5x_reg_read(u32 reg_offset, u32 *reg_val)
{
    unsigned long irqflags = 0;

    if (reg_val == NULL) {
        return -EINVAL;
    }

    if (atomic_read(&g_a5x_mgr.init_flag) == 0 || g_a5x_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_a5x_mgr.lock, irqflags);
    *reg_val = readl((void *)(g_a5x_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_a5x_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(a5x_reg_read);

/**
 * @brief: atomic write operation on the a5x register
 * @param: u32 reg_offset: register offset, u32 reg_bal: the read value
 * @retval: 0 is success, other is failure
 */
s32 a5x_reg_write(u32 reg_offset, u32 reg_val)
{
    unsigned long irqflags = 0;

    if (atomic_read(&g_a5x_mgr.init_flag) == 0 || g_a5x_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_a5x_mgr.lock, irqflags);
    writel(reg_val, (void *)(g_a5x_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_a5x_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(a5x_reg_write);

