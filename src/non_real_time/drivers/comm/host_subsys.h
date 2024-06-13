// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * common hp_subsys module header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HOST_SUBSYS_H__
#define __HOST_SUBSYS_H__

extern s32 host_subsys_init(void);
extern s32 host_subsys_exit(void);
extern s32 host_subsys_reg_read(u32 reg_offset, u32 *reg_val);
extern s32 host_subsys_reg_write(u32 reg_offset, u32 reg_val);

#endif /* __HOST_SUBSYS_H__ */

