// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __GMAC_HW_H__
#define __GMAC_HW_H__

#include "gmac.h"

#define GMAC_TX_ADDR_REMAP_MODE 0x1
#define GMAC_TX_HADDR_OFFSET 0x8
#define GMAC_RX_ADDR_REMAP_MODE 0x1
#define GMAC_RX_HADDR_OFFSET 0x8

#define GMAC_SERDES_DEFAULT_RX_EQ 0x0
#define GMAC_SERDES_MAX_RX_EQ 0x7
#define GMAC_SERDES_DEFAULT_TX_PREEMPH 0x13
#define GMAC_SERDES_MAX_TX_PREEMPH 0x7f
#define GMAC_SERDES_DEFAULT_TX_AMPLITUDE 0x7f
#define GMAC_SERDES_MAX_TX_AMPLITUDE 0x7f

#define MPLL_LOCK_TIMEOUT 25   /* us */
#define TX_CM_TIMEOUT 230      /* us */
#define TX_RX_ENABLE_TIMEOUT 4 /* us */
#define PHY_INIT_DELAY_TIME 10 /* us */

#define PAUSE_FORCE_ENABLE 0x2
#define PAUSE_AUTO_ENABLE 0x0

#define GMAC_SRST_BIT 0x7
#define GMAC_MAC_CORE_SRST_BIT 0x2
#define GMAC_REG_CFG_SRST_BIT 0x3
#define GMAC_BUS_SYS_SRST_BIT 0x4
#define GMAC_RX_SERDES_SRST_BIT 0x5
#define GMAC_TX_SERDES_SRST_BIT 0x6

typedef union {
    struct {
        u32 phy_cr_cap_data_req : 16; /* [15..0]  */
        u32 phy_cr_cap_en : 1;        /* [16]  */
        u32 phy_cr_cap_write : 1;     /* [17]  */
        u32 phy_cr_cap_read : 1;      /* [18]  */
        u32 phy_cr_cap_data : 1;      /* [19]  */
        u32 phy_cr_cap_addr : 1;      /* [20]  */
        u32 reserved_0 : 11;          /* [31..21]  */
    } bits;

    u32 val;
} u_sds_cr_ctrl;

typedef union {
    struct {
        u32 phy_cr_cap_data_ack : 16; /* [15..0]  */
        u32 phy_cr_cap_ack : 1;       /* [16]  */
        u32 reserved_0 : 15;          /* [31..17]  */
    } bits;

    u32 val;
} u_sds_cr_data_out;

s32 gmac_hw_set_pkt_store_format(unsigned long base_addr, const gmac_pkt_store_ctrl_cfg_s *pkt_store_ctrl,
                                 unsigned int id);
s32 gmac_hw_set_pkt_input_parse_mode(unsigned long base_addr, const gmac_rx_pkt_mode_cfg_s *rx_pkt_mode);
s32 gmac_hw_set_bus_ctrl(unsigned long base_addr, u32 value);
s32 gmac_hw_set_rx_max_frame_len(unsigned long base_addr, u32 frame_len);
s32 gmac_hw_set_max_frame_len(unsigned long base_addr, u32 max_frame_len);
s32 gmac_hw_set_short_frame_len(unsigned long base_addr, u32 min_frame_len);
s32 gmac_hw_set_tx_add_crc(unsigned long base_addr, u32 crc_en);
s32 gmac_hw_set_rx_strip_crc(unsigned long base_addr, u32 crc_en);
s32 gmac_hw_set_rx_strip_pad(unsigned long base_addr, u32 en);
s32 gmac_hw_set_rx_short_frame_recv(unsigned long base_addr, u32 en);
s32 gmac_hw_set_port_mode_all(const struct net_device *dev, s32 lIndex);
s32 gmac_hw_enable_interrupt(u32 mac_num, unsigned long ulBaseAddr);
s32 gmac_hw_set_autoneg(const struct net_device *net_dev, const eth_drv_auto_nego_set_s *set);
void gmac_hw_rx_enable(unsigned long ulBaseAddr, u32 ulEnValue);
void gmac_hw_tx_enable(unsigned long ulBaseAddr, u32 ulEnValue);
s32 gmac_hw_set_bmu_close(const struct net_device *dev);
s32 gmac_hw_core_reset(u32 mac_num);
s32 gmac_hw_ptp_reset(unsigned long base_addr);
s32 gmac_hw_reset(unsigned long base_addr, u32 mac_num);
s32 gmac_hw_get_pause_en(const struct net_device *dev, u32 *tx_en, u32 *rx_en);
s32 gmac_hw_set_pause_en(const struct net_device *dev, u32 tx_en, u32 rx_en);
u32 gmac_hw_get_tx_ddr_relative_addr(u32 gmac_id, dma_addr_t phy_addr);
u32 gmac_hw_get_rx_ddr_relative_addr(u32 gmac_id, dma_addr_t phy_addr);
u32 gmac_hw_get_max_frame_len(unsigned long base_addr);
s32 gmac_hw_haddr_extern_config(u32 mac_num);
s32 gmac_serdes_init(u32 mac_num);
s32 gmac_serdes_power_set(u32 mac_num, u32 mode);
s32 gmac_get_serdes_neg_state(const struct net_device *net_dev, u32 mac_num, void *user);
void gmac_hw_txrx_fifo_thrsld_init(unsigned long base_addr);
void gmac_hw_tx_flow_ctrl_config(unsigned long base_addr);
void gmac_hw_hp_clock_enable(u32 mac_num);
s32 gmac_hw_serdes_write(u32 id, u32 addr, u32 data);
s32 gmac_hw_serdes_read(u32 id, u32 addr);
s32 gmac_hw_debug_lb_test(struct net_device *dev, u32 mode);
s32 gmac_hw_config_rmii_clk_info(struct net_device *dev, struct eth_drv_rmii_config *rmii_info);

#endif  // _GMAC_HW_H_
