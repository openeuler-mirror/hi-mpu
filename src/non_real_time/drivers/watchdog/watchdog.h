// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * watchdog driver header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#include "comm_knl_api.h"

#define DEV_WATCHDOG_NAME "wdt"

#define WDG_MAX_NUM 6
#define WDG_TIMEOUT 150000U
#define WDG_FEED_TIME (30000UL)
#define WDG_APP_WAIT_COUNTER (120000UL / WDG_FEED_TIME)
#define WDG_DELAY_RESET_TIME 2000U

typedef struct {
    u32 virq;
    void __iomem *map_addr;
    u32 feed_by_kernel;              /* 0: The user feeds the watchdog. 1: The kernel feeds the watchdog. */
    u32 stop_feed;                   /* Indicates whether to stop feeding the watchdog. 0: no; 1: yes. */
    struct semaphore cmd_access_sem; /* Command mutex semaphore */
    struct hisi_timer *feed_timer;   /* Dog feeding timer */
    u32 wait_counter;                /* Watchdog feeding timeout count */
    wait_queue_head_t wait_queue;    /* Waiting for the watchdog timeout queue */
} wdg_priv_info_s;

#endif /* _WATCHDOG_H */
