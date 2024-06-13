// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hwlock user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _HW_LOCK_USR_API_H_
#define _HW_LOCK_USR_API_H_

#define DEV_HW_LOCK_NAME       "/dev/hwlock"

#define HWLOCK_IOC_MAGIC                'h'
#define HWLOCK_CMD_MAX_NR                3

#define HW_LOCK_CMD_CORRECTNESS_TEST    _IOWR(HWLOCK_IOC_MAGIC, 0, hw_lock_test)
#define HW_LOCK_CMD_BALABCE_TEST        _IOWR(HWLOCK_IOC_MAGIC, 1, hw_lock_test)
#define HW_EXLOCK_CMD_CORRECTNESS_TEST  _IOWR(HWLOCK_IOC_MAGIC, 2, hw_lock_test)

typedef struct {
    unsigned int times;
    unsigned int lock_id;
}hw_lock_test;

#endif