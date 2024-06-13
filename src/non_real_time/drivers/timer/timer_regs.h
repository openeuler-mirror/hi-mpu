// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * timer driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __TIMER_REGS_H__
#define __TIMER_REGS_H__

/* Register Offset */
#define TIMER_LOAD_L_REG    (0x0UL)
#define TIMER_LOAD_H_REG    (0x4UL)
#define TIMER_VALUE_L_REG   (0x8UL)
#define TIMER_VALUE_H_REG   (0xcUL)
#define TIMER_CTRL_REG      (0x10UL)
#define TIMER_INTCLR_REG    (0x14UL)
#define TIMER_RIS_REG       (0x18UL)
#define TIMER_MIS_REG       (0x1cUL)
#define TIMER_BGLOAD_L_REG  (0x20UL)
#define TIMER_BGLOAD_H_REG  (0x24UL)
#define TIMER_REG_OFF_MAX   (TIMER_BGLOAD_H_REG)

/* Register bits define */
/* Timer ctrl */
#define TIMER_CTRL_EN               BIT(7)
#define TIMER_CTRL_MODE             BIT(6)
#define TIMER_CTRL_INTEN            BIT(5)
#define TIMER_CTRL_PREDIV_SHIFT     2
#define TIMER_CTRL_PREDIV           (0x3 << TIMER_CTRL_PREDIV_SHIFT)
#define TIMER_CTRL_SZ               BIT(1)
#define TIMER_CTRL_ONESHOT          BIT(0)
#define TIMER_CTRL_INVAL            (~0U)

#endif /* __TIMER_REGS_H__ */