// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HP_SUB_CSR_C_UNION_DEFINE_H__
#define __HP_SUB_CSR_C_UNION_DEFINE_H__


/* Define the union u_hp_srst_st1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_eth_srst_st     : 1; /* [0]  */
        unsigned int    gmac0_fifo_srst_st    : 1; /* [1]  */
        unsigned int    gmac0_macsys_srst_st  : 1; /* [2]  */
        unsigned int    gmac0_cfg_srst_st     : 1; /* [3]  */
        unsigned int    gmac0_sys_srst_st     : 1; /* [4]  */
        unsigned int    gmac0_rx_srst_st      : 1; /* [5]  */
        unsigned int    gmac0_tx_srst_st      : 1; /* [6]  */
        unsigned int    gmac1_eth_srst_st     : 1; /* [7]  */
        unsigned int    gmac1_fifo_srst_st    : 1; /* [8]  */
        unsigned int    gmac1_macsys_srst_st  : 1; /* [9]  */
        unsigned int    gmac1_cfg_srst_st     : 1; /* [10]  */
        unsigned int    gmac1_sys_srst_st     : 1; /* [11]  */
        unsigned int    gmac1_rx_srst_st      : 1; /* [12]  */
        unsigned int    gmac1_tx_srst_st      : 1; /* [13]  */
        unsigned int    gmac2_eth_srst_st     : 1; /* [14]  */
        unsigned int    gmac2_fifo_srst_st    : 1; /* [15]  */
        unsigned int    gmac2_macsys_srst_st  : 1; /* [16]  */
        unsigned int    gmac2_cfg_srst_st     : 1; /* [17]  */
        unsigned int    gmac2_sys_srst_st     : 1; /* [18]  */
        unsigned int    gmac2_rx_srst_st      : 1; /* [19]  */
        unsigned int    gmac2_tx_srst_st      : 1; /* [20]  */
        unsigned int    gmac3_eth_srst_st     : 1; /* [21]  */
        unsigned int    gmac3_fifo_srst_st    : 1; /* [22]  */
        unsigned int    gmac3_macsys_srst_st  : 1; /* [23]  */
        unsigned int    gmac3_cfg_srst_st     : 1; /* [24]  */
        unsigned int    gmac3_sys_srst_st     : 1; /* [25]  */
        unsigned int    gmac3_rx_srst_st      : 1; /* [26]  */
        unsigned int    gmac3_tx_srst_st      : 1; /* [27]  */
        unsigned int    sec_ahb_srst_st       : 1; /* [28]  */
        unsigned int    ptp_srst_st           : 1; /* [29]  */
        unsigned int    sec_srst_st           : 1; /* [30]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_srst_st1;

/* Define the union u_hp_srst_st2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    usb0_apb_rst_srst_st  : 1; /* [1]  */
        unsigned int    usb0_phy_srst_st      : 1; /* [2]  */
        unsigned int    usb0_srst_st          : 1; /* [3]  */
        unsigned int    usb1_apb_rst_srst_st  : 1; /* [4]  */
        unsigned int    usb1_phy_srst_st      : 1; /* [5]  */
        unsigned int    usb1_srst_st          : 1; /* [6]  */
        unsigned int    usb2_apb_rst_srst_st  : 1; /* [7]  */
        unsigned int    usb2_phy_srst_st      : 1; /* [8]  */
        unsigned int    usb2_srst_st          : 1; /* [9]  */
        unsigned int    usb3_apb_rst_srst_st  : 1; /* [10]  */
        unsigned int    usb3_phy_srst_st      : 1; /* [11]  */
        unsigned int    usb3_srst_st          : 1; /* [12]  */
        unsigned int    usb3_cmb_phy_srst_st  : 1; /* [13]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_srst_st2;

/* Define the union u_hp_srst_req1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_eth_srst_req_ena : 1; /* [0]  */
        unsigned int    rev_srst_ena01        : 1; /* [1]  */
        unsigned int    gmac0_macsys_srst_req_ena : 1; /* [2]  */
        unsigned int    gmac0_cfg_srst_req_ena : 1; /* [3]  */
        unsigned int    gmac0_sys_srst_req_ena : 1; /* [4]  */
        unsigned int    gmac0_rx_srst_req_ena : 1; /* [5]  */
        unsigned int    rev_srst_ena06        : 1; /* [6]  */
        unsigned int    gmac1_eth_srst_req_ena : 1; /* [7]  */
        unsigned int    rev_srst_ena08        : 1; /* [8]  */
        unsigned int    gmac1_macsys_srst_req_ena : 1; /* [9]  */
        unsigned int    gmac1_cfg_srst_req_ena : 1; /* [10]  */
        unsigned int    gmac1_sys_srst_req_ena : 1; /* [11]  */
        unsigned int    gmac1_rx_srst_req_ena : 1; /* [12]  */
        unsigned int    rev_srst_ena13        : 1; /* [13]  */
        unsigned int    gmac2_eth_srst_req_ena : 1; /* [14]  */
        unsigned int    rev_srst_ena15        : 1; /* [15]  */
        unsigned int    gmac2_macsys_srst_req_ena : 1; /* [16]  */
        unsigned int    gmac2_cfg_srst_req_ena : 1; /* [17]  */
        unsigned int    gmac2_sys_srst_req_ena : 1; /* [18]  */
        unsigned int    gmac2_rx_srst_req_ena : 1; /* [19]  */
        unsigned int    gmac2_tx_srst_req_ena : 1; /* [20]  */
        unsigned int    gmac3_eth_srst_req_ena : 1; /* [21]  */
        unsigned int    rev_srst_ena22        : 1; /* [22]  */
        unsigned int    gmac3_macsys_srst_req_ena : 1; /* [23]  */
        unsigned int    gmac3_cfg_srst_req_ena : 1; /* [24]  */
        unsigned int    gmac3_sys_srst_req_ena : 1; /* [25]  */
        unsigned int    gmac3_rx_srst_req_ena : 1; /* [26]  */
        unsigned int    gmac3_tx_srst_req_ena : 1; /* [27]  */
        unsigned int    rev_srst_ena28        : 1; /* [28]  */
        unsigned int    ptp_srst_req_ena      : 1; /* [29]  */
        unsigned int    rev_srst_ena30        : 1; /* [30]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_srst_req1;

/* Define the union u_hp_srst_req2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    usb0_apb_rst_srst_req : 1; /* [1]  */
        unsigned int    usb0_phy_srst_req     : 1; /* [2]  */
        unsigned int    usb0_srst_req         : 1; /* [3]  */
        unsigned int    usb1_apb_rst_srst_req : 1; /* [4]  */
        unsigned int    usb1_phy_srst_req     : 1; /* [5]  */
        unsigned int    usb1_srst_req         : 1; /* [6]  */
        unsigned int    usb2_apb_rst_srst_req : 1; /* [7]  */
        unsigned int    usb2_phy_srst_req     : 1; /* [8]  */
        unsigned int    usb2_srst_req         : 1; /* [9]  */
        unsigned int    usb3_apb_rst_srst_req : 1; /* [10]  */
        unsigned int    usb3_phy_srst_req     : 1; /* [11]  */
        unsigned int    usb3_srst_req         : 1; /* [12]  */
        unsigned int    usb3_cmb_phy_srst_req : 1; /* [13]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_srst_req2;

/* Define the union u_hp_srst_dis1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_eth_srst_req_dis : 1; /* [0]  */
        unsigned int    rev_srst_dis01        : 1; /* [1]  */
        unsigned int    gmac0_macsys_srst_req_dis : 1; /* [2]  */
        unsigned int    gmac0_cfg_srst_req_dis : 1; /* [3]  */
        unsigned int    gmac0_sys_srst_req_dis : 1; /* [4]  */
        unsigned int    gmac0_rx_srst_req_dis : 1; /* [5]  */
        unsigned int    rev_srst_dis06        : 1; /* [6]  */
        unsigned int    gmac1_eth_srst_req_dis : 1; /* [7]  */
        unsigned int    rev_srst_dis08        : 1; /* [8]  */
        unsigned int    gmac1_macsys_srst_req_dis : 1; /* [9]  */
        unsigned int    gmac1_cfg_srst_req_dis : 1; /* [10]  */
        unsigned int    gmac1_sys_srst_req_dis : 1; /* [11]  */
        unsigned int    gmac1_rx_srst_req_dis : 1; /* [12]  */
        unsigned int    rev_srst_dis13        : 1; /* [13]  */
        unsigned int    gmac2_eth_srst_req_dis : 1; /* [14]  */
        unsigned int    rev_srst_dis15        : 1; /* [15]  */
        unsigned int    gmac2_macsys_srst_req_dis : 1; /* [16]  */
        unsigned int    gmac2_cfg_srst_req_dis : 1; /* [17]  */
        unsigned int    gmac2_sys_srst_req_dis : 1; /* [18]  */
        unsigned int    gmac2_rx_srst_req_dis : 1; /* [19]  */
        unsigned int    gmac2_tx_srst_req_dis : 1; /* [20]  */
        unsigned int    gmac3_eth_srst_req_dis : 1; /* [21]  */
        unsigned int    rev_srst_dis22        : 1; /* [22]  */
        unsigned int    gmac3_macsys_srst_req_dis : 1; /* [23]  */
        unsigned int    gmac3_cfg_srst_req_dis : 1; /* [24]  */
        unsigned int    gmac3_sys_srst_req_dis : 1; /* [25]  */
        unsigned int    gmac3_rx_srst_req_dis : 1; /* [26]  */
        unsigned int    gmac3_tx_srst_req_dis : 1; /* [27]  */
        unsigned int    rev_srst_dis28        : 1; /* [28]  */
        unsigned int    ptp_srst_req_dis      : 1; /* [29]  */
        unsigned int    rev_srst_dis30        : 1; /* [30]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_srst_dis1;

/* Define the union u_hp_srst_dis2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    usb0_apb_rst_srst_dis : 1; /* [1]  */
        unsigned int    usb0_phy_srst_dis     : 1; /* [2]  */
        unsigned int    usb0_srst_dis         : 1; /* [3]  */
        unsigned int    usb1_apb_rst_srst_dis : 1; /* [4]  */
        unsigned int    usb1_phy_srst_dis     : 1; /* [5]  */
        unsigned int    usb1_srst_dis         : 1; /* [6]  */
        unsigned int    usb2_apb_rst_srst_dis : 1; /* [7]  */
        unsigned int    usb2_phy_srst_dis     : 1; /* [8]  */
        unsigned int    usb2_srst_dis         : 1; /* [9]  */
        unsigned int    usb3_apb_rst_srst_dis : 1; /* [10]  */
        unsigned int    usb3_phy_srst_dis     : 1; /* [11]  */
        unsigned int    usb3_srst_dis         : 1; /* [12]  */
        unsigned int    usb3_cmb_phy_srst_dis : 1; /* [13]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_srst_dis2;

/* Define the union u_hp_clk_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_sysclk_cfg      : 1; /* [0]  */
        unsigned int    gmac0_macclk_cfg      : 1; /* [1]  */
        unsigned int    gmac1_sysclk_cfg      : 1; /* [2]  */
        unsigned int    gmac1_macclk_cfg      : 1; /* [3]  */
        unsigned int    gmac2_sysclk_cfg      : 1; /* [4]  */
        unsigned int    gmac2_macclk_cfg      : 1; /* [5]  */
        unsigned int    gmac3_sysclk_cfg      : 1; /* [6]  */
        unsigned int    gmac3_macclk_cfg      : 1; /* [7]  */
        unsigned int    ptp_clk_cfg           : 1; /* [8]  */
        unsigned int    usb3_sysclk_cfg       : 1; /* [9]  */
        unsigned int    usb3_ref_clk_cfg      : 1; /* [10]  */
        unsigned int    usb3_pipe_clk_cfg     : 1; /* [11]  */
        unsigned int    usb0_clk_cfg          : 1; /* [12]  */
        unsigned int    usb1_clk_cfg          : 1; /* [13]  */
        unsigned int    usb2_clk_cfg          : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_clk_cfg;

/* Define the union u_hp_clk_dis */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_sysclk_dis      : 1; /* [0]  */
        unsigned int    gmac0_macclk_dis      : 1; /* [1]  */
        unsigned int    gmac1_sysclk_dis      : 1; /* [2]  */
        unsigned int    gmac1_macclk_dis      : 1; /* [3]  */
        unsigned int    gmac2_sysclk_dis      : 1; /* [4]  */
        unsigned int    gmac2_macclk_dis      : 1; /* [5]  */
        unsigned int    gmac3_sysclk_dis      : 1; /* [6]  */
        unsigned int    gmac3_macclk_dis      : 1; /* [7]  */
        unsigned int    ptp_clk_dis           : 1; /* [8]  */
        unsigned int    usb3_sysclk_dis       : 1; /* [9]  */
        unsigned int    usb3_ref_clk_dis      : 1; /* [10]  */
        unsigned int    usb3_pipe_clk_dis     : 1; /* [11]  */
        unsigned int    usb0_clk_dis          : 1; /* [12]  */
        unsigned int    usb1_clk_dis          : 1; /* [13]  */
        unsigned int    usb2_clk_dis          : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_clk_dis;

/* Define the union u_hp_clk_enb */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_sysclk_en       : 1; /* [0]  */
        unsigned int    gmac0_macclk_en       : 1; /* [1]  */
        unsigned int    gmac1_sysclk_en       : 1; /* [2]  */
        unsigned int    gmac1_macclk_en       : 1; /* [3]  */
        unsigned int    gmac2_sysclk_en       : 1; /* [4]  */
        unsigned int    gmac2_macclk_en       : 1; /* [5]  */
        unsigned int    gmac3_sysclk_en       : 1; /* [6]  */
        unsigned int    gmac3_macclk_en       : 1; /* [7]  */
        unsigned int    ptp_clk_en            : 1; /* [8]  */
        unsigned int    usb3_sysclk_en        : 1; /* [9]  */
        unsigned int    usb3_ref_clk_en       : 1; /* [10]  */
        unsigned int    usb3_pipe_clk_en      : 1; /* [11]  */
        unsigned int    usb0_clk_en           : 1; /* [12]  */
        unsigned int    usb1_clk_en           : 1; /* [13]  */
        unsigned int    usb2_clk_en           : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_clk_enb;

/* Define the union u_hp_clk_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_rmii_sel        : 1; /* [0]  */
        unsigned int    gmac1_rmii_sel        : 1; /* [1]  */
        unsigned int    pclk_sel              : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_clk_sel;

/* Define the union u_hp_usb_ram_cfg_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    usb0_sp_ram_tmod      : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    usb0_tp_ram_tmod      : 8; /* [15..8]  */
        unsigned int    usb0_mem_power_mode   : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_usb_ram_cfg_0;

/* Define the union u_hp_usb_ram_cfg_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    usb1_sp_ram_tmod      : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    usb1_tp_ram_tmod      : 8; /* [15..8]  */
        unsigned int    usb1_mem_power_mode   : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_usb_ram_cfg_1;

/* Define the union u_hp_usb_ram_cfg_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    usb2_sp_ram_tmod      : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    usb2_tp_ram_tmod      : 8; /* [15..8]  */
        unsigned int    usb2_mem_power_mode   : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_usb_ram_cfg_2;

/* Define the union u_hp_usb_ram_cfg_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    usb3_sp_ram_tmod      : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    usb3_tp_ram_tmod      : 8; /* [15..8]  */
        unsigned int    usb3_mem_power_mode   : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_usb_ram_cfg_3;

/* Define the union u_hp_mac_ram_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mac_sp_ram_tmod       : 7; /* [6..0]  */
        unsigned int    reserved_0            : 1; /* [7]  */
        unsigned int    mac_tp_ram_tmod       : 8; /* [15..8]  */
        unsigned int    mac_mem_power_mode    : 6; /* [21..16]  */
        unsigned int    reserved_1            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_mac_ram_cfg;

/* Define the union u_hp_clk_div_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_rmii_div         : 4; /* [3..0]  */
        unsigned int    gmac_pipe_cfg         : 4; /* [7..4]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_clk_div_cfg;

/* Define the union u_hp_usb_sus_clk_div */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    usb3_suspend_cfg_div  : 9; /* [8..0]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_usb_sus_clk_div;

/* Define the union u_hp_gmac_hub_timer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_hub_wait_timer   : 15; /* [14..0]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_gmac_hub_timer;

/* Define the union u_hp_gmac_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_clr_statistic    : 1; /* [0]  */
        unsigned int    reserved_0            : 1; /* [1]  */
        unsigned int    gmac_hub_timer_en     : 1; /* [2]  */
        unsigned int    reserved_1            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_gmac_ctrl;

/* Define the union u_hp_gmac_h2x_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_h2x_dbg_rd_outstd : 4; /* [3..0]  */
        unsigned int    gmac_h2x_dbg_wr_outstd : 4; /* [7..4]  */
        unsigned int    gmac_h2x_dbg_port_sel : 4; /* [11..8]  */
        unsigned int    gmac_h2x_dbg_incr_spen : 1; /* [12]  */
        unsigned int    gmac_h2x_dbg_stat_ctrl : 1; /* [13]  */
        unsigned int    gmac_h2x_dbg_h2x_sel  : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_gmac_h2x_ctrl1;

/* Define the union u_hp_gmac_mem_ecc */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_mem_err          : 12; /* [11..0]  */
        unsigned int    reserved_0            : 4; /* [15..12]  */
        unsigned int    gmac_mem_uerr         : 12; /* [27..16]  */
        unsigned int    reserved_1            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_gmac_mem_ecc;

/* Define the union u_hp_axi_lp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    as_5x1_csysreq        : 1; /* [0]  */
        unsigned int    reserved_0            : 3; /* [3..1]  */
        unsigned int    as_1x2_csysreq        : 1; /* [4]  */
        unsigned int    reserved_1            : 3; /* [7..5]  */
        unsigned int    as_1x16_csysreq       : 1; /* [8]  */
        unsigned int    reserved_2            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_axi_lp_ctrl;

/* Define the union u_hp_gmac_led_active */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_led_active       : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_gmac_led_active;

/* Define the union u_hp_occ_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hp_occ_en             : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_occ_en;

/* Define the union u_hp_xsemp_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hp_emp_en             : 1; /* [0]  */
        unsigned int    hp_emp_err_mode       : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_xsemp_en;

/* Define the union u_hp_xsemp_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    xsemp5_sv_st          : 1; /* [0]  */
        unsigned int    xsemp5_bresp_err_int  : 1; /* [1]  */
        unsigned int    xsemp5_rresp_err_int  : 1; /* [2]  */
        unsigned int    xsemp5_emp_ok         : 1; /* [3]  */
        unsigned int    xsemp5_emp_linkdown   : 1; /* [4]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_xsemp_st;

/* Define the union u_hp_as5x1_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    as5x1_mst_priority_m1 : 3; /* [2..0]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    as5x1_mst_priority_m2 : 3; /* [6..4]  */
        unsigned int    reserved_1            : 1; /* [7]  */
        unsigned int    as5x1_mst_priority_m3 : 3; /* [10..8]  */
        unsigned int    reserved_2            : 1; /* [11]  */
        unsigned int    as5x1_mst_priority_m4 : 3; /* [14..12]  */
        unsigned int    reserved_3            : 1; /* [15]  */
        unsigned int    as5x1_mst_priority_m5 : 3; /* [18..16]  */
        unsigned int    reserved_4            : 1; /* [19]  */
        unsigned int    as5x1_mst_priority_m6 : 3; /* [22..20]  */
        unsigned int    reserved_5            : 1; /* [23]  */
        unsigned int    as5x1_slv_prority_s1  : 1; /* [24]  */
        unsigned int    as5x1_prority_hard_cfg_en : 1; /* [25]  */
        unsigned int    reserved_6            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_as5x1_priority;

/* Define the union u_hp_as1x2_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    as1x2_mst_priority_m1 : 1; /* [0]  */
        unsigned int    reserved_0            : 3; /* [3..1]  */
        unsigned int    as1x2_slv_priority_s1 : 2; /* [5..4]  */
        unsigned int    reserved_1            : 2; /* [7..6]  */
        unsigned int    as1x2_slv_priority_s2 : 2; /* [9..8]  */
        unsigned int    reserved_2            : 2; /* [11..10]  */
        unsigned int    as1x2_slv_priority_s3 : 2; /* [13..12]  */
        unsigned int    as1x2_prority_hard_cfg_en : 1; /* [14]  */
        unsigned int    reserved_3            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_as1x2_priority;

/* Define the union u_hp_as5x1_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    a5x1_dlock_irp        : 1; /* [0]  */
        unsigned int    a5x1_dlock_wr         : 1; /* [1]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    a5x1_dlock_id         : 6; /* [9..4]  */
        unsigned int    reserved_1            : 6; /* [15..10]  */
        unsigned int    a5x1_dlock_slv        : 1; /* [16]  */
        unsigned int    reserved_2            : 3; /* [19..17]  */
        unsigned int    a5x1_dlock_mst        : 3; /* [22..20]  */
        unsigned int    reserved_3            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_as5x1_dlock;

/* Define the union u_hp_as1x2_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    a1x2_dlock_irp        : 1; /* [0]  */
        unsigned int    a1x2_dlock_wr         : 1; /* [1]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    a1x2_dlock_id         : 12; /* [15..4]  */
        unsigned int    a1x2_dlock_slv        : 2; /* [17..16]  */
        unsigned int    reserved_1            : 2; /* [19..18]  */
        unsigned int    a1x2_dlock_mst        : 1; /* [20]  */
        unsigned int    reserved_2            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_as1x2_dlock;

/* Define the union u_hp_mac_h2x_user */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac_h2x_awuser       : 1; /* [0]  */
        unsigned int    gmac_h2x_aruser       : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_mac_h2x_user;

/* Define the union u_hp_mac_haddr_extend */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m0_haddr_offset       : 4; /* [3..0]  */
        unsigned int    m0_addr_remap_mode    : 2; /* [5..4]  */
        unsigned int    m0_haddr_hi           : 2; /* [7..6]  */
        unsigned int    m1_haddr_offset       : 4; /* [11..8]  */
        unsigned int    m1_addr_remap_mode    : 2; /* [13..12]  */
        unsigned int    m1_haddr_hi           : 2; /* [15..14]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_mac_haddr_extend;

/* Define the union u_hp_speed_mon_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m1_mon_cnt_en     : 1; /* [0]  */
        unsigned int    m2_mon_cnt_en     : 1; /* [1]  */
        unsigned int    m3_mon_cnt_en     : 1; /* [2]  */
        unsigned int    m4_mon_cnt_en     : 1; /* [3]  */
        unsigned int    m5_mon_cnt_en     : 1; /* [4]  */
        unsigned int    reserved_0        : 3; /* [7..5]  */
        unsigned int    m1_mon_wbyte_clr  : 1; /* [8]  */
        unsigned int    m2_mon_wbyte_clr  : 1; /* [9]  */
        unsigned int    m3_mon_wbyte_clr  : 1; /* [10]  */
        unsigned int    m4_mon_wbyte_clr  : 1; /* [11]  */
        unsigned int    m5_mon_wbyte_clr  : 1; /* [12]  */
        unsigned int    reserved_1        : 3; /* [15..13]  */
        unsigned int    m1_mon_rbyte_clr  : 1; /* [16]  */
        unsigned int    m2_mon_rbyte_clr  : 1; /* [17]  */
        unsigned int    m3_mon_rbyte_clr  : 1; /* [18]  */
        unsigned int    m4_mon_rbyte_clr  : 1; /* [19]  */
        unsigned int    m5_mon_rbyte_clr  : 1; /* [20]  */
        unsigned int    reserved_2        : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_speed_mon_cfg;

/* Define the union u_gmac_phy0_los_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    los_level_0           : 5; /* [4..0]  */
        unsigned int    los_bias_0            : 3; /* [7..5]  */
        unsigned int    rx0_los_filt_cnt_0    : 7; /* [14..8]  */
        unsigned int    tx_vboost_lvl_0       : 3; /* [17..15]  */
        unsigned int    rx_los_valid_rst_msk_0 : 2; /* [19..18]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy0_los_cfg;

/* Define the union u_gmac_phy1_los_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    los_level_1           : 5; /* [4..0]  */
        unsigned int    los_bias_1            : 3; /* [7..5]  */
        unsigned int    rx0_los_filt_cnt_1    : 7; /* [14..8]  */
        unsigned int    tx_vboost_lvl_1       : 3; /* [17..15]  */
        unsigned int    rx_los_valid_rst_msk_1 : 2; /* [19..18]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy1_los_cfg;

/* Define the union u_gmac_phy0_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tx0_invert_0          : 1; /* [0]  */
        unsigned int    lane_10bit_sel_0      : 1; /* [1]  */
        unsigned int    ref_ssp_en_0          : 1; /* [2]  */
        unsigned int    tx0_vboost_en_0       : 1; /* [3]  */
        unsigned int    tx0_reset_0           : 1; /* [4]  */
        unsigned int    tx0_en_0              : 1; /* [5]  */
        unsigned int    tx0_cm_en_0           : 1; /* [6]  */
        unsigned int    rx0_term_en_0         : 1; /* [7]  */
        unsigned int    rx0_reset_0           : 1; /* [8]  */
        unsigned int    rx0_pll_en_0          : 1; /* [9]  */
        unsigned int    rx0_los_en_0          : 1; /* [10]  */
        unsigned int    rx0_invert_0          : 1; /* [11]  */
        unsigned int    rx0_data_en_0         : 1; /* [12]  */
        unsigned int    rx0_align_en_0        : 1; /* [13]  */
        unsigned int    mpll_en_0             : 1; /* [14]  */
        unsigned int    lane0_loopbk_en_0     : 1; /* [15]  */
        unsigned int    test_bypass_0         : 1; /* [16]  */
        unsigned int    test_burnin_0         : 1; /* [17]  */
        unsigned int    ssc_ref_clk_sel_0     : 1; /* [18]  */
        unsigned int    mpll_multiplier_0     : 7; /* [25..19]  */
        unsigned int    vreg_bypass_0         : 1; /* [26]  */
        unsigned int    test_powerdown_0      : 1; /* [27]  */
        unsigned int    ref_use_pad_0         : 1; /* [28]  */
        unsigned int    ref_clkdiv2_0         : 1; /* [29]  */
        unsigned int    phy_reset_0           : 1; /* [30]  */
        unsigned int    phy_rtune_req_0       : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy0_cfg;

/* Define the union u_gmac_phy1_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tx0_invert_1          : 1; /* [0]  */
        unsigned int    lane_10bit_sel_1      : 1; /* [1]  */
        unsigned int    ref_ssp_en_1          : 1; /* [2]  */
        unsigned int    tx0_vboost_en_1       : 1; /* [3]  */
        unsigned int    tx0_reset_1           : 1; /* [4]  */
        unsigned int    tx0_en_1              : 1; /* [5]  */
        unsigned int    tx0_cm_en_1           : 1; /* [6]  */
        unsigned int    rx0_term_en_1         : 1; /* [7]  */
        unsigned int    rx0_reset_1           : 1; /* [8]  */
        unsigned int    rx0_pll_en_1          : 1; /* [9]  */
        unsigned int    rx0_los_en_1          : 1; /* [10]  */
        unsigned int    rx0_invert_1          : 1; /* [11]  */
        unsigned int    rx0_data_en_1         : 1; /* [12]  */
        unsigned int    rx0_align_en_1        : 1; /* [13]  */
        unsigned int    mpll_en_1             : 1; /* [14]  */
        unsigned int    lane0_loopbk_en_1     : 1; /* [15]  */
        unsigned int    test_bypass_1         : 1; /* [16]  */
        unsigned int    test_burnin_1         : 1; /* [17]  */
        unsigned int    ssc_ref_clk_sel_1     : 1; /* [18]  */
        unsigned int    mpll_multiplier_1     : 7; /* [25..19]  */
        unsigned int    vreg_bypass_1         : 1; /* [26]  */
        unsigned int    test_powerdown_1      : 1; /* [27]  */
        unsigned int    ref_use_pad_1         : 1; /* [28]  */
        unsigned int    ref_clkdiv2_1         : 1; /* [29]  */
        unsigned int    phy_reset_1           : 1; /* [30]  */
        unsigned int    phy_rtune_req_1       : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy1_cfg;

/* Define the union u_gmac_phy0_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    phy_rx0_eq_0          : 3; /* [2..0]  */
        unsigned int    phy_tx0_term_offset_0 : 5; /* [7..3]  */
        unsigned int    tx0_amplitude_0       : 7; /* [14..8]  */
        unsigned int    tx0_detect_rx_req_0   : 1; /* [15]  */
        unsigned int    tx0_preemph_0         : 7; /* [22..16]  */
        unsigned int    tx0_data_en_0         : 1; /* [23]  */
        unsigned int    phy_data_reverse_en0  : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy0_cfg2;

/* Define the union u_gmac_phy1_cfg2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    phy_rx0_eq_1          : 3; /* [2..0]  */
        unsigned int    phy_tx0_term_offset_1 : 5; /* [7..3]  */
        unsigned int    tx0_amplitude_1       : 7; /* [14..8]  */
        unsigned int    tx0_detect_rx_req_1   : 1; /* [15]  */
        unsigned int    tx0_preemph_1         : 7; /* [22..16]  */
        unsigned int    tx0_data_en_1         : 1; /* [23]  */
        unsigned int    phy_data_reverse_en1  : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy1_cfg2;

/* Define the union u_gmac_phy0_sts */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tx0_state_0           : 1; /* [0]  */
        unsigned int    tx0_detect_rx_result_0 : 1; /* [1]  */
        unsigned int    tx0_detect_rx_ack_0   : 1; /* [2]  */
        unsigned int    tx0_cm_state_0        : 1; /* [3]  */
        unsigned int    rx0_pll_state_0       : 1; /* [4]  */
        unsigned int    mpll_state_0          : 1; /* [5]  */
        unsigned int    rx_valid_0            : 1; /* [6]  */
        unsigned int    rx_los_0              : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy0_sts;

/* Define the union u_gmac_phy1_sts */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    tx0_state_1           : 1; /* [0]  */
        unsigned int    tx0_detect_rx_result_1 : 1; /* [1]  */
        unsigned int    tx0_detect_rx_ack_1   : 1; /* [2]  */
        unsigned int    tx0_cm_state_1        : 1; /* [3]  */
        unsigned int    rx0_pll_state_1       : 1; /* [4]  */
        unsigned int    mpll_state_1          : 1; /* [5]  */
        unsigned int    rx_valid_1            : 1; /* [6]  */
        unsigned int    rx_los_1              : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_gmac_phy1_sts;

/* Define the union u_hp_gmac_mem_ecc_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    gmac0_pcu_tx_err_inj  : 2; /* [1..0]  */
        unsigned int    gmac1_pcu_tx_err_inj  : 2; /* [3..2]  */
        unsigned int    gmac2_pcu_tx_err_inj  : 2; /* [5..4]  */
        unsigned int    gmac3_pcu_tx_err_inj  : 2; /* [7..6]  */
        unsigned int    gmac0_pcu_rx_err_inj  : 2; /* [9..8]  */
        unsigned int    gmac1_pcu_rx_err_inj  : 2; /* [11..10]  */
        unsigned int    gmac2_pcu_rx_err_inj  : 2; /* [13..12]  */
        unsigned int    gmac3_pcu_rx_err_inj  : 2; /* [15..14]  */
        unsigned int    gmac0_pcu_cfg_err_inj : 2; /* [17..16]  */
        unsigned int    gmac1_pcu_cfg_err_inj : 2; /* [19..18]  */
        unsigned int    gmac2_pcu_cfg_err_inj : 2; /* [21..20]  */
        unsigned int    gmac3_pcu_cfg_err_inj : 2; /* [23..22]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_gmac_mem_ecc_req;

/* Define the union u_hp_sds0_cr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    phy0_cr_cap_data_req  : 16; /* [15..0]  */
        unsigned int    phy0_cr_cap_en        : 1; /* [16]  */
        unsigned int    phy0_cr_cap_write     : 1; /* [17]  */
        unsigned int    phy0_cr_cap_read      : 1; /* [18]  */
        unsigned int    phy0_cr_cap_data      : 1; /* [19]  */
        unsigned int    phy0_cr_cap_addr      : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_sds0_cr_ctrl;

/* Define the union u_hp_sds0_cr_data_out */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    phy0_cr_cap_data_ack  : 16; /* [15..0]  */
        unsigned int    phy0_cr_cap_ack       : 1; /* [16]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_sds0_cr_data_out;

/* Define the union u_hp_sds1_cr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    phy1_cr_cap_data_req  : 16; /* [15..0]  */
        unsigned int    phy1_cr_cap_en        : 1; /* [16]  */
        unsigned int    phy1_cr_cap_write     : 1; /* [17]  */
        unsigned int    phy1_cr_cap_read      : 1; /* [18]  */
        unsigned int    phy1_cr_cap_data      : 1; /* [19]  */
        unsigned int    phy1_cr_cap_addr      : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_sds1_cr_ctrl;

/* Define the union u_hp_sds1_cr_data_out */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    phy1_cr_cap_data_ack  : 16; /* [15..0]  */
        unsigned int    phy1_cr_cap_ack       : 1; /* [16]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_sds1_cr_data_out;

/* Define the union u_hp_axi_latency_ctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hp_latency_sts_en     : 1; /* [0]  */
        unsigned int    hp_latency_wr_sel     : 1; /* [1]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    hp_latency_sts_porl_sel : 4; /* [7..4]  */
        unsigned int    reserved_1            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_axi_latency_ctl;

/* Define the union u_hp_fix_rst_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hp_fix_rst_en         : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hp_fix_rst_en;

#endif /* __HP_SUB_CSR_C_UNION_DEFINE_H__ */
