// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __MDIO_C_UNION_DEFINE_H__
#define __MDIO_C_UNION_DEFINE_H__

/* Define the union u_mdio_command_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mdio_devad : 5;       /* [4..0]  */
        unsigned int mdio_prtad : 5;       /* [9..5]  */
        unsigned int mdio_op : 2;          /* [11..10]  */
        unsigned int mdio_st : 2;          /* [13..12]  */
        unsigned int mdio_start : 1;       /* [14]  */
        unsigned int mdio_clk_sel : 1;     /* [15]  */
        unsigned int mdio_auto_scan : 1;   /* [16]  */
        unsigned int mdio_clk_sel_exp : 1; /* [17]  */
        unsigned int reserved_0 : 14;      /* [31..18]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_command_reg;

/* Define the union u_mdio_addr_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mdio_address : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;   /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_addr_reg;

/* Define the union u_mdio_wdata_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mdio_wdata : 16; /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_wdata_reg;

/* Define the union u_mdio_rdata_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mdio_rdata : 16; /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_rdata_reg;

/* Define the union u_mdio_sta_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mdio_sta : 1;    /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_sta_reg;

/* Define the union u_mdio_auto_cfg_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int auto_devad0 : 5; /* [4..0]  */
        unsigned int auto_devad1 : 5; /* [9..5]  */
        unsigned int auto_prtad : 5;  /* [14..10]  */
        unsigned int reserved_0 : 17; /* [31..15]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_auto_cfg_0;

/* Define the union u_mdio_auto_cfg_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int auto_reg0_bit_en : 16; /* [15..0]  */
        unsigned int auto_reg1_bit_en : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_auto_cfg_1;

/* Define the union u_mdio_auto_cfg_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int auto_scan_inter : 10; /* [9..0]  */
        unsigned int reserved_0 : 22;      /* [31..10]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_auto_cfg_2;

/* Define the union u_mdio_auto_intr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int phy_reg_sts_chage_int : 5; /* [4..0]  */
        unsigned int reserved_0 : 27;           /* [31..5]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_auto_intr;

/* Define the union u_mdio_auto_intr_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int phy_reg_sts_chage_int_en : 5; /* [4..0]  */
        unsigned int reserved_0 : 27;              /* [31..5]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_mdio_auto_intr_en;

#endif /* __MDIO_C_UNION_DEFINE_H__ */
