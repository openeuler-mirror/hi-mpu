// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __BT_C_UNION_DEFINE_H__
#define __BT_C_UNION_DEFINE_H__

/* Define the union u_bt_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_wr_ptr : 1;  /* [0] */
        unsigned int clr_rd_ptr : 1;  /* [1] */
        unsigned int h2b_ant : 1;     /* [2] */
        unsigned int b2h_ant : 1;     /* [3] */
        unsigned int bt_sms_ant : 1;  /* [4] */
        unsigned int oem0 : 1;        /* [5] */
        unsigned int h_busy : 1;      /* [6] */
        unsigned int b_busy : 1;      /* [7] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_bt_ctrl;

/* Define the union u_bt_bmc2host */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bmc2host : 8;    /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bt_bmc2host;

/* Define the union u_bt_host2bmc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int host2bmc : 8;    /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bt_host2bmc;

/* Define the union u_bt_intmask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int b2h_irq_en : 1;  /* [0] */
        unsigned int b2h_irq : 1;     /* [1] */
        unsigned int reserved_0 : 1;  /* [2] */
        unsigned int reserved_1 : 1;  /* [3] */
        unsigned int reserved_2 : 1;  /* [4] */
        unsigned int reserved_3 : 2;  /* [6..5] */
        unsigned int bmc_hwrst : 1;   /* [7] */
        unsigned int reserved_4 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bt_intmask;

/* Define the union u_bmc_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int h2b_int_en : 1;  /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_bmc_int_en;

#endif
