// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module lp_subsys external interface
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
#include <linux/delay.h>
#include "lp_subsys_csr_addr_define.h"
#include "m_reset.h"
#include "lp_subsys.h"

#define LP_SUBSYS_REG_SIZE 0x1000

struct lp_subsys_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

static struct lp_subsys_mgr g_lpsubsys_mgr;

/* lp_subsys_init/exit should not be exposed to other modules */

/**
 * @brief: initialize lp_subsys resource.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_init(void)
{
    if (atomic_read(&g_lpsubsys_mgr.init_flag)) {
        return 0;
    }

    g_lpsubsys_mgr.virt_base = (unsigned long)ioremap(LP_SUBSYS_CSR_BASE_ADDR, LP_SUBSYS_REG_SIZE);
    if (!g_lpsubsys_mgr.virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_lpsubsys_mgr.lock);
    atomic_set(&g_lpsubsys_mgr.init_flag, 1);

    return 0;
}

/**
 * @brief: release lp_subsys resource.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_exit(void)
{
    if (atomic_read(&g_lpsubsys_mgr.init_flag)) {
        atomic_set(&g_lpsubsys_mgr.init_flag, 0);
        iounmap((void *)g_lpsubsys_mgr.virt_base);
    }

    return 0;
}

/**
 * @brief: atomic read operation for lp_subsys register.
 * @param: reg_offset: register offset.
 *         reg_val: the read val.
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_reg_read(u32 reg_offset, u32 *reg_val)
{
    unsigned long irqflags = 0;

    if (reg_val == NULL) {
        return -EINVAL;
    }

    if (atomic_read(&g_lpsubsys_mgr.init_flag) == 0 || g_lpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_lpsubsys_mgr.lock, irqflags);
    *reg_val = readl((void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_lpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(lp_subsys_reg_read);

/**
 * @brief: atomic write operation for lp_subsys register.
 * @param: reg_offset: register offset.
 *         reg_val: the write val.
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_reg_write(u32 reg_offset, u32 reg_val)
{
    unsigned long irqflags = 0;

    if (atomic_read(&g_lpsubsys_mgr.init_flag) == 0 || g_lpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_lpsubsys_mgr.lock, irqflags);
    writel(reg_val, (void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_lpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(lp_subsys_reg_write);

/**
 * @brief: Atomic read write operation for lp_subsys register.
 *         the entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the LP SUBSYS based on MASK.
 *         reg_val &= ~reg_mask
 *         reg_val |= (val & reg_mask);
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 *         val: register value, the function only cares about the bit specified by reg_mask.
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val)
{
    unsigned long irqflags = 0;
    u32 reg_val;

    if (atomic_read(&g_lpsubsys_mgr.init_flag) == 0 || g_lpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_lpsubsys_mgr.lock, irqflags);
    reg_val = readl((void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    reg_val &= ~reg_mask;
    reg_val |= (val & reg_mask);
    writel(reg_val, (void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_lpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(lp_subsys_reg_read_write);

/**
 * @brief: The operation on the lp_subsys register is an atomic read-modify-write operation.
 *         The entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the LP SUBSYS based on MASK.
 *         If the mask is 0xa and set_bit = 1, then it sets BIT1 and BIT3.
 *         If the mask is 0x1 and set_bit = 0, then it clears BIT0.
 *         If the mask is 0x5 and set_bit = 0, then it clears BIT0 and BIT2.
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit)
{
    unsigned long irqflags = 0;
    u32 reg_val;

    if (atomic_read(&g_lpsubsys_mgr.init_flag) == 0 || g_lpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_lpsubsys_mgr.lock, irqflags);
    reg_val = readl((void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    reg_val &= ~reg_mask;

    if (set_bit) {
        reg_val |= reg_mask;
    }

    writel(reg_val, (void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_lpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(lp_subsys_reg_set_bits);

/**
 * @brief: Determine if lp_subsys is available.
 * @param: void
 * @retval: 1 is vailable, 0 is not vailable.
 */
s32 lp_subsys_ready(void)
{
    if (atomic_read(&g_lpsubsys_mgr.init_flag)) {
        return 1;
    } else {
        return 0;
    }
}
EXPORT_SYMBOL(lp_subsys_ready);

/**
 * @brief: Perform a software reset on the lp_subsys module.
 * @param: reg_offset: register offset.
 *         mask: register mask.
 * @retval: 0 is success, other is failure.
 */
s32 lp_subsys_srst(u32 reg_offset, u32 mask)
{
    unsigned long irqflags = 0;
    u32 rst_dreq_off;

    if (atomic_read(&g_lpsubsys_mgr.init_flag) == 0 || g_lpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    if ((reg_offset < RESET_REG_MIN_OFFSET)
        || (reg_offset > RESET_REG_MAX_OFFSET)
        || ((reg_offset % (sizeof(u32))) != 0)) {
            return -EINVAL;
    }

    rst_dreq_off = (reg_offset - LP_SUBSYS_CSR_LPSC_SRST_REQ0_OFFSET_ADDR);

    spin_lock_irqsave(&g_lpsubsys_mgr.lock, irqflags);

    writel(mask, (void *)(g_lpsubsys_mgr.virt_base + reg_offset));
    udelay(20);
    /* Reset, write to the reset register corresponding to the MASK */
    reg_offset = LP_SUBSYS_CSR_LPSC_RESET_DREQ0_OFFSET_ADDR + rst_dreq_off;
    writel(mask, (void *)(g_lpsubsys_mgr.virt_base + reg_offset));

    spin_unlock_irqrestore(&g_lpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(lp_subsys_srst);