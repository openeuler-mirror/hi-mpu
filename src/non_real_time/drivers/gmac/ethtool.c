// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gmac ethtool
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <asm/uaccess.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include "comm_knl_api.h"
#include "com_phy.h"
#include "gmac_csr_addr_define.h"
#include "pcu_csr_addr_define.h"
#include "gmac.h"
#include "gmac_hw.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#define ETHTOOL_GET_SETTING get_link_ksettings
#define ETHTOOL_SET_SETTING set_link_ksettings
#else
#define ETHTOOL_GET_SETTING get_settings
#define ETHTOOL_SET_SETTING set_settings
#endif

static const struct {
    const char string[ETH_GSTRING_LEN];
} ethtool_stats_keys[GMAC_NUM_STATS] = {
    {"rx_desc_l3_wrong_head_cnt"},
    {"rx_desc_l3_csum_err_cnt"},
    {"rx_desc_l3_len_err_cnt"},
    {"rx_desc_l3_zero_ttl_cnt"},
    {"rx_desc_l3_rsvd_cnt"},
    {"rx_desc_l4_wrong_head_cnt"},
    {"rx_desc_l4_len_err_cnt"},
    {"rx_desc_l4_csum_err_cnt"},
    {"rx_desc_l4_zero_port_num_cnt"},
    {"rx_desc_l4_rsvd_cnt"},
    {"rx_desc_frag_cnt"},
    {"rx_desc_ip_ver_err_cnt"},
    {"rx_desc_ipv4_pkt_cnt"},
    {"rx_desc_ipv6_pkt_cnt"},
    {"rx_desc_no_ip_pkt_cnt"},
    {"rx_desc_ip_pkt_cnt"},
    {"rx_desc_tcp_pkt_cnt"},
    {"rx_desc_udp_pkt_cnt"},
    {"rx_desc_vlan_pkt_cnt"},
    {"rx_desc_icmp_pkt_cnt"},
    {"rx_desc_arp_pkt_cnt"},
    {"rx_desc_rarp_pkt_cnt"},
    {"rx_desc_multicast_pkt_cnt"},
    {"rx_desc_broadcast_pkt_cnt"},
    {"rx_desc_ipsec_pkt_cnt"},
    {"rx_desc_ip_opt_pkt_cnt"},
    {"rx_desc_key_match_cnt"},
    {"rx_octets_total_ok_cnt"},
    {"rx_uc_pkts_cnt"},
    {"rx_mc_pkts_cnt"},
    {"rx_bc_pkts_cnt"},
    {"rx_vlan_pkt_cnt"},
    {"rx_fw_ctrl_frame_cnt"},
    {"rx_octets_bad_cnt"},
    {"rx_octets_total_filt_cnt"},
    {"rx_filt_pkt_cnt"},

    {"rx_framsize_64"},
    {"rx_framsize_65_127"},
    {"rx_framsize_128_255"},
    {"rx_framsize_256_511"},
    {"rx_framsize_512_1023"},
    {"rx_framsize_1024_1518"},
    {"rx_framsize_bt_1518"},

    {"rx_fcs_error_cnt"},
    {"rx_data_error_cnt"},
    {"rx_align_error_cnt"},
    {"rx_frame_long_err_cnt"},
    {"rx_frame_very_long_err_cnt"},
    {"rx_frame_runt_err_cnt"},
    {"rx_frame_short_err_cnt"},
    {"rx_overrun_cnt"},

    {"rx_rbuff_rq_err_cnt"},

    {"tx_octets_total_ok_cnt"},
    {"tx_uc_pkts_cnt"},
    {"tx_mc_pkts_cnt"},
    {"tx_bc_pkts_cnt"},
    {"tx_vlan_pkt_cnt"},
    {"tx_fw_ctrl_frame_cnt"},
    {"tx_octets_bad_cnt"},

    {"tx_framsize_64"},
    {"tx_framsize_65_127"},
    {"tx_framsize_128_255"},
    {"tx_framsize_256_511"},
    {"tx_framsize_512_1023"},
    {"tx_framsize_1024_1518"},
    {"tx_framsize_bt_1518"},

    {"tx_fcs_error_cnt"},
    {"tx_underrun_err_cnt"},
    {"tx_add_cs_fail_cnt"},
    {"tx_buffrl_err_cnt"},
    {"tx_we_err_cnt"},

    {"rx_overrun_drop_cnt"},

    {"tx_excessive_drop_cnt"},
    {"tx_overrun_drop_cnt"},
};

static int gmac_ethtool_get_speed(struct net_device *dev, u16 *speed)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    if ((gmac_priv->oldspeed == MAC_10M_MII) || (gmac_priv->oldspeed == MAC_10M_RGMII) ||
        (gmac_priv->oldspeed == MAC_10M_SGMII)) {
        *speed = SPEED_10;
    } else if ((gmac_priv->oldspeed == MAC_100M_MII) || (gmac_priv->oldspeed == MAC_100M_RGMII) ||
               (gmac_priv->oldspeed == MAC_100M_SGMII)) {
        *speed = SPEED_100;
    } else if ((gmac_priv->oldspeed == MAC_1000M_GMII) || (gmac_priv->oldspeed == MAC_1000M_RGMII) ||
               (gmac_priv->oldspeed == MAC_1000M_SGMII)) {
        *speed = SPEED_1000;
    } else {
        return -EINVAL;
    }

    return GMAC_OK;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
static int gmac_ethtool_get_settings(struct net_device *dev, struct ethtool_link_ksettings *cmd)
{
    gmac_private_s *priv = netdev_priv(dev);
    u32 supported, advertising;
    u16 speed;
    s32 ret;

    supported = SUPPORTED_Autoneg;
    supported |= SUPPORTED_100baseT_Half |
                SUPPORTED_100baseT_Full |
                SUPPORTED_10baseT_Half |
                SUPPORTED_10baseT_Full |
                SUPPORTED_TP;

    if (priv->id == ETH2 || priv->id == ETH3)
        supported |= SUPPORTED_1000baseT_Full;
    advertising = SUPPORTED_Autoneg;

    ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.supported, supported);
    ethtool_convert_legacy_u32_to_link_mode(cmd->link_modes.advertising, advertising);

    ret = gmac_ethtool_get_speed(dev, &speed);
    if (ret) {
        LOG(LOG_ERROR, "mac(%u) get speed failed", priv->id);
        return ret;
    }

    cmd->base.speed = speed;
    cmd->base.duplex = priv->oldduplex ? DUPLEX_FULL : DUPLEX_HALF;
    cmd->base.phy_address = (u8)(priv->phy_addr & 0xff);
    cmd->base.autoneg = (u8)(priv->oldautoneg & 0xff);

    return 0;
}
#else
static int gmac_ethtool_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    int ret;
    u16 speed = 0;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    cmd->supported = (SUPPORTED_Autoneg);
    cmd->supported |= (SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full | SUPPORTED_10baseT_Half |
                       SUPPORTED_10baseT_Full | SUPPORTED_TP);

    if ((gmac_priv->id == ETH2) || (gmac_priv->id == ETH3)) {
        cmd->supported |= SUPPORTED_1000baseT_Full;
    }
    cmd->advertising = cmd->supported;

    ret = gmac_ethtool_get_speed(dev, &speed);
    if (ret) {
        LOG(LOG_ERROR, "gmac(%u) get speed error", gmac_priv->id);
        return ret;
    }

    cmd->speed = speed;
    cmd->duplex = gmac_priv->oldduplex ? DUPLEX_FULL : DUPLEX_HALF;

    cmd->phy_address = (u8)gmac_priv->phy_addr;
    cmd->autoneg = (u8)gmac_priv->oldautoneg;
    return 0;
}
#endif

static int gmac_ethtool_phy_autoneg(struct net_device *dev)
{
    u32 ul_value;
    s32 ret;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    if ((gmac_priv->phy_handle == NULL) || (gmac_priv->phy_handle->read == NULL)) {
        return -EIO;
    }

    ret = gmac_priv->phy_handle->read(gmac_priv->phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, &ul_value);
    if (ret) {
        LOG(LOG_ERROR, "gmac%u read control reg error", gmac_priv->id);
        return ret;
    }
    if ((ul_value & 0xffff) == 0xffff) {
        LOG(LOG_ERROR, "gmac%u read control reg error", gmac_priv->id);
        return -EIO;
    }

    if ((gmac_priv->id == ETH0) || (gmac_priv->id == ETH1)) {
        ul_value |= (MASK_PHY_CTRL_ANENABLE | MASK_PHY_CTRL_ANRESTART);
        ul_value |= BIT(15); /* 15bit, soft-reset to take effect. */
        ul_value |= BIT(13); /* 13bit, config 100M. */
        ul_value |= BIT(8);  /* 8bit, config full duplex. */
    } else {
        ul_value |= (MASK_PHY_CTRL_ANENABLE | MASK_PHY_CTRL_ANRESTART);
    }

    if ((gmac_priv->phy_handle->write == NULL)) {
        return -EIO;
    }

    ret = gmac_priv->phy_handle->write(gmac_priv->phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, ul_value);
    if (ret != 0) {
        LOG(LOG_ERROR, "gmac%u write control reg error", gmac_priv->id);
        return ret;
    }

    return ret;
}

static int gmac_ethtool_phy_setmode(struct net_device *dev, u32 speed, u32 duplex)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);
    u32 bmcr = 0;
    u32 value;
    s32 ret;

    if (speed == SPEED_1000) {
        bmcr |= MASK_PHY_CTRL_SPEED_6;
    } else if (speed == SPEED_100) {
        bmcr |= MASK_PHY_CTRL_SPEED_13;
    }

    if (duplex == DUPLEX_FULL) {
        bmcr |= MASK_PHY_CTRL_DUPLEX;
    }

    if ((gmac_priv->phy_handle == NULL) || (gmac_priv->phy_handle->read == NULL)) {
        return -EIO;
    }

    ret = gmac_priv->phy_handle->read(gmac_priv->phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, &value);
    if (ret) {
        LOG(LOG_ERROR, "gmac%u read control reg error", gmac_priv->id);
        return ret;
    }

    if (value != bmcr) {
        if (gmac_priv->phy_handle->write == NULL) {
            return -EIO;
        }

        ret = gmac_priv->phy_handle->write(gmac_priv->phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, bmcr);
        if (ret) {
            LOG(LOG_ERROR, "gmac%u phy write failed", gmac_priv->id);
            return ret;
        }
    }

    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
static int gmac_ethtool_set_settings(struct net_device *dev,
                                     const struct ethtool_link_ksettings *cmd)
{
    gmac_private_s *priv = netdev_priv(dev);
    u8 autoneg = cmd->base.autoneg;
    u8 duplex = cmd->base.duplex;
    u32 speed = cmd->base.speed;

    if (autoneg != AUTONEG_ENABLE && autoneg != AUTONEG_DISABLE) {
        return -EINVAL;
    }

    if (autoneg == AUTONEG_DISABLE &&
        (((priv->id < ETH2 || speed != SPEED_1000) &&
            speed != SPEED_100 && speed != SPEED_10) ||
            (duplex != DUPLEX_HALF && duplex != DUPLEX_FULL))) {
                return -EINVAL;
            }

    priv->oldautoneg = cmd->base.autoneg;

    if (!netif_running(dev)) {
        return 0;
    }

    if (autoneg == AUTONEG_ENABLE) {
        gmac_ethtool_phy_autoneg(dev);
    } else {
        gmac_ethtool_phy_setmode(dev, speed, duplex);
    }

    return 0;
}
#else
static int gmac_ethtool_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    u32 check;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    check = cmd->autoneg != AUTONEG_ENABLE && cmd->autoneg != AUTONEG_DISABLE;
    if (check) {
        return -EINVAL;
    }

    check = AUTONEG_DISABLE == cmd->autoneg && cmd->duplex != DUPLEX_FULL && cmd->duplex != DUPLEX_HALF;
    if (check) {
        return -EINVAL;
    }

    if (cmd->autoneg == AUTONEG_ENABLE) {
        u32 mask = ADVERTISED_Autoneg | ADVERTISED_Pause | ADVERTISED_Asym_Pause;
        check = (ETH2 == gmac_priv->id) || (ETH3 == gmac_priv->id);
        if (check) {
            mask |= ADVERTISED_1000baseT_Full;
        }
        mask |= (ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full | ADVERTISED_10baseT_Half |
                 ADVERTISED_10baseT_Full | ADVERTISED_TP);
        if (cmd->advertising & ~mask) {
            return -EINVAL;
        }
        mask &= (ADVERTISED_1000baseT_Full | ADVERTISED_100baseT_Half | ADVERTISED_100baseT_Full |
                 ADVERTISED_10baseT_Half | ADVERTISED_10baseT_Full);
        cmd->advertising &= mask;
    } else {
        if (cmd->speed == SPEED_1000) {
            check = (gmac_priv->id != ETH2) && (gmac_priv->id != ETH3);
            if (check) {
                return -EINVAL;
            }
            if (cmd->duplex != DUPLEX_FULL) {
                return -EINVAL;
            }
        } else if ((cmd->speed != SPEED_10) && (cmd->speed != SPEED_100)) {
            return -EINVAL;
        }
    }

    /* When auto-negotiation is set, the status is saved. */
    gmac_priv->oldautoneg = cmd->autoneg;
    if (netif_running(dev)) {
        if (cmd->autoneg == AUTONEG_ENABLE) {
            (void)gmac_ethtool_phy_autoneg(dev);
        } else {
            (void)gmac_ethtool_phy_setmode(dev, cmd->speed, cmd->duplex);
        }
    }

    return 0;
}
#endif

static void gmac_ethtool_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
    (void)strncpy_s(info->driver, ETH_TOOL_VER_LEN, DRV_MODULE_NAME, sizeof(DRV_MODULE_NAME));
    (void)strncpy_s(info->version, ETH_TOOL_VER_LEN, DRV_MODULE_VERSION, sizeof(DRV_MODULE_VERSION));
    (void)strncpy_s(info->bus_info, ETH_TOOL_VER_LEN, "NA", sizeof("NA"));
    (void)strncpy_s(info->fw_version, ETH_TOOL_VER_LEN, "NA", sizeof("NA"));
}

static unsigned int gmac_ethtool_get_link(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);
    return (gmac_priv->oldlink) ? 1U : 0U;
}

static int gmac_ethtool_get_regs_len(struct net_device *dev)
{
    return GMAC_REGDUMP_LEN;
}

static void gmac_ethtool_get_regs(struct net_device *dev, struct ethtool_regs *regs, void *_p)
{
    u32 *p = (u32 *)_p;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);
    unsigned long base_addr = dev->base_addr;
    u8 *orig_p = (u8 *)_p;
    int i;

    regs->version = 0;
    (void)memset_s(p, GMAC_REGDUMP_LEN, 0, GMAC_REGDUMP_LEN);
    spin_lock(&gmac_priv->lock);

#define GET_REG32(reg) (*(p)++ = REG32(base_addr + (reg)))
#define GET_REG32_1(reg)            \
    do {                            \
        p = (u32 *)(orig_p + (reg)); \
        GET_REG32((reg));         \
    } while (0)
    for (i = 0; i < (0x0400); i += 4) { // offset 4
        GET_REG32_1((unsigned int)i);
    }
    for (i = (0x0400); i < (0x0600); i += 4) { // offset 4
        GET_REG32_1((unsigned int)i);
    }
#undef GET_REG32
#undef GET_REG32_1

    spin_unlock(&gmac_priv->lock);
}

static void gmac_ethtool_get_strings(struct net_device *dev, u32 stringset, u8 *buf)
{
    switch (stringset) {
        case ETH_SS_STATS:
            (void)memcpy_s(buf, sizeof(ethtool_stats_keys), ethtool_stats_keys, sizeof(ethtool_stats_keys));
            break;
        default:
            WARN_ON(1); /* we need a WARN() */
            break;
    }
}

static int gmac_ethtool_get_sset_count(struct net_device *dev, int sset)
{
    switch (sset) {
        case ETH_SS_STATS:
            return GMAC_NUM_STATS;
        default:
            return -EOPNOTSUPP;
    }
}

static void gmac_ethtool_get_estats_rx_cnt(unsigned long base_addr, struct gmac_ethtool_stats *pges)
{
    u64 read_val = 0;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_UC_PKTS_OFFSET_ADDR), read_val);
    pges->rx_uc_pkts_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_MC_PKTS_OFFSET_ADDR), read_val);
    pges->rx_mc_pkts_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_BC_PKTS_OFFSET_ADDR), read_val);
    pges->rx_bc_pkts_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_TAGGED_OFFSET_ADDR), read_val);
    pges->rx_vlan_pkt_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PAUSE_MACCONTROL_FRAMCOUNTER_OFFSET_ADDR), read_val);
    pges->rx_fw_ctrl_frame_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_OCTETS_TOTAL_OK_OFFSET_ADDR), read_val);
    pges->rx_octets_total_ok_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_OCTETS_BAD_OFFSET_ADDR), read_val);
    pges->rx_octets_bad_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_OCTETS_TOTAL_FILT_OFFSET_ADDR), read_val);
    pges->rx_octets_total_filt_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_FILT_PKT_CNT_OFFSET_ADDR), read_val);
    pges->rx_filt_pkt_cnt += read_val;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_64OCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_64 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_65TO127OCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_65_127 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_128TO255OCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_128_255 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_255TO511OCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_256_511 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_512TO1023OCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_512_1023 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_1024TO1518OCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_1024_1518 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_PKTS_1519TOMAXOCTETS_OFFSET_ADDR), read_val);
    pges->rx_framsize_bt_1518 += read_val;

    return;
}

static void gmac_ethtool_get_estats_rx_errcnt(unsigned long base_addr, struct gmac_ethtool_stats *pges)
{
    u64 read_val = 0;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_LONG_ERRORS_OFFSET_ADDR), read_val);
    pges->rx_frame_long_err_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_VERY_LONG_ERR_CNT_OFFSET_ADDR), read_val);
    pges->rx_frame_very_long_err_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_RUNT_ERR_CNT_OFFSET_ADDR), read_val);
    pges->rx_frame_runt_err_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_SHORT_ERR_CNT_OFFSET_ADDR), read_val);
    pges->rx_frame_short_err_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_OVERRUN_CNT_OFFSET_ADDR), read_val);
    pges->rx_overrun_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_FCS_ERRORS_OFFSET_ADDR), read_val);
    pges->rx_fcs_error_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_DATA_ERR_OFFSET_ADDR), read_val);
    pges->rx_data_error_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_RX_ALIGN_ERRORS_OFFSET_ADDR), read_val);
    pges->rx_align_error_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_RX_BUFRQ_ERR_CNT_OFFSET_ADDR), read_val);
    pges->rx_rbuff_rq_err_cnt += read_val;

    return;
}

static void gmac_ethtool_get_estats_tx_cnt(unsigned long base_addr, struct gmac_ethtool_stats *pges)
{
    u64 read_val = 0;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_UC_PKTS_OFFSET_ADDR), read_val);
    pges->tx_uc_pkts_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_MC_PKTS_OFFSET_ADDR), read_val);
    pges->tx_mc_pkts_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_BC_PKTS_OFFSET_ADDR), read_val);
    pges->tx_bc_pkts_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_TAGGED_OFFSET_ADDR), read_val);
    pges->tx_vlan_pkt_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PAUSE_FRAMES_OFFSET_ADDR), read_val);
    pges->tx_fw_ctrl_frame_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_OCTETS_TRANSMITTED_OK_OFFSET_ADDR), read_val);
    pges->tx_octets_total_ok_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_OCTETS_TRANSMITTED_BAD_OFFSET_ADDR), read_val);
    pges->tx_octets_bad_cnt += read_val;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_64OCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_64 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_65TO127OCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_65_127 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_128TO255OCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_128_255 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_255TO511OCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_256_511 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_512TO1023OCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_512_1023 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_1024TO1518OCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_1024_1518 += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_PKTS_1519TOMAXOCTETS_OFFSET_ADDR), read_val);
    pges->tx_framsize_bt_1518 += read_val;

    return;
}

static void gmac_ethtool_get_estats_tx_errcnt(unsigned long base_addr, struct gmac_ethtool_stats *pges)
{
    u64 read_val = 0;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_CRC_ERROR_OFFSET_ADDR), read_val);
    pges->tx_fcs_error_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_UNDERRUN_OFFSET_ADDR), read_val);
    pges->tx_underrun_err_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_TX_CS_FAIL_CNT_OFFSET_ADDR), read_val);
    pges->tx_add_cs_fail_cnt += read_val;

    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_TX_BUFRL_ERR_CNT_OFFSET_ADDR), read_val);
    pges->tx_buffrl_err_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_RX_WE_ERR_CNT_OFFSET_ADDR), read_val);
    pges->tx_we_err_cnt += read_val;

    return;
}

static void gmac_ethtool_get_estats_rx_drpcnt(
    unsigned long base_addr, gmac_private_s *gmac_priv, struct gmac_ethtool_stats *pges)
{
    u64 read_val = 0;

    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_RX_OVER_FLOW_CNT_OFFSET_ADDR), read_val);
    pges->rx_overrun_drop_cnt += read_val;
    pges->rx_overrun_drop_cnt += gmac_priv->drop_count;
    gmac_priv->drop_count = 0;

    return;
}

static void gmac_ethtool_get_estats_tx_drpcnt(unsigned long base_addr, struct gmac_ethtool_stats *pges)
{
    u64 read_val = 0;

    gmac_reg_read((uintptr_t)(base_addr + GMAC_CSR_TX_EXCESSIVE_LENGTH_DROP_OFFSET_ADDR), read_val);
    pges->tx_excessive_drop_cnt += read_val;
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_TX_DROP_CNT_OFFSET_ADDR), read_val);
    pges->tx_overrun_drop_cnt += read_val;

    return;
}

struct gmac_ethtool_stats *gmac_ethtool_get_estats(gmac_private_s *gmac_priv)
{
    struct gmac_ethtool_stats *pges = &gmac_priv->ges;
    unsigned long base_addr = g_gmac_dev->mac_devs[gmac_priv->id]->base_addr;

    /* RX COUNT */
    gmac_ethtool_get_estats_rx_cnt(base_addr, pges);

    /* RX ERROR COUNT */
    gmac_ethtool_get_estats_rx_errcnt(base_addr, pges);

    /* TX COUNT */
    gmac_ethtool_get_estats_tx_cnt(base_addr, pges);

    /* TX ERROR COUNT */
    gmac_ethtool_get_estats_tx_errcnt(base_addr, pges);

    /* RX DROP COUNT */
    gmac_ethtool_get_estats_rx_drpcnt(base_addr, gmac_priv, pges);

    /* TX DROP COUNT */
    gmac_ethtool_get_estats_tx_drpcnt(base_addr, pges);

    return pges;
}

static void gmac_ethtool_get_stats(struct net_device *dev, struct ethtool_stats *estats, u64 *tmp_stats)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);
    (void)memcpy_s(tmp_stats, sizeof(gmac_priv->ges), gmac_ethtool_get_estats(gmac_priv), sizeof(gmac_priv->ges));
}

static void gmac_ethtool_get_pauseparam(struct net_device *net_dev, struct ethtool_pauseparam *param)
{
    (void)gmac_hw_get_pause_en(net_dev, &param->tx_pause, &param->rx_pause);

    return;
}

static int gmac_ethtool_set_pauseparam(struct net_device *net_dev, struct ethtool_pauseparam *param)
{
    return gmac_hw_set_pause_en(net_dev, !!param->tx_pause, !!param->rx_pause);
}

static s32 gmac_ethtool_nway_reset(struct net_device *net_dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    if (!netif_running(net_dev)) {
        return 0;
    }

    if (gmac_priv->oldautoneg != AUTONEG_ENABLE) {
        return -EINVAL;
    }

    if ((gmac_priv->phy_handle) && (gmac_priv->phy_handle->restart_autoneg)) {
        return gmac_priv->phy_handle->restart_autoneg(gmac_priv->phy_addr);
    }

    return 0;
}

struct ethtool_ops g_gmac_ethtool_ops = {
    .ETHTOOL_GET_SETTING = gmac_ethtool_get_settings,
    .ETHTOOL_SET_SETTING = gmac_ethtool_set_settings,
    .get_pauseparam = gmac_ethtool_get_pauseparam,
    .set_pauseparam = gmac_ethtool_set_pauseparam,
    .get_drvinfo = gmac_ethtool_get_drvinfo,
    .get_link = gmac_ethtool_get_link,
    .get_regs_len = gmac_ethtool_get_regs_len,
    .get_regs = gmac_ethtool_get_regs,
    .get_strings = gmac_ethtool_get_strings,
    .get_sset_count = gmac_ethtool_get_sset_count,
    .get_ethtool_stats = gmac_ethtool_get_stats,
    .nway_reset = gmac_ethtool_nway_reset,
};
