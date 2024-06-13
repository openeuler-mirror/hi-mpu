// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __KCS_C_UNION_DEFINE_H__
#define __KCS_C_UNION_DEFINE_H__

/* Define the union u_kcs_data_in */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int data_in : 8;     /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_kcs_data_in;

/* Define the union u_kcs_data_out */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int data_out : 8;    /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_kcs_data_out;

/* Define the union u_kcs_commmand */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int command : 8;     /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_kcs_commmand;

/* Define the union u_kcs_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int obf : 1;         /* [0] */
        unsigned int ibf : 1;         /* [1] */
        unsigned int kcs_sms_ant : 1; /* [2] */
        unsigned int com_datan : 1;   /* [3] */
        unsigned int oem1 : 1;        /* [4] */
        unsigned int oem2 : 1;        /* [5] */
        unsigned int s1_s0 : 2;       /* [7..6] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_kcs_status;

/* Define the union u_kcs_bmc_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bmc_int_en : 1;  /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_kcs_bmc_int_en;

#endif
