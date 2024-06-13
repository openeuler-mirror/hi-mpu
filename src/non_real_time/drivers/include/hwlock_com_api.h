// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hardware lock module interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _HWLOCK_COM_API_H_
#define _HWLOCK_COM_API_H_

/* used hw_lock number & hw_lock status */
#define A55_LOCKED           1
#define M3_LOCKED            2

#define EMMC_CTLR_LOCK_ID    2
#define DMAC_HARD_LOCK_ID    3

/*
 * Hardware lock status: 0: unlocked; 1: A55 lock; 2: M3 lock; 3: reserved;
 * Other values: The status query fails.
 */
s32 hw_query_lock_status(u32 lock_id);

/*
 * Obtaining the hardware lock. 0: Obtaining the lock succeeded.
 * Other: Obtaining the lock failed. Do not use this parameter in the interrupt context.
 */
s32 hw_acquire_hard_lock(u32 lock_id);

/* Release a hardware lock. Do not use it for interrupt contexts. */
void hw_release_hard_lock(u32 lock_id);

/*
 * Obtaining the exclusive lock. 0: Obtaining the lock succeeded.
 * Other: Failed to obtain the lock. Do not use it for interrupt context.
 */
s32 hw_acquire_exclusive_lock(u32 lock_id, u32 value);

/* Releasing an exclusive lock */
void hw_release_exclusive_lock(u32 lock_id);

#endif
