// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gamc hw configuration
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>

#include "comm_knl_api.h"
#include "gmac_c_union_define.h"
#include "gmac_csr_addr_define.h"
#include "hp_sub_csr_addr_define.h"
#include "hp_sub_c_union_define.h"
#include "hp_subsys.h"
#include "io_config.h"
#include "io_config_r_addr_define.h"
#include "pcu_csr_addr_define.h"
#include "pcu_c_union_define.h"
#include "usr_data.h"
#include "gmac_hw.h"

static const u8 g_hw_loop_packet[GMAC_LOOP_DATA_SIZE] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x0d, 0x88, 0xf8, 0x10, 0xba, 0xff, 0xff,
    0x01, 0x65, 0x74, 0x50, 0x6f, 0x72, 0x74, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x50, 0x61, 0x63,
    0x02, 0x65, 0x74, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x03, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x04, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x00, 0x3e, 0x03, 0x02, 0xfa, 0xff, 0xff, 0xff, 0xff,
    0x05, 0x65, 0x74, 0x50, 0x6f, 0x72, 0x74, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x50, 0x61, 0x63,
    0x06, 0x65, 0x74, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x07, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x08, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x00, 0x3e, 0x03, 0x02, 0xfa, 0xff, 0xff, 0xff, 0xff,
    0x09, 0x65, 0x74, 0x50, 0x6f, 0x72, 0x74, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x50, 0x61, 0x63,
    0x0a, 0x65, 0x74, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x0b, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x0c, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x00, 0x3e, 0x03, 0x02, 0xfa, 0xff, 0xff, 0xff, 0xff,
    0x0d, 0x65, 0x74, 0x50, 0x6f, 0x72, 0x74, 0x20, 0x54, 0x65, 0x73, 0x74, 0x20, 0x50, 0x61, 0x63,
    0x0e, 0x65, 0x74, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55,
    0x0f, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0x55, 0x55, 0x55, 0x55
};

void gmac_hw_rx_enable(unsigned long ul_base_addr, u32 un_en_value)
{
    u_port_en un_port_en;

    gmac_reg_read((ul_base_addr + GMAC_CSR_PORT_EN_OFFSET_ADDR), un_port_en.val);
    un_port_en.bits.rx_en = un_en_value;
    gmac_reg_write((ul_base_addr + GMAC_CSR_PORT_EN_OFFSET_ADDR), un_port_en.val);

    return;
}

void gmac_hw_tx_enable(unsigned long ul_base_addr, u32 un_en_value)
{
    u_port_en un_port_en;

    /* read the RX channel configuration register to get current status */
    gmac_reg_read((ul_base_addr + GMAC_CSR_PORT_EN_OFFSET_ADDR), un_port_en.val);
    un_port_en.bits.tx_en = un_en_value;
    gmac_reg_write((ul_base_addr + GMAC_CSR_PORT_EN_OFFSET_ADDR), un_port_en.val);

    return;
}

s32 gmac_hw_core_reset(u32 mac_num)
{
    u32 rst_mask;

    /* mac Core soft reset */
    rst_mask = (0x1U << (GMAC_MAC_CORE_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    /* Bus interface soft reset */
    rst_mask = (0x1U << (0 + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    /* tx SerDes clock soft reset */
    if ((mac_num == ETH2) || (mac_num == ETH3)) {
        rst_mask = (0x1U << (GMAC_TX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
        (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);
    }

    /* rx serDes clock soft reset */
    rst_mask = (0x1U << (GMAC_RX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    mdelay(20);
    /* Reset deassertion. */
    rst_mask = (0x1U << (GMAC_MAC_CORE_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    rst_mask = (0x1U << (0 + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    if ((mac_num == ETH2) || (mac_num == ETH3)) {
        rst_mask = (0x1U << (GMAC_TX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
        (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);
    }

    rst_mask = (0x1U << (GMAC_RX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    return 0;
}

s32 gmac_hw_ptp_reset(unsigned long base_addr)
{
    static u8 init_flag = 0;
    u_hp_srst_req1 hp_srst_req1;
    u_hp_srst_dis1 hp_srst_dis1;

    if (init_flag) {
        return 0;
    }

    init_flag = 0x1;

    /* Resetting the PTP */
    hp_srst_req1.val = 0;
    hp_srst_req1.bits.ptp_srst_req_ena = 0x1;
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, hp_srst_req1.val);

    mdelay(20); /* delay 20ms */

    hp_srst_dis1.val = 0;
    hp_srst_dis1.bits.ptp_srst_req_dis = 0x1;
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, hp_srst_dis1.val);

    return 0;
}

void gmac_hw_hp_clock_enable(u32 mac_num)
{
    u_hp_clk_cfg hp_clk_cfg;
    u_hp_clk_enb hp_clk_enb;

    (void)hp_subsys_reg_read(HP_SUB_CSR_HP_CLK_ENB_OFFSET_ADDR, &hp_clk_enb.val);
    (void)hp_subsys_reg_read(HP_SUB_CSR_HP_CLK_CFG_OFFSET_ADDR, &hp_clk_cfg.val);
    if (hp_clk_cfg.bits.ptp_clk_cfg == DISABLE) {
        hp_clk_enb.bits.ptp_clk_en = ENABLE;
    }

    switch (mac_num) {
        case ETH0:
            hp_clk_enb.bits.gmac0_sysclk_en = ENABLE;
            hp_clk_enb.bits.gmac0_macclk_en = ENABLE;
            break;
        case ETH1:
            hp_clk_enb.bits.gmac1_sysclk_en = ENABLE;
            hp_clk_enb.bits.gmac1_macclk_en = ENABLE;
            break;
        case ETH2:
            hp_clk_enb.bits.gmac2_sysclk_en = ENABLE;
            hp_clk_enb.bits.gmac2_macclk_en = ENABLE;
            break;
        case ETH3:
            hp_clk_enb.bits.gmac3_sysclk_en = ENABLE;
            hp_clk_enb.bits.gmac3_macclk_en = ENABLE;
            break;
        default:
            break;
    }

    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_CLK_ENB_OFFSET_ADDR, hp_clk_enb.val);

    return;
}

s32 gmac_hw_reset(unsigned long base_addr, u32 mac_num)
{
    u32 rst_mask;
    u_bus_rst_en bus_rst_en;

    gmac_reg_read((base_addr + PCU_CSR_BUS_RST_EN_OFFSET_ADDR), bus_rst_en.val);
    bus_rst_en.bits.bus_rst_en = 0x1;
    gmac_reg_write((base_addr + PCU_CSR_BUS_RST_EN_OFFSET_ADDR), bus_rst_en.val);

    rst_mask = (0x1U << (GMAC_BUS_SYS_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    rst_mask = (0x1U << (0 + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    rst_mask = (0x1U << (GMAC_REG_CFG_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    if ((mac_num == ETH2) || (mac_num == ETH3)) {
        rst_mask = (0x1U << (GMAC_TX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
        (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);
    }

    rst_mask = (0x1U << (GMAC_RX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR, rst_mask);

    mdelay(20);

    /* Reset deassertion. */
    rst_mask = (0x1U << (GMAC_BUS_SYS_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    rst_mask = (0x1U << (0 + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    rst_mask = (0x1U << (GMAC_REG_CFG_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    if ((mac_num == ETH2) || (mac_num == ETH3)) {
        rst_mask = (0x1U << (GMAC_TX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
        (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);
    }

    rst_mask = (0x1U << (GMAC_RX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
    (void)hp_subsys_reg_write(HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR, rst_mask);

    return 0;
}

s32 gmac_hw_set_poe_close(const struct net_device *dev)
{
    unsigned long base_addr;
    u_we_addr work_entry;

    if (dev == NULL) {
        LOG(LOG_ERROR, "dev == NULL");
        return -EINVAL;
    }

    base_addr = dev->base_addr;

    gmac_reg_read((base_addr + PCU_CSR_WE_ADDR_OFFSET_ADDR), work_entry.val);
    work_entry.bits.cf_we_enable = 0;
    gmac_reg_write((base_addr + PCU_CSR_WE_ADDR_OFFSET_ADDR), work_entry.val);

    return GMAC_OK;
}

s32 gmac_hw_set_bmu_close(const struct net_device *dev)
{
    unsigned long base_addr;
    u_rx_ctrl rx_ctrl;

    if (dev == NULL) {
        LOG(LOG_ERROR, "gmac set bmu close, but net_dev parm is NULL!");
        return -EINVAL;
    }

    base_addr = dev->base_addr;

    /* The debugging Ethernet port must be set to 1 to passively receive the bus write address. */
    gmac_reg_read((base_addr + PCU_CSR_RX_CTRL_OFFSET_ADDR), rx_ctrl.val);
    rx_ctrl.bits.cf_rx_get_addr_mode = 1;
    gmac_reg_write((base_addr + PCU_CSR_RX_CTRL_OFFSET_ADDR), rx_ctrl.val);

    return 0;
}

u32 gmac_hw_set_sgmii_an_enable(const struct net_device *dev)
{
    u_transmit_control tx_ctrl;
    unsigned long base_addr = dev->base_addr;

    gmac_reg_write(base_addr + GMAC_CSR_TX_LOCAL_PAGE_OFFSET_ADDR, 0x1);

    gmac_reg_write(base_addr + GMAC_CSR_SIXTEEN_BIT_CNTR_OFFSET_ADDR, 0x3F);

    gmac_reg_write(base_addr + GMAC_CSR_LD_LINK_COUNTER_OFFSET_ADDR, 0x2);

    gmac_reg_read(base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR, tx_ctrl.val);
    tx_ctrl.bits.an_enable = 1;
    gmac_reg_write(base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR, tx_ctrl.val);

    return 0;
}

s32 gmac_hw_set_port_mode(const struct net_device *dev, u32 speed, u32 duplex)
{
    s32 ret;
    u_port_mode port_mode;
    unsigned long base_addr = dev->base_addr;
    u_mode_change_en mode_change_en;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    /* Check the validity of the parameter. */
    if (speed > MAC_1000M_SGMII) {
        LOG(LOG_ERROR, "the value of speed(0x%x) is invalid", speed);
        return -EINVAL;
    }

    if (duplex > GMAC_DUPLEX_FULL) {
        LOG(LOG_ERROR, "the value of duplex is invalid");
        return -EINVAL;
    }

    gmac_hw_rx_enable(base_addr, 0);
    gmac_hw_tx_enable(base_addr, 0);

    gmac_reg_read((base_addr + GMAC_CSR_DUPLEX_TYPE_OFFSET_ADDR), port_mode.val);
    port_mode.bits.port_mode = duplex;
    gmac_reg_write((base_addr + GMAC_CSR_DUPLEX_TYPE_OFFSET_ADDR), port_mode.val);

    gmac_reg_read((base_addr + GMAC_CSR_PORT_MODE_OFFSET_ADDR), port_mode.val);
    port_mode.bits.port_mode = speed;
    gmac_reg_write((base_addr + GMAC_CSR_PORT_MODE_OFFSET_ADDR), port_mode.val);

    gmac_reg_read((base_addr + GMAC_CSR_MODE_CHANGE_EN_OFFSET_ADDR), mode_change_en.val);
    mode_change_en.bits.mode_change_en = 1;
    gmac_reg_write((base_addr + GMAC_CSR_MODE_CHANGE_EN_OFFSET_ADDR), mode_change_en.val);

    ret = gmac_hw_core_reset(gmac_priv->id);
    if (ret) {
        LOG(LOG_ERROR, "mac(0x%x) set port mode, reset gmac core failed!", gmac_priv->id);
        return -EFAULT;
    }

    return GMAC_OK;
}

s32 gmac_hw_set_pkt_store_format(unsigned long base_addr, const gmac_pkt_store_ctrl_cfg_s *pkt_store_ctrl,
                                 unsigned int id)
{
    u_rx_ctrl rx_ctrl;

    if (pkt_store_ctrl == NULL) {
        LOG(LOG_ERROR, "rx ctrl parameter is NULL!");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + PCU_CSR_RX_CTRL_OFFSET_ADDR), rx_ctrl.val);

    rx_ctrl.bits.cf_rxbuf_1st_skip_size = pkt_store_ctrl->cf_buf_1st_skip1;
    rx_ctrl.bits.cf_rxbuf_no_1st_skip_size = pkt_store_ctrl->cf_buf_n1st_skip;
    rx_ctrl.bits.cf_rx_align_num = pkt_store_ctrl->cf_rx_align_num;
    rx_ctrl.bits.cf_addr_mode = 0;
    /* The lower 3 bits indicate cf_cache_line_l. */
    rx_ctrl.bits.cf_cache_line_h = (pkt_store_ctrl->cf_cache_line) >> 3;
    rx_ctrl.bits.cf_cache_line_l = (pkt_store_ctrl->cf_cache_line) & 0x7;
    rx_ctrl.bits.cf_rxbuf_1st_skip_size2 = pkt_store_ctrl->cf_rxbuf_1st_skip_size2;
    rx_ctrl.bits.cf_port_num = id;

    gmac_reg_write((base_addr + PCU_CSR_RX_CTRL_OFFSET_ADDR), rx_ctrl.val);

    return 0;
}

s32 gmac_hw_set_pkt_input_parse_mode(unsigned long base_addr, const gmac_rx_pkt_mode_cfg_s *rx_pkt_mode)
{
    u_rx_pkt_mode rx_mode;
    u_mac_skip_len mac_skip_len;

    if (rx_pkt_mode == NULL) {
        LOG(LOG_ERROR, "packet mode parameter NULL!");
        return -EINVAL;
    }

    /* To set the packet input mode */
    gmac_reg_read((base_addr + PCU_CSR_RX_PKT_MODE_OFFSET_ADDR), rx_mode.val);
    rx_mode.bits.cf_instr_head_mode = rx_pkt_mode->cf_instr_head_mode;
    rx_mode.bits.cf_skip_len = rx_pkt_mode->cf_skip_len;
    rx_mode.bits.cf_parse_mode = rx_pkt_mode->cf_parse_mode;
    rx_mode.bits.cf_gen_id = 0;
    gmac_reg_write((base_addr + PCU_CSR_RX_PKT_MODE_OFFSET_ADDR), rx_mode.val);

    gmac_reg_read((base_addr + GMAC_CSR_MAC_SKIP_LEN_OFFSET_ADDR), mac_skip_len.val);
    mac_skip_len.bits.mac_skip_len = rx_pkt_mode->mac_skip_len;
    mac_skip_len.bits.mac_skip_crc = 1;
    gmac_reg_write((base_addr + GMAC_CSR_MAC_SKIP_LEN_OFFSET_ADDR), mac_skip_len.val);

    return 0;
}

s32 gmac_hw_set_bus_ctrl(unsigned long base_addr, u32 value)
{
    u_bus_ctrl bus_ctrl;

    /* In this module, the system is set to big endian, the control word is big endian, and the data is big endian. */
    gmac_reg_read((base_addr + PCU_CSR_BUS_CTRL_OFFSET_ADDR), bus_ctrl.val);
    bus_ctrl.bits.cf_endian = value;
    gmac_reg_write((base_addr + PCU_CSR_BUS_CTRL_OFFSET_ADDR), bus_ctrl.val);

    return GMAC_OK;
}

s32 gmac_hw_set_rx_max_frame_len(unsigned long base_addr, u32 frame_len)
{
    u_max_frame_len rx_max_frame_len;

    if (frame_len > 0xFFFF) {
        LOG(LOG_ERROR, "the value of frame_len is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + PCU_CSR_MAX_FRAME_LEN_OFFSET_ADDR), rx_max_frame_len.val);

    rx_max_frame_len.bits.max_frame_len = (frame_len & 0xFFFC);
    gmac_reg_write((base_addr + PCU_CSR_MAX_FRAME_LEN_OFFSET_ADDR), rx_max_frame_len.val);

    return GMAC_OK;
}

s32 gmac_hw_set_max_frame_len(unsigned long base_addr, u32 max_frame_len)
{
    u_max_frm_size max_frame_size;

    if (max_frame_len > 0x3FFF) {
        LOG(LOG_ERROR, "the value of max_frame_len is invalid");
        return -EINVAL;
    }

    /* As nManger says, frame size + 4 is the maximum length for VLAN */
    gmac_reg_read((base_addr + GMAC_CSR_MAX_FRM_SIZE_OFFSET_ADDR), max_frame_size.val);
    max_frame_size.bits.max_frm_size = max_frame_len;
    gmac_reg_write((base_addr + GMAC_CSR_MAX_FRM_SIZE_OFFSET_ADDR), max_frame_size.val);

    return GMAC_OK;
}

u32 gmac_hw_get_max_frame_len(unsigned long base_addr)
{
    u_max_frm_size max_frame_size;

    gmac_reg_read((base_addr + GMAC_CSR_MAX_FRM_SIZE_OFFSET_ADDR), max_frame_size.val);
    return max_frame_size.val;
}

s32 gmac_hw_set_short_frame_len(unsigned long base_addr, u32 min_frame_len)
{
    u_short_runts_thr short_frame_thr;

    if ((min_frame_len == 0) || (min_frame_len > 0x1F)) {
        LOG(LOG_ERROR, "the value of min_frame_len is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + GMAC_CSR_SHORT_RUNTS_THR_OFFSET_ADDR), short_frame_thr.val);
    short_frame_thr.bits.short_runts_thr = min_frame_len;
    gmac_reg_write((base_addr + GMAC_CSR_SHORT_RUNTS_THR_OFFSET_ADDR), short_frame_thr.val);

    return GMAC_OK;
}

s32 gmac_hw_set_tx_add_crc(unsigned long base_addr, u32 crc_en)
{
    u_transmit_control tran_ctrl;

    if (crc_en > 1) {
        LOG(LOG_ERROR, "the value of ulCrcEn is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR), tran_ctrl.val);
    tran_ctrl.bits.crc_add = crc_en;
    gmac_reg_write((base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR), tran_ctrl.val);

    return GMAC_OK;
}

s32 gmac_hw_set_rx_strip_crc(unsigned long base_addr, u32 crc_en)
{
    u_cf_crc_strip rx_crc_strip;

    if (crc_en > 1) {
        LOG(LOG_ERROR, "the value of ulCrcEn is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + GMAC_CSR_CF_CRC_STRIP_OFFSET_ADDR), rx_crc_strip.val);
    rx_crc_strip.bits.cf_crc_strip = crc_en;
    gmac_reg_write((base_addr + GMAC_CSR_CF_CRC_STRIP_OFFSET_ADDR), rx_crc_strip.val);

    return GMAC_OK;
}

s32 gmac_hw_set_rx_strip_pad(unsigned long base_addr, u32 en)
{
    u_recv_control recv_ctrl;

    if (en > 1) {
        LOG(LOG_ERROR, "the value of ulCrcEn is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + GMAC_CSR_RECV_CONTROL_OFFSET_ADDR), recv_ctrl.val);
    recv_ctrl.bits.strip_pad_en = en;
    gmac_reg_write((base_addr + GMAC_CSR_RECV_CONTROL_OFFSET_ADDR), recv_ctrl.val);

    return GMAC_OK;
}

s32 gmac_hw_set_rx_short_frame_recv(unsigned long base_addr, u32 en)
{
    u_recv_control recv_ctrl;

    if (en > 1) {
        LOG(LOG_ERROR, "the value of en is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + GMAC_CSR_RECV_CONTROL_OFFSET_ADDR), recv_ctrl.val);
    recv_ctrl.bits.runt_pkt_en = en;
    gmac_reg_write((base_addr + GMAC_CSR_RECV_CONTROL_OFFSET_ADDR), recv_ctrl.val);

    return GMAC_OK;
}

s32 gmac_hw_set_tx_pad_add(unsigned long base_addr, u32 en_value)
{
    u_transmit_control tx_ctrl;
    u32 cmp;

    if (en_value > 1) {
        LOG(LOG_ERROR, "the value of un_en_value is invalid");
        return -EINVAL;
    }

    gmac_reg_read((base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR), tx_ctrl.val);
    tx_ctrl.bits.pad_enable = en_value;
    gmac_reg_write((base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR), tx_ctrl.val);

    gmac_reg_read((base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR), cmp);
    if (cmp != tx_ctrl.val) {
        LOG(LOG_ERROR, "cmp != en_value.val");
        return -EIO;
    }

    return GMAC_OK;
}

s32 gmac_hw_enable_interrupt(u32 mac_num, unsigned long ul_base_addr)
{
    u_cf_intrpt_msk un_int_msk;

    gmac_reg_read((ul_base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), un_int_msk.val);

    un_int_msk.bits.cf_we_err_int_msk = ENABLE;
    un_int_msk.bits.cf_rbreq_err_int_msk = ENABLE;
    un_int_msk.bits.cf_mac_ff_err_int_msk = ENABLE;
    un_int_msk.bits.cf_rx_ahb_err_int_msk = ENABLE;
    un_int_msk.bits.cf_pkg_intrpt_msk = ENABLE;
    /* The receive packet loss interrupt should be disabled only when the test is performed. */
    un_int_msk.bits.cf_rx_drop_int_msk = DISABLE;
    /* The TX packet loss interrupt should be disabled only when the test is performed. */
    un_int_msk.bits.cf_tx_drop_int_msk = ENABLE;
    un_int_msk.bits.cf_txcfg_avl_msk = ENABLE;
    un_int_msk.bits.cf_rel_buf_msk = ENABLE;
    un_int_msk.bits.cf_buf_avl_msk = DISABLE;
    un_int_msk.bits.cf_tx_ahb_err_int_msk = ENABLE;
    un_int_msk.bits.reserved_0 = DISABLE;
    un_int_msk.bits.cf_sram_parity_err_int_msk = DISABLE;

    if ((mac_num == ETH0) || (mac_num == ETH1)) {
        un_int_msk.bits.cf_intrpt_pkt = 0xf;
        un_int_msk.bits.cf_intrpt_time = 0x1;
    } else {
        un_int_msk.bits.cf_intrpt_pkt = 0x3F;
        un_int_msk.bits.cf_intrpt_time = 0x1;
    }

    gmac_reg_write((ul_base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), un_int_msk.val);

    return GMAC_OK;
}

s32 gmac_hw_set_port_mode_all(const struct net_device *dev, s32 l_index)
{
    s32 ret;
    u32 value = 0;
    u32 times = 0;
    unsigned long base_addr = dev->base_addr;

    if ((l_index == ETH0) || (l_index == ETH1)) {
        ret = gmac_hw_set_port_mode(dev, MAC_100M_MII, GMAC_DUPLEX_FULL);
        if (ret) {
            LOG(LOG_ERROR, "gmac_hw_set_port_mode fail");
            return ret;
        }
    } else {
        ret = gmac_hw_set_port_mode(dev, MAC_1000M_SGMII, GMAC_DUPLEX_FULL);
        if (ret) {
            LOG(LOG_ERROR, "gmac_hw_set_port_mode fail(eth%d)", l_index);
            return ret;
        }

        (void)gmac_hw_set_sgmii_an_enable(dev);

        do {
            gmac_reg_read(base_addr + GMAC_CSR_AN_NEG_STATE_OFFSET_ADDR, value);

            if (times >= 200) { // Up to 200 cycles
                LOG(LOG_WARN, "SGMII auto-negotiation not complete(0x%x)", value);
                break;
            }

            mdelay(5);
            times++;
        } while ((value & BIT(21)) == 0); // bit 21
    }

    ret = gmac_hw_set_tx_pad_add(base_addr, 1);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_tx_pad_add fail");
        return ret;
    }

    ret = gmac_hw_set_bmu_close(dev);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_bmu_close failed");
        return ret;
    }

    ret = gmac_hw_set_poe_close(dev);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_poe_close failed");
        return ret;
    }

    return GMAC_OK;
}

s32 gmac_hw_set_autoneg(const struct net_device *net_dev, const eth_drv_auto_nego_set_s *set)
{
    u_transmit_control un_tran_ctrl;

    if (set->phy_auto_nego_enable && !set->gmac_auto_nego_enable) {
        LOG(LOG_ERROR, "if phy_auto_nego_enable is 'enable', gmac_auto_nego_enable must be 'enable'");
        return -EINVAL;
    }
    gmac_reg_read(net_dev->base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR, un_tran_ctrl.val);
    if (set->gmac_auto_nego_enable) {
        un_tran_ctrl.bits.an_enable = 1;
    } else {
        un_tran_ctrl.bits.an_enable = 0;
    }
    gmac_reg_write(net_dev->base_addr + GMAC_CSR_TRANSMIT_CONTROL_OFFSET_ADDR, un_tran_ctrl.val);

    return 0;
}

s32 gmac_hw_set_pause_en(const struct net_device *dev, u32 tx_en, u32 rx_en)
{
    u_pause_en pause_en;
    unsigned long base_addr = dev->base_addr;

    gmac_reg_read(base_addr + GMAC_CSR_PAUSE_EN_OFFSET_ADDR, pause_en.val);
    pause_en.bits.tx_fdfc = tx_en;
    pause_en.bits.rx_fdfc = rx_en;
    gmac_reg_write(base_addr + GMAC_CSR_PAUSE_EN_OFFSET_ADDR, pause_en.val);

    return 0;
}

s32 gmac_hw_get_pause_en(const struct net_device *dev, u32 *tx_en, u32 *rx_en)
{
    u_pause_en pause_en;
    unsigned long base_addr = dev->base_addr;

    gmac_reg_read(base_addr + GMAC_CSR_PAUSE_EN_OFFSET_ADDR, pause_en.val);
    *tx_en = pause_en.bits.tx_fdfc;
    *rx_en = pause_en.bits.rx_fdfc;

    return 0;
}

u32 gmac_hw_get_tx_ddr_relative_addr(u32 gmac_id, dma_addr_t phy_addr)
{
    s32 ret;
    dma_addr_t tx_mac_haddr;
    dma_addr_t relative_addr;
    u_hp_mac_haddr_extend hp_mac_haddr_extend;

    ret = hp_subsys_reg_read((HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR + (gmac_id * 0x4)),
                             &hp_mac_haddr_extend.val);
    if (ret) {
        LOG(LOG_ERROR, "gmac%u tx bus get ddr relative addr failed(%d)!", gmac_id, ret);
        return (u32)phy_addr;
    }

    switch (hp_mac_haddr_extend.bits.m1_addr_remap_mode) {
        case 0x1:
            tx_mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m1_haddr_offset)) << 28); /* need left shift 28 bits */
            break;
        case 0x2:
            tx_mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m1_haddr_hi)) << 32); /* need left shift 32 bits */
            break;
        default:
            tx_mac_haddr = 0;
            break;
    }

    if (phy_addr < tx_mac_haddr) {
        LOG(LOG_ERROR, "gmac%u's HADDR_EXTEND(0x%x) is inval!", gmac_id, hp_mac_haddr_extend.val);
        return (u32)phy_addr;
    }

    relative_addr = phy_addr - tx_mac_haddr;
    if ((relative_addr >> 32) != 0) { // >> 32
        LOG(LOG_ERROR, "gmac%u's tx HP_MAC_HADDR_EXTEND(0x%x) is inval!", gmac_id, hp_mac_haddr_extend.val);
        return (u32)phy_addr;
    }

    return (u32)relative_addr;
}

u32 gmac_hw_get_rx_ddr_relative_addr(u32 gmac_id, dma_addr_t phy_addr)
{
    s32 ret;
    dma_addr_t rx_mac_haddr;
    dma_addr_t relative_addr;
    u_hp_mac_haddr_extend hp_mac_haddr_extend;

    ret = hp_subsys_reg_read((HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR + (gmac_id * 0x4)),
                             &hp_mac_haddr_extend.val);
    if (ret) {
        LOG(LOG_ERROR, "gmac%u rx bus get ddr relative addr failed(%d)!", gmac_id, ret);
        return (u32)phy_addr;
    }

    switch (hp_mac_haddr_extend.bits.m0_addr_remap_mode) {
        case 0x1:
            rx_mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m0_haddr_offset)) << 28); /* need left shift 28 bits */
            break;
        case 0x2:
            rx_mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m0_haddr_hi)) << 32); /* need left shift 32 bits */
            break;
        default:
            rx_mac_haddr = 0;
            break;
    }

    if (phy_addr < rx_mac_haddr) {
        LOG(LOG_ERROR, "gmac%u's HADDR_EXTEND(0x%x) is inval!", gmac_id, hp_mac_haddr_extend.val);
        return (u32)phy_addr;
    }

    relative_addr = phy_addr - rx_mac_haddr;
    if ((relative_addr >> 32) != 0) { // >> 32
        LOG(LOG_ERROR, "gmac%u's rx HP_MAC_HADDR_EXTEND(0x%x) is inval!", gmac_id, hp_mac_haddr_extend.val);
        return (u32)phy_addr;
    }

    return (u32)relative_addr;
}

s32 gmac_hw_haddr_extern_config(u32 mac_num)
{
    u_hp_mac_haddr_extend hp_mac_haddr_extend;

    hp_mac_haddr_extend.val = 0;

    hp_mac_haddr_extend.bits.m0_haddr_offset = GMAC_RX_HADDR_OFFSET;
    hp_mac_haddr_extend.bits.m0_addr_remap_mode = GMAC_RX_ADDR_REMAP_MODE;
    hp_mac_haddr_extend.bits.m1_haddr_offset = GMAC_TX_HADDR_OFFSET;
    hp_mac_haddr_extend.bits.m1_addr_remap_mode = GMAC_TX_ADDR_REMAP_MODE;

    hp_subsys_reg_write((HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR + (mac_num * 0x4)),
                        hp_mac_haddr_extend.val);

    return GMAC_OK;
}

u32 gmac_get_tx_amplitude(u32 mac_num)
{
    s32 ret;
    u32 type;
    u32 tx_amplitude = 0;

    if (mac_num == ETH2) {
        type = SGMII0_TX_AMPLITUDE;
    } else {
        type = SGMII1_TX_AMPLITUDE;
    }

    ret = usr_data_read(type, &tx_amplitude);
    if (ret || (tx_amplitude > GMAC_SERDES_MAX_TX_AMPLITUDE)) {
        LOG(LOG_ERROR, "get tx_amplitude(0x%x) failed", tx_amplitude);
        tx_amplitude = GMAC_SERDES_DEFAULT_TX_AMPLITUDE;
    }

    return tx_amplitude;
}

u32 gmac_get_tx_preemph(u32 mac_num)
{
    s32 ret;
    u32 type;
    u32 tx_preemph = 0;

    if (mac_num == ETH2) {
        type = SGMII0_TX_PREEMPH;
    } else {
        type = SGMII1_TX_PREEMPH;
    }

    ret = usr_data_read(type, &tx_preemph);
    if (ret || (tx_preemph > GMAC_SERDES_MAX_TX_PREEMPH)) {
        LOG(LOG_ERROR, "get tx_preemph(0x%x) failed", tx_preemph);
        tx_preemph = GMAC_SERDES_DEFAULT_TX_PREEMPH;
    }

    return tx_preemph;
}

u32 gmac_get_rx_eq(u32 mac_num)
{
    s32 ret;
    u32 type;
    u32 rx_eq = 0;

    if (mac_num == ETH2) {
        type = SGMII0_RX_EQ;
    } else {
        type = SGMII1_RX_EQ;
    }

    ret = usr_data_read(type, &rx_eq);
    if (ret || (rx_eq > GMAC_SERDES_MAX_RX_EQ)) {
        LOG(LOG_ERROR, "get rx_eq(0x%x) failed", rx_eq);
        rx_eq = GMAC_SERDES_DEFAULT_RX_EQ;
    }

    return rx_eq;
}

void gmac_sds_phy_reset(u32 sds_phy_cfg1_offset_addr)
{
    u_gmac_phy0_cfg phy_cfg;

    hp_subsys_reg_read(sds_phy_cfg1_offset_addr, &phy_cfg.val);
    if (phy_cfg.bits.phy_reset_0 != ENABLE) {
        phy_cfg.bits.phy_reset_0 = ENABLE;
        hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);
    }
    udelay(PHY_INIT_DELAY_TIME);

    return;
}

void gmac_sds_phy_los_reg_init(u32 sds_phy_los_cfg_offset_addr)
{
    u_gmac_phy0_los_cfg phy_los_cfg;

    hp_subsys_reg_read(sds_phy_los_cfg_offset_addr, &phy_los_cfg.val);
    phy_los_cfg.bits.rx_los_valid_rst_msk_0 = 0x3;
    phy_los_cfg.bits.tx_vboost_lvl_0 = 0x4;
    phy_los_cfg.bits.rx0_los_filt_cnt_0 = 0x5;
    phy_los_cfg.bits.los_bias_0 = 0x2;
    phy_los_cfg.bits.los_level_0 = 0x9;
    hp_subsys_reg_write(sds_phy_los_cfg_offset_addr, phy_los_cfg.val);

    return;
}

void gmac_sds_phy_cfg2_reg_init(u32 sds_phy_cfg2_offset_addr, u32 mac_num)
{
    u_gmac_phy0_cfg2 phy_cfg2;

    hp_subsys_reg_read(sds_phy_cfg2_offset_addr, &phy_cfg2.val);
    phy_cfg2.bits.phy_data_reverse_en0 = DISABLE;
    phy_cfg2.bits.tx0_data_en_0 = DISABLE;
    phy_cfg2.bits.tx0_preemph_0 = gmac_get_tx_preemph(mac_num);
    phy_cfg2.bits.tx0_detect_rx_req_0 = 0x0;
    phy_cfg2.bits.tx0_amplitude_0 = gmac_get_tx_amplitude(mac_num);
    phy_cfg2.bits.phy_tx0_term_offset_0 = 0x0;
    phy_cfg2.bits.phy_rx0_eq_0 = gmac_get_rx_eq(mac_num);
    hp_subsys_reg_write(sds_phy_cfg2_offset_addr, phy_cfg2.val);

    return;
}

void gmac_sds_phy_cfg1_reg_init(u32 sds_phy_cfg1_offset_addr)
{
    u_gmac_phy0_cfg phy_cfg;

    hp_subsys_reg_read(sds_phy_cfg1_offset_addr, &phy_cfg.val);
    phy_cfg.bits.phy_reset_0 = ENABLE;
    phy_cfg.bits.phy_rtune_req_0 = 0x0;
    phy_cfg.bits.ref_clkdiv2_0 = 0x0;
    phy_cfg.bits.ref_use_pad_0 = 0x1;
    phy_cfg.bits.vreg_bypass_0 = 0x1;
    phy_cfg.bits.mpll_multiplier_0 = 0x19;
    phy_cfg.bits.ssc_ref_clk_sel_0 = 0x0;
    phy_cfg.bits.lane0_loopbk_en_0 = DISABLE;
    phy_cfg.bits.rx0_align_en_0 = ENABLE;
    phy_cfg.bits.rx0_data_en_0 = DISABLE;
    phy_cfg.bits.rx0_invert_0 = 0x0;
    phy_cfg.bits.rx0_los_en_0 = ENABLE;
    phy_cfg.bits.rx0_pll_en_0 = DISABLE;
    phy_cfg.bits.rx0_reset_0 = 0x0;
    phy_cfg.bits.rx0_term_en_0 = ENABLE;
    phy_cfg.bits.tx0_cm_en_0 = DISABLE;
    phy_cfg.bits.tx0_en_0 = DISABLE;
    phy_cfg.bits.tx0_reset_0 = 0x0;
    phy_cfg.bits.tx0_vboost_en_0 = DISABLE;
    phy_cfg.bits.ref_ssp_en_0 = 0x0;
    phy_cfg.bits.tx0_invert_0 = 0x0;
    phy_cfg.bits.test_powerdown_0 = 0x0;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);
    udelay(PHY_INIT_DELAY_TIME);

    return;
}

s32 gmac_sds_phy_init(u32 sds_phy_cfg1_offset_addr, u32 sds_phy_cfg2_offset_addr,
                      u32 sds_phy_state_offset_addr)
{
    unsigned long timeout;
    u_gmac_phy0_cfg phy_cfg;
    u_gmac_phy0_cfg2 phy_cfg2;
    u_gmac_phy0_sts phy_state;

    hp_subsys_reg_read(sds_phy_cfg1_offset_addr, &phy_cfg.val);

    phy_cfg.bits.ref_ssp_en_0 = ENABLE;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

    phy_cfg.bits.phy_reset_0 = DISABLE;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

    phy_cfg.bits.mpll_en_0 = ENABLE;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

    timeout = jiffies + usecs_to_jiffies(MPLL_LOCK_TIMEOUT);
    do {
        if (time_after(jiffies, timeout)) {
            LOG(LOG_ERROR, "mpll lock timeout!");
            return -EIO;
        }

        hp_subsys_reg_read(sds_phy_state_offset_addr, &phy_state.val);
    } while (!(phy_state.bits.mpll_state_0));

    phy_cfg.bits.tx0_cm_en_0 = ENABLE;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

    timeout = jiffies + usecs_to_jiffies(TX_CM_TIMEOUT);
    do {
        if (time_after(jiffies, timeout)) {
            LOG(LOG_ERROR, "tx cm timeout!");
            return -EIO;
        }
        hp_subsys_reg_read(sds_phy_state_offset_addr, &phy_state.val);
    } while (!(phy_state.bits.tx0_cm_state_0));

    phy_cfg.bits.rx0_pll_en_0 = ENABLE;
    phy_cfg.bits.tx0_en_0 = ENABLE;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

    timeout = jiffies + usecs_to_jiffies(TX_RX_ENABLE_TIMEOUT);
    do {
        if (time_after(jiffies, timeout)) {
            LOG(LOG_ERROR, "tx/rx enable timeout!");
            return -EIO;
        }
        hp_subsys_reg_read(sds_phy_state_offset_addr, &phy_state.val);
    } while (!((phy_state.bits.tx0_state_0) && (phy_state.bits.rx0_pll_state_0)));

    /* Start data transmission in the transmit direction */
    hp_subsys_reg_read(sds_phy_cfg2_offset_addr, &phy_cfg2.val);
    phy_cfg2.bits.tx0_data_en_0 = ENABLE;
    hp_subsys_reg_write(sds_phy_cfg2_offset_addr, phy_cfg2.val);

    /* Start data transmission in the receive direction */
    phy_cfg.bits.rx0_data_en_0 = ENABLE;
    hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

    return GMAC_OK;
}

s32 gmac_serdes_init(u32 mac_num)
{
    u32 sds_phy_los_cfg_offset_addr;
    u32 sds_phy_cfg1_offset_addr;
    u32 sds_phy_cfg2_offset_addr;
    u32 sds_phy_state_offset_addr;

    if (mac_num == ETH2) {
        sds_phy_los_cfg_offset_addr = HP_SUB_CSR_GMAC_PHY0_LOS_CFG_OFFSET_ADDR;
        sds_phy_cfg1_offset_addr = HP_SUB_CSR_GMAC_PHY0_CFG_OFFSET_ADDR;
        sds_phy_cfg2_offset_addr = HP_SUB_CSR_GMAC_PHY0_CFG2_OFFSET_ADDR;
        sds_phy_state_offset_addr = HP_SUB_CSR_GMAC_PHY0_STS_OFFSET_ADDR;
    } else if (mac_num == ETH3) {
        sds_phy_los_cfg_offset_addr = HP_SUB_CSR_GMAC_PHY1_LOS_CFG_OFFSET_ADDR;
        sds_phy_cfg1_offset_addr = HP_SUB_CSR_GMAC_PHY1_CFG_OFFSET_ADDR;
        sds_phy_cfg2_offset_addr = HP_SUB_CSR_GMAC_PHY1_CFG2_OFFSET_ADDR;
        sds_phy_state_offset_addr = HP_SUB_CSR_GMAC_PHY1_STS_OFFSET_ADDR;
    } else {
        return -EINVAL;
    }

    gmac_sds_phy_reset(sds_phy_cfg1_offset_addr);

    gmac_sds_phy_los_reg_init(sds_phy_los_cfg_offset_addr);
    gmac_sds_phy_cfg2_reg_init(sds_phy_cfg2_offset_addr, mac_num);
    gmac_sds_phy_cfg1_reg_init(sds_phy_cfg1_offset_addr);

    return gmac_sds_phy_init(sds_phy_cfg1_offset_addr, sds_phy_cfg2_offset_addr,
                             sds_phy_state_offset_addr);
}

s32 gmac_serdes_power_set(u32 mac_num, u32 mode)
{
    u_gmac_phy0_cfg phy_cfg;
    u32 sds_phy_cfg1_offset_addr;
    s32 ret = GMAC_OK;

    if (mac_num == ETH2) {
        sds_phy_cfg1_offset_addr = HP_SUB_CSR_GMAC_PHY0_CFG_OFFSET_ADDR;
    } else if (mac_num == ETH3) {
        sds_phy_cfg1_offset_addr = HP_SUB_CSR_GMAC_PHY1_CFG_OFFSET_ADDR;
    } else {
        return -EINVAL;
    }

    if (mode == 0) {
        /* Serdes power down enable */
        hp_subsys_reg_read(sds_phy_cfg1_offset_addr, &phy_cfg.val);
        phy_cfg.bits.test_powerdown_0 = ENABLE;
        hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);
    } else {
        hp_subsys_reg_read(sds_phy_cfg1_offset_addr, &phy_cfg.val);
        phy_cfg.bits.phy_reset_0 = ENABLE;
        hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);

        /* Power on */
        phy_cfg.bits.test_powerdown_0 = DISABLE;
        hp_subsys_reg_write(sds_phy_cfg1_offset_addr, phy_cfg.val);
        udelay(10);

        ret = gmac_serdes_init(mac_num);
        if (ret) {
            return ret;
        }
    }

    return ret;
}

s32 gmac_get_serdes_neg_state(const struct net_device *net_dev, u32 mac_num, void *user)
{
    struct eth_serdes_neg_state serdes_info = { 0 };

    if (mac_num != ETH2 && mac_num != ETH3) {
        LOG(LOG_ERROR, "mac_num(%u) is invalid", mac_num);
        return -EINVAL;
    }

    gmac_reg_read(net_dev->base_addr + GMAC_CSR_AN_NEG_STATE_OFFSET_ADDR, serdes_info.state);
    serdes_info.cmd = ETH_SERDES_NEG_STATE;

    if (copy_to_user((char *)user, &serdes_info, sizeof(struct eth_serdes_neg_state))) {
        return -EFAULT;
    }

    return 0;
}

void gmac_hw_txrx_fifo_thrsld_init(unsigned long base_addr)
{
    u_cf_tx_fifo_thrsld tx_fifo;
    u_cf_rx_fifo_thrsld rx_fifo;
    u_cf_cfg_fifo_thrsld cfg_thrsld;

    /* Configuring the TX BD Threshold */
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_CF_CFG_FIFO_THRSLD_OFFSET_ADDR), cfg_thrsld.val);
    cfg_thrsld.bits.cfg_tx_p_full_th = 0x1;
    gmac_reg_write((uintptr_t)(base_addr + PCU_CSR_CF_CFG_FIFO_THRSLD_OFFSET_ADDR), cfg_thrsld.val);

    /* Configuring the TX data threshold */
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_CF_TX_FIFO_THRSLD_OFFSET_ADDR), tx_fifo.val);
    tx_fifo.bits.cf_tx_p_empty_th = 0x80;
    tx_fifo.bits.cf_tx_p_full_th = 0x300;
    gmac_reg_write((uintptr_t)(base_addr + PCU_CSR_CF_TX_FIFO_THRSLD_OFFSET_ADDR), tx_fifo.val);

    /* Configuring the RX data threshold */
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_CF_RX_FIFO_THRSLD_OFFSET_ADDR), rx_fifo.val);
    rx_fifo.bits.cf_rx_p_empty_th = 0x80;
    rx_fifo.bits.cf_rx_p_full_th = 0x100;
    gmac_reg_write((uintptr_t)(base_addr + PCU_CSR_CF_RX_FIFO_THRSLD_OFFSET_ADDR), rx_fifo.val);

    return;
}

void gmac_hw_tx_flow_ctrl_config(unsigned long base_addr)
{
    u_fc_tx_timer tx_timer;
    u_pause_thr pause_thr;

    /* Configuring the Send Flow Control Time Parameters */
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_FC_TX_TIMER_OFFSET_ADDR), tx_timer.val);
    tx_timer.bits.fc_tx_timer = 0xFFFF;
    gmac_reg_write((uintptr_t)(base_addr + GMAC_CSR_FC_TX_TIMER_OFFSET_ADDR), tx_timer.val);

    /* Configuring the TX Flow Control Interval */
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_PAUSE_THR_OFFSET_ADDR), pause_thr.val);
    pause_thr.bits.pause_thr = 0x7FF;
    gmac_reg_write((uintptr_t)(base_addr + GMAC_CSR_PAUSE_THR_OFFSET_ADDR), pause_thr.val);

    return;
}

static s32 gmac_hw_serdes_addr_lock(u32 cr_addr, u32 data_addr, u32 serdes_addr)
{
    u_sds_cr_ctrl  cr_ctrl = { .val = 0 };
    u_sds_cr_data_out cr_data_out = { .val = 0 };

    hp_subsys_reg_read(cr_addr, &cr_ctrl.val);
    cr_ctrl.bits.phy_cr_cap_data_req = serdes_addr;
    hp_subsys_reg_write(cr_addr, cr_ctrl.val);
    udelay(1);

    hp_subsys_reg_read(cr_addr, &cr_ctrl.val);
    cr_ctrl.bits.phy_cr_cap_addr = 1;
    hp_subsys_reg_write(cr_addr, cr_ctrl.val);
    udelay(1);

    hp_subsys_reg_read(data_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack == 1) {
        hp_subsys_reg_read(cr_addr, &cr_ctrl.val);
        cr_ctrl.bits.phy_cr_cap_addr = 0;
        hp_subsys_reg_write(cr_addr, cr_ctrl.val);
        udelay(1);
    } else {
        LOG(LOG_ERROR, "addr lock error");
        return -1;
    }

    hp_subsys_reg_read(data_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack != 0) {
        LOG(LOG_ERROR, "addr lock error");
        return -1;
    } else {
        return 0;
    }
}

static int gmac_hw_serdes_prepare(u32 id, u32 addr, u32 *cr_addr, u32 *data_addr)
{
    u_sds_cr_ctrl  cr_ctrl;

    if (id == ETH2) {
        *cr_addr = HP_SUB_CSR_HP_SDS0_CR_CTRL_OFFSET_ADDR;
        *data_addr = HP_SUB_CSR_HP_SDS0_CR_DATA_OUT_OFFSET_ADDR;
    } else if (id == ETH3) {
        *cr_addr = HP_SUB_CSR_HP_SDS1_CR_CTRL_OFFSET_ADDR;
        *data_addr = HP_SUB_CSR_HP_SDS1_CR_DATA_OUT_OFFSET_ADDR;
    } else {
        LOG(LOG_ERROR, "id error");
        return -1;
    }

    cr_ctrl.val = 0;
    cr_ctrl.bits.phy_cr_cap_en = 1;
    hp_subsys_reg_write(*cr_addr, cr_ctrl.val);

    return gmac_hw_serdes_addr_lock(*cr_addr, *data_addr, addr);
}

s32 gmac_hw_serdes_read(u32 id, u32 addr)
{
    u32 serdes_cr_ctr_addr, serdes_data_out_addr;
    u_sds_cr_ctrl  cr_ctrl;
    u_sds_cr_data_out cr_data_out;
    u32 read_value;

    if (gmac_hw_serdes_prepare(id, addr, &serdes_cr_ctr_addr, &serdes_data_out_addr)) {
        return -1;
    }

    hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
    cr_ctrl.bits.phy_cr_cap_read = 1;
    hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
    udelay(1);

    hp_subsys_reg_read(serdes_data_out_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack == 1) {
        read_value = cr_data_out.bits.phy_cr_cap_data_ack;
        hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
        cr_ctrl.bits.phy_cr_cap_read = 0;
        hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
        udelay(1);
    } else {
        LOG(LOG_ERROR, "read cr interface error");
        return -1;
    }

    hp_subsys_reg_read(serdes_data_out_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack == 1) {
        LOG(LOG_ERROR, "read interface error");
        return -1;
    }

    return read_value;
}

s32 gmac_hw_serdes_write(u32 id, u32 addr, u32 data)
{
    u32 serdes_cr_ctr_addr, serdes_data_out_addr;
    u_sds_cr_ctrl cr_ctrl;
    u_sds_cr_data_out cr_data_out;

    if (gmac_hw_serdes_prepare(id, addr, &serdes_cr_ctr_addr, &serdes_data_out_addr)) {
        return -1;
    }

    hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
    cr_ctrl.bits.phy_cr_cap_data_req = data;
    hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
    udelay(1);

    hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
    cr_ctrl.bits.phy_cr_cap_data = 1;
    hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
    udelay(1);

    hp_subsys_reg_read(serdes_data_out_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack == 1) {
        hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
        cr_ctrl.bits.phy_cr_cap_data = 0;
        hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
        udelay(1);
    } else {
        LOG(LOG_ERROR, "write cr interface error");
        return -1;
    }

    hp_subsys_reg_read(serdes_data_out_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack != 0) {
        LOG(LOG_ERROR, "read interface error");
        return -1;
    }

    hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
    cr_ctrl.bits.phy_cr_cap_write = 1;
    hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
    udelay(1);

    hp_subsys_reg_read(serdes_data_out_addr, &cr_data_out.val);
    if (cr_data_out.bits.phy_cr_cap_ack == 1) {
        hp_subsys_reg_read(serdes_cr_ctr_addr, &cr_ctrl.val);
        cr_ctrl.bits.phy_cr_cap_write = 0;
        hp_subsys_reg_write(serdes_cr_ctr_addr, cr_ctrl.val);
        udelay(1);
    } else {
        LOG(LOG_ERROR, "write cr interface error");
        return -1;
    }

    hp_subsys_reg_read(serdes_data_out_addr, &cr_data_out.val);
    return (cr_data_out.bits.phy_cr_cap_ack != 0) ? -1 : 0;
}

void gmac_hw_mac_lb_en(const struct net_device *dev, u32 value_enable)
{
    u_line_loop_back loopback;
    u_loop_reg loopreg;
    unsigned long ul_base_addr = dev->base_addr;

    /* Disable line loopback. */
    gmac_reg_read(ul_base_addr + GMAC_CSR_LINE_LOOP_BACK_OFFSET_ADDR, loopback.val);
    loopback.bits.line_loop_back = 0;
    gmac_reg_write(ul_base_addr + GMAC_CSR_LINE_LOOP_BACK_OFFSET_ADDR, loopback.val);

    /* Enable application loopback */
    gmac_reg_read(ul_base_addr + GMAC_CSR_LOOP_REG_OFFSET_ADDR, loopreg.val);
    loopreg.bits.cf2mi_lp_en = value_enable;
    gmac_reg_write(ul_base_addr + GMAC_CSR_LOOP_REG_OFFSET_ADDR, loopreg.val);
}

s32 gmac_hw_mac_line_en(const struct net_device *dev, u32 value_enable)
{
    u_line_loop_back loopback;
    u_loop_reg loopreg;
    unsigned long ul_base_addr = dev->base_addr;

    switch (value_enable) {
        case 0: /* 0: disabled */
            gmac_reg_read(ul_base_addr + GMAC_CSR_LINE_LOOP_BACK_OFFSET_ADDR, loopback.val);
            loopback.bits.line_loop_back = 0;
            gmac_reg_write(ul_base_addr + GMAC_CSR_LINE_LOOP_BACK_OFFSET_ADDR, loopback.val);

            gmac_reg_read(ul_base_addr + GMAC_CSR_LOOP_REG_OFFSET_ADDR, loopreg.val);
            loopreg.bits.cf_ext_drive_lp = 1;
            gmac_reg_write(ul_base_addr + GMAC_CSR_LOOP_REG_OFFSET_ADDR, loopreg.val);
            break;

        case 1: /* 1: enabled */
            gmac_reg_read(ul_base_addr + GMAC_CSR_LINE_LOOP_BACK_OFFSET_ADDR, loopback.val);
            loopback.bits.line_loop_back = 1;
            gmac_reg_write(ul_base_addr + GMAC_CSR_LINE_LOOP_BACK_OFFSET_ADDR, loopback.val);

            gmac_reg_read(ul_base_addr + GMAC_CSR_LOOP_REG_OFFSET_ADDR, loopreg.val);
            loopreg.bits.cf_ext_drive_lp = 0;
            gmac_reg_write(ul_base_addr + GMAC_CSR_LOOP_REG_OFFSET_ADDR, loopreg.val);
            break;

        default:
            LOG(LOG_ERROR, "un_en_value is error!");
            return -EINVAL;
    }

    return GMAC_OK;
}

static s32 gmac_hw_serdes_lb_en(const struct net_device *dev, u32 value_enable)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);
    u_gmac_phy0_cfg phy_cfg = {.val = 0};
    u32 hp_cfg_offset;

    if (gmac_priv->id == ETH2) {
        hp_cfg_offset = HP_SUB_CSR_GMAC_PHY0_CFG_OFFSET_ADDR;
    } else {
        hp_cfg_offset = HP_SUB_CSR_GMAC_PHY1_CFG_OFFSET_ADDR;
    }

    (void)hp_subsys_reg_read(hp_cfg_offset, &phy_cfg.val);
    phy_cfg.bits.lane0_loopbk_en_0 = value_enable;
    (void)hp_subsys_reg_write(hp_cfg_offset, phy_cfg.val);

    mdelay(1);

    return 0;
}

static s32 gmac_hw_set_lb_mode(const struct net_device *dev, u32 loop_value, loop_mode_e loop_mode)
{
    s32 ret = 0;

    if ((loop_value != ENABLE) && (loop_value != DISABLE)) {
        LOG(LOG_ERROR, "the value of ulLoopEn is invalid!");
        return -EINVAL;
    }

    if (loop_mode == ETH_NO_LOOP) {
        LOG(LOG_ERROR, "the value of enLoopMode is ETH_NO_LOOP");
        return -EINVAL;
    }

    switch (loop_mode) {
        case ETH_MAC_SYS_LOOP:
            gmac_hw_mac_lb_en(dev, loop_value);
            break;
        case ETH_MAC_LINE_LOOP:
            (void)gmac_hw_mac_line_en(dev, loop_value);
            break;
        case ETH_SERDES_SYS_LOOP:
            ret = gmac_hw_serdes_lb_en(dev, loop_value);
            break;
        case ETH_SERDES_LINE_LOOP:
            break;
        default:
            LOG(LOG_ERROR, "the value of loop_mode is invalid");
            return -EINVAL;
    }

    return ret;
}

s32 gmac_hw_debug_pkt_send(struct net_device *dev, const u8 *pkt, u32 len)
{
    struct sk_buff *skb = NULL;
    s32 ret;

    if ((len == 0) || (len > GMAC_MAX_PKT_LEN)) {
        LOG(LOG_ERROR, "The packet length is invalid!");
        return -EINVAL;
    }

    skb = dev_alloc_skb(RX_BUFFER_SIZE);
    if (skb == NULL) {
        LOG(LOG_ERROR, "PktSend dev_alloc_skb failed");
        return -ENOMEM;
    }

    (void)memset_s(skb->head, RX_BUFFER_SIZE, 0x0, RX_BUFFER_SIZE);
    ret = memcpy_s(skb->data, RX_BUFFER_SIZE, (void *)pkt, len);
    if (ret) {
        dev_kfree_skb_any(skb);
        return ret;
    }

    skb->len = len;

    ret = dev->netdev_ops->ndo_start_xmit(skb, dev);
    if (ret) {
        LOG(LOG_ERROR, "Send Packet fail with code %d!", ret);
        dev_kfree_skb_any(skb);
        return -EBUSY;
    }

    return GMAC_OK;
}

void gmac_hw_lb_recv(const struct net_device *dev, const struct sk_buff *pst_skbuff)
{
    s8 *data_buf = NULL;
    s8 *rx_descrip = NULL; /* MAC packet descriptor */
    u32 len;
    gmac_private_s *gmac_priv = NULL;

    if ((dev == NULL) ||  (pst_skbuff == NULL)) {
        return;
    }

    gmac_priv = (gmac_private_s *)netdev_priv(dev);
    rx_descrip = pst_skbuff->data;
    // << 8
    len = ((u8)rx_descrip[GMAC_DESC_PKT_LEN_HIGH] << 8) + rx_descrip[GMAC_DESC_PKT_LEN_LOW];
    data_buf = (s8 *)(pst_skbuff->data + (PACKET_HEAD_SIZE + NET_IP_ALIGN));
    if (len == GMAC_LOOP_DATA_SIZE) {
        if (memcmp((void *)data_buf, (void *)g_hw_loop_packet, GMAC_LOOP_DATA_SIZE) == 0) {
            gmac_priv->gmac_loop_cnt++;
        }
    }
}

s32 gmac_hw_check_loop_mode_ok(u32 mode, u32 mac_num)
{
    if (mode == ETH_NO_LOOP) {
        LOG(LOG_ERROR, "the value of enLoopMode is GMAC_NO_LOOP");
        return -EINVAL;
    }

    if (mode >= ETH_UNKNOWN_LOOP_MODE) {
        LOG(LOG_ERROR, "the value of Loop Mode is invalid.");
        return -EINVAL;
    }

    /* SerDes is used only for eth2 and eth3. */
    if (((mode == ETH_SERDES_SYS_LOOP) || (mode == ETH_SERDES_LINE_LOOP)) &&
        (mac_num != ETH2) && (mac_num != ETH3)) {
        LOG(LOG_ERROR, "eth%u do not support ETH_SERDES_SYS_LOOP mode", mac_num);
        return -EINVAL;
    }

    return 0;
}

s32 gmac_hw_debug_lb_test(struct net_device *dev, u32 mode)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);
    loop_mode_e loop_mode = mode;
    u32 index;
    s32 ret;
    unsigned long flags;

    ret = gmac_hw_check_loop_mode_ok(mode, gmac_priv->id);
    if (ret) {
        return ret;
    }

    spin_lock_irqsave(&gmac_priv->lock, flags);

    /* Set the loopback mode. */
    ret = gmac_hw_set_lb_mode(dev, ENABLE, loop_mode);
    if (ret) {
        spin_unlock_irqrestore(&gmac_priv->lock, flags);
        LOG(LOG_ERROR, "gmac_hw_set_lb_mode failed");
        return ret;
    }

    gmac_priv->gmac_loop_recv_hook = gmac_hw_lb_recv;
    spin_unlock_irqrestore(&gmac_priv->lock, flags);
    gmac_priv->gmac_loop_cnt = 0;
    mdelay(1);

    for (index = 0; index < GMAC_LOOP_NUM; index++) {
        if (gmac_hw_debug_pkt_send(dev, g_hw_loop_packet, GMAC_LOOP_DATA_SIZE)) {
            LOG(LOG_ERROR, "PktSend failed");
            goto ERROUT;
        }

        mdelay(1);
    }

    /* wait frame receive */
    for (index = 0; index < GMAC_LOOP_WAIT; index++) {
        msleep(10); /* 10ms */
        if (gmac_priv->gmac_loop_cnt == GMAC_LOOP_NUM) {
            break;
        }
    }

    if (index >= GMAC_LOOP_WAIT) {
        ret = -ETIME;
    }

    LOG(LOG_ERROR, "(Transmitted %u frames,received %u frames!)", GMAC_LOOP_NUM, gmac_priv->gmac_loop_cnt);
    LOG(LOG_ERROR, "loop test %s", (gmac_priv->gmac_loop_cnt != GMAC_LOOP_NUM) ? "fail" : "success");

ERROUT:
    spin_lock_irqsave(&gmac_priv->lock, flags);
    gmac_priv->gmac_loop_recv_hook = NULL;

    /* Cancel the loopback mode. */
    if (gmac_hw_set_lb_mode(dev, DISABLE, loop_mode)) {
        LOG(LOG_ERROR, "gmac_hw_set_lb_mode failed");
    }
    spin_unlock_irqrestore(&gmac_priv->lock, flags);

    return ret;
}

s32 gmac_hw_config_rmii_clk_info(struct net_device *dev, struct eth_drv_rmii_config *rmii_info)
{
    gmac_private_s *priv = (gmac_private_s *)netdev_priv(dev);
    u_port_mode port_mode;
    u_hp_clk_sel hp_clk_sel;
    u32 val = 0;
    s32 ret;

#define PADMG39_DS_OFFSET	4
#define PADMG39_DS_MASK		0x7

    if (rmii_info->clk_sel != 0 && rmii_info->clk_sel != 1) {
        LOG(LOG_ERROR, "clk sel(%u) is invalid", rmii_info->clk_sel);
        return -EINVAL;
    }

    if (rmii_info->clk_edge_sel != 0 && rmii_info->clk_edge_sel != 1) {
        LOG(LOG_ERROR, "clk edge(%u) is invalid", rmii_info->clk_edge_sel);
        return -EINVAL;
    }

    io_config_r_reg_read(IO_CONFIG_R_PADMG039_OFFSET_ADDR, &val);
    gmac_reg_read(dev->base_addr + GMAC_CSR_PORT_MODE_OFFSET_ADDR, port_mode.val);
    ret = hp_subsys_reg_read(HP_SUB_CSR_HP_CLK_SEL_OFFSET_ADDR, &hp_clk_sel.val);
    if (ret) {
        return ret;
    }

    if (priv->id == ETH0) {
        hp_clk_sel.bits.gmac0_rmii_sel = rmii_info->clk_sel;
        port_mode.bits.rmii_clk_edge = rmii_info->clk_edge_sel;
        val &= ~(PADMG39_DS_MASK << PADMG39_DS_OFFSET);
        val |= ((rmii_info->dri_power & PADMG39_DS_MASK) << PADMG39_DS_OFFSET);
    } else if (priv->id == ETH1) {
        hp_clk_sel.bits.gmac1_rmii_sel = rmii_info->clk_sel;
        port_mode.bits.rmii_clk_edge = rmii_info->clk_edge_sel;
    } else {
        LOG(LOG_ERROR, "mac(%u) is invalid", priv->id);
        return EINVAL;
    }

    io_config_r_reg_write(IO_CONFIG_R_PADMG039_OFFSET_ADDR, val);
    gmac_reg_write(dev->base_addr + GMAC_CSR_PORT_MODE_OFFSET_ADDR, port_mode.val);

    return hp_subsys_reg_write(HP_SUB_CSR_HP_CLK_SEL_OFFSET_ADDR, hp_clk_sel.val);
}