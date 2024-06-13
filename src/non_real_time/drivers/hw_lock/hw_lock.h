// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hardware lock module interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _HI_HW_LOCK_H_
#define _HI_HW_LOCK_H_

#ifdef __KERNEL__
#include <linux/spinlock_types.h>
#include "comm_knl_api.h"
#endif /* __KERNEL__ */

#define DEV_HWLOCK_NAME       "hwlock"

#define HW_LOCK_MAX_LOOP_NUM 100
#define HW_LOCK_NUM          16
#define EX_LOCK_NUM          8
#define NOT_LOCKED           0
#define LOCKED               1
#define TIMEOUT              2
#define EXCLUSIVE_LOCK_MASK  0xFU
#define EXCLUSIVE_LOCK_IDLE  0
#define LOCK_REQ             1U
#define RELEASE_REQ          1U
#define STATUS_MASK          0x3
#define HWLOCK_BIT_NUM       2
#define EXLOCK_BIT_NUM       4
#define RELEASE_ALL_HWLOCK   0xFFFF


struct hw_lock {
    cdev_st hwlock_dev;
    unsigned int lock_count[HW_LOCK_NUM];
    struct semaphore hw_sem[HW_LOCK_NUM];
    struct semaphore ex_sem[EX_LOCK_NUM];
};

#endif
