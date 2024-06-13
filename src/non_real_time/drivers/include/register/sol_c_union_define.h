// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SOL_C_UNION_DEFINE_H__
#define __SOL_C_UNION_DEFINE_H__


/* Define the union u_sol_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sol_en                : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sol_start;

/* Define the union u_tx_ddr_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tx_ddr_size           : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_tx_ddr_size;

/* Define the union u_rx_ddr_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rx_ddr_size           : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_rx_ddr_size;

/* Define the union u_fifo_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    fifo_rx_cnt           : 6; /* [5..0]  */
        unsigned int    fifo_empty_rx         : 1; /* [6]  */
        unsigned int    fifo_full_rx          : 1; /* [7]  */
        unsigned int    fifo_tx_cnt           : 6; /* [13..8]  */
        unsigned int    fifo_empty_tx         : 1; /* [14]  */
        unsigned int    fifo_full_tx          : 1; /* [15]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_fifo_status;

/* Define the union u_sol_irq_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sol_irq_en            : 1; /* [0]  */
        unsigned int    sol_irq               : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sol_irq_st;

/* Define the union u_re_time_th */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    re_time_th            : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_re_time_th;


#endif /* __SOL_C_UNION_DEFINE_H__ */
