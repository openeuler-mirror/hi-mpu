// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * dual boot startup information interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __DBOOT_KER_H__
#define __DBOOT_KER_H__

#include "comm_knl_api.h"

#define BMC_RESET_COUNT_OFFSET       (12)
#define BMC_RESET_COUNT_MASK         (0xF << BMC_RESET_COUNT_OFFSET)
#define BMC_UNSEC_RESET_COUNT_OFFSET (8)
#define BMC_UNSEC_RESET_COUNT_MASK   (0xF << BMC_UNSEC_RESET_COUNT_OFFSET)
#define HOST_RESET_COUNT_OFFSET      (8)
#define HOST_RESET_COUNT_MASK        (0xF << HOST_RESET_COUNT_OFFSET)
#define CHIP_RESET_COUNT_OFFSET      (12)
#define CHIP_RESET_COUNT_MASK        (0xF << CHIP_RESET_COUNT_OFFSET)

#define BOOT_CNT0_SHIFT		8
#define BOOT_CNT1_SHIFT		16
#define GET_BOOT_CNT_OPS 	0xFF
#define UBOOT_VERSION_LEN 	48

typedef struct {
    cdev_st *cdev; /* Pointer to character device attributes */
    struct semaphore sema;
    s8 uboot0_ver[UBOOT_VERSION_LEN]; /* U-Boot version read from the DTB */
    s8 uboot1_ver[UBOOT_VERSION_LEN];
    u32 boot_area;      /* Record the zone from which the system is started */
    void *np_uboot_ver; /* Record the DTB node of the U-Boot version. */
    void *np_l0fw_ver;
    void *np_l1fw_ver;
} dboot_ctrl_s;

#endif
