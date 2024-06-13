// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * wdi user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _WDI_USR_API_H_
#define _WDI_USR_API_H_

#define WDI_DEV_NAME    "/dev/wdi"
#define WDI_IOC_MAGIC   'w'
#define WDI_CMD_IRQ     _IOWR(WDI_IOC_MAGIC, 0, wdi_config_s)
#define WDI_IOC_MAXNR   1

/* An FRB-2 (fault-resilient booting, level 2) timeout has occurred */
#define WDI_TU_BIOS_FRB2    1

/*
 * In this mode, the timeout occurred while the watchdog timer was being used
 * by the BIOS for some purpose other than FRB-2 or OS Load Watchdog.
 */
#define WDI_TU_BIOS_POST    2

/*
 * The last reset or power cycle was caused by the timer being used to
 * watchdog’ the interval from ‘boot’ to OS up and running.
 */
#define WDI_TU_OS_LOAD      3

/*
 * This indicates that the timer was being used by System Management Software.
 * During run-time, System Management Software (SMS) starts the timer, then
 * periodically resets it to keep it from expiring
 */
#define WDI_TU_SMS_OS       4

/* Indicates that the timer was being used for an OEM-specific function */
#define WDI_TU_OEM          5

/* Pre-interrupt mode */
#define WDI_PIM_NONE        0
#define WDI_PIM_SMI         1
#define WDI_PIM_NMI         2
#define WDI_PIM_IRQ         4

/* Action after WDT timeout */
#define WDI_TA_NONE         0
#define WDI_TA_HARD_RESET   1
#define WDI_TA_POWER_DOWN   2
#define WDI_TA_POWER_CYCLE  3

#pragma pack(1)

typedef struct {
    /*
     * Set Pre-Interrupt-Reset Timer
     * It contains two bytes of information (cause of system crash), which is used for query by the host.
     */
    unsigned char sp1;
    unsigned char sp2;
    /* Timer Use(TU) */
    unsigned char tu    : 3;
    /*
     * Pre-timeout Interrupt Mode(PIM)
     * When PIM is 0 or a reserved value, the interface logic does not send interrupt signals,
     * When the PIM type is SMI, NMI, or IRQ, the corresponding interrupt message is sent through SERIRQ.
     */
    unsigned char pim   : 3;
    /* Timeout Action(TA) */
    unsigned char ta    : 3;
    /*
     * Watchdog Pre-Timeout(WPTO)
     * When the watchdog timer reaches the pre-interrupt moment, this bit is set to 1.
     * An interrupt is sent to the host according to the configuration.
     */
    unsigned char wpto  : 1;
}wdi_config_s, *p_wdi_config;

#pragma pack()

#endif

