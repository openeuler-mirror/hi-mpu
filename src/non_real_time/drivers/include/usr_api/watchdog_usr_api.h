// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * watchdog user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _WATCHDOG_USR_API_H_
#define _WATCHDOG_USR_API_H_


#define DEV_WDG_NAME                "/dev/wdt"

/* use 'w' as magic number */
#define WDG_IOC_MAGIC               'w'
#define WDG_CMD_MAX_NR              6

/* User feeds the watchdog. */
#define WDG_CMD_FEED_BY_USER        _IOW(WDG_IOC_MAGIC, 0, unsigned int)

/* Kernel driver watchdog */
#define WDG_CMD_FEED_BY_KERNEL      _IOW(WDG_IOC_MAGIC, 1, unsigned int)

/* Clearing the Watchdog */
#define WDG_CMD_CLR                 _IOW(WDG_IOC_MAGIC, 2, unsigned int)

/* Stop feeding the dog */
#define WDG_CMD_STOP                _IOW(WDG_IOC_MAGIC, 3, unsigned int)

/* Enabling/Disabling the Watchdog */
#define WDG_CMD_ENABLE              _IOW(WDG_IOC_MAGIC, 4, wdg_enable_s)

/* Waiting for the watchdog timed out */
#define WDG_CMD_WAIT                _IOW(WDG_IOC_MAGIC, 5, unsigned int)

/* Simulate watchdog timeout */
#define WDG_CMD_TIMEOUT             _IOW(WDG_IOC_MAGIC, 6, unsigned int)

typedef struct {
    unsigned int wdg_id;
    unsigned int enable;
}wdg_enable_s;

#endif

