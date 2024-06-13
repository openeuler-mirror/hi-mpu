// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * peci module sdk interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _PECI_H_
#define _PECI_H_

#define PECI_DEV_NAME "peci"

/* Maximum space reserved for PECI registers */
#define PECI_PHY_ADDR_SIZE 0x1000

#define PECI_SPEED 0x300

#define PECI_MAX_LOOP_NUM 100 /* Maximum number of cycles */

#define PECI_READ_TIMEOUT 1000 /* ms */

#define PECI_INT_STATE_READY 1
#define PECI_INT_STATE_IDLE  0

#define PECI_MIN_WR_LEN 3

#define PECI_MIN_SPEED 12
#define PECI_MAX_SPEED 12500

#define PECI_BUS_READY_MASK 0x80
#define PECI_INT_STATE_MASK 0x20

typedef union {
    struct {
        unsigned int    start                 : 1; /* [0]  */
        unsigned int    abort                 : 1; /* [1]  */
        unsigned int    cfg_err               : 1; /* [2]  */
        unsigned int    tbit_err              : 1; /* [3]  */
        unsigned int    reserved_0            : 1; /* [4]  */
        unsigned int    int0                  : 1; /* [5]  */
        unsigned int    reserved_1            : 1; /* [6]  */
        unsigned int    rdy                   : 1; /* [7]  */
        unsigned int    reserved_2            : 24; /* [31..8]  */
    } bits;
    unsigned int    val;
} u_peci_ctrl;

typedef union {
    struct {
        unsigned int    normal_irq_en         : 1; /* [0]  */
        unsigned int    cfg_err_irq_en        : 1; /* [1]  */
        unsigned int    tbit_err_irq_en       : 1; /* [2]  */
        unsigned int    peci_irq_en           : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;
    unsigned int    val;
} u_peci_int_en;

typedef union {
    struct {
        unsigned int    typ_num               : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;
    unsigned int    val;
} u_host_speed;


typedef struct {
    cdev_st peci_dev;
    void __iomem *peci_map_addr;
    s32 peci_int_state;
    struct semaphore peci_access_sem;
} peci_mgr_s;

#endif
