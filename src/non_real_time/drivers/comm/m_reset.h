// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Chip module reset driver header fil
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _M_RESET_H_
#define _M_RESET_H_

#define RESET_REG_NUM 3

#define RESET_REG_MIN_OFFSET 0x4
#define RESET_REG_MAX_OFFSET 0xc

/* m_reset module initialization */
s32 m_reset_init(void);

/* m_reset module deinitializtion */
s32 m_reset_exit(void);

#endif /* _M_RESET_H_ */

