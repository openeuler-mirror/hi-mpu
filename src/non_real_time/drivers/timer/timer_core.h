// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * timer driver core API file
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __TIMER_CORE_H__
#define __TIMER_CORE_H__

#include "timer.h"
/* Timer core mgmt API */
extern void hitimer_init(struct hisi_timer *hitimer, u32 timer_id);
extern void hitimer_deinit(struct hisi_timer *hitimer);
extern s32 hitimer_setup_device(struct hisi_timer *hitimer, s32 v_irq, phys_addr_t phys_base,
                                unsigned long phys_size, u32 clk_sel);
extern s32 hitimer_close_device(struct hisi_timer *hitimer);
extern s32 hitimer_reference(struct hisi_timer *hitimer, const char *module_name);
extern s32 hitimer_unreference(struct hisi_timer *hitimer, const char *module_name);
extern s32 hitimer_query_reference(struct hisi_timer *hitimer, u32 *is_held, char *held_module, size_t dest_max);

#endif /* __TIMER_CORE_H__ */
