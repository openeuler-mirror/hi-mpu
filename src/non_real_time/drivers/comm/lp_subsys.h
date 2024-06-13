// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module lp_subsys external interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __LP_SUBSYS_H__
#define __LP_SUBSYS_H__

extern s32 lp_subsys_init(void);
extern s32 lp_subsys_exit(void);
/*
 * lp_subsys_reg_read/write uses spin_lock_irqsave()
 * and spin_unlock_irqrestore() to protect register access for racing conditions
 */
extern s32 lp_subsys_reg_read(u32 offset, u32 *reg_val);
extern s32 lp_subsys_reg_write(u32 offset, u32 reg_val);
extern s32 lp_subsys_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val);
extern s32 lp_subsys_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit);
extern s32 lp_subsys_srst(u32 reg_offset, u32 mask);
extern s32 lp_subsys_ready(void);
#endif /* __LP_SUBSYS_H__ */
