// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * a5x_subsys module interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __A5X_SUBSYS_H__
#define __A5X_SUBSYS_H__

extern s32 a5x_init(void);
extern s32 a5x_exit(void);
extern s32 a5x_reg_read(u32 reg_offset, u32 *reg_val);
extern s32 a5x_reg_write(u32 reg_offset, u32 reg_val);

#endif /* __A5X_SUBSYS_H__ */

