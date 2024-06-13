// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Interface for obtaining the clock of the BMCRTC
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _BMCRTC_USR_API_H_
#define _BMCRTC_USR_API_H_

#define BMCRTC_DEV_NAME "/dev/bmcrtc"

#define RTC_IOC_MAGIC 'r'

#define RTC_IOCIG_ENABLE  _IOWR(RTC_IOC_MAGIC, 0, int)
#define RTC_IOCIG_DISABLE _IOWR(RTC_IOC_MAGIC, 1, int)
#define RTC_GET_TIME      _IOWR(RTC_IOC_MAGIC, 2, int)
#define RTC_MOD_TIME      _IOWR(RTC_IOC_MAGIC, 3, int)

#define RTC_IOC_MAXNR 3

#define TOP_LIMIT 100000000 /* Maximum number of nanoseconds */

/* Time structure */
typedef struct {
    unsigned int seconds;
    unsigned int nanoseconds;
} bmcrtc_time_s;

#endif
