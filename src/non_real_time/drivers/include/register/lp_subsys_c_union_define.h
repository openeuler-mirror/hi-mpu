// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __LP_SUBSYS_CSR_C_UNION_DEFINE_H__
#define __LP_SUBSYS_CSR_C_UNION_DEFINE_H__


/* Define the union u_lpsc_addr_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m3_ibus_addr_sel      : 2; /* [1..0]  */
        unsigned int    m3_dbus_addr_sel      : 2; /* [3..2]  */
        unsigned int    m3_sbus_addr_sel      : 2; /* [5..4]  */
        unsigned int    sfc1_addr_sel         : 2; /* [7..6]  */
        unsigned int    emmc_addr_sel         : 2; /* [9..8]  */
        unsigned int    sdio_addr_sel         : 2; /* [11..10]  */
        unsigned int    lpc_sol_addr_sel      : 2; /* [13..12]  */
        unsigned int    espi_sol_addr_sel     : 2; /* [15..14]  */
        unsigned int    uart2_sol_addr_sel    : 2; /* [17..16]  */
        unsigned int    uart3_sol_addr_sel    : 2; /* [19..18]  */
        unsigned int    uart4_sol_addr_sel    : 2; /* [21..20]  */
        unsigned int    espi_dma_addr_sel     : 2; /* [23..22]  */
        unsigned int    sfc0_addr_sel         : 2; /* [25..24]  */
        unsigned int    dmac_awaddr_sel       : 2; /* [27..26]  */
        unsigned int    dmac_araddr_sel       : 2; /* [29..28]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_addr_sel;

/* Define the union u_lpsc_srst_req0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    exwdog_srst_req       : 1; /* [0]  */
        unsigned int    m3_dap_srst_req       : 1; /* [1]  */
        unsigned int    m3_srst_req           : 1; /* [2]  */
        unsigned int    sdio_srst_req         : 1; /* [3]  */
        unsigned int    lb_srst_req           : 1; /* [4]  */
        unsigned int    sfc0_srst_req         : 1; /* [5]  */
        unsigned int    sfc1_srst_req         : 1; /* [6]  */
        unsigned int    spi0_srst_req         : 1; /* [7]  */
        unsigned int    spi1_srst_req         : 1; /* [8]  */
        unsigned int    i2c0_srst_req         : 1; /* [9]  */
        unsigned int    i2c1_srst_req         : 1; /* [10]  */
        unsigned int    i2c2_srst_req         : 1; /* [11]  */
        unsigned int    i2c3_srst_req         : 1; /* [12]  */
        unsigned int    i2c4_srst_req         : 1; /* [13]  */
        unsigned int    i2c5_srst_req         : 1; /* [14]  */
        unsigned int    i2c6_srst_req         : 1; /* [15]  */
        unsigned int    i2c7_srst_req         : 1; /* [16]  */
        unsigned int    i2c8_srst_req         : 1; /* [17]  */
        unsigned int    i2c9_srst_req         : 1; /* [18]  */
        unsigned int    i2c10_srst_req        : 1; /* [19]  */
        unsigned int    i2c11_srst_req        : 1; /* [20]  */
        unsigned int    i2c12_srst_req        : 1; /* [21]  */
        unsigned int    i2c13_srst_req        : 1; /* [22]  */
        unsigned int    i2c14_srst_req        : 1; /* [23]  */
        unsigned int    i2c15_srst_req        : 1; /* [24]  */
        unsigned int    gpio0_srst_req        : 1; /* [25]  */
        unsigned int    gpio1_srst_req        : 1; /* [26]  */
        unsigned int    gpio2_srst_req        : 1; /* [27]  */
        unsigned int    gpio3_srst_req        : 1; /* [28]  */
        unsigned int    gpio4_srst_req        : 1; /* [29]  */
        unsigned int    lpc_srst_req          : 1; /* [30]  */
        unsigned int    peci_srst_req         : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_srst_req0;

/* Define the union u_lpsc_srst_req1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pwm_srst_req          : 1; /* [0]  */
        unsigned int    adc0_srst_req         : 1; /* [1]  */
        unsigned int    canbus0_srst_req      : 1; /* [2]  */
        unsigned int    canbus1_srst_req      : 1; /* [3]  */
        unsigned int    kcs0_srst_req         : 1; /* [4]  */
        unsigned int    kcs1_srst_req         : 1; /* [5]  */
        unsigned int    kcs2_srst_req         : 1; /* [6]  */
        unsigned int    kcs3_srst_req         : 1; /* [7]  */
        unsigned int    bt0_srst_req          : 1; /* [8]  */
        unsigned int    bt1_srst_req          : 1; /* [9]  */
        unsigned int    c80_srst_req          : 1; /* [10]  */
        unsigned int    mailbos_srst_req      : 1; /* [11]  */
        unsigned int    hostuart1_srst_req    : 1; /* [12]  */
        unsigned int    hostuart0_srst_req    : 1; /* [13]  */
        unsigned int    sol_srst_req          : 1; /* [14]  */
        unsigned int    hostwdi_srst_req      : 1; /* [15]  */
        unsigned int    bmcrtc_srst_req       : 1; /* [16]  */
        unsigned int    ipmbi2c0_srst_req     : 1; /* [17]  */
        unsigned int    ipmbi2c1_srst_req     : 1; /* [18]  */
        unsigned int    ipmbi2c2_srst_req     : 1; /* [19]  */
        unsigned int    ipmbi2c3_srst_req     : 1; /* [20]  */
        unsigned int    mdio_srst_req         : 1; /* [21]  */
        unsigned int    uart2_srst_req        : 1; /* [22]  */
        unsigned int    uart3_srst_req        : 1; /* [23]  */
        unsigned int    uart4_srst_req        : 1; /* [24]  */
        unsigned int    uart5_srst_req        : 1; /* [25]  */
        unsigned int    uart6_srst_req        : 1; /* [26]  */
        unsigned int    uart7_srst_req        : 1; /* [27]  */
        unsigned int    sgpio0_srst_req       : 1; /* [28]  */
        unsigned int    sgpio1_srst_req       : 1; /* [29]  */
        unsigned int    jlc0_srst_req         : 1; /* [30]  */
        unsigned int    jlc1_srst_req         : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_srst_req1;

/* Define the union u_lpsc_srst_req2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdg0_srst_req         : 1; /* [0]  */
        unsigned int    wdg1_srst_req         : 1; /* [1]  */
        unsigned int    wdg2_srst_req         : 1; /* [2]  */
        unsigned int    wdg3_srst_req         : 1; /* [3]  */
        unsigned int    wdg4_srst_req         : 1; /* [4]  */
        unsigned int    wdg5_srst_req         : 1; /* [5]  */
        unsigned int    hpm_srst_req          : 1; /* [6]  */
        unsigned int    timer01_srst_req      : 1; /* [7]  */
        unsigned int    timer23_srst_req      : 1; /* [8]  */
        unsigned int    timer45_srst_req      : 1; /* [9]  */
        unsigned int    uart2_sol_srst_req    : 1; /* [10]  */
        unsigned int    uart3_sol_srst_req    : 1; /* [11]  */
        unsigned int    uart4_sol_srst_req    : 1; /* [12]  */
        unsigned int    espi_sol_srst_req     : 1; /* [13]  */
        unsigned int    adc1_srst_req         : 1; /* [14]  */
        unsigned int    peci_single_srst_req  : 1; /* [15]  */
        unsigned int    trng_srst_req         : 1; /* [16]  */
        unsigned int    efuse_srst_req        : 1; /* [17]  */
        unsigned int    dmac_srst_req         : 1; /* [18]  */
        unsigned int    djtag_mstr_srst_req   : 1; /* [19]  */
        unsigned int    ipcm_srst_req         : 1; /* [20]  */
        unsigned int    oob_rx_srst_req       : 1; /* [21]  */
        unsigned int    oob_tx_srst_req       : 1; /* [22]  */
        unsigned int    vw_irq_srst_req       : 1; /* [23]  */
        unsigned int    vw_gpio_srst_req      : 1; /* [24]  */
        unsigned int    vw_sys_srst_req       : 1; /* [25]  */
        unsigned int    dma_tx_srst_req       : 1; /* [26]  */
        unsigned int    dma_rx_srst_req       : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_srst_req2;

/* Define the union u_lpsc_clken_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    jlc0_clk_enb          : 1; /* [0]  */
        unsigned int    jlc1_clk_enb          : 1; /* [1]  */
        unsigned int    timer01_clk_enb       : 1; /* [2]  */
        unsigned int    timer23_clk_enb       : 1; /* [3]  */
        unsigned int    timer45_clk_enb       : 1; /* [4]  */
        unsigned int    sfc0_clk_enb          : 1; /* [5]  */
        unsigned int    sfc1_clk_enb          : 1; /* [6]  */
        unsigned int    m3_clk_enb            : 1; /* [7]  */
        unsigned int    spi0_clk_enb          : 1; /* [8]  */
        unsigned int    spi1_clk_enb          : 1; /* [9]  */
        unsigned int    i2c0_clk_enb          : 1; /* [10]  */
        unsigned int    i2c1_clk_enb          : 1; /* [11]  */
        unsigned int    i2c2_clk_enb          : 1; /* [12]  */
        unsigned int    i2c3_clk_enb          : 1; /* [13]  */
        unsigned int    i2c4_clk_enb          : 1; /* [14]  */
        unsigned int    i2c5_clk_enb          : 1; /* [15]  */
        unsigned int    i2c6_clk_enb          : 1; /* [16]  */
        unsigned int    i2c7_clk_enb          : 1; /* [17]  */
        unsigned int    i2c8_clk_enb          : 1; /* [18]  */
        unsigned int    i2c9_clk_enb          : 1; /* [19]  */
        unsigned int    i2c10_clk_enb         : 1; /* [20]  */
        unsigned int    i2c11_clk_enb         : 1; /* [21]  */
        unsigned int    i2c12_clk_enb         : 1; /* [22]  */
        unsigned int    i2c13_clk_enb         : 1; /* [23]  */
        unsigned int    i2c14_clk_enb         : 1; /* [24]  */
        unsigned int    i2c15_clk_enb         : 1; /* [25]  */
        unsigned int    gpio0_clk_enb         : 1; /* [26]  */
        unsigned int    gpio1_clk_enb         : 1; /* [27]  */
        unsigned int    gpio2_clk_enb         : 1; /* [28]  */
        unsigned int    gpio3_clk_enb         : 1; /* [29]  */
        unsigned int    gpio4_clk_enb         : 1; /* [30]  */
        unsigned int    lpc_clk_enb           : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clken_ctrl0;

/* Define the union u_lpsc_clken_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipbmi2c0_clk_enb      : 1; /* [0]  */
        unsigned int    ipbmi2c1_clk_enb      : 1; /* [1]  */
        unsigned int    ipbmi2c2_clk_enb      : 1; /* [2]  */
        unsigned int    ipbmi2c3_clk_enb      : 1; /* [3]  */
        unsigned int    superio_clk_enb       : 1; /* [4]  */
        unsigned int    lb_clk_enb            : 1; /* [5]  */
        unsigned int    uart2_clk_enb         : 1; /* [6]  */
        unsigned int    uart3_clk_enb         : 1; /* [7]  */
        unsigned int    uart4_clk_enb         : 1; /* [8]  */
        unsigned int    uart5_clk_enb         : 1; /* [9]  */
        unsigned int    uart6_clk_enb         : 1; /* [10]  */
        unsigned int    uart7_clk_enb         : 1; /* [11]  */
        unsigned int    peci_clk_enb          : 1; /* [12]  */
        unsigned int    pwm_clk_enb           : 1; /* [13]  */
        unsigned int    canbus0_clk_enb       : 1; /* [14]  */
        unsigned int    canbus1_clk_enb       : 1; /* [15]  */
        unsigned int    dmac_clk_enb          : 1; /* [16]  */
        unsigned int    wdg0_pclk_enb         : 1; /* [17]  */
        unsigned int    wdg1_pclk_enb         : 1; /* [18]  */
        unsigned int    wdg2_pclk_enb         : 1; /* [19]  */
        unsigned int    wdg3_pclk_enb         : 1; /* [20]  */
        unsigned int    wdg4_pclk_enb         : 1; /* [21]  */
        unsigned int    wdg5_pclk_enb         : 1; /* [22]  */
        unsigned int    hpm_clk_enb           : 1; /* [23]  */
        unsigned int    sgpio0_clk_enb        : 1; /* [24]  */
        unsigned int    sgpio1_clk_enb        : 1; /* [25]  */
        unsigned int    sdio_clk_enb          : 1; /* [26]  */
        unsigned int    bmcrtc_clk_enb        : 1; /* [27]  */
        unsigned int    adc0_clk_enb          : 1; /* [28]  */
        unsigned int    adc1_clk_enb          : 1; /* [29]  */
        unsigned int    peci_single_clk_enb   : 1; /* [30]  */
        unsigned int    trng_clk_enb          : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clken_ctrl1;

/* Define the union u_lpsc_crg_div_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sdio_clk_div_cnt      : 5; /* [4..0]  */
        unsigned int    emmc_clk_div_cnt      : 5; /* [9..5]  */
        unsigned int    sfc0_clk_div_cnt      : 5; /* [14..10]  */
        unsigned int    sfc1_clk_div_cnt      : 5; /* [19..15]  */
        unsigned int    apb_clk_div_cnt       : 4; /* [23..20]  */
        unsigned int    ad_clk_div_cnt        : 5; /* [28..24]  */
        unsigned int    reserved_0            : 3; /* [31..29]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_crg_div_ctrl;

/* Define the union u_lpsc_spi_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    spi0_in_n             : 1; /* [0]  */
        unsigned int    spi0_cs_polarity0     : 1; /* [1]  */
        unsigned int    spi0_cs_polarity1     : 1; /* [2]  */
        unsigned int    spi0_wire_mode        : 1; /* [3]  */
        unsigned int    spi0_rx_only          : 1; /* [4]  */
        unsigned int    spi1_in_n             : 1; /* [5]  */
        unsigned int    spi1_cs_polarity0     : 1; /* [6]  */
        unsigned int    spi1_cs_polarity1     : 1; /* [7]  */
        unsigned int    spi1_wire_mode        : 1; /* [8]  */
        unsigned int    spi1_rx_only          : 1; /* [9]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_spi_ctrl;

/* Define the union u_lpsc_mdio_sdio_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    mdio_intf_mode_port0  : 1; /* [0]  */
        unsigned int    mdio_intf_mode_port1  : 1; /* [1]  */
        unsigned int    mdio_intf_mode_port2  : 1; /* [2]  */
        unsigned int    mdio_intf_mode_port3  : 1; /* [3]  */
        unsigned int    mdio_intf_mode_port4  : 1; /* [4]  */
        unsigned int    mmc_cclk_samp_ctrl    : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_mdio_sdio_ctrl;

/* Define the union u_lpsc_superio_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    cfg_noused_irq_level  : 1; /* [0]  */
        unsigned int    cfg_used_irq_level    : 1; /* [1]  */
        unsigned int    lpc_espi_sel          : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_superio_ctrl;

/* Define the union u_lpsc_iom3_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m3_dbg_en             : 1; /* [0]  */
        unsigned int    m3_mpudisable         : 1; /* [1]  */
        unsigned int    m3_halted_bypass      : 1; /* [2]  */
        unsigned int    m3_stclk_bypass       : 1; /* [3]  */
        unsigned int    m3_dap_clken          : 1; /* [4]  */
        unsigned int    m3_stcalib            : 26; /* [30..5]  */
        unsigned int    reserved_0            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_iom3_ctrl;

/* Define the union u_lpsc_uart_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    port0_sel             : 4; /* [3..0]  */
        unsigned int    port1_sel             : 4; /* [7..4]  */
        unsigned int    port2_sel             : 4; /* [11..8]  */
        unsigned int    port3_sel             : 4; /* [15..12]  */
        unsigned int    port4_sel             : 4; /* [19..16]  */
        unsigned int    port5_sel             : 4; /* [23..20]  */
        unsigned int    port6_sel             : 4; /* [27..24]  */
        unsigned int    port7_sel             : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_uart_ctrl0;

/* Define the union u_lpsc_uart_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    uart0_sel             : 4; /* [3..0]  */
        unsigned int    uart1_sel             : 4; /* [7..4]  */
        unsigned int    uart2_sel             : 4; /* [11..8]  */
        unsigned int    uart3_sel             : 4; /* [15..12]  */
        unsigned int    uart4_sel             : 4; /* [19..16]  */
        unsigned int    uart5_sel             : 4; /* [23..20]  */
        unsigned int    uart6_sel             : 4; /* [27..24]  */
        unsigned int    uart7_sel             : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_uart_ctrl1;

/* Define the union u_lpsc_uart_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    uart_enable           : 8; /* [7..0]  */
        unsigned int    port_enable           : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_uart_sel;

/* Define the union u_lpsc_reset_sel */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pwm_rst_sel           : 1; /* [0]  */
        unsigned int    gpio0_rst_sel         : 1; /* [1]  */
        unsigned int    gpio1_rst_sel         : 1; /* [2]  */
        unsigned int    gpio2_rst_sel         : 1; /* [3]  */
        unsigned int    gpio3_rst_sel         : 1; /* [4]  */
        unsigned int    gpio4_rst_sel         : 1; /* [5]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_sel;

/* Define the union u_lpsc_peri_dbg_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    app_m_dbg_wfq_weight  : 16; /* [15..0]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_peri_dbg_ctrl0;

/* Define the union u_lpsc_peri_dbg_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    app_m_dbg_incr_spen   : 1; /* [0]  */
        unsigned int    app_m_dbg_rd_outstd   : 4; /* [4..1]  */
        unsigned int    app_m_dbg_wr_outstd   : 4; /* [8..5]  */
        unsigned int    app_m_dbg_h2x_sel     : 1; /* [9]  */
        unsigned int    app_m_dbg_port_sel    : 4; /* [13..10]  */
        unsigned int    app_m_dbg_stat_clr    : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_peri_dbg_ctrl1;

/* Define the union u_lpsc_i2c_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c0_clk_mux_sel      : 1; /* [0]  */
        unsigned int    i2c0_clk_oe_cfg       : 1; /* [1]  */
        unsigned int    i2c0_scl_cfg          : 1; /* [2]  */
        unsigned int    i2c0_dat_mux_sel      : 1; /* [3]  */
        unsigned int    i2c0_dat_oe_cfg       : 1; /* [4]  */
        unsigned int    i2c0_sda_cfg          : 1; /* [5]  */
        unsigned int    i2c1_clk_mux_sel      : 1; /* [6]  */
        unsigned int    i2c1_clk_oe_cfg       : 1; /* [7]  */
        unsigned int    i2c1_scl_cfg          : 1; /* [8]  */
        unsigned int    i2c1_dat_mux_sel      : 1; /* [9]  */
        unsigned int    i2c1_dat_oe_cfg       : 1; /* [10]  */
        unsigned int    i2c1_sda_cfg          : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl0;

/* Define the union u_lpsc_i2c_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c2_clk_mux_sel      : 1; /* [0]  */
        unsigned int    i2c2_clk_oe_cfg       : 1; /* [1]  */
        unsigned int    i2c2_scl_cfg          : 1; /* [2]  */
        unsigned int    i2c2_dat_mux_sel      : 1; /* [3]  */
        unsigned int    i2c2_dat_oe_cfg       : 1; /* [4]  */
        unsigned int    i2c2_sda_cfg          : 1; /* [5]  */
        unsigned int    i2c3_clk_mux_sel      : 1; /* [6]  */
        unsigned int    i2c3_clk_oe_cfg       : 1; /* [7]  */
        unsigned int    i2c3_scl_cfg          : 1; /* [8]  */
        unsigned int    i2c3_dat_mux_sel      : 1; /* [9]  */
        unsigned int    i2c3_dat_oe_cfg       : 1; /* [10]  */
        unsigned int    i2c3_sda_cfg          : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl1;

/* Define the union u_lpsc_i2c_ctrl2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c4_clk_mux_sel      : 1; /* [0]  */
        unsigned int    i2c4_clk_oe_cfg       : 1; /* [1]  */
        unsigned int    i2c4_scl_cfg          : 1; /* [2]  */
        unsigned int    i2c4_dat_mux_sel      : 1; /* [3]  */
        unsigned int    i2c4_dat_oe_cfg       : 1; /* [4]  */
        unsigned int    i2c4_sda_cfg          : 1; /* [5]  */
        unsigned int    i2c5_clk_mux_sel      : 1; /* [6]  */
        unsigned int    i2c5_clk_oe_cfg       : 1; /* [7]  */
        unsigned int    i2c5_scl_cfg          : 1; /* [8]  */
        unsigned int    i2c5_dat_mux_sel      : 1; /* [9]  */
        unsigned int    i2c5_dat_oe_cfg       : 1; /* [10]  */
        unsigned int    i2c5_sda_cfg          : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl2;

/* Define the union u_lpsc_i2c_ctrl3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c6_clk_mux_sel      : 1; /* [0]  */
        unsigned int    i2c6_clk_oe_cfg       : 1; /* [1]  */
        unsigned int    i2c6_scl_cfg          : 1; /* [2]  */
        unsigned int    i2c6_dat_mux_sel      : 1; /* [3]  */
        unsigned int    i2c6_dat_oe_cfg       : 1; /* [4]  */
        unsigned int    i2c6_sda_cfg          : 1; /* [5]  */
        unsigned int    i2c7_clk_mux_sel      : 1; /* [6]  */
        unsigned int    i2c7_clk_oe_cfg       : 1; /* [7]  */
        unsigned int    i2c7_scl_cfg          : 1; /* [8]  */
        unsigned int    i2c7_dat_mux_sel      : 1; /* [9]  */
        unsigned int    i2c7_dat_oe_cfg       : 1; /* [10]  */
        unsigned int    i2c7_sda_cfg          : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl3;

/* Define the union u_lpsc_i2c_ctrl4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c8_clk_mux_sel      : 1; /* [0]  */
        unsigned int    i2c8_clk_oe_cfg       : 1; /* [1]  */
        unsigned int    i2c8_scl_cfg          : 1; /* [2]  */
        unsigned int    i2c8_dat_mux_sel      : 1; /* [3]  */
        unsigned int    i2c8_dat_oe_cfg       : 1; /* [4]  */
        unsigned int    i2c8_sda_cfg          : 1; /* [5]  */
        unsigned int    i2c9_clk_mux_sel      : 1; /* [6]  */
        unsigned int    i2c9_clk_oe_cfg       : 1; /* [7]  */
        unsigned int    i2c9_scl_cfg          : 1; /* [8]  */
        unsigned int    i2c9_dat_mux_sel      : 1; /* [9]  */
        unsigned int    i2c9_dat_oe_cfg       : 1; /* [10]  */
        unsigned int    i2c9_sda_cfg          : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl4;

/* Define the union u_lpsc_i2c_ctrl5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c10_clk_mux_sel     : 1; /* [0]  */
        unsigned int    i2c10_clk_oe_cfg      : 1; /* [1]  */
        unsigned int    i2c10_scl_cfg         : 1; /* [2]  */
        unsigned int    i2c10_dat_mux_sel     : 1; /* [3]  */
        unsigned int    i2c10_dat_oe_cfg      : 1; /* [4]  */
        unsigned int    i2c10_sda_cfg         : 1; /* [5]  */
        unsigned int    i2c11_clk_mux_sel     : 1; /* [6]  */
        unsigned int    i2c11_clk_oe_cfg      : 1; /* [7]  */
        unsigned int    i2c11_scl_cfg         : 1; /* [8]  */
        unsigned int    i2c11_dat_mux_sel     : 1; /* [9]  */
        unsigned int    i2c11_dat_oe_cfg      : 1; /* [10]  */
        unsigned int    i2c11_sda_cfg         : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl5;

/* Define the union u_lpsc_i2c_ctrl6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c12_clk_mux_sel     : 1; /* [0]  */
        unsigned int    i2c12_clk_oe_cfg      : 1; /* [1]  */
        unsigned int    i2c12_scl_cfg         : 1; /* [2]  */
        unsigned int    i2c12_dat_mux_sel     : 1; /* [3]  */
        unsigned int    i2c12_dat_oe_cfg      : 1; /* [4]  */
        unsigned int    i2c12_sda_cfg         : 1; /* [5]  */
        unsigned int    i2c13_clk_mux_sel     : 1; /* [6]  */
        unsigned int    i2c13_clk_oe_cfg      : 1; /* [7]  */
        unsigned int    i2c13_scl_cfg         : 1; /* [8]  */
        unsigned int    i2c13_dat_mux_sel     : 1; /* [9]  */
        unsigned int    i2c13_dat_oe_cfg      : 1; /* [10]  */
        unsigned int    i2c13_sda_cfg         : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl6;

/* Define the union u_lpsc_i2c_ctrl7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c14_clk_mux_sel     : 1; /* [0]  */
        unsigned int    i2c14_clk_oe_cfg      : 1; /* [1]  */
        unsigned int    i2c14_scl_cfg         : 1; /* [2]  */
        unsigned int    i2c14_dat_mux_sel     : 1; /* [3]  */
        unsigned int    i2c14_dat_oe_cfg      : 1; /* [4]  */
        unsigned int    i2c14_sda_cfg         : 1; /* [5]  */
        unsigned int    i2c15_clk_mux_sel     : 1; /* [6]  */
        unsigned int    i2c15_clk_oe_cfg      : 1; /* [7]  */
        unsigned int    i2c15_scl_cfg         : 1; /* [8]  */
        unsigned int    i2c15_dat_mux_sel     : 1; /* [9]  */
        unsigned int    i2c15_dat_oe_cfg      : 1; /* [10]  */
        unsigned int    i2c15_sda_cfg         : 1; /* [11]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c_ctrl7;

/* Define the union u_lpsc_dmac_req_sel0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel0         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel0;

/* Define the union u_lpsc_dmac_req_sel1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel1         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel1;

/* Define the union u_lpsc_dmac_req_sel2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel2         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel2;

/* Define the union u_lpsc_dmac_req_sel3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel3         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel3;

/* Define the union u_lpsc_dmac_req_sel4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel4         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel4;

/* Define the union u_lpsc_dmac_req_sel5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel5         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel5;

/* Define the union u_lpsc_dmac_req_sel6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel6         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel6;

/* Define the union u_lpsc_dmac_req_sel7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel7         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel7;

/* Define the union u_lpsc_dmac_req_sel8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel8         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel8;

/* Define the union u_lpsc_dmac_req_sel9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel9         : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel9;

/* Define the union u_lpsc_dmac_req_sel10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel10        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel10;

/* Define the union u_lpsc_dmac_req_sel11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel11        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel11;

/* Define the union u_lpsc_dmac_req_sel12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel12        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel12;

/* Define the union u_lpsc_dmac_req_sel13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel13        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel13;

/* Define the union u_lpsc_dmac_req_sel14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel14        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel14;

/* Define the union u_lpsc_dmac_req_sel15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel15        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel15;

/* Define the union u_lpsc_dmac_req_sel16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel16        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel16;

/* Define the union u_lpsc_dmac_req_sel17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel17        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel17;

/* Define the union u_lpsc_dmac_req_sel18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel18        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel18;

/* Define the union u_lpsc_dmac_req_sel19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel19        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel19;

/* Define the union u_lpsc_dmac_req_sel20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel20        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel20;

/* Define the union u_lpsc_dmac_req_sel21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel21        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel21;

/* Define the union u_lpsc_dmac_req_sel22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel22        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel22;

/* Define the union u_lpsc_dmac_req_sel23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel23        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel23;

/* Define the union u_lpsc_dmac_req_sel24 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel24        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel24;

/* Define the union u_lpsc_dmac_req_sel25 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel25        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel25;

/* Define the union u_lpsc_dmac_req_sel26 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel26        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel26;

/* Define the union u_lpsc_dmac_req_sel27 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel27        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel27;

/* Define the union u_lpsc_dmac_req_sel28 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel28        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel28;

/* Define the union u_lpsc_dmac_req_sel29 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel29        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel29;

/* Define the union u_lpsc_dmac_req_sel30 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel30        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel30;

/* Define the union u_lpsc_dmac_req_sel31 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_req_sel31        : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_req_sel31;

/* Define the union u_lpsc_ahb_mst_hprot */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sdio_hprot            : 4; /* [3..0]  */
        unsigned int    emmc_hprot            : 4; /* [7..4]  */
        unsigned int    sfc_hprot             : 4; /* [11..8]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ahb_mst_hprot;

/* Define the union u_lpsc_emmc_func_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sc_sdxc_fix_bypass    : 1; /* [0]  */
        unsigned int    sc_sdio_drv_ps_sel    : 3; /* [3..1]  */
        unsigned int    sc_sdio_sap_ps_sel    : 3; /* [6..4]  */
        unsigned int    sc_sdio_sap_dll_tune  : 4; /* [10..7]  */
        unsigned int    sc_sdio_sap_dll_slave_en : 1; /* [11]  */
        unsigned int    sc_sdio_sap_dll_bypass : 1; /* [12]  */
        unsigned int    sc_sdio_sap_dll_stop  : 1; /* [13]  */
        unsigned int    sc_sdio_sap_dll_dllmode : 1; /* [14]  */
        unsigned int    sc_sdio_sap_dll_dll_ssel : 8; /* [22..15]  */
        unsigned int    reserved_0            : 9; /* [31..23]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emmc_func_ctrl;

/* Define the union u_lpsc_emmc_func_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sc_emmc_dll_ds_tune   : 4; /* [3..0]  */
        unsigned int    sc_emmc_dll_ds_slave_en : 1; /* [4]  */
        unsigned int    sc_emmc_dll_ds_bypass : 1; /* [5]  */
        unsigned int    sc_emmc_dll_ds_stop   : 1; /* [6]  */
        unsigned int    sc_emmc_dll_ds_dllmode : 1; /* [7]  */
        unsigned int    sc_emmc_dll_ds_dllssel : 8; /* [15..8]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emmc_func_ctrl1;

/* Define the union u_lpsc_emmc_crg_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    emmc_dll_srst_req     : 1; /* [0]  */
        unsigned int    emmc_srst_req         : 1; /* [1]  */
        unsigned int    emmc_dll_clk_en       : 1; /* [2]  */
        unsigned int    emmc_clk_en           : 1; /* [3]  */
        unsigned int    emmc_bus_clk_en       : 1; /* [4]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emmc_crg_ctrl;

/* Define the union u_lpsc_low_power_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    p0_1x16_dw_axi_x2p_csysreq : 1; /* [0]  */
        unsigned int    p1_1x16_dw_axi_x2p_csysreq : 1; /* [1]  */
        unsigned int    p2_1x16_dw_axi_x2p_csysreq : 1; /* [2]  */
        unsigned int    p3_1x16_dw_axi_x2p_csysreq : 1; /* [3]  */
        unsigned int    p4_1x16_dw_axi_x2p_csysreq : 1; /* [4]  */
        unsigned int    p5_1x16_dw_axi_x2p_csysreq : 1; /* [5]  */
        unsigned int    p6_1x16_dw_axi_x2p_csysreq : 1; /* [6]  */
        unsigned int    m3_ibus_csysreq       : 1; /* [7]  */
        unsigned int    m3_dbus_csysreq       : 1; /* [8]  */
        unsigned int    m3_sbus_csysreq       : 1; /* [9]  */
        unsigned int    se_sec_1x6_dw_axi_csysreq : 1; /* [10]  */
        unsigned int    se_sec_1x3_dw_axi_csysreq : 1; /* [11]  */
        unsigned int    se_sec_4x6_dw_axi_csysreq : 1; /* [12]  */
        unsigned int    se_dma_3x1_dw_axi_csysreq : 1; /* [13]  */
        unsigned int    peri_axi2ahb_brg_csysreq : 1; /* [14]  */
        unsigned int    lp_sol_2x1_axi_0_csysreq : 1; /* [15]  */
        unsigned int    lp_sol_2x1_axi_1_csysreq : 1; /* [16]  */
        unsigned int    se_m3_3x1_dw_axi_csysreq : 1; /* [17]  */
        unsigned int    dma_x2p_csysreq       : 1; /* [18]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_low_power_ctrl;

/* Define the union u_lpsc_smbus_i2c_mux */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    smbus_i2c_sel0        : 1; /* [0]  */
        unsigned int    smbus_i2c_sel1        : 1; /* [1]  */
        unsigned int    smbus_i2c_sel2        : 1; /* [2]  */
        unsigned int    smbus_i2c_sel3        : 1; /* [3]  */
        unsigned int    smbus_i2c_sel4        : 1; /* [4]  */
        unsigned int    smbus_i2c_sel5        : 1; /* [5]  */
        unsigned int    smbus_i2c_sel6        : 1; /* [6]  */
        unsigned int    smbus_i2c_sel7        : 1; /* [7]  */
        unsigned int    smbus_i2c_sel8        : 1; /* [8]  */
        unsigned int    smbus_i2c_sel9        : 1; /* [9]  */
        unsigned int    smbus_i2c_sel10       : 1; /* [10]  */
        unsigned int    smbus_i2c_sel11       : 1; /* [11]  */
        unsigned int    smbus_i2c_sel12       : 1; /* [12]  */
        unsigned int    smbus_i2c_sel13       : 1; /* [13]  */
        unsigned int    smbus_i2c_sel14       : 1; /* [14]  */
        unsigned int    smbus_i2c_sel15       : 1; /* [15]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_smbus_i2c_mux;

/* Define the union u_lpsc_pad_pwm_oen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_oen           : 28; /* [27..0]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_oen;

/* Define the union u_lpsc_pad_pwm_sel0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel0          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel0;

/* Define the union u_lpsc_pad_pwm_sel1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel1          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel1;

/* Define the union u_lpsc_pad_pwm_sel2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel2          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel2;

/* Define the union u_lpsc_pad_pwm_sel3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel3          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel3;

/* Define the union u_lpsc_pad_pwm_sel4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel4          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel4;

/* Define the union u_lpsc_pad_pwm_sel5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel5          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel5;

/* Define the union u_lpsc_pad_pwm_sel6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel6          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel6;

/* Define the union u_lpsc_pad_pwm_sel7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel7          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel7;

/* Define the union u_lpsc_pad_pwm_sel8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel8          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel8;

/* Define the union u_lpsc_pad_pwm_sel9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel9          : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel9;

/* Define the union u_lpsc_pad_pwm_sel10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel10         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel10;

/* Define the union u_lpsc_pad_pwm_sel11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel11         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel11;

/* Define the union u_lpsc_pad_pwm_sel12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel12         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel12;

/* Define the union u_lpsc_pad_pwm_sel13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel13         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel13;

/* Define the union u_lpsc_pad_pwm_sel14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel14         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel14;

/* Define the union u_lpsc_pad_pwm_sel15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel15         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel15;

/* Define the union u_lpsc_pad_pwm_sel16 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel16         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel16;

/* Define the union u_lpsc_pad_pwm_sel17 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel17         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel17;

/* Define the union u_lpsc_pad_pwm_sel18 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel18         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel18;

/* Define the union u_lpsc_pad_pwm_sel19 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel19         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel19;

/* Define the union u_lpsc_pad_pwm_sel20 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel20         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel20;

/* Define the union u_lpsc_pad_pwm_sel21 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel21         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel21;

/* Define the union u_lpsc_pad_pwm_sel22 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel22         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel22;

/* Define the union u_lpsc_pad_pwm_sel23 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel23         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel23;

/* Define the union u_lpsc_pad_pwm_sel24 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel24         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel24;

/* Define the union u_lpsc_pad_pwm_sel25 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel25         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel25;

/* Define the union u_lpsc_pad_pwm_sel26 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel26         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel26;

/* Define the union u_lpsc_pad_pwm_sel27 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_pwm_sel27         : 4; /* [3..0]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_pwm_sel27;

/* Define the union u_lpsc_pad_fan_sel0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel0          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel0;

/* Define the union u_lpsc_pad_fan_sel1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel1          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel1;

/* Define the union u_lpsc_pad_fan_sel2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel2          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel2;

/* Define the union u_lpsc_pad_fan_sel3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel3          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel3;

/* Define the union u_lpsc_pad_fan_sel4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel4          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel4;

/* Define the union u_lpsc_pad_fan_sel5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel5          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel5;

/* Define the union u_lpsc_pad_fan_sel6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel6          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel6;

/* Define the union u_lpsc_pad_fan_sel7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel7          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel7;

/* Define the union u_lpsc_pad_fan_sel8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel8          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel8;

/* Define the union u_lpsc_pad_fan_sel9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel9          : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel9;

/* Define the union u_lpsc_pad_fan_sel10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel10         : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel10;

/* Define the union u_lpsc_pad_fan_sel11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel11         : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel11;

/* Define the union u_lpsc_pad_fan_sel12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel12         : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel12;

/* Define the union u_lpsc_pad_fan_sel13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel13         : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel13;

/* Define the union u_lpsc_pad_fan_sel14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel14         : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel14;

/* Define the union u_lpsc_pad_fan_sel15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pad_fan_sel15         : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_fan_sel15;

/* Define the union u_lpsc_clkdis_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    jlc0_clk_dis          : 1; /* [0]  */
        unsigned int    jlc1_clk_dis          : 1; /* [1]  */
        unsigned int    timer01_clk_dis       : 1; /* [2]  */
        unsigned int    timer23_clk_dis       : 1; /* [3]  */
        unsigned int    timer45_clk_dis       : 1; /* [4]  */
        unsigned int    sfc0_clk_dis          : 1; /* [5]  */
        unsigned int    sfc1_clk_dis          : 1; /* [6]  */
        unsigned int    m3_clk_dis            : 1; /* [7]  */
        unsigned int    spi0_clk_dis          : 1; /* [8]  */
        unsigned int    spi1_clk_dis          : 1; /* [9]  */
        unsigned int    i2c0_clk_dis          : 1; /* [10]  */
        unsigned int    i2c1_clk_dis          : 1; /* [11]  */
        unsigned int    i2c2_clk_dis          : 1; /* [12]  */
        unsigned int    i2c3_clk_dis          : 1; /* [13]  */
        unsigned int    i2c4_clk_dis          : 1; /* [14]  */
        unsigned int    i2c5_clk_dis          : 1; /* [15]  */
        unsigned int    i2c6_clk_dis          : 1; /* [16]  */
        unsigned int    i2c7_clk_dis          : 1; /* [17]  */
        unsigned int    i2c8_clk_dis          : 1; /* [18]  */
        unsigned int    i2c9_clk_dis          : 1; /* [19]  */
        unsigned int    i2c10_clk_dis         : 1; /* [20]  */
        unsigned int    i2c11_clk_dis         : 1; /* [21]  */
        unsigned int    i2c12_clk_dis         : 1; /* [22]  */
        unsigned int    i2c13_clk_dis         : 1; /* [23]  */
        unsigned int    i2c14_clk_dis         : 1; /* [24]  */
        unsigned int    i2c15_clk_dis         : 1; /* [25]  */
        unsigned int    gpio0_clk_dis         : 1; /* [26]  */
        unsigned int    gpio1_clk_dis         : 1; /* [27]  */
        unsigned int    gpio2_clk_dis         : 1; /* [28]  */
        unsigned int    gpio3_clk_dis         : 1; /* [29]  */
        unsigned int    gpio4_clk_dis         : 1; /* [30]  */
        unsigned int    lpc_clk_dis           : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clkdis_ctrl0;

/* Define the union u_lpsc_clksta_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    jlc0_clk_status       : 1; /* [0]  */
        unsigned int    jlc1_clk_status       : 1; /* [1]  */
        unsigned int    timer01_clk_status    : 1; /* [2]  */
        unsigned int    timer23_clk_status    : 1; /* [3]  */
        unsigned int    timer45_clk_status    : 1; /* [4]  */
        unsigned int    sfc0_clk_status       : 1; /* [5]  */
        unsigned int    sfc1_clk_status       : 1; /* [6]  */
        unsigned int    m3_clk_status         : 1; /* [7]  */
        unsigned int    spi0_clk_status       : 1; /* [8]  */
        unsigned int    spi1_clk_status       : 1; /* [9]  */
        unsigned int    i2c0_clk_status       : 1; /* [10]  */
        unsigned int    i2c1_clk_status       : 1; /* [11]  */
        unsigned int    i2c2_clk_status       : 1; /* [12]  */
        unsigned int    i2c3_clk_status       : 1; /* [13]  */
        unsigned int    i2c4_clk_status       : 1; /* [14]  */
        unsigned int    i2c5_clk_status       : 1; /* [15]  */
        unsigned int    i2c6_clk_status       : 1; /* [16]  */
        unsigned int    i2c7_clk_status       : 1; /* [17]  */
        unsigned int    i2c8_clk_status       : 1; /* [18]  */
        unsigned int    i2c9_clk_status       : 1; /* [19]  */
        unsigned int    i2c10_clk_status      : 1; /* [20]  */
        unsigned int    i2c11_clk_status      : 1; /* [21]  */
        unsigned int    i2c12_clk_status      : 1; /* [22]  */
        unsigned int    i2c13_clk_status      : 1; /* [23]  */
        unsigned int    i2c14_clk_status      : 1; /* [24]  */
        unsigned int    i2c15_clk_status      : 1; /* [25]  */
        unsigned int    gpio0_clk_status      : 1; /* [26]  */
        unsigned int    gpio1_clk_status      : 1; /* [27]  */
        unsigned int    gpio2_clk_status      : 1; /* [28]  */
        unsigned int    gpio3_clk_status      : 1; /* [29]  */
        unsigned int    gpio4_clk_status      : 1; /* [30]  */
        unsigned int    lpc_clk_status        : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clksta_ctrl0;

/* Define the union u_lpsc_clkdis_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipbmi2c0_clk_dis      : 1; /* [0]  */
        unsigned int    ipbmi2c1_clk_dis      : 1; /* [1]  */
        unsigned int    ipbmi2c2_clk_dis      : 1; /* [2]  */
        unsigned int    ipbmi2c3_clk_dis      : 1; /* [3]  */
        unsigned int    superio_clk_dis       : 1; /* [4]  */
        unsigned int    lb_clk_dis            : 1; /* [5]  */
        unsigned int    uart2_clk_dis         : 1; /* [6]  */
        unsigned int    uart3_clk_dis         : 1; /* [7]  */
        unsigned int    uart4_clk_dis         : 1; /* [8]  */
        unsigned int    uart5_clk_dis         : 1; /* [9]  */
        unsigned int    uart6_clk_dis         : 1; /* [10]  */
        unsigned int    uart7_clk_dis         : 1; /* [11]  */
        unsigned int    peci_clk_dis          : 1; /* [12]  */
        unsigned int    pwm_clk_dis           : 1; /* [13]  */
        unsigned int    canbus0_clk_dis       : 1; /* [14]  */
        unsigned int    canbus1_clk_dis       : 1; /* [15]  */
        unsigned int    dmac_clk_dis          : 1; /* [16]  */
        unsigned int    wdg0_pclk_dis         : 1; /* [17]  */
        unsigned int    wdg1_pclk_dis         : 1; /* [18]  */
        unsigned int    wdg2_pclk_dis         : 1; /* [19]  */
        unsigned int    wdg3_pclk_dis         : 1; /* [20]  */
        unsigned int    wdg4_pclk_dis         : 1; /* [21]  */
        unsigned int    wdg5_pclk_dis         : 1; /* [22]  */
        unsigned int    hpm_clk_dis           : 1; /* [23]  */
        unsigned int    sgpio0_clk_dis        : 1; /* [24]  */
        unsigned int    sgpio1_clk_dis        : 1; /* [25]  */
        unsigned int    sdio_clk_dis          : 1; /* [26]  */
        unsigned int    bmcrtc_clk_dis        : 1; /* [27]  */
        unsigned int    adc0_clk_dis          : 1; /* [28]  */
        unsigned int    adc1_clk_dis          : 1; /* [29]  */
        unsigned int    peci_single_clk_dis   : 1; /* [30]  */
        unsigned int    trng_clk_dis          : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clkdis_ctrl1;

/* Define the union u_lpsc_clksta_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipbmi2c0_clk_status   : 1; /* [0]  */
        unsigned int    ipbmi2c1_clk_status   : 1; /* [1]  */
        unsigned int    ipbmi2c2_clk_status   : 1; /* [2]  */
        unsigned int    ipbmi2c3_clk_status   : 1; /* [3]  */
        unsigned int    superio_clk_status    : 1; /* [4]  */
        unsigned int    lb_clk_status         : 1; /* [5]  */
        unsigned int    uart2_clk_status      : 1; /* [6]  */
        unsigned int    uart3_clk_status      : 1; /* [7]  */
        unsigned int    uart4_clk_status      : 1; /* [8]  */
        unsigned int    uart5_clk_status      : 1; /* [9]  */
        unsigned int    uart6_clk_status      : 1; /* [10]  */
        unsigned int    uart7_clk_status      : 1; /* [11]  */
        unsigned int    peci_clk_status       : 1; /* [12]  */
        unsigned int    pwm_clk_status        : 1; /* [13]  */
        unsigned int    canbus0_clk_status    : 1; /* [14]  */
        unsigned int    canbus1_clk_status    : 1; /* [15]  */
        unsigned int    dmac_clk_status       : 1; /* [16]  */
        unsigned int    wdg0_pclk_status      : 1; /* [17]  */
        unsigned int    wdg1_pclk_status      : 1; /* [18]  */
        unsigned int    wdg2_pclk_status      : 1; /* [19]  */
        unsigned int    wdg3_pclk_status      : 1; /* [20]  */
        unsigned int    wdg4_pclk_status      : 1; /* [21]  */
        unsigned int    wdg5_pclk_status      : 1; /* [22]  */
        unsigned int    hpm_clk_status        : 1; /* [23]  */
        unsigned int    sgpio0_clk_status     : 1; /* [24]  */
        unsigned int    sgpio1_clk_status     : 1; /* [25]  */
        unsigned int    sdio_clk_status       : 1; /* [26]  */
        unsigned int    bmcrtc_clk_status     : 1; /* [27]  */
        unsigned int    adc0_clk_status       : 1; /* [28]  */
        unsigned int    adc1_clk_status       : 1; /* [29]  */
        unsigned int    peci_single_clk_status : 1; /* [30]  */
        unsigned int    trng_clk_status       : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clksta_ctrl1;

/* Define the union u_lpsc_sol_brg_dbg_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sol_dbg_wfq_weight    : 28; /* [27..0]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sol_brg_dbg_ctrl0;

/* Define the union u_lpsc_sol_brg_dbg_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sol_dbg_incr_spen     : 1; /* [0]  */
        unsigned int    sol_dbg_rd_outstd     : 4; /* [4..1]  */
        unsigned int    sol_dbg_wr_outstd     : 4; /* [8..5]  */
        unsigned int    sol_dbg_h2x_sel       : 1; /* [9]  */
        unsigned int    sol_dbg_port_sel      : 4; /* [13..10]  */
        unsigned int    sol_dbg_stat_clr      : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sol_brg_dbg_ctrl1;

/* Define the union u_lpsc_jlc_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    jlc0_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    jlc0_sp_ram_tmod      : 7; /* [12..6]  */
        unsigned int    jlc1_mem_power_mode   : 6; /* [18..13]  */
        unsigned int    jlc1_sp_ram_tmod      : 7; /* [25..19]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_jlc_mem_ctrl;

/* Define the union u_lpsc_sram_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sram_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    sram_sp_ram_tmod      : 7; /* [12..6]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sram_mem_ctrl;

/* Define the union u_lpsc_dmac_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    dmac_sp_ram_tmod      : 7; /* [12..6]  */
        unsigned int    reserved_0            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_dmac_mem_ctrl;

/* Define the union u_lpsc_ipmb0_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipmb0_mem_power_mode  : 6; /* [5..0]  */
        unsigned int    ipmb0_tp_ram_tmod     : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ipmb0_mem_ctrl;

/* Define the union u_lpsc_ipmb1_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipmb1_mem_power_mode  : 6; /* [5..0]  */
        unsigned int    ipmb1_tp_ram_tmod     : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ipmb1_mem_ctrl;

/* Define the union u_lpsc_ipmb2_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipmb2_mem_power_mode  : 6; /* [5..0]  */
        unsigned int    ipmb2_tp_ram_tmod     : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ipmb2_mem_ctrl;

/* Define the union u_lpsc_ipmb3_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    ipmb3_mem_power_mode  : 6; /* [5..0]  */
        unsigned int    ipmb3_tp_ram_tmod     : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ipmb3_mem_ctrl;

/* Define the union u_lpsc_sfc0_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sfc0_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    sfc0_tp_ram_tmod      : 8; /* [13..6]  */
        unsigned int    sfc0_sp_ram_tmod      : 7; /* [20..14]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sfc0_mem_ctrl;

/* Define the union u_lpsc_sfc1_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sfc1_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    sfc1_tp_ram_tmod      : 8; /* [13..6]  */
        unsigned int    sfc1_sp_ram_tmod      : 7; /* [20..14]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sfc1_mem_ctrl;

/* Define the union u_lpsc_emmc_mem_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    emmc_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    emmc_tp_ram_tmod      : 8; /* [13..6]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emmc_mem_ctrl;

/* Define the union u_lpsc_reset_dreq0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    exwdog_srst_dreq      : 1; /* [0]  */
        unsigned int    m3_dap_srst_dreq      : 1; /* [1]  */
        unsigned int    m3_srst_dreq          : 1; /* [2]  */
        unsigned int    sdio_srst_dreq        : 1; /* [3]  */
        unsigned int    lb_srst_dreq          : 1; /* [4]  */
        unsigned int    sfc0_srst_dreq        : 1; /* [5]  */
        unsigned int    sfc1_srst_dreq        : 1; /* [6]  */
        unsigned int    spi0_srst_dreq        : 1; /* [7]  */
        unsigned int    spi1_srst_dreq        : 1; /* [8]  */
        unsigned int    i2c0_srst_dreq        : 1; /* [9]  */
        unsigned int    i2c1_srst_dreq        : 1; /* [10]  */
        unsigned int    i2c2_srst_dreq        : 1; /* [11]  */
        unsigned int    i2c3_srst_dreq        : 1; /* [12]  */
        unsigned int    i2c4_srst_dreq        : 1; /* [13]  */
        unsigned int    i2c5_srst_dreq        : 1; /* [14]  */
        unsigned int    i2c6_srst_dreq        : 1; /* [15]  */
        unsigned int    i2c7_srst_dreq        : 1; /* [16]  */
        unsigned int    i2c8_srst_dreq        : 1; /* [17]  */
        unsigned int    i2c9_srst_dreq        : 1; /* [18]  */
        unsigned int    i2c10_srst_dreq       : 1; /* [19]  */
        unsigned int    i2c11_srst_dreq       : 1; /* [20]  */
        unsigned int    i2c12_srst_dreq       : 1; /* [21]  */
        unsigned int    i2c13_srst_dreq       : 1; /* [22]  */
        unsigned int    i2c14_srst_dreq       : 1; /* [23]  */
        unsigned int    i2c15_srst_dreq       : 1; /* [24]  */
        unsigned int    gpio0_srst_dreq       : 1; /* [25]  */
        unsigned int    gpio1_srst_dreq       : 1; /* [26]  */
        unsigned int    gpio2_srst_dreq       : 1; /* [27]  */
        unsigned int    gpio3_srst_dreq       : 1; /* [28]  */
        unsigned int    gpio4_srst_dreq       : 1; /* [29]  */
        unsigned int    lpc_srst_dreq         : 1; /* [30]  */
        unsigned int    peci_srst_dreq        : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_dreq0;

/* Define the union u_lpsc_reset_dreq1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pwm_srst_dreq         : 1; /* [0]  */
        unsigned int    adc0_srst_dreq        : 1; /* [1]  */
        unsigned int    canbus0_srst_dreq     : 1; /* [2]  */
        unsigned int    canbus1_srst_dreq     : 1; /* [3]  */
        unsigned int    kcs0_srst_dreq        : 1; /* [4]  */
        unsigned int    kcs1_srst_dreq        : 1; /* [5]  */
        unsigned int    kcs2_srst_dreq        : 1; /* [6]  */
        unsigned int    kcs3_srst_dreq        : 1; /* [7]  */
        unsigned int    bt0_srst_dreq         : 1; /* [8]  */
        unsigned int    bt1_srst_dreq         : 1; /* [9]  */
        unsigned int    c80_srst_dreq         : 1; /* [10]  */
        unsigned int    mailbos_srst_dreq     : 1; /* [11]  */
        unsigned int    hostuart1_srst_dreq   : 1; /* [12]  */
        unsigned int    hostuart0_srst_dreq   : 1; /* [13]  */
        unsigned int    sol_srst_dreq         : 1; /* [14]  */
        unsigned int    hostwdi_srst_dreq     : 1; /* [15]  */
        unsigned int    bmcrtc_srst_dreq      : 1; /* [16]  */
        unsigned int    ipmbi2c0_srst_dreq    : 1; /* [17]  */
        unsigned int    ipmbi2c1_srst_dreq    : 1; /* [18]  */
        unsigned int    ipmbi2c2_srst_dreq    : 1; /* [19]  */
        unsigned int    ipmbi2c3_srst_dreq    : 1; /* [20]  */
        unsigned int    mdio_srst_dreq        : 1; /* [21]  */
        unsigned int    uart2_srst_dreq       : 1; /* [22]  */
        unsigned int    uart3_srst_dreq       : 1; /* [23]  */
        unsigned int    uart4_srst_dreq       : 1; /* [24]  */
        unsigned int    uart5_srst_dreq       : 1; /* [25]  */
        unsigned int    uart6_srst_dreq       : 1; /* [26]  */
        unsigned int    uart7_srst_dreq       : 1; /* [27]  */
        unsigned int    sgpio0_srst_dreq      : 1; /* [28]  */
        unsigned int    sgpio1_srst_dreq      : 1; /* [29]  */
        unsigned int    jlc0_srst_dreq        : 1; /* [30]  */
        unsigned int    jlc1_srst_dreq        : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_dreq1;

/* Define the union u_lpsc_reset_dreq2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdg0_srst_dreq        : 1; /* [0]  */
        unsigned int    wdg1_srst_dreq        : 1; /* [1]  */
        unsigned int    wdg2_srst_dreq        : 1; /* [2]  */
        unsigned int    wdg3_srst_dreq        : 1; /* [3]  */
        unsigned int    wdg4_srst_dreq        : 1; /* [4]  */
        unsigned int    wdg5_srst_dreq        : 1; /* [5]  */
        unsigned int    hpm_srst_dreq         : 1; /* [6]  */
        unsigned int    timer01_srst_dreq     : 1; /* [7]  */
        unsigned int    timer23_srst_dreq     : 1; /* [8]  */
        unsigned int    timer45_srst_dreq     : 1; /* [9]  */
        unsigned int    uart2_sol_srst_dreq   : 1; /* [10]  */
        unsigned int    uart3_sol_srst_dreq   : 1; /* [11]  */
        unsigned int    uart4_sol_srst_dreq   : 1; /* [12]  */
        unsigned int    espi_sol_srst_dreq    : 1; /* [13]  */
        unsigned int    adc1_srst_dreq        : 1; /* [14]  */
        unsigned int    peci_single_srst_dreq : 1; /* [15]  */
        unsigned int    trng_srst_dreq        : 1; /* [16]  */
        unsigned int    efuse_srst_dreq       : 1; /* [17]  */
        unsigned int    dmac_srst_dreq        : 1; /* [18]  */
        unsigned int    djtag_mstr_srst_dreq  : 1; /* [19]  */
        unsigned int    ipcm_srst_dreq        : 1; /* [20]  */
        unsigned int    oob_rx_srst_dreq      : 1; /* [21]  */
        unsigned int    oob_tx_srst_dreq      : 1; /* [22]  */
        unsigned int    vw_irq_srst_dreq      : 1; /* [23]  */
        unsigned int    vw_gpio_srst_dreq     : 1; /* [24]  */
        unsigned int    vw_sys_srst_dreq      : 1; /* [25]  */
        unsigned int    dma_tx_srst_dreq      : 1; /* [26]  */
        unsigned int    dma_rx_srst_dreq      : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_dreq2;

/* Define the union u_lpsc_reset_st0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    exwdog_srst_st        : 1; /* [0]  */
        unsigned int    m3_dap_srst_st        : 1; /* [1]  */
        unsigned int    m3_srst_st            : 1; /* [2]  */
        unsigned int    sdio_srst_st          : 1; /* [3]  */
        unsigned int    lb_srst_st            : 1; /* [4]  */
        unsigned int    sfc0_srst_st          : 1; /* [5]  */
        unsigned int    sfc1_srst_st          : 1; /* [6]  */
        unsigned int    spi0_srst_st          : 1; /* [7]  */
        unsigned int    spi1_srst_st          : 1; /* [8]  */
        unsigned int    i2c0_srst_st          : 1; /* [9]  */
        unsigned int    i2c1_srst_st          : 1; /* [10]  */
        unsigned int    i2c2_srst_st          : 1; /* [11]  */
        unsigned int    i2c3_srst_st          : 1; /* [12]  */
        unsigned int    i2c4_srst_st          : 1; /* [13]  */
        unsigned int    i2c5_srst_st          : 1; /* [14]  */
        unsigned int    i2c6_srst_st          : 1; /* [15]  */
        unsigned int    i2c7_srst_st          : 1; /* [16]  */
        unsigned int    i2c8_srst_st          : 1; /* [17]  */
        unsigned int    i2c9_srst_st          : 1; /* [18]  */
        unsigned int    i2c10_srst_st         : 1; /* [19]  */
        unsigned int    i2c11_srst_st         : 1; /* [20]  */
        unsigned int    i2c12_srst_st         : 1; /* [21]  */
        unsigned int    i2c13_srst_st         : 1; /* [22]  */
        unsigned int    i2c14_srst_st         : 1; /* [23]  */
        unsigned int    i2c15_srst_st         : 1; /* [24]  */
        unsigned int    gpio0_srst_st         : 1; /* [25]  */
        unsigned int    gpio1_srst_st         : 1; /* [26]  */
        unsigned int    gpio2_srst_st         : 1; /* [27]  */
        unsigned int    gpio3_srst_st         : 1; /* [28]  */
        unsigned int    gpio4_srst_st         : 1; /* [29]  */
        unsigned int    lpc_srst_st           : 1; /* [30]  */
        unsigned int    peci_srst_st          : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_st0;

/* Define the union u_lpsc_reset_st1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    pwm_srst_st           : 1; /* [0]  */
        unsigned int    adc0_srst_st          : 1; /* [1]  */
        unsigned int    canbus0_srst_st       : 1; /* [2]  */
        unsigned int    canbus1_srst_st       : 1; /* [3]  */
        unsigned int    kcs0_srst_st          : 1; /* [4]  */
        unsigned int    kcs1_srst_st          : 1; /* [5]  */
        unsigned int    kcs2_srst_st          : 1; /* [6]  */
        unsigned int    kcs3_srst_st          : 1; /* [7]  */
        unsigned int    bt0_srst_st           : 1; /* [8]  */
        unsigned int    bt1_srst_st           : 1; /* [9]  */
        unsigned int    c80_srst_st           : 1; /* [10]  */
        unsigned int    mailbos_srst_st       : 1; /* [11]  */
        unsigned int    hostuart1_srst_st     : 1; /* [12]  */
        unsigned int    hostuart0_srst_st     : 1; /* [13]  */
        unsigned int    sol_srst_st           : 1; /* [14]  */
        unsigned int    hostwdi_srst_st       : 1; /* [15]  */
        unsigned int    bmcrtc_srst_st        : 1; /* [16]  */
        unsigned int    ipmbi2c0_srst_st      : 1; /* [17]  */
        unsigned int    ipmbi2c1_srst_st      : 1; /* [18]  */
        unsigned int    ipmbi2c2_srst_st      : 1; /* [19]  */
        unsigned int    ipmbi2c3_srst_st      : 1; /* [20]  */
        unsigned int    mdio_srst_st          : 1; /* [21]  */
        unsigned int    uart2_srst_st         : 1; /* [22]  */
        unsigned int    uart3_srst_st         : 1; /* [23]  */
        unsigned int    uart4_srst_st         : 1; /* [24]  */
        unsigned int    uart5_srst_st         : 1; /* [25]  */
        unsigned int    uart6_srst_st         : 1; /* [26]  */
        unsigned int    uart7_srst_st         : 1; /* [27]  */
        unsigned int    sgpio0_srst_st        : 1; /* [28]  */
        unsigned int    sgpio1_srst_st        : 1; /* [29]  */
        unsigned int    jlc0_srst_st          : 1; /* [30]  */
        unsigned int    jlc1_srst_st          : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_st1;

/* Define the union u_lpsc_reset_st2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdg0_srst_st          : 1; /* [0]  */
        unsigned int    wdg1_srst_st          : 1; /* [1]  */
        unsigned int    wdg2_srst_st          : 1; /* [2]  */
        unsigned int    wdg3_srst_st          : 1; /* [3]  */
        unsigned int    wdg4_srst_st          : 1; /* [4]  */
        unsigned int    wdg5_srst_st          : 1; /* [5]  */
        unsigned int    hpm_srst_st           : 1; /* [6]  */
        unsigned int    timer01_srst_st       : 1; /* [7]  */
        unsigned int    timer23_srst_st       : 1; /* [8]  */
        unsigned int    timer45_srst_st       : 1; /* [9]  */
        unsigned int    uart2_sol_srst_st     : 1; /* [10]  */
        unsigned int    uart3_sol_srst_st     : 1; /* [11]  */
        unsigned int    uart4_sol_srst_st     : 1; /* [12]  */
        unsigned int    espi_sol_srst_st      : 1; /* [13]  */
        unsigned int    adc1_srst_st          : 1; /* [14]  */
        unsigned int    peci_single_srst_st   : 1; /* [15]  */
        unsigned int    trng_srst_st          : 1; /* [16]  */
        unsigned int    efuse_srst_st         : 1; /* [17]  */
        unsigned int    dmac_srst_st          : 1; /* [18]  */
        unsigned int    djtag_mstr_srst_st    : 1; /* [19]  */
        unsigned int    ipcm_srst_st          : 1; /* [20]  */
        unsigned int    oob_rx_srst_st        : 1; /* [21]  */
        unsigned int    oob_tx_srst_st        : 1; /* [22]  */
        unsigned int    vw_irq_srst_st        : 1; /* [23]  */
        unsigned int    vw_gpio_srst_st       : 1; /* [24]  */
        unsigned int    vw_sys_srst_st        : 1; /* [25]  */
        unsigned int    dma_tx_srst_st        : 1; /* [26]  */
        unsigned int    dma_rx_srst_st        : 1; /* [27]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_reset_st2;

/* Define the union u_lpsc_emp_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    xsemp_slv_en_10       : 1; /* [0]  */
        unsigned int    xsemp_err_mode        : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emp_ctrl;

/* Define the union u_lpsc_emp_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    xsemp_slv_st_10       : 1; /* [0]  */
        unsigned int    xsemp_slv_ok_10       : 1; /* [1]  */
        unsigned int    xsemp_linkdown_10     : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emp_st;

/* Define the union u_lpsc_sol_brg_dbg_ctrl3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sol_1_dbg_wfq_weight  : 28; /* [27..0]  */
        unsigned int    reserved_0            : 4; /* [31..28]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sol_brg_dbg_ctrl3;

/* Define the union u_lpsc_sol_brg_dbg_ctrl4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sol_1_dbg_incr_spen   : 1; /* [0]  */
        unsigned int    sol_1_dbg_rd_outstd   : 4; /* [4..1]  */
        unsigned int    sol_1_dbg_wr_outstd   : 4; /* [8..5]  */
        unsigned int    sol_1_dbg_h2x_sel     : 1; /* [9]  */
        unsigned int    sol_1_dbg_port_sel    : 4; /* [13..10]  */
        unsigned int    sol_1_dbg_stat_clr    : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sol_brg_dbg_ctrl4;

/* Define the union u_lpsc_se_m3_3x1_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_m3_3x1_dlock_irq   : 1; /* [0]  */
        unsigned int    se_m3_3x1_dlock_wr    : 1; /* [1]  */
        unsigned int    se_m3_3x1_dlock_id    : 3; /* [4..2]  */
        unsigned int    se_m3_3x1_dlock_slv   : 1; /* [5]  */
        unsigned int    se_m3_3x1_dlock_mst   : 2; /* [7..6]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_m3_3x1_dlock;

/* Define the union u_lpsc_se_m3_3x1_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_m3_3x1_priority_s1 : 1; /* [0]  */
        unsigned int    se_m3_3x1_priority_m3 : 2; /* [2..1]  */
        unsigned int    se_m3_3x1_priority_m2 : 2; /* [4..3]  */
        unsigned int    se_m3_3x1_priority_m1 : 2; /* [6..5]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_m3_3x1_priority;

/* Define the union u_lpsc_se_dma_3x1_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_dma_3x1_dlock_irq  : 1; /* [0]  */
        unsigned int    se_dma_3x1_dlock_wr   : 1; /* [1]  */
        unsigned int    se_dma_3x1_dlock_id   : 3; /* [4..2]  */
        unsigned int    se_dma_3x1_dlock_slv  : 1; /* [5]  */
        unsigned int    se_dma_3x1_dlock_mst  : 2; /* [7..6]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_dma_3x1_dlock;

/* Define the union u_lpsc_se_dma_3x1_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_dma_3x1_priority_s1 : 1; /* [0]  */
        unsigned int    se_dma_3x1_priority_m3 : 2; /* [2..1]  */
        unsigned int    se_dma_3x1_priority_m2 : 2; /* [4..3]  */
        unsigned int    se_dma_3x1_priority_m1 : 2; /* [6..5]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_dma_3x1_priority;

/* Define the union u_lpsc_se_sec_4x6_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_4x6_dlock_irq  : 1; /* [0]  */
        unsigned int    se_sec_4x6_dlock_wr   : 1; /* [1]  */
        unsigned int    se_sec_4x6_dlock_id   : 10; /* [11..2]  */
        unsigned int    se_sec_4x6_dlock_slv  : 3; /* [14..12]  */
        unsigned int    se_sec_4x6_dlock_mst  : 2; /* [16..15]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_sec_4x6_dlock;

/* Define the union u_lpsc_se_sec_4x6_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_4x6_priorit_s6 : 3; /* [2..0]  */
        unsigned int    se_sec_4x6_priorit_s5 : 3; /* [5..3]  */
        unsigned int    se_sec_4x6_priorit_s4 : 3; /* [8..6]  */
        unsigned int    se_sec_4x6_priorit_s3 : 3; /* [11..9]  */
        unsigned int    se_sec_4x6_priorit_s2 : 3; /* [14..12]  */
        unsigned int    se_sec_4x6_priorit_s1 : 3; /* [17..15]  */
        unsigned int    se_sec_4x6_priorit_m4 : 2; /* [19..18]  */
        unsigned int    se_sec_4x6_priorit_m3 : 2; /* [21..20]  */
        unsigned int    se_sec_4x6_priorit_m2 : 2; /* [23..22]  */
        unsigned int    se_sec_4x6_priorit_m1 : 2; /* [25..24]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_sec_4x6_priority;

/* Define the union u_lpsc_se_sec_1x3_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_1x3_dlock_irq  : 1; /* [0]  */
        unsigned int    se_sec_1x3_dlock_wr   : 1; /* [1]  */
        unsigned int    se_sec_1x3_dlock_id   : 12; /* [13..2]  */
        unsigned int    se_sec_1x3_dlock_slv  : 2; /* [15..14]  */
        unsigned int    se_sec_1x3_dlock_mst  : 1; /* [16]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_sec_1x3_dlock;

/* Define the union u_lpsc_se_sec_1x3_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_1x3_priorit_s3 : 2; /* [1..0]  */
        unsigned int    se_sec_1x3_priorit_s2 : 2; /* [3..2]  */
        unsigned int    se_sec_1x3_priorit_s1 : 2; /* [5..4]  */
        unsigned int    se_sec_1x3_priorit_m1 : 1; /* [6]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_sec_1x3_priority;

/* Define the union u_lpsc_se_sec_1x6_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_1x6_dlock_irq  : 1; /* [0]  */
        unsigned int    se_sec_1x6_dlock_wr   : 1; /* [1]  */
        unsigned int    se_sec_1x6_dlock_id   : 12; /* [13..2]  */
        unsigned int    se_sec_1x6_dlock_slv  : 3; /* [16..14]  */
        unsigned int    se_sec_1x6_dlock_mst  : 1; /* [17]  */
        unsigned int    reserved_0            : 14; /* [31..18]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_sec_1x6_dlock;

/* Define the union u_lpsc_se_sec_1x6_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_1x6_priorit_s7 : 3; /* [2..0]  */
        unsigned int    se_sec_1x6_priorit_s6 : 3; /* [5..3]  */
        unsigned int    se_sec_1x6_priorit_s5 : 3; /* [8..6]  */
        unsigned int    se_sec_1x6_priorit_s4 : 3; /* [11..9]  */
        unsigned int    se_sec_1x6_priorit_s3 : 3; /* [14..12]  */
        unsigned int    se_sec_1x6_priorit_s2 : 3; /* [17..15]  */
        unsigned int    se_sec_1x6_priorit_s1 : 3; /* [20..18]  */
        unsigned int    se_sec_1x6_priorit_m1 : 1; /* [21]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_se_sec_1x6_priority;

/* Define the union u_lpsc_lp_sol_2x1_dlock */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    lp_sol_2x1_dlock_irq  : 1; /* [0]  */
        unsigned int    lp_sol_2x1_dlock_wr   : 1; /* [1]  */
        unsigned int    lp_sol_2x1_dlock_id   : 12; /* [13..2]  */
        unsigned int    lp_sol_2x1_dlock_slv  : 1; /* [14]  */
        unsigned int    lp_sol_2x1_dlock_mst  : 1; /* [15]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_lp_sol_2x1_dlock;

/* Define the union u_lpsc_lp_sol_2x1_priority */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    lp_sol_2x1_priorit_s1 : 1; /* [0]  */
        unsigned int    lp_sol_2x1_priorit_m2 : 1; /* [1]  */
        unsigned int    lp_sol_2x1_priorit_m1 : 1; /* [2]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_lp_sol_2x1_priority;

/* Define the union u_lpsc_addr_extend0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sfc0_haddr_offset     : 4; /* [3..0]  */
        unsigned int    sfc0_addr_remap_mode  : 2; /* [5..4]  */
        unsigned int    sfc1_haddr_offset     : 4; /* [9..6]  */
        unsigned int    sfc1_addr_remap_mode  : 2; /* [11..10]  */
        unsigned int    emmc_haddr_offset     : 4; /* [15..12]  */
        unsigned int    emmc_addr_remap_mode  : 2; /* [17..16]  */
        unsigned int    sdio_haddr_offset     : 4; /* [21..18]  */
        unsigned int    sdio_addr_remap_mode  : 2; /* [23..22]  */
        unsigned int    reserved_0            : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_addr_extend0;

/* Define the union u_lpsc_addr_extend1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    dmac_awaddr_offset    : 4; /* [3..0]  */
        unsigned int    dmac_araddr_offset    : 4; /* [7..4]  */
        unsigned int    dmac_addr_remap_mode  : 2; /* [9..8]  */
        unsigned int    reserved_0            : 22; /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_addr_extend1;

/* Define the union u_lpsc_adc_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    adc0_selbg            : 1; /* [0]  */
        unsigned int    adc0_selref           : 1; /* [1]  */
        unsigned int    adc0_selres           : 2; /* [3..2]  */
        unsigned int    adc0_enadc            : 1; /* [4]  */
        unsigned int    adc0_enctr            : 3; /* [7..5]  */
        unsigned int    adc1_selbg            : 1; /* [8]  */
        unsigned int    adc1_selref           : 1; /* [9]  */
        unsigned int    adc1_selres           : 2; /* [11..10]  */
        unsigned int    adc1_enadc            : 1; /* [12]  */
        unsigned int    adc1_enctr            : 3; /* [15..13]  */
        unsigned int    reserved_0            : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_adc_ctrl;

/* Define the union u_lpsc_sram_ini */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sram_ini_start        : 1; /* [0]  */
        unsigned int    sram_ini_done         : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sram_ini;

/* Define the union u_lpsc_emp10_err */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bresp_err_int         : 1; /* [0]  */
        unsigned int    rresp_err_int         : 1; /* [1]  */
        unsigned int    reserved_0            : 30; /* [31..2]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emp10_err;

/* Define the union u_lpsc_timer_div */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    timer01_clk_div_cnt   : 5; /* [4..0]  */
        unsigned int    timer23_clk_div_cnt   : 5; /* [9..5]  */
        unsigned int    timer45_clk_div_cnt   : 5; /* [14..10]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_timer_div;

/* Define the union u_lpsc_wdg_div */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    wdg0_clk_div_cnt      : 5; /* [4..0]  */
        unsigned int    wdg1_clk_div_cnt      : 5; /* [9..5]  */
        unsigned int    wdg2_clk_div_cnt      : 5; /* [14..10]  */
        unsigned int    wdg3_clk_div_cnt      : 5; /* [19..15]  */
        unsigned int    wdg4_clk_div_cnt      : 5; /* [24..20]  */
        unsigned int    wdg5_clk_div_cnt      : 5; /* [29..25]  */
        unsigned int    reserved_0            : 2; /* [31..30]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_wdg_div;

/* Define the union u_lpsc_trng_tst_fro_iddq */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_tst_fro_iddq     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_tst_fro_iddq;

/* Define the union u_lpsc_trng_tst_fro_ctrl_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_tst_fro_ctrl_en  : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_tst_fro_ctrl_en;

/* Define the union u_lpsc_trng_tst_fro_select */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_tst_fro_select   : 5; /* [4..0]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_tst_fro_select;

/* Define the union u_lpsc_trng_tst_fro_enable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_tst_fro_enable   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_tst_fro_enable;

/* Define the union u_lpsc_trng_tst_delay */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_tst_fro_delay    : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_tst_delay;

/* Define the union u_lpsc_trng_sp_ram_tmod */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_sp_ram_tmod      : 7; /* [6..0]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_sp_ram_tmod;

/* Define the union u_lpsc_trng_mem_power_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_mem_power_mode   : 6; /* [5..0]  */
        unsigned int    reserved_0            : 26; /* [31..6]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_mem_power_mode;

/* Define the union u_lpsc_trng_fsm_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_fsm_state        : 3; /* [2..0]  */
        unsigned int    reserved_0            : 29; /* [31..3]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_fsm_state;

/* Define the union u_lpsc_trng_pproc_state */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_pproc_state      : 7; /* [6..0]  */
        unsigned int    reserved_0            : 25; /* [31..7]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_trng_pproc_state;

/* Define the union u_lpsc_m3_brg_dbg_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m3_m_dbg_wfq_weight   : 12; /* [11..0]  */
        unsigned int    reserved_0            : 20; /* [31..12]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_m3_brg_dbg_ctrl0;

/* Define the union u_lpsc_m3_brg_dbg_ctrl1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m3_m_dbg_incr_spen    : 1; /* [0]  */
        unsigned int    m3_m_dbg_rd_outstd    : 4; /* [4..1]  */
        unsigned int    m3_m_dbg_wr_outstd    : 4; /* [8..5]  */
        unsigned int    m3_m_dbg_h2x_sel      : 1; /* [9]  */
        unsigned int    m3_m_dbg_port_sel     : 4; /* [13..10]  */
        unsigned int    m3_m_dbg_stat_clr     : 1; /* [14]  */
        unsigned int    reserved_0            : 17; /* [31..15]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_m3_brg_dbg_ctrl1;

/* Define the union u_lpsc_speed_cnt_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m3_spdcnt_en          : 1; /* [0]  */
        unsigned int    m3_spdcnt_rbyte_clr   : 1; /* [1]  */
        unsigned int    m3_spdcnt_wbyte_clr   : 1; /* [2]  */
        unsigned int    reserved_0            : 1; /* [3]  */
        unsigned int    storage_dma_spdcnt_en : 1; /* [4]  */
        unsigned int    storage_dma_spdcnt_rbyte_clr : 1; /* [5]  */
        unsigned int    storage_dma_spdcnt_wbyte_clr : 1; /* [6]  */
        unsigned int    reserved_1            : 1; /* [7]  */
        unsigned int    dmac_spdcnt_en        : 1; /* [8]  */
        unsigned int    dmac_spdcnt_rbyte_clr : 1; /* [9]  */
        unsigned int    dmac_spdcnt_wbyte_clr : 1; /* [10]  */
        unsigned int    reserved_2            : 21; /* [31..11]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_speed_cnt_cfg;

/* Define the union u_lpsc_hpm_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hpm_en                : 1; /* [0]  */
        unsigned int    hpm_div               : 6; /* [6..1]  */
        unsigned int    hpm_clk_sel           : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_hpm_ctrl;

/* Define the union u_lpsc_hpm_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    hpm_pc_valid          : 1; /* [0]  */
        unsigned int    hpm_pc_0_org          : 10; /* [10..1]  */
        unsigned int    hpm_pc_1_org          : 10; /* [20..11]  */
        unsigned int    reserved_0            : 11; /* [31..21]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_hpm_status;

/* Define the union u_lpsc_djtag_mstr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    a55_djtag_mstr_busy   : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_djtag_mstr;

/* Define the union u_lpsc_ecc_int_raw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_ecc_2bit_err_int : 1; /* [0]  */
        unsigned int    trng_ecc_1bit_err_int : 1; /* [1]  */
        unsigned int    dmac_ecc_2bit_err_int : 1; /* [2]  */
        unsigned int    dmac_ecc_1bit_err_int : 1; /* [3]  */
        unsigned int    jlc1_rx_ecc_2bit_err_int : 1; /* [4]  */
        unsigned int    jlc1_rx_ecc_1bit_err_int : 1; /* [5]  */
        unsigned int    jlc1_tx_ecc_2bit_err_int : 1; /* [6]  */
        unsigned int    jlc1_tx_ecc_1bit_err_int : 1; /* [7]  */
        unsigned int    jlc0_rx_ecc_2bit_err_int : 1; /* [8]  */
        unsigned int    jlc0_rx_ecc_1bit_err_int : 1; /* [9]  */
        unsigned int    jlc0_tx_ecc_2bit_err_int : 1; /* [10]  */
        unsigned int    jlc0_tx_ecc_1bit_err_int : 1; /* [11]  */
        unsigned int    emmc_rx_ecc_2bit_err_int : 1; /* [12]  */
        unsigned int    emmc_rx_ecc_1bit_err_int : 1; /* [13]  */
        unsigned int    emmc_tx_ecc_2bit_err_int : 1; /* [14]  */
        unsigned int    emmc_tx_ecc_1bit_err_int : 1; /* [15]  */
        unsigned int    sfc1_ecc_2bit_err_int : 1; /* [16]  */
        unsigned int    sfc1_ecc_1bit_err_int : 1; /* [17]  */
        unsigned int    sfc0_ecc_2bit_err_int : 1; /* [18]  */
        unsigned int    sfc0_ecc_1bit_err_int : 1; /* [19]  */
        unsigned int    sram_ecc_2bit_err_int : 1; /* [20]  */
        unsigned int    sram_ecc_1bit_err_int : 1; /* [21]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ecc_int_raw;

/* Define the union u_lpsc_ecc_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_ecc_2bit_err_int_mask : 1; /* [0]  */
        unsigned int    trng_ecc_1bit_err_int_mask : 1; /* [1]  */
        unsigned int    dmac_ecc_2bit_err_int_mask : 1; /* [2]  */
        unsigned int    dmac_ecc_1bit_err_int_mask : 1; /* [3]  */
        unsigned int    jlc1_rx_ecc_2bit_err_int_mask : 1; /* [4]  */
        unsigned int    jlc1_rx_ecc_1bit_err_int_mask : 1; /* [5]  */
        unsigned int    jlc1_tx_ecc_2bit_err_int_mask : 1; /* [6]  */
        unsigned int    jlc1_tx_ecc_1bit_err_int_mask : 1; /* [7]  */
        unsigned int    jlc0_rx_ecc_2bit_err_int_mask : 1; /* [8]  */
        unsigned int    jlc0_rx_ecc_1bit_err_int_mask : 1; /* [9]  */
        unsigned int    jlc0_tx_ecc_2bit_err_int_mask : 1; /* [10]  */
        unsigned int    jlc0_tx_ecc_1bit_err_int_mask : 1; /* [11]  */
        unsigned int    emmc_rx_ecc_2bit_err_int_mask : 1; /* [12]  */
        unsigned int    emmc_rx_ecc_1bit_err_int_mask : 1; /* [13]  */
        unsigned int    emmc_tx_ecc_2bit_err_int_mask : 1; /* [14]  */
        unsigned int    emmc_tx_ecc_1bit_err_int_mask : 1; /* [15]  */
        unsigned int    sfc1_ecc_2bit_err_int_mask : 1; /* [16]  */
        unsigned int    sfc1_ecc_1bit_err_int_mask : 1; /* [17]  */
        unsigned int    sfc0_ecc_2bit_err_int_mask : 1; /* [18]  */
        unsigned int    sfc0_ecc_1bit_err_int_mask : 1; /* [19]  */
        unsigned int    sram_ecc_2bit_err_int_mask : 1; /* [20]  */
        unsigned int    sram_ecc_1bit_err_int_mask : 1; /* [21]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ecc_int_mask;

/* Define the union u_lpsc_ecc_inst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    trng_ecc_inst_req     : 2; /* [1..0]  */
        unsigned int    dmac_ecc_inst_req     : 2; /* [3..2]  */
        unsigned int    jlc1_rx_ecc_inst_req  : 2; /* [5..4]  */
        unsigned int    jlc1_tx_ecc_inst_req  : 2; /* [7..6]  */
        unsigned int    jlc0_rx_ecc_inst_req  : 2; /* [9..8]  */
        unsigned int    jlc0_tx_ecc_inst_req  : 2; /* [11..10]  */
        unsigned int    emmc_rx_ecc_inst_req  : 2; /* [13..12]  */
        unsigned int    emmc_tx_ecc_inst_req  : 2; /* [15..14]  */
        unsigned int    sfc1_ecc_inst_req     : 2; /* [17..16]  */
        unsigned int    sfc0_ecc_inst_req     : 2; /* [19..18]  */
        unsigned int    sram_ecc_inst_req     : 2; /* [21..20]  */
        unsigned int    reserved_0            : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_ecc_inst;

/* Define the union u_lpsc_jlc_ram_ini */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    jlc0_tx_ram_ini_start : 1; /* [0]  */
        unsigned int    jlc0_rx_ram_ini_start : 1; /* [1]  */
        unsigned int    jlc1_tx_ram_ini_start : 1; /* [2]  */
        unsigned int    jlc1_rx_ram_ini_start : 1; /* [3]  */
        unsigned int    jlc0_tx_ram_ini_done  : 1; /* [4]  */
        unsigned int    jlc0_rx_ram_ini_done  : 1; /* [5]  */
        unsigned int    jlc1_tx_ram_ini_done  : 1; /* [6]  */
        unsigned int    jlc1_rx_ram_ini_done  : 1; /* [7]  */
        unsigned int    reserved_0            : 24; /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_jlc_ram_ini;

/* Define the union u_lpsc_m3_boot_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    m3_boot_addr          : 17; /* [16..0]  */
        unsigned int    reserved_0            : 15; /* [31..17]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_m3_boot_addr;

/* Define the union u_lpsc_iomux_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    spi1_mux_t_r          : 1; /* [0]  */
        unsigned int    ipmb3_mux_t_r         : 1; /* [1]  */
        unsigned int    ipmb2_mux_t_r         : 1; /* [2]  */
        unsigned int    ipmb1_mux_t_r         : 1; /* [3]  */
        unsigned int    ipmb0_mux_t_r         : 1; /* [4]  */
        unsigned int    uart_mux_ctrl         : 6; /* [10..5]  */
        unsigned int    i2c_mux_ctrl          : 3; /* [13..11]  */
        unsigned int    reserved_0            : 18; /* [31..14]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_iomux_ctrl;

/* Define the union u_lpsc_clken_ctrl2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bmcuart2_sol_clk_enb  : 1; /* [0]  */
        unsigned int    bmcuart3_sol_clk_enb  : 1; /* [1]  */
        unsigned int    bmcuart4_sol_clk_enb  : 1; /* [2]  */
        unsigned int    espi_sample_clk_enb   : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clken_ctrl2;

/* Define the union u_lpsc_clkdis_ctrl2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bmcuart2_sol_clk_dis  : 1; /* [0]  */
        unsigned int    bmcuart3_sol_clk_dis  : 1; /* [1]  */
        unsigned int    bmcuart4_sol_clk_dis  : 1; /* [2]  */
        unsigned int    espi_sample_clk_dis   : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clkdis_ctrl2;

/* Define the union u_lpsc_clksta_ctrl2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    bmcuart2_sol_clk_status : 1; /* [0]  */
        unsigned int    bmcuart3_sol_clk_status : 1; /* [1]  */
        unsigned int    bmcuart4_sol_clk_status : 1; /* [2]  */
        unsigned int    espi_sample_clk_status : 1; /* [3]  */
        unsigned int    reserved_0            : 28; /* [31..4]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_clksta_ctrl2;

/* Define the union u_lpsc_i2c0_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c0_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c0_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c0_debug_master_act : 1; /* [2]  */
        unsigned int    i2c0_debug_hs         : 1; /* [3]  */
        unsigned int    i2c0_debug_wr         : 1; /* [4]  */
        unsigned int    i2c0_debug_rd         : 1; /* [5]  */
        unsigned int    i2c0_debug_addr       : 1; /* [6]  */
        unsigned int    i2c0_debug_data       : 1; /* [7]  */
        unsigned int    i2c0_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c0_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c0_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c0_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c0_debug;

/* Define the union u_lpsc_i2c1_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c1_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c1_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c1_debug_master_act : 1; /* [2]  */
        unsigned int    i2c1_debug_hs         : 1; /* [3]  */
        unsigned int    i2c1_debug_wr         : 1; /* [4]  */
        unsigned int    i2c1_debug_rd         : 1; /* [5]  */
        unsigned int    i2c1_debug_addr       : 1; /* [6]  */
        unsigned int    i2c1_debug_data       : 1; /* [7]  */
        unsigned int    i2c1_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c1_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c1_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c1_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c1_debug;

/* Define the union u_lpsc_i2c2_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c2_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c2_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c2_debug_master_act : 1; /* [2]  */
        unsigned int    i2c2_debug_hs         : 1; /* [3]  */
        unsigned int    i2c2_debug_wr         : 1; /* [4]  */
        unsigned int    i2c2_debug_rd         : 1; /* [5]  */
        unsigned int    i2c2_debug_addr       : 1; /* [6]  */
        unsigned int    i2c2_debug_data       : 1; /* [7]  */
        unsigned int    i2c2_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c2_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c2_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c2_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c2_debug;

/* Define the union u_lpsc_i2c3_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c3_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c3_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c3_debug_master_act : 1; /* [2]  */
        unsigned int    i2c3_debug_hs         : 1; /* [3]  */
        unsigned int    i2c3_debug_wr         : 1; /* [4]  */
        unsigned int    i2c3_debug_rd         : 1; /* [5]  */
        unsigned int    i2c3_debug_addr       : 1; /* [6]  */
        unsigned int    i2c3_debug_data       : 1; /* [7]  */
        unsigned int    i2c3_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c3_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c3_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c3_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c3_debug;

/* Define the union u_lpsc_i2c4_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c4_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c4_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c4_debug_master_act : 1; /* [2]  */
        unsigned int    i2c4_debug_hs         : 1; /* [3]  */
        unsigned int    i2c4_debug_wr         : 1; /* [4]  */
        unsigned int    i2c4_debug_rd         : 1; /* [5]  */
        unsigned int    i2c4_debug_addr       : 1; /* [6]  */
        unsigned int    i2c4_debug_data       : 1; /* [7]  */
        unsigned int    i2c4_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c4_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c4_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c4_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c4_debug;

/* Define the union u_lpsc_i2c5_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c5_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c5_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c5_debug_master_act : 1; /* [2]  */
        unsigned int    i2c5_debug_hs         : 1; /* [3]  */
        unsigned int    i2c5_debug_wr         : 1; /* [4]  */
        unsigned int    i2c5_debug_rd         : 1; /* [5]  */
        unsigned int    i2c5_debug_addr       : 1; /* [6]  */
        unsigned int    i2c5_debug_data       : 1; /* [7]  */
        unsigned int    i2c5_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c5_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c5_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c5_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c5_debug;

/* Define the union u_lpsc_i2c6_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c6_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c6_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c6_debug_master_act : 1; /* [2]  */
        unsigned int    i2c6_debug_hs         : 1; /* [3]  */
        unsigned int    i2c6_debug_wr         : 1; /* [4]  */
        unsigned int    i2c6_debug_rd         : 1; /* [5]  */
        unsigned int    i2c6_debug_addr       : 1; /* [6]  */
        unsigned int    i2c6_debug_data       : 1; /* [7]  */
        unsigned int    i2c6_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c6_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c6_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c6_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c6_debug;

/* Define the union u_lpsc_i2c7_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c7_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c7_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c7_debug_master_act : 1; /* [2]  */
        unsigned int    i2c7_debug_hs         : 1; /* [3]  */
        unsigned int    i2c7_debug_wr         : 1; /* [4]  */
        unsigned int    i2c7_debug_rd         : 1; /* [5]  */
        unsigned int    i2c7_debug_addr       : 1; /* [6]  */
        unsigned int    i2c7_debug_data       : 1; /* [7]  */
        unsigned int    i2c7_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c7_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c7_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c7_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c7_debug;

/* Define the union u_lpsc_i2c8_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c8_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c8_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c8_debug_master_act : 1; /* [2]  */
        unsigned int    i2c8_debug_hs         : 1; /* [3]  */
        unsigned int    i2c8_debug_wr         : 1; /* [4]  */
        unsigned int    i2c8_debug_rd         : 1; /* [5]  */
        unsigned int    i2c8_debug_addr       : 1; /* [6]  */
        unsigned int    i2c8_debug_data       : 1; /* [7]  */
        unsigned int    i2c8_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c8_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c8_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c8_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c8_debug;

/* Define the union u_lpsc_i2c9_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c9_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c9_debug_slave_act  : 1; /* [1]  */
        unsigned int    i2c9_debug_master_act : 1; /* [2]  */
        unsigned int    i2c9_debug_hs         : 1; /* [3]  */
        unsigned int    i2c9_debug_wr         : 1; /* [4]  */
        unsigned int    i2c9_debug_rd         : 1; /* [5]  */
        unsigned int    i2c9_debug_addr       : 1; /* [6]  */
        unsigned int    i2c9_debug_data       : 1; /* [7]  */
        unsigned int    i2c9_debug_p_gen      : 1; /* [8]  */
        unsigned int    i2c9_debug_s_gen      : 1; /* [9]  */
        unsigned int    i2c9_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c9_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c9_debug;

/* Define the union u_lpsc_i2c10_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c10_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c10_debug_slave_act : 1; /* [1]  */
        unsigned int    i2c10_debug_master_act : 1; /* [2]  */
        unsigned int    i2c10_debug_hs        : 1; /* [3]  */
        unsigned int    i2c10_debug_wr        : 1; /* [4]  */
        unsigned int    i2c10_debug_rd        : 1; /* [5]  */
        unsigned int    i2c10_debug_addr      : 1; /* [6]  */
        unsigned int    i2c10_debug_data      : 1; /* [7]  */
        unsigned int    i2c10_debug_p_gen     : 1; /* [8]  */
        unsigned int    i2c10_debug_s_gen     : 1; /* [9]  */
        unsigned int    i2c10_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c10_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c10_debug;

/* Define the union u_lpsc_i2c11_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c11_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c11_debug_slave_act : 1; /* [1]  */
        unsigned int    i2c11_debug_master_act : 1; /* [2]  */
        unsigned int    i2c11_debug_hs        : 1; /* [3]  */
        unsigned int    i2c11_debug_wr        : 1; /* [4]  */
        unsigned int    i2c11_debug_rd        : 1; /* [5]  */
        unsigned int    i2c11_debug_addr      : 1; /* [6]  */
        unsigned int    i2c11_debug_data      : 1; /* [7]  */
        unsigned int    i2c11_debug_p_gen     : 1; /* [8]  */
        unsigned int    i2c11_debug_s_gen     : 1; /* [9]  */
        unsigned int    i2c11_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c11_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c11_debug;

/* Define the union u_lpsc_i2c12_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c12_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c12_debug_slave_act : 1; /* [1]  */
        unsigned int    i2c12_debug_master_act : 1; /* [2]  */
        unsigned int    i2c12_debug_hs        : 1; /* [3]  */
        unsigned int    i2c12_debug_wr        : 1; /* [4]  */
        unsigned int    i2c12_debug_rd        : 1; /* [5]  */
        unsigned int    i2c12_debug_addr      : 1; /* [6]  */
        unsigned int    i2c12_debug_data      : 1; /* [7]  */
        unsigned int    i2c12_debug_p_gen     : 1; /* [8]  */
        unsigned int    i2c12_debug_s_gen     : 1; /* [9]  */
        unsigned int    i2c12_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c12_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c12_debug;

/* Define the union u_lpsc_i2c13_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c13_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c13_debug_slave_act : 1; /* [1]  */
        unsigned int    i2c13_debug_master_act : 1; /* [2]  */
        unsigned int    i2c13_debug_hs        : 1; /* [3]  */
        unsigned int    i2c13_debug_wr        : 1; /* [4]  */
        unsigned int    i2c13_debug_rd        : 1; /* [5]  */
        unsigned int    i2c13_debug_addr      : 1; /* [6]  */
        unsigned int    i2c13_debug_data      : 1; /* [7]  */
        unsigned int    i2c13_debug_p_gen     : 1; /* [8]  */
        unsigned int    i2c13_debug_s_gen     : 1; /* [9]  */
        unsigned int    i2c13_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c13_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c13_debug;

/* Define the union u_lpsc_i2c14_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c14_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c14_debug_slave_act : 1; /* [1]  */
        unsigned int    i2c14_debug_master_act : 1; /* [2]  */
        unsigned int    i2c14_debug_hs        : 1; /* [3]  */
        unsigned int    i2c14_debug_wr        : 1; /* [4]  */
        unsigned int    i2c14_debug_rd        : 1; /* [5]  */
        unsigned int    i2c14_debug_addr      : 1; /* [6]  */
        unsigned int    i2c14_debug_data      : 1; /* [7]  */
        unsigned int    i2c14_debug_p_gen     : 1; /* [8]  */
        unsigned int    i2c14_debug_s_gen     : 1; /* [9]  */
        unsigned int    i2c14_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c14_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c14_debug;

/* Define the union u_lpsc_i2c15_debug */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    i2c15_debug_addr_10bit : 1; /* [0]  */
        unsigned int    i2c15_debug_slave_act : 1; /* [1]  */
        unsigned int    i2c15_debug_master_act : 1; /* [2]  */
        unsigned int    i2c15_debug_hs        : 1; /* [3]  */
        unsigned int    i2c15_debug_wr        : 1; /* [4]  */
        unsigned int    i2c15_debug_rd        : 1; /* [5]  */
        unsigned int    i2c15_debug_addr      : 1; /* [6]  */
        unsigned int    i2c15_debug_data      : 1; /* [7]  */
        unsigned int    i2c15_debug_p_gen     : 1; /* [8]  */
        unsigned int    i2c15_debug_s_gen     : 1; /* [9]  */
        unsigned int    i2c15_debug_slv_cstate : 4; /* [13..10]  */
        unsigned int    i2c15_debug_mst_cstate : 5; /* [18..14]  */
        unsigned int    reserved_0            : 13; /* [31..19]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_i2c15_debug;

/* Define the union u_lpsc_latency_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sts_en                : 1; /* [0]  */
        unsigned int    reserved_0            : 3; /* [3..1]  */
        unsigned int    port_sel              : 5; /* [8..4]  */
        unsigned int    reserved_1            : 3; /* [11..9]  */
        unsigned int    wr_sel                : 1; /* [12]  */
        unsigned int    reserved_2            : 19; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_latency_ctrl;

/* Define the union u_lpsc_hard_rr_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    se_sec_4x6_hard_en    : 1; /* [0]  */
        unsigned int    se_dma_3x1_hard_en    : 1; /* [1]  */
        unsigned int    sec_sec_1x3_hard_en   : 1; /* [2]  */
        unsigned int    se_sec_1x6_hard_en    : 1; /* [3]  */
        unsigned int    lp_sol_2x1_hard_en    : 1; /* [4]  */
        unsigned int    reserved_0            : 27; /* [31..5]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_hard_rr_ctrl;

/* Define the union u_lpsc_emmc_dll_st */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sc_sdio_sap_dll_dll_s_scan_out_0 : 1; /* [0]  */
        unsigned int    sc_sdio_sap_dll_dll_m_scan_out : 1; /* [1]  */
        unsigned int    sc_sdio_sap_dll_mdly_tap : 8; /* [9..2]  */
        unsigned int    sc_sdio_sap_dll_overflow : 1; /* [10]  */
        unsigned int    sc_sdio_sap_dll_ready : 1; /* [11]  */
        unsigned int    sc_sdio_sap_dll_locked : 1; /* [12]  */
        unsigned int    sc_emmc_dll_ds_dll_s_scan_out_0 : 1; /* [13]  */
        unsigned int    sc_emmc_dll_ds_dll_m_scan_out : 1; /* [14]  */
        unsigned int    sc_emmc_dll_ds_mdly_tap : 8; /* [22..15]  */
        unsigned int    sc_emmc_dll_ds_overflow : 1; /* [23]  */
        unsigned int    sc_emmc_dll_ds_ready  : 1; /* [24]  */
        unsigned int    sc_emmc_dll_ds_locked : 1; /* [25]  */
        unsigned int    reserved_0            : 6; /* [31..26]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_emmc_dll_st;

/* Define the union u_lpsc_pad_int_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sc_pad_int_level      : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_int_level;

/* Define the union u_lpsc_pad_int_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sc_pad_int_mask       : 20; /* [19..0]  */
        unsigned int    reserved_0            : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_pad_int_mask;

/* Define the union u_lpsc_sram_wr_back */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    sram_wr_back_en       : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_sram_wr_back;

/* Define the union u_lpsc_rst_fix */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int    lp_rst_fix_enable     : 1; /* [0]  */
        unsigned int    reserved_0            : 31; /* [31..1]  */
    } bits;

    /* Define an unsigned member */
    unsigned int    val;
} u_lpsc_rst_fix;

#endif /* __LP_SUBSYS_CSR_C_UNION_DEFINE_H__ */
