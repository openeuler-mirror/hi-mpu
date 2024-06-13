// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * io multiplex interface
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
#include "io_config_t_addr_define.h"
#include "io_config_r_addr_define.h"
#include "io_config.h"

#define IO_CONFIG_REG_SIZE 0x1000
#define IO_CONFIG_TYPE_CNT 2

struct io_config_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

struct io_config_mgr g_io_mgr[IO_CONFIG_TYPE_CNT];

static s32 io_config_init_by_type(u32 type)
{
    if (atomic_read(&g_io_mgr[type].init_flag)) {
        return 0;
    }

    if (type == IO_CONFIG_R) {
        g_io_mgr[type].virt_base = (unsigned long)ioremap(IO_CONFIG_R_BASE_ADDR, IO_CONFIG_REG_SIZE);
    } else {
        g_io_mgr[type].virt_base = (unsigned long)ioremap(IO_CONFIG_T_BASE_ADDR, IO_CONFIG_REG_SIZE);
    }

    if (!g_io_mgr[type].virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_io_mgr[type].lock);
    atomic_set(&g_io_mgr[type].init_flag, 1);

    return 0;
}

static s32 io_config_exit_by_type(u32 type)
{
    if (atomic_read(&g_io_mgr[type].init_flag)) {
        atomic_set(&g_io_mgr[type].init_flag, 0);

        if (g_io_mgr[type].virt_base) {
            iounmap((void *)g_io_mgr[type].virt_base);
        }
    }

    return 0;
}

/**
 * @brief: release io_config resource
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 io_config_exit(void)
{
    s32 ret;

    ret = io_config_exit_by_type(IO_CONFIG_R);
    if (ret) {
        return ret;
    }

    return io_config_exit_by_type(IO_CONFIG_T);
}

/**
 * @brief: initialize io_config resource
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 io_config_init(void)
{
    s32 ret;

    ret = io_config_init_by_type(IO_CONFIG_R);
    if (ret) {
        goto err_out;
    }

    ret = io_config_init_by_type(IO_CONFIG_T);
    if (ret) {
        goto err_out;
    }

    return 0;

err_out:
    io_config_exit();

    return ret;
}

static s32 io_config_reg_read(u32 type, u32 reg_offset, u32 *reg_val)
{
    unsigned long irqflags = 0;

    if (reg_val == NULL) {
        return -EINVAL;
    }

    if (atomic_read(&g_io_mgr[type].init_flag) == 0 || g_io_mgr[type].virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_io_mgr[type].lock, irqflags);
    *reg_val = readl((void *)(g_io_mgr[type].virt_base + reg_offset));
    spin_unlock_irqrestore(&g_io_mgr[type].lock, irqflags);

    return 0;
}

/**
 * @brief: atomic read operation for io_config_r register
 * @param: reg_offset: register offset.
 *         reg_val: the read value.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_r_reg_read(u32 reg_offset, u32 *reg_val)
{
    return io_config_reg_read(IO_CONFIG_R, reg_offset, reg_val);
}
EXPORT_SYMBOL(io_config_r_reg_read);

/**
 * @brief: atomic read operation for io_config_t register
 * @param: reg_offset: register offset.
 *         reg_val: the read value.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_t_reg_read(u32 reg_offset, u32 *reg_val)
{
    return io_config_reg_read(IO_CONFIG_T, reg_offset, reg_val);
}
EXPORT_SYMBOL(io_config_t_reg_read);

static s32 io_config_reg_write(u32 type, u32 reg_offset, u32 reg_val)
{
    unsigned long irqflags = 0;

    if (atomic_read(&g_io_mgr[type].init_flag) == 0 || g_io_mgr[type].virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_io_mgr[type].lock, irqflags);
    writel(reg_val, (void *)(g_io_mgr[type].virt_base + reg_offset));
    spin_unlock_irqrestore(&g_io_mgr[type].lock, irqflags);

    return 0;
}

/**
 * @brief: atomic write operation for io_config_r register
 * @param: reg_offset: register offset.
 *         reg_val: the write value.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_r_reg_write(u32 reg_offset, u32 reg_val)
{
    return io_config_reg_write(IO_CONFIG_R, reg_offset, reg_val);
}
EXPORT_SYMBOL(io_config_r_reg_write);

/**
 * @brief: atomic write operation for io_config_t register
 * @param: reg_offset: register offset.
 *         reg_val: the write value.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_t_reg_write(u32 reg_offset, u32 reg_val)
{
    return io_config_reg_write(IO_CONFIG_T, reg_offset, reg_val);
}
EXPORT_SYMBOL(io_config_t_reg_write);

static s32 io_config_reg_read_write(u32 type, u32 reg_offset, u32 reg_mask, u32 val)
{
    unsigned long irqflags = 0;
    u32 reg_val;

    if (atomic_read(&g_io_mgr[type].init_flag) == 0 || g_io_mgr[type].virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_io_mgr[type].lock, irqflags);
    reg_val = readl((void *)(g_io_mgr[type].virt_base + reg_offset));
    reg_val &= ~reg_mask;
    reg_val |= (val & reg_mask);
    writel(reg_val, (void *)(g_io_mgr[type].virt_base + reg_offset));
    spin_unlock_irqrestore(&g_io_mgr[type].lock, irqflags);

    return 0;
}

/**
 * @brief: Atomic read write operation for io_config_r register.
 *         the entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the LP SUBSYS based on MASK.
 *         reg_val &= ~reg_mask
 *         reg_val |= (val & reg_mask);
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 *         val: register value, the function only cares about the bit specified by reg_mask.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_r_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val)
{
    return io_config_reg_read_write(IO_CONFIG_R, reg_offset, reg_mask, val);
}
EXPORT_SYMBOL(io_config_r_reg_read_write);

/**
 * @brief: Atomic read write operation for io_config_t register.
 *         the entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the LP SUBSYS based on MASK.
 *         reg_val &= ~reg_mask
 *         reg_val |= (val & reg_mask);
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 *         val: register value, the function only cares about the bit specified by reg_mask.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_t_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val)
{
    return io_config_reg_read_write(IO_CONFIG_T, reg_offset, reg_mask, val);
}
EXPORT_SYMBOL(io_config_t_reg_read_write);

static s32 io_config_reg_set_bits(u32 type, u32 reg_offset, u32 reg_mask, u32 set_bit)
{
    unsigned long irqflags = 0;
    u32 reg_val;

    if (atomic_read(&g_io_mgr[type].init_flag) == 0 || g_io_mgr[type].virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_io_mgr[type].lock, irqflags);
    reg_val = readl((void *)(g_io_mgr[type].virt_base + reg_offset));
    reg_val &= ~reg_mask;

    if (set_bit) {
        reg_val |= reg_mask;
    }

    writel(reg_val, (void *)(g_io_mgr[type].virt_base + reg_offset));
    spin_unlock_irqrestore(&g_io_mgr[type].lock, irqflags);

    return 0;
}

/**
 * @brief: The operation on the io_config_r register is an atomic read-modify-write operation.
 *         The entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the LP SUBSYS based on MASK.
 *         If the mask is 0xa and set_bit = 1, then it sets BIT1 and BIT3.
 *         If the mask is 0x1 and set_bit = 0, then it clears BIT0.
 *         If the mask is 0x5 and set_bit = 0, then it clears BIT0 and BIT2.
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_r_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit)
{
    return io_config_reg_set_bits(IO_CONFIG_R, reg_offset, reg_mask, set_bit);
}
EXPORT_SYMBOL(io_config_r_reg_set_bits);

/**
 * @brief: The operation on the io_config_t register is an atomic read-modify-write operation.
 *         The entire process is locked using spin_lock_irqsave, making it an atomic operation.
 *         This function is used to manipulate certain BIT positions of the LP SUBSYS based on MASK.
 *         If the mask is 0xa and set_bit = 1, then it sets BIT1 and BIT3.
 *         If the mask is 0x1 and set_bit = 0, then it clears BIT0.
 *         If the mask is 0x5 and set_bit = 0, then it clears BIT0 and BIT2.
 * @param: reg_offset: register offset.
 *         reg_mask: register mask.
 * @retval: 0 is success, other is failure.
 */
s32 io_config_t_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit)
{
    return io_config_reg_set_bits(IO_CONFIG_T, reg_offset, reg_mask, set_bit);
}
EXPORT_SYMBOL(io_config_t_reg_set_bits);

/**
 * @brief: determine if io_config is available.
 * @param: void
 * @retval: 1 is available, other is not available.
 */
s32 io_config_ready(void)
{
    if ((atomic_read(&g_io_mgr[IO_CONFIG_R].init_flag)) && (atomic_read(&g_io_mgr[IO_CONFIG_T].init_flag))) {
        return 1;
    }

    return 0;
}
EXPORT_SYMBOL(io_config_ready);
