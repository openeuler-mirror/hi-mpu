// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sysctrl module interface
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
#include "sysctrl_csr_addr_define.h"
#include "sysctrl.h"

#define SYSCTRL_CSR_REG_SIZE 0x1000

struct _sysctrl_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

static struct _sysctrl_mgr g_sysctrl_mgr;

/* sysctrl_init/exit should not be exposed to other modules */
/**
 * @brief: sysctrl initialization function.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 sysctrl_init(void)
{
    if (atomic_read(&g_sysctrl_mgr.init_flag)) {
        return 0;
    }

    g_sysctrl_mgr.virt_base = (unsigned long)ioremap(SYSCTRL_CSR_BASE_ADDR, SYSCTRL_CSR_REG_SIZE);
    if (!g_sysctrl_mgr.virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_sysctrl_mgr.lock);
    atomic_set(&g_sysctrl_mgr.init_flag, 1);

    return 0;
}

/**
 * @brief: release sysctrl resource.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 sysctrl_exit(void)
{
    if (atomic_read(&g_sysctrl_mgr.init_flag)) {
        atomic_set(&g_sysctrl_mgr.init_flag, 0);
        iounmap((void *)g_sysctrl_mgr.virt_base);
    }

    return 0;
}

/**
 * @brief: atomic read operation for sysctrl register.
 * @param: reg_offset: register offset.
 *         reg_val: the read val.
 * @retval: 0 is success, other is failure.
 */
s32 sysctrl_reg_read(u32 reg_offset, u32 *reg_val)
{
    unsigned long irqflags = 0;

    if (reg_val == NULL) {
        return -EINVAL;
    }

    if (atomic_read(&g_sysctrl_mgr.init_flag) == 0 || g_sysctrl_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_sysctrl_mgr.lock, irqflags);
    *reg_val = readl((void *)(g_sysctrl_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_sysctrl_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(sysctrl_reg_read);

/**
 * @brief: atomic write operation for sysctrl register.
 * @param: reg_offset: register offset.
 *         reg_val: the write val.
 * @retval: 0 is success, other is failure.
 */
s32 sysctrl_reg_write(u32 reg_offset, u32 reg_val)
{
    unsigned long irqflags = 0;

    if (atomic_read(&g_sysctrl_mgr.init_flag) == 0 || g_sysctrl_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_sysctrl_mgr.lock, irqflags);
    writel(reg_val, (void *)(g_sysctrl_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_sysctrl_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(sysctrl_reg_write);

/**
 * @brief: Atomic read write operation for sysctrl register.
 *         the entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the sysctrl based on MASK.
 *         reg_val &= ~reg_mask
 *         reg_val |= (val & reg_mask);
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 *         val: register value, the function only cares about the bit specified by reg_mask.
 * @retval: 0 is success, other is failure.
 */
s32 sysctrl_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val)
{
    unsigned long irqflags = 0;
    u32 reg_val;

    if (atomic_read(&g_sysctrl_mgr.init_flag) == 0 || g_sysctrl_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_sysctrl_mgr.lock, irqflags);
    reg_val = readl((void *)(g_sysctrl_mgr.virt_base + reg_offset));
    reg_val &= ~reg_mask;
    reg_val |= (val & reg_mask);
    writel(reg_val, (void *)(g_sysctrl_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_sysctrl_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(sysctrl_reg_read_write);

/**
 * @brief: The operation on the sysctrl register is an atomic read-modify-write operation.
 *         The entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the sysctrl based on MASK.
 *         If the mask is 0xa and set_bit = 1, then it sets BIT1 and BIT3.
 *         If the mask is 0x1 and set_bit = 0, then it clears BIT0.
 *         If the mask is 0x5 and set_bit = 0, then it clears BIT0 and BIT2.
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 * @retval: 0 is success, other is failure.
 */
s32 sysctrl_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit)
{
    unsigned long irqflags = 0;
    u32 reg_val;

    if (atomic_read(&g_sysctrl_mgr.init_flag) == 0 || g_sysctrl_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_sysctrl_mgr.lock, irqflags);
    reg_val = readl((void *)(g_sysctrl_mgr.virt_base + reg_offset));
    reg_val &= ~reg_mask;

    if (set_bit) {
        reg_val |= reg_mask;
    }

    writel(reg_val, (void *)(g_sysctrl_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_sysctrl_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(sysctrl_reg_set_bits);

/**
 * @brief: Determine if sysctrl is available.
 * @param: void
 * @retval: 1 is vailable, 0 is not vailable.
 */
s32 sysctrl_ready(void)
{
    if (atomic_read(&g_sysctrl_mgr.init_flag)) {
        return 1;
    }

    return 0;
}
EXPORT_SYMBOL(sysctrl_ready);
