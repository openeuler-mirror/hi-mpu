// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * localbus driver header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _LOCAL_BUS_KER__H_
#define _LOCAL_BUS_KER__H_

#include "local_bus_usr_api.h"
#define CS_NUM 3

#define CS1 0
#define CS2 1
#define CS3 2
#define CS0 3
#define CF_ADDR_OFFSET0 0
#define CF_ADDR_OFFSET1 1

#define L_BUS_PHY_MAP_LEN 0x1000

#define CS0_PHY_BASE_ADDR 0x30000000 // 0x50000000
#define CS1_PHY_BASE_ADDR 0x40000000 // 0x50000000
#define CS2_PHY_BASE_ADDR 0x50000000 // 0x60000000
#define CS3_PHY_BASE_ADDR 0x58000000 // 0x68000000

#define CS_MAP_LEN  0x2000000  /* 32M */
#define CS0_MAP_LEN 0x10000000 /* 256M */
#define CS1_MAP_LEN 0x10000000 /* 256M */
#define CS2_MAP_LEN 0x8000000  /* 128M */
#define CS3_MAP_LEN 0x8000000  /* 128M */

#define CS_MAX_DATA_LEN 0x1000 /* Maximum amount of data that can be accessed at a time */

#define CS_WIDTH_8  0
#define CS_WIDTH_16 1
#define CS_WIDTH_32 2

#define OP_TYPE_BYTES 0
#define OP_TYPE_BITS  1

#define ENABLE  1
#define DISABLE 0

#define MASK_SIZE   32
#define OFFSET_SIZE 4

#define ALE_BIT       25U
#define ADDR2DATA_BIT 29U

#define WIDTH8_BYTE_NUM  1
#define WIDTH16_BYTE_NUM 2
#define WIDTH32_BYTE_NUM 4

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0               : 4;  /* [3..0]  */
        unsigned int    cf_wr_prot_intmsk_3      : 1;  /* [4]  */
        unsigned int    cf_wr_prot_intmsk_2      : 1;  /* [5]  */
        unsigned int    cf_wr_prot_intmsk_1      : 1;  /* [6]  */
        unsigned int    cf_wr_prot_intmsk_0      : 1;  /* [7]  */
        unsigned int    reserved_1               : 4;  /* [11..8]  */
        unsigned int    cf_rdy_overtime_intmsk_3 : 1;  /* [12]  */
        unsigned int    cf_rdy_overtime_intmsk_2 : 1;  /* [13]  */
        unsigned int    cf_rdy_overtime_intmsk_1 : 1;  /* [14]  */
        unsigned int    cf_rdy_overtime_intmsk_0 : 1;  /* [15]  */
        unsigned int    reserved_2               : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lb_intmsk;

/* Define the union u_lb_intsts */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0               : 4;  /* [3..0]  */
        unsigned int    cf_wr_prot_intsts_3      : 1;  /* [4]  */
        unsigned int    cf_wr_prot_intsts_2      : 1;  /* [5]  */
        unsigned int    cf_wr_prot_intsts_1      : 1;  /* [6]  */
        unsigned int    cf_wr_prot_intsts_0      : 1;  /* [7]  */
        unsigned int    reserved_1               : 4;  /* [11..8]  */
        unsigned int    cf_rdy_overtime_intsts_3 : 1;  /* [12]  */
        unsigned int    cf_rdy_overtime_intsts_2 : 1;  /* [13]  */
        unsigned int    cf_rdy_overtime_intsts_1 : 1;  /* [14]  */
        unsigned int    cf_rdy_overtime_intsts_0 : 1;  /* [15]  */
        unsigned int    reserved_2               : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lb_intsts;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0             : 4; /* [3..0]  */
        unsigned int    cf_wr_prot_rint_3      : 1; /* [4]  */
        unsigned int    cf_wr_prot_rint_2      : 1; /* [5]  */
        unsigned int    cf_wr_prot_rint_1      : 1; /* [6]  */
        unsigned int    cf_wr_prot_rint_0      : 1; /* [7]  */
        unsigned int    reserved_1             : 4; /* [11..8]  */
        unsigned int    cf_rdy_overtime_rint_3 : 1; /* [12]  */
        unsigned int    cf_rdy_overtime_rint_2 : 1; /* [13]  */
        unsigned int    cf_rdy_overtime_rint_1 : 1; /* [14]  */
        unsigned int    cf_rdy_overtime_rint_0 : 1; /* [15]  */
        unsigned int    reserved_2             : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lb_rint;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cf_mem_size_1         : 5; /* [4..0]  */
        unsigned int    cf_data_wid_1         : 2; /* [6..5]  */
        unsigned int    cf_data_order_1       : 1; /* [7]  */
        unsigned int    cf_byte_order_1       : 1; /* [8]  */
        unsigned int    cf_rdy_mode_1         : 1; /* [9]  */
        unsigned int    cf_rdy_pol_1          : 1; /* [10]  */
        unsigned int    cf_addr_offset_1      : 1; /* [11]  */
        unsigned int    cf_lbctl_en_1         : 1; /* [12]  */
        unsigned int    cf_page_en_1          : 1; /* [13]  */
        unsigned int    cf_page_size_1        : 2; /* [15..14]  */
        unsigned int    cf_rdy_tout_en_1      : 1; /* [16]  */
        unsigned int    cf_rble_1             : 1; /* [17]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lb_cs1_ctrl;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cf_t_cdn_1            : 5; /* [4..0]  */
        unsigned int    cf_t_cwen_1           : 5; /* [9..5]  */
        unsigned int    cf_wen_pre_csn_1      : 1; /* [10]  */
        unsigned int    cf_t_awe_1            : 5; /* [15..11]  */
        unsigned int    cf_t_cswp_1           : 9; /* [24..16]  */
        unsigned int    cf_t_lale_1           : 5; /* [29..25]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lb_cs1_t_set1;

typedef struct {
    cdev_st localbus_dev;
    void __iomem *regbase;
    struct semaphore localbus_access_sem[CS_NUM];
    void __iomem *cs_io_addr[CS_NUM];
    u32 width[CS_NUM];
    u32 localbus_virq;
} localbus_mgr_s;

typedef struct {
    u32 offset;
    u32 mask;
    u32 value;
} localbus_reg_wr_s;

extern s32 localbus_set_cs_width_offset(u32 cs, u32 width, u32 offset);
extern s32 localbus_get_data(l_bus_opt_s *l_bus_opt, s32 mode);
extern s32 localbus_write_data(l_bus_opt_s *l_bus_opt, s32 mode);

#endif
