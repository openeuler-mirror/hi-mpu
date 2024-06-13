// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * common module hp_subsys interface files
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
#include "hp_sub_csr_addr_define.h"
#include "hp_subsys.h"

#define HP_SUBSYS_REG_SIZE 0x1000

struct hp_subsys_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

static struct hp_subsys_mgr g_hpsubsys_mgr = { 0 };

/**
 * @brief: initialize hp subsys resources.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 hp_subsys_init(void)
{
    if (atomic_read(&g_hpsubsys_mgr.init_flag)) {
        return 0;
    }

    g_hpsubsys_mgr.virt_base = (unsigned long)ioremap(HP_SUB_CSR_BASE_ADDR, HP_SUBSYS_REG_SIZE);
    if (!g_hpsubsys_mgr.virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_hpsubsys_mgr.lock);
    atomic_set(&g_hpsubsys_mgr.init_flag, 1);

    return 0;
}

/**
 * @brief: release hp subsys resources
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 hp_subsys_exit(void)
{
    if (atomic_read(&g_hpsubsys_mgr.init_flag)) {
        atomic_set(&g_hpsubsys_mgr.init_flag, 0);
        iounmap((void *)g_hpsubsys_mgr.virt_base);
    }

    return 0;
}

/**
 * @brief: atomic read operation for hp subsys registers.
 * @param: reg_offset: register offset.
 *         reg_val: the read value.
 * @retval: 0 is success, other is failure.
 */
s32 hp_subsys_reg_read(u32 reg_offset, u32 *reg_val)
{
    unsigned long irqflags = 0;

    if (reg_val == NULL) {
        return -EINVAL;
    }

    if (atomic_read(&g_hpsubsys_mgr.init_flag) == 0 || g_hpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_hpsubsys_mgr.lock, irqflags);
    *reg_val = readl((void *)(g_hpsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_hpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(hp_subsys_reg_read);

/**
 * @brief: atomic write operation for hp subsys registers.
 * @param: reg_offset: register offset.
 *         reg_val: the read value.
 * @retval: 0 is success, other is failure.
 */
s32 hp_subsys_reg_write(u32 reg_offset, u32 reg_val)
{
    unsigned long irqflags = 0;

    if (atomic_read(&g_hpsubsys_mgr.init_flag) == 0 || g_hpsubsys_mgr.virt_base == 0) {
        return -ENODEV;
    }

    spin_lock_irqsave(&g_hpsubsys_mgr.lock, irqflags);
    writel(reg_val, (void *)(g_hpsubsys_mgr.virt_base + reg_offset));
    spin_unlock_irqrestore(&g_hpsubsys_mgr.lock, irqflags);

    return 0;
}
EXPORT_SYMBOL(hp_subsys_reg_write);

