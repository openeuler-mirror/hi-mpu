// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SYSCTRL_CSR_C_UNION_DEFINE_H__
#define __SYSCTRL_CSR_C_UNION_DEFINE_H__


/* Define the union u_strap_status_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    boot_mode             : 2; /* [1..0]  */
        unsigned int    us_boot_core          : 1; /* [2]  */
        unsigned int    boot_dbios            : 1; /* [3]  */
        unsigned int    boot_size             : 1; /* [4]  */
        unsigned int    default_sfc0_addr_mode : 1; /* [5]  */
        unsigned int    lb_boot_offset        : 1; /* [6]  */
        unsigned int    lb_boot_width         : 1; /* [7]  */
        unsigned int    jtag_default_mode     : 1; /* [8]  */
        unsigned int    espi_rst_pcie         : 1; /* [9]  */
        unsigned int    pad_debug_out         : 1; /* [10]  */
        unsigned int    strap_rsv             : 5; /* [15..11]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_strap_status_reg;

/* Define the union u_boot_define_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    secure_boot           : 1; /* [0]  */
        unsigned int    a55_boot_en           : 1; /* [1]  */
        unsigned int    m3_boot_en            : 1; /* [2]  */
        unsigned int    sc_boot_en            : 1; /* [3]  */
        unsigned int    a55_boot_remap_en     : 1; /* [4]  */
        unsigned int    m3_boot_remap_en      : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_boot_define_reg;

/* Define the union u_reboot_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reboot_swap           : 1; /* [0]  */
        unsigned int    reserved              : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_reboot_ctrl;

/* Define the union u_clk_div_chg_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    clk_div_chg_en        : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_clk_div_chg_en;

/* Define the union u_clk_div_ctrl_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hp_clk_div_num        : 4; /* [3..0]  */
        unsigned int    lp_clk_div_num        : 4; /* [7..4]  */
        unsigned int    ddrc_clk_div_num      : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_clk_div_ctrl_reg;

/* Define the union u_clk_bypass_ctrl_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    core_clk_bypass       : 1; /* [0]  */
        unsigned int    bus_clk_bypass        : 1; /* [1]  */
        unsigned int    ddrc_clk_bypass       : 1; /* [2]  */
        unsigned int    gpu_clk_bypass        : 1; /* [3]  */
        unsigned int    cpll_out_bypass       : 1; /* [4]  */
        unsigned int    ppll0_out_bypass      : 1; /* [5]  */
        unsigned int    ppll1_out_bypass      : 1; /* [6]  */
        unsigned int    ppll2_out_bypass      : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_clk_bypass_ctrl_reg;

/* Define the union u_cpll_ctrl_reg_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpll_frac             : 24; /* [23..0]  */
        unsigned int    cpll_postdiv          : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_cpll_ctrl_reg_0;

/* Define the union u_cpll_ctrl_reg_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpll_fbdiv            : 12; /* [11..0]  */
        unsigned int    cpll_refdiv           : 6; /* [17..12]  */
        unsigned int    cpll_foutvco_pd       : 1; /* [18]  */
        unsigned int    cpll_foutpostdiv_pd   : 1; /* [19]  */
        unsigned int    cpll_fout4phase_pd    : 1; /* [20]  */
        unsigned int    cpll_dac_pd           : 1; /* [21]  */
        unsigned int    cpll_dsm_pd           : 1; /* [22]  */
        unsigned int    cpll_pd               : 1; /* [23]  */
        unsigned int    cpll_bypass           : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_cpll_ctrl_reg_1;

/* Define the union u_ppll0_ctrl_reg_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ppll0_frac            : 24; /* [23..0]  */
        unsigned int    ppll0_postdiv         : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ppll0_ctrl_reg_0;

/* Define the union u_ppll0_ctrl_reg_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ppll0_fbdiv           : 12; /* [11..0]  */
        unsigned int    ppll0_refdiv          : 6; /* [17..12]  */
        unsigned int    ppll0_foutvco_pd      : 1; /* [18]  */
        unsigned int    ppll0_foutpostdiv_pd  : 1; /* [19]  */
        unsigned int    ppll0_fout4phase_pd   : 1; /* [20]  */
        unsigned int    ppll0_dac_pd          : 1; /* [21]  */
        unsigned int    ppll0_dsm_pd          : 1; /* [22]  */
        unsigned int    ppll0_pd              : 1; /* [23]  */
        unsigned int    ppll0_bypass          : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ppll0_ctrl_reg_1;

/* Define the union u_ppll1_ctrl_reg_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ppll1_frac            : 24; /* [23..0]  */
        unsigned int    ppll1_postdiv         : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ppll1_ctrl_reg_0;

/* Define the union u_ppll1_ctrl_reg_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ppll1_fbdiv           : 12; /* [11..0]  */
        unsigned int    ppll1_refdiv          : 6; /* [17..12]  */
        unsigned int    ppll1_foutvco_pd      : 1; /* [18]  */
        unsigned int    ppll1_foutpostdiv_pd  : 1; /* [19]  */
        unsigned int    ppll1_fout4phase_pd   : 1; /* [20]  */
        unsigned int    ppll1_dac_pd          : 1; /* [21]  */
        unsigned int    ppll1_dsm_pd          : 1; /* [22]  */
        unsigned int    ppll1_pd              : 1; /* [23]  */
        unsigned int    ppll1_bypass          : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ppll1_ctrl_reg_1;

/* Define the union u_ppll2_ctrl_reg_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ppll2_frac            : 24; /* [23..0]  */
        unsigned int    ppll2_postdiv         : 6; /* [29..24]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ppll2_ctrl_reg_0;

/* Define the union u_ppll2_ctrl_reg_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ppll2_fbdiv           : 12; /* [11..0]  */
        unsigned int    ppll2_refdiv          : 6; /* [17..12]  */
        unsigned int    ppll2_foutvco_pd      : 1; /* [18]  */
        unsigned int    ppll2_foutpostdiv_pd  : 1; /* [19]  */
        unsigned int    ppll2_fout4phase_pd   : 1; /* [20]  */
        unsigned int    ppll2_dac_pd          : 1; /* [21]  */
        unsigned int    ppll2_dsm_pd          : 1; /* [22]  */
        unsigned int    ppll2_pd              : 1; /* [23]  */
        unsigned int    ppll2_bypass          : 1; /* [24]  */
        unsigned int    reserved_0            : 7; /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ppll2_ctrl_reg_1;

/* Define the union u_pll_lock_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpll_lock             : 1; /* [0]  */
        unsigned int    ppll0_lock            : 1; /* [1]  */
        unsigned int    ppll1_lock            : 1; /* [2]  */
        unsigned int    ppll2_lock            : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_pll_lock_status;

/* Define the union u_wd_ctrl_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wd0_mask              : 1; /* [0]  */
        unsigned int    wd0_rst_ctrl          : 2; /* [2..1]  */
        unsigned int    wd0_out_en            : 1; /* [3]  */
        unsigned int    wd1_mask              : 1; /* [4]  */
        unsigned int    wd1_rst_ctrl          : 2; /* [6..5]  */
        unsigned int    wd1_out_en            : 1; /* [7]  */
        unsigned int    wd2_mask              : 1; /* [8]  */
        unsigned int    wd2_rst_ctrl          : 2; /* [10..9]  */
        unsigned int    wd2_out_en            : 1; /* [11]  */
        unsigned int    wd3_mask              : 1; /* [12]  */
        unsigned int    wd3_rst_ctrl          : 2; /* [14..13]  */
        unsigned int    wd3_out_en            : 1; /* [15]  */
        unsigned int    wd4_mask              : 1; /* [16]  */
        unsigned int    wd4_rst_ctrl          : 2; /* [18..17]  */
        unsigned int    wd4_out_en            : 1; /* [19]  */
        unsigned int    wd5_mask              : 1; /* [20]  */
        unsigned int    wd5_rst_ctrl          : 2; /* [22..21]  */
        unsigned int    wd5_out_en            : 1; /* [23]  */
        unsigned int    wd6_mask              : 1; /* [24]  */
        unsigned int    wd6_rst_ctrl          : 2; /* [26..25]  */
        unsigned int    wd6_out_en            : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wd_ctrl_reg;

/* Define the union u_reset_source_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_bmc_rst           : 1; /* [0]  */
        unsigned int    pad_hs_rst            : 1; /* [1]  */
        unsigned int    wd0_rst               : 1; /* [2]  */
        unsigned int    wd1_rst               : 1; /* [3]  */
        unsigned int    wd2_rst               : 1; /* [4]  */
        unsigned int    wd3_rst               : 1; /* [5]  */
        unsigned int    wd4_rst               : 1; /* [6]  */
        unsigned int    wd5_rst               : 1; /* [7]  */
        unsigned int    wd6_rst               : 1; /* [8]  */
        unsigned int    a55_rst_n             : 1; /* [9]  */
        unsigned int    m3_rst_n              : 1; /* [10]  */
        unsigned int    sc_rst_n              : 1; /* [11]  */
        unsigned int    hs_rst_n              : 1; /* [12]  */
        unsigned int    pad_bmc_rst_occur     : 1; /* [13]  */
        unsigned int    pad_hs_rst_occur      : 1; /* [14]  */
        unsigned int    wd0_rst_occur         : 1; /* [15]  */
        unsigned int    wd1_rst_occur         : 1; /* [16]  */
        unsigned int    wd2_rst_occur         : 1; /* [17]  */
        unsigned int    wd3_rst_occur         : 1; /* [18]  */
        unsigned int    wd4_rst_occur         : 1; /* [19]  */
        unsigned int    wd5_rst_occur         : 1; /* [20]  */
        unsigned int    wd6_rst_occur         : 1; /* [21]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_reset_source_st;

/* Define the union u_reset_source_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_bmc_rst_clr       : 1; /* [0]  */
        unsigned int    pad_hs_rst_clr        : 1; /* [1]  */
        unsigned int    wd0_rst_clr           : 1; /* [2]  */
        unsigned int    wd1_rst_clr           : 1; /* [3]  */
        unsigned int    wd2_rst_clr           : 1; /* [4]  */
        unsigned int    wd3_rst_clr           : 1; /* [5]  */
        unsigned int    wd4_rst_clr           : 1; /* [6]  */
        unsigned int    wd5_rst_clr           : 1; /* [7]  */
        unsigned int    wd6_rst_clr           : 1; /* [8]  */
        unsigned int    reserved_0            : 23; /* [31..9]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_reset_source_clr;

/* Define the union u_ddrc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ddrc_csysreq_ctrl_n   : 1; /* [0]  */
        unsigned int    ddrc_ioctl_lhen_ctrl  : 1; /* [1]  */
        unsigned int    ddrc_ioctl_lhen_clr   : 1; /* [2]  */
        unsigned int    ddrc_csysack_to_clr   : 1; /* [3]  */
        unsigned int    ddrc_dbgmux_sel       : 4; /* [7..4]  */
        unsigned int    ddrc_hs_dbgmux_sel    : 6; /* [13..8]  */
        unsigned int    reserved_0            : 2; /* [15..14]  */
        unsigned int    ddrc_ls_dbgmux_sel    : 3; /* [18..16]  */
        unsigned int    ddr_reset_retention   : 1; /* [19]  */
        unsigned int    reserved_1            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ddrc_ctrl;

/* Define the union u_ddrc_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ddrc_csysreq_st       : 1; /* [0]  */
        unsigned int    ddrc_csysack_st       : 1; /* [1]  */
        unsigned int    ddrc_csysack_to       : 1; /* [2]  */
        unsigned int    ddrc_ioctl_lhen_st    : 1; /* [3]  */
        unsigned int    ddrc_ls_dbgmux_out    : 2; /* [5..4]  */
        unsigned int    reserved_0            : 2; /* [7..6]  */
        unsigned int    ddrc_dbgmux_out       : 6; /* [13..8]  */
        unsigned int    reserved_1            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_ddrc_status;

/* Define the union u_dboot_status_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dbios_boot_area       : 1; /* [0]  */
        unsigned int    reserved_0            : 7; /* [7..1]  */
        unsigned int    boot0_cnt             : 8; /* [15..8]  */
        unsigned int    boot1_cnt             : 8; /* [23..16]  */
        unsigned int    reserved_1            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_dboot_status_reg;

/* Define the union u_usb_ref_clk_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cfg_usb0_phy_clk_div  : 2; /* [1..0]  */
        unsigned int    reserved_0            : 2; /* [3..2]  */
        unsigned int    cfg_usb1_phy_clk_div  : 2; /* [5..4]  */
        unsigned int    reserved_1            : 2; /* [7..6]  */
        unsigned int    cfg_usb2_phy_clk_div  : 2; /* [9..8]  */
        unsigned int    reserved_2            : 2; /* [11..10]  */
        unsigned int    serdes_inner_refclk_gating : 1; /* [12]  */
        unsigned int    reserved_3            : 1; /* [13]  */
        unsigned int    reserved_4            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_usb_ref_clk_cfg;

/* Define the union u_auto_clk_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rst_bypass_cpll_en    : 1; /* [0]  */
        unsigned int    warm_rst_gate_pll_en  : 1; /* [1]  */
        unsigned int    bmc_rst_gate_cpll_en  : 1; /* [2]  */
        unsigned int    bmc_rst_gate_ppll0_en : 1; /* [3]  */
        unsigned int    bmc_rst_gate_ppll1_en : 1; /* [4]  */
        unsigned int    bmc_rst_gate_ppll2_en : 1; /* [5]  */
        unsigned int    bmc_unse_rst_gate_cpll_en : 1; /* [6]  */
        unsigned int    bmc_unse_rst_gate_ppll0_en : 1; /* [7]  */
        unsigned int    bmc_unse_rst_gate_ppll1_en : 1; /* [8]  */
        unsigned int    bmc_unse_rst_gate_ppll2_en : 1; /* [9]  */
        unsigned int    host_rst_gate_cpll_en : 1; /* [10]  */
        unsigned int    host_rst_gate_ppll0_en : 1; /* [11]  */
        unsigned int    host_rst_gate_ppll1_en : 1; /* [12]  */
        unsigned int    host_rst_gate_ppll2_en : 1; /* [13]  */
        unsigned int    pcie0_rst_gate_cpll_en : 1; /* [14]  */
        unsigned int    pcie0_rst_gate_ppll0_en : 1; /* [15]  */
        unsigned int    pcie0_rst_gate_ppll1_en : 1; /* [16]  */
        unsigned int    pcie0_rst_gate_ppll2_en : 1; /* [17]  */
        unsigned int    pcie1_rst_gate_cpll_en : 1; /* [18]  */
        unsigned int    pcie1_rst_gate_ppll0_en : 1; /* [19]  */
        unsigned int    pcie1_rst_gate_ppll1_en : 1; /* [20]  */
        unsigned int    pcie1_rst_gate_ppll2_en : 1; /* [21]  */
        unsigned int    cpll_dlock_bypass_en  : 1; /* [22]  */
        unsigned int    ppll0_dlock_bypass_en : 1; /* [23]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_auto_clk_ctrl;

/* Define the union u_sys_clk_enable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpll_clk_en           : 1; /* [0]  */
        unsigned int    ppll0_clk_en          : 1; /* [1]  */
        unsigned int    ppll1_clk_en          : 1; /* [2]  */
        unsigned int    ppll2_clk_en          : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_clk_enable;

/* Define the union u_sys_clk_disable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpll_clk_dis          : 1; /* [0]  */
        unsigned int    ppll0_clk_dis         : 1; /* [1]  */
        unsigned int    ppll1_clk_dis         : 1; /* [2]  */
        unsigned int    ppll2_clk_dis         : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_clk_disable;

/* Define the union u_sys_clk_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cpll_clk_st           : 1; /* [0]  */
        unsigned int    ppll0_clk_st          : 1; /* [1]  */
        unsigned int    ppll1_clk_st          : 1; /* [2]  */
        unsigned int    ppll2_clk_st          : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_clk_state;

/* Define the union u_xempty_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 1; /* [0]  */
        unsigned int    bmc_xsemp_en          : 1; /* [1]  */
        unsigned int    bmc_unse_xsemp_en     : 1; /* [2]  */
        unsigned int    pcie0_xsemp_en        : 1; /* [3]  */
        unsigned int    pcie1_xsemp_en        : 1; /* [4]  */
        unsigned int    bmc_xsemp_to_ctrl     : 2; /* [6..5]  */
        unsigned int    bmc_unse_xsemp_to_ctrl : 2; /* [8..7]  */
        unsigned int    pcie0_xsemp_to_ctrl   : 2; /* [10..9]  */
        unsigned int    pcie1_xsemp_to_ctrl   : 2; /* [12..11]  */
        unsigned int    reserved_1            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_xempty_en;

/* Define the union u_xempty_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 4; /* [3..0]  */
        unsigned int    bmc_xsemp_timeout     : 1; /* [4]  */
        unsigned int    bmc_unse_xsemp_timeout : 1; /* [5]  */
        unsigned int    pcie0_xsemp_timeout   : 1; /* [6]  */
        unsigned int    pcie1_xsemp_timeout   : 1; /* [7]  */
        unsigned int    reserved_1            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_xempty_status;

/* Define the union u_sys_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bmc_rst_int_mask      : 1; /* [0]  */
        unsigned int    bmc_unse_rst_int_mask : 1; /* [1]  */
        unsigned int    hs_rst_int_mask       : 1; /* [2]  */
        unsigned int    pcie0_rst_int_mask    : 1; /* [3]  */
        unsigned int    pcie1_rst_int_mask    : 1; /* [4]  */
        unsigned int    sc_lock_int_mask      : 1; /* [5]  */
        unsigned int    a55_lock_int_mask     : 1; /* [6]  */
        unsigned int    bmc_unse_rst_pos_mask : 1; /* [7]  */
        unsigned int    hs_rst_pos_mask       : 1; /* [8]  */
        unsigned int    pcie0_rst_pos_mask    : 1; /* [9]  */
        unsigned int    pcie1_rst_pos_mask    : 1; /* [10]  */
        unsigned int    cpll_dlock_mask       : 1; /* [11]  */
        unsigned int    ppll0_dlock_mask      : 1; /* [12]  */
        unsigned int    ppll1_dlock_mask      : 1; /* [13]  */
        unsigned int    ppll2_dlock_mask      : 1; /* [14]  */
        unsigned int    dac_cable_mask        : 1; /* [15]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_int_mask;

/* Define the union u_sys_int_stauts */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bmc_rst_int_st        : 1; /* [0]  */
        unsigned int    bmc_unse_rst_int_st   : 1; /* [1]  */
        unsigned int    hs_rst_int_st         : 1; /* [2]  */
        unsigned int    pcie0_rst_int_st      : 1; /* [3]  */
        unsigned int    pcie1_rst_int_st      : 1; /* [4]  */
        unsigned int    sc_lock_int_st        : 1; /* [5]  */
        unsigned int    a55_lock_int_st       : 1; /* [6]  */
        unsigned int    bmc_unse_rst_pos_st   : 1; /* [7]  */
        unsigned int    hs_rst_pos_st         : 1; /* [8]  */
        unsigned int    pcie0_rst_pos_st      : 1; /* [9]  */
        unsigned int    pcie1_rst_pos_st      : 1; /* [10]  */
        unsigned int    cpll_dlock_st         : 1; /* [11]  */
        unsigned int    ppll0_dlock_st        : 1; /* [12]  */
        unsigned int    ppll1_dlock_st        : 1; /* [13]  */
        unsigned int    ppll2_dlock_st        : 1; /* [14]  */
        unsigned int    dac_cable_st          : 1; /* [15]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_int_stauts;

/* Define the union u_sys_int_set */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bmc_rst_int_set       : 1; /* [0]  */
        unsigned int    bmc_unse_rst_int_set  : 1; /* [1]  */
        unsigned int    hs_rst_int_set        : 1; /* [2]  */
        unsigned int    pcie0_rst_int_set     : 1; /* [3]  */
        unsigned int    pcie1_rst_int_set     : 1; /* [4]  */
        unsigned int    sc_lock_int_set       : 1; /* [5]  */
        unsigned int    a55_lock_int_set      : 1; /* [6]  */
        unsigned int    bmc_unse_rst_pos_set  : 1; /* [7]  */
        unsigned int    hs_rst_pos_set        : 1; /* [8]  */
        unsigned int    pcie0_rst_pos_set     : 1; /* [9]  */
        unsigned int    pcie1_rst_pos_set     : 1; /* [10]  */
        unsigned int    cpll_dlock_set        : 1; /* [11]  */
        unsigned int    ppll0_dlock_set       : 1; /* [12]  */
        unsigned int    ppll1_dlock_set       : 1; /* [13]  */
        unsigned int    ppll2_dlock_set       : 1; /* [14]  */
        unsigned int    dac_cable_set         : 1; /* [15]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_int_set;

/* Define the union u_sys_rst_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    chip_rst_req          : 1; /* [0]  */
        unsigned int    hs_rst_req            : 1; /* [1]  */
        unsigned int    bmc_rst_req           : 1; /* [2]  */
        unsigned int    bmc_unse_rst_req      : 1; /* [3]  */
        unsigned int    pcie0_rst_req         : 1; /* [4]  */
        unsigned int    pcie1_rst_req         : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_rst_req;

/* Define the union u_sys_rst_dreq */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    chip_rst_dreq         : 1; /* [0]  */
        unsigned int    hs_rst_dreq           : 1; /* [1]  */
        unsigned int    bmc_rst_dreq          : 1; /* [2]  */
        unsigned int    bmc_unse_rst_dreq     : 1; /* [3]  */
        unsigned int    pcie0_rst_dreq        : 1; /* [4]  */
        unsigned int    pcie1_rst_dreq        : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_rst_dreq;

/* Define the union u_sys_rst_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    chip_rst_st           : 1; /* [0]  */
        unsigned int    hs_rst_st             : 1; /* [1]  */
        unsigned int    bmc_rst_st            : 1; /* [2]  */
        unsigned int    bmc_unse_rst_st       : 1; /* [3]  */
        unsigned int    pcie0_rst_st          : 1; /* [4]  */
        unsigned int    pcie1_rst_st          : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_sys_rst_st;

/* Define the union u_rst_out_ctrl_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    chip_rst_out_en       : 1; /* [0]  */
        unsigned int    bmc_rst_out_en        : 1; /* [1]  */
        unsigned int    bmc_unse_rst_out_en   : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_rst_out_ctrl_reg;

/* Define the union u_inner_chip_rst_record */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    last_in_chip_rst_src  : 3; /* [2..0]  */
        unsigned int    second_in_chip_rst_src : 3; /* [5..3]  */
        unsigned int    third_in_chip_rst_src : 3; /* [8..6]  */
        unsigned int    fourth_in_chip_rst_src : 3; /* [11..9]  */
        unsigned int    in_chip_rst_cnt       : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_inner_chip_rst_record;

/* Define the union u_bmc_rst_record */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    last_bmc_rst_src      : 3; /* [2..0]  */
        unsigned int    second_bmc_rst_src    : 3; /* [5..3]  */
        unsigned int    third_bmc_rst_src     : 3; /* [8..6]  */
        unsigned int    fourth_bmc_rst_src    : 3; /* [11..9]  */
        unsigned int    bmc_rst_cnt           : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_bmc_rst_record;

/* Define the union u_bmc_unse_rst_record */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    last_bmc_unse_rst_src : 2; /* [1..0]  */
        unsigned int    second_bmc_unse_rst_src : 2; /* [3..2]  */
        unsigned int    third_bmc_unse_rst_src : 2; /* [5..4]  */
        unsigned int    fourth_bmc_unse_rst_src : 2; /* [7..6]  */
        unsigned int    bmc_unse_rst_cnt      : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_bmc_unse_rst_record;

/* Define the union u_hs_rst_record */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    last_hs_rst_src       : 2; /* [1..0]  */
        unsigned int    second_hs_rst_src     : 2; /* [3..2]  */
        unsigned int    third_hs_rst_src      : 2; /* [5..4]  */
        unsigned int    fourth_hs_rst_src     : 2; /* [7..6]  */
        unsigned int    hs_rst_cnt            : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_hs_rst_record;

/* Define the union u_a55_lock_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    a55_lock_req0         : 8; /* [7..0]  */
        unsigned int    a55_lock_req1         : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_a55_lock_req;

/* Define the union u_a55_loc_dreq */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    a55_lock_dreq0        : 8; /* [7..0]  */
        unsigned int    a55_lock_dreq1        : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_a55_loc_dreq;

/* Define the union u_a55_lock_req_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    a55_lock_req_st       : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_a55_lock_req_st;

/* Define the union u_xsemp_to_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    xsemp_to_num          : 31; /* [30..0]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_xsemp_to_ctrl;

/* Define the union u_tsensor_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    temp_ct_sel           : 2; /* [1..0]  */
        unsigned int    temp_en               : 1; /* [2]  */
        unsigned int    tsensor0_temp_calib   : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_tsensor_ctrl;

/* Define the union u_tsensor_out */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    temp_out              : 10; /* [9..0]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_tsensor_out;

/* Define the union u_xempty_to_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    reserved_0            : 4; /* [3..0]  */
        unsigned int    bmc_xsemp_to_clr      : 1; /* [4]  */
        unsigned int    bmc_unse_xsemp_to_clr : 1; /* [5]  */
        unsigned int    pcie0_xsemp_to_clr    : 1; /* [6]  */
        unsigned int    pcie1_xsemp_to_clr    : 1; /* [7]  */
        unsigned int    reserved_1            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_xempty_to_clr;

/* Define the union u_pad_bmc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rstn_type             : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_pad_bmc_ctrl;

/* Define the union u_efuse_wait_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    efuse_wait_num        : 20; /* [19..0]  */
        unsigned int    reserved_0            : 11; /* [30..20]  */
        unsigned int    efuse_wait_to         : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_efuse_wait_reg;

/* Define the union u_wd_timer_clk_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdclken0              : 1; /* [0]  */
        unsigned int    wdclken1              : 1; /* [1]  */
        unsigned int    wdclken2              : 1; /* [2]  */
        unsigned int    wdclken3              : 1; /* [3]  */
        unsigned int    wdclken4              : 1; /* [4]  */
        unsigned int    wdclken5              : 1; /* [5]  */
        unsigned int    wdclken6              : 1; /* [6]  */
        unsigned int    timerclken0           : 1; /* [7]  */
        unsigned int    timerclken1           : 1; /* [8]  */
        unsigned int    timerclken2           : 1; /* [9]  */
        unsigned int    timerclken3           : 1; /* [10]  */
        unsigned int    timerclken4           : 1; /* [11]  */
        unsigned int    timerclken5           : 1; /* [12]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_wd_timer_clk_en;

/* Define the union u_misc_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dcm_reg               : 1; /* [0]  */
        unsigned int    gmac2_rtune_ack       : 1; /* [1]  */
        unsigned int    gmac3_rtune_ack       : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_misc_st;

/* Define the union u_misc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    h2b_edma_int_clr      : 1; /* [0]  */
        unsigned int    vgas_clk_sel          : 1; /* [1]  */
        unsigned int    gpu_bypass_en         : 1; /* [2]  */
        unsigned int    pcie1_rst_domain      : 1; /* [3]  */
        unsigned int    dac_rsvl              : 1; /* [4]  */
        unsigned int    sysctrl_probe         : 3; /* [7..5]  */
        unsigned int    io_cfg_done           : 1; /* [8]  */
        unsigned int    usb0_utmi_clk_sel     : 1; /* [9]  */
        unsigned int    usb1_utmi_clk_sel     : 1; /* [10]  */
        unsigned int    usb2_utmi_clk_sel     : 1; /* [11]  */
        unsigned int    usb3_utmi_clk_sel     : 1; /* [12]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_misc_ctrl;

/* Define the union u_heartbeat_cnt_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hb_cnt_cfg            : 31; /* [30..0]  */
        unsigned int    hb_cnt_en             : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_heartbeat_cnt_ctrl;

/* Define the union u_jtag0_mode_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    jlc0_mode_set         : 1; /* [0]  */
        unsigned int    jlc0_mode             : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_jtag0_mode_sel;

/* Define the union u_dac_ctl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sc_dac_en             : 3; /* [2..0]  */
        unsigned int    dac_dac0gc            : 6; /* [8..3]  */
        unsigned int    dac_dac1gc            : 6; /* [14..9]  */
        unsigned int    dac_dac2gc            : 6; /* [20..15]  */
        unsigned int    dac_enctr             : 3; /* [23..21]  */
        unsigned int    dac_ensc              : 3; /* [26..24]  */
        unsigned int    dac_envbg             : 1; /* [27]  */
        unsigned int    dac_enextref          : 1; /* [28]  */
        unsigned int    reserved_0            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_dac_ctl;

/* Define the union u_dac_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dac_cableout          : 3; /* [2..0]  */
        unsigned int    dac_enctr3            : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_dac_status;

/* Define the union u_core_tap_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    core_tap_sel          : 2; /* [1..0]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_core_tap_sel;

/* Define the union u_rw_exclusive */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    rw_exclu0             : 4; /* [3..0]  */
        unsigned int    rw_exclu1             : 4; /* [7..4]  */
        unsigned int    rw_exclu2             : 4; /* [11..8]  */
        unsigned int    rw_exclu3             : 4; /* [15..12]  */
        unsigned int    rw_exclu4             : 4; /* [19..16]  */
        unsigned int    rw_exclu5             : 4; /* [23..20]  */
        unsigned int    rw_exclu6             : 4; /* [27..24]  */
        unsigned int    rw_exclu7             : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val ;
} u_rw_exclusive;

/* Define the union u_pcie0_rst_record */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    last_pcie0_rst_src    : 2; /* [1..0]  */
        unsigned int    second_pcie0_rst_src  : 2; /* [3..2]  */
        unsigned int    third_pcie0_rst_src   : 2; /* [5..4]  */
        unsigned int    fourth_pcie0_rst_src  : 2; /* [7..6]  */
        unsigned int    pcie0_rst_cnt         : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_pcie0_rst_record;

/* Define the union u_pcie1_rst_record */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    last_pcie1_rst_src    : 3; /* [2..0]  */
        unsigned int    second_pcie1_rst_src  : 3; /* [5..3]  */
        unsigned int    third_pcie1_rst_src   : 3; /* [8..6]  */
        unsigned int    fourth_pcie1_rst_src  : 3; /* [11..9]  */
        unsigned int    pcie1_rst_cnt         : 4; /* [15..12]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_pcie1_rst_record;

/* Define the union u_warm_ret_lock1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    efuse_wait_num_lock   : 20; /* [19..0]  */
        unsigned int    chip_rst_out_en_lock  : 1; /* [20]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_warm_ret_lock1;

/* Define the union u_dieid11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    efuse_bit376_352      : 25; /* [24..0]  */
        unsigned int    reserved_0            : 6; /* [30..25]  */
        unsigned int    dieid_valid           : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_dieid11;

/* Define the union u_func_bist_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    func_div_num          : 4; /* [3..0]  */
        unsigned int    cpu_enable            : 1; /* [4]  */
        unsigned int    cpu_writeen           : 1; /* [5]  */
        unsigned int    sms_func_mbist_mode   : 1; /* [6]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_func_bist_ctrl;

#endif /* __SYSCTRL_CSR_C_UNION_DEFINE_H__ */
