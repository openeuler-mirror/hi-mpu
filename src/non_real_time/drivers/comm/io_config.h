// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * io multiplex header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

#define IO_CONFIG_R 0
#define IO_CONFIG_T 1

extern s32 io_config_init(void);
extern s32 io_config_exit(void);
/*
 * io_config_r_reg_read/write uses spin_lock_irqsave()
 * and spin_unlock_irqrestore() to protect register access for racing conditions
 */
extern s32 io_config_r_reg_read(u32 offset, u32 *reg_val);
extern s32 io_config_t_reg_read(u32 offset, u32 *reg_val);

extern s32 io_config_r_reg_write(u32 offset, u32 reg_val);
extern s32 io_config_t_reg_write(u32 offset, u32 reg_val);

extern s32 io_config_r_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val);
extern s32 io_config_t_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val);

extern s32 io_config_r_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit);
extern s32 io_config_t_reg_set_bits(u32 reg_offset, u32 reg_mask, u32 set_bit);

extern s32 io_config_ready(void);
#endif /* __IO_CONFIG_H__ */
