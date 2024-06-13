// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sysctrl module interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SYSCTRL_H__
#define __SYSCTRL_H__

extern s32 sysctrl_init(void);
extern s32 sysctrl_exit(void);
/*
 * sysctrl_reg_read/write uses spin_lock_irqsave/spin_unlock_irqrestor
 * to protect from racing conditions
 */
extern s32 sysctrl_reg_read(u32 offset, u32 *reg_val);
extern s32 sysctrl_reg_write(u32 offset, u32 reg_val);
extern s32 sysctrl_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val);
extern s32 sysctrl_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit);
extern s32 sysctrl_ready(void);
#endif /* __SYSCTRL_H__ */
