// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HOST_WDI_C_UNION_DEFINE_H__
#define __HOST_WDI_C_UNION_DEFINE_H__


/* Define the union u_wdi_sta_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tu                    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 4; /* [6..3]  */
        unsigned int    wpto                  : 1; /* [7]  */
        unsigned int    reserved_1            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_sta_reg;

/* Define the union u_wdi_irm_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ta                    : 3; /* [2..0]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    pim                   : 3; /* [6..4]  */
        unsigned int    reserved_1            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_irm_reg;

/* Define the union u_wdi_sp1_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdi_sp1_reg           : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_sp1_reg;

/* Define the union u_wdi_sp2_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdi_sp2_reg           : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_sp2_reg;

/* Define the union u_wdi_ien_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    irq_num               : 4; /* [3..0]  */
        unsigned int    psmien                : 1; /* [4]  */
        unsigned int    reserved_0            : 2; /* [6..5]  */
        unsigned int    wdiie                 : 1; /* [7]  */
        unsigned int    reserved_1            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_ien_reg;

/* Define the union u_wdi_msc_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 7; /* [6..0]  */
        unsigned int    fpi                   : 1; /* [7]  */
        unsigned int    reserved_1            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_msc_reg;

/* Define the union u_wdi_idf_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wd_idf                : 8; /* [7..0]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wdi_idf_reg;


#endif /* __HOST_WDI_C_UNION_DEFINE_H__ */
