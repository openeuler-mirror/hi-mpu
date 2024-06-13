// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _BMCRTC_H_
#define _BMCRTC_H_

#define BMCRTC_DEV_NAME_KER "bmcrtc"

/* RTC REGISTER */
#define RTC_CLOCK_CFG       0x00
#define RTC_SECOND_CFG      0x04
#define RTC_NANO_SECOND_CFG 0x08
#define RTC_SWITCH          0x0c

/* CFG SWITCH */
#define RTC_ENABLE       0x00000001
#define RTC_DISABLE      0x00000000
#define RTC_50MB_SOURCE  0x00000001
#define RTC_100MB_SOURCE 0x00000000

/* MAP ADDR */
#define RTC_REG_BASE 0x0875A000
#define RTC_REG_SIZE 0x1000

typedef struct {
    cdev_st chrdev;
    unsigned long phys_base; /* BMCRTC physical reg base */
    void __iomem *virt_base; /* BMCRTC virtual reg base */
    spinlock_t lock;
} bmcrtc_info_s;

#endif
