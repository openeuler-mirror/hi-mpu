// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __WDG_C_UNION_DEFINE_H__
#define __WDG_C_UNION_DEFINE_H__


/* Define the union u_wdg_control */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    inten                 : 1; /* [0]  */
        unsigned int    resen                 : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdg_control;

/* Define the union u_wdg_ris */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdogris               : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdg_ris;

/* Define the union u_wdg_mis */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdogmis               : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdg_mis;


#endif /* __WDG_C_UNION_DEFINE_H__ */
