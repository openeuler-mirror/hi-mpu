// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gamc driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/jiffies.h>
#include "gmac_csr_addr_define.h"
#include "pcu_csr_addr_define.h"
#include "gmac_c_union_define.h"
#include "pcu_c_union_define.h"
#include "comm_knl_api.h"
#include "com_phy.h"
#include "gmac_hw.h"
#include "eth_tx.h"
#include "eth_rx.h"
#include "ethtool.h"
#include "gmac.h"

struct gmac_phy_info g_hi_gmac_phy_info_table[] = {
    /*
     * The YT PHY must be placed in the front，which can prevent the YT ext0 register
     * being incorrectly modified due to page 0 switching during PHY ID detection.
     */
    {
        "YT8521",
        IDENTIFIER_YT8521,
        &g_phy_yt8521_ops,
    },

    {
        "YT8512",
        IDENTIFIER_YT8512,
        &g_phy_yt8512_ops,
    },

    {
        "RTL8201",
        IDENTIFIER_RELTEK8201,
        &g_phy_8201_ops,
    },

    {
        "MAR1512",
        IDENTIFIER_MARVELL1512,
        &g_phy_1512_ops,
    },

    {
        "RTL8211",
        IDENTIFIER_RELTEK8211,
        &g_phy_8211_ops,
    },

};

static gmac_devs_s g_gmac_devices;
static struct timer_list g_gmac_rx_timer;
const gmac_devs_s *g_gmac_dev = NULL;
static struct workqueue_struct *g_gmac_wq = NULL;
static struct delayed_work g_gmac_delayed_work;

u32 g_rx_mode = 0;
module_param(g_rx_mode, uint, 0600);
MODULE_PARM_DESC(g_rx_mode, "g_rx_mode, 0:RX irq mode, 1:RX Timer mode");

const intr_disp_info_s g_gmac_err_src[] = {
    { BIT_MASK(20), 0, 0,           NULL },
    { BIT_MASK(21), 0, IRQ_HANDLED, gmac_interrupt_handle_tx_ahb_err },
    { BIT_MASK(22), 0, IRQ_HANDLED, gmac_interrupt_handle_buf_avl },
    { BIT_MASK(23), 0, IRQ_HANDLED, gmac_interrupt_handle_tx_release_err },
    { BIT_MASK(24), 0, IRQ_HANDLED, gmac_interrupt_handle_tx_bd },
    { BIT_MASK(25), 0, IRQ_HANDLED, gmac_interrupt_handle_tx_drop_err },
    { BIT_MASK(26), 0, IRQ_HANDLED, gmac_interrupt_handle_rx_drop_err },
    { BIT_MASK(27), 0, IRQ_HANDLED, gmac_interrupt_handle_rx_intrpt },
    { BIT_MASK(28), 0, IRQ_HANDLED, gmac_interrupt_handle_rx_ahb_err },
    { BIT_MASK(29), 0, IRQ_HANDLED, gmac_interrupt_handle_mac_fifo_err },
    { BIT_MASK(30), 0, IRQ_HANDLED, gmac_interrupt_handle_rbreq_err },
    { BIT_MASK(31), 0, IRQ_HANDLED, gmac_interrupt_handle_we_err },
};

static s32 gmac_change_mtu(struct net_device *net_dev, s32 new_mtu)
{
    u32 old_mtu;
    u32 frm_len;

    /* new_mtu represent the length of the IP data,and support double vlan (vlan * 2) \
       so need to add head len when config frame len. if increase head len \
       need to modify the MAX_FRAME_SIZE definition */
    frm_len = new_mtu + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN * 2;

    /* The minimum MTU value is 256. */
    if (NEW_MTU_CHECK) {
        return -EINVAL;
    }

    if (new_mtu == net_dev->mtu) {
        return 0;
    }

    /* Need to consider when running data flow configuration mtu */
    old_mtu = gmac_hw_get_max_frame_len(net_dev->base_addr);

    /* Need stop tx&rx traffic when configure MTU */
    if (gmac_hw_set_max_frame_len(net_dev->base_addr, frm_len)) {
        return -EIO;
    }

    if (new_mtu < DEFAULT_MTU_SIZE) {
        frm_len = DEFAULT_MTU_SIZE + ETH_HLEN + ETH_FCS_LEN + VLAN_HLEN * 2; /* 2:double vlan */
    }

    if (gmac_hw_set_rx_max_frame_len(net_dev->base_addr, frm_len)) {
        /* Rollback TX MTU setting */
        (void)gmac_hw_set_max_frame_len(net_dev->base_addr, old_mtu);
        return -EIO;
    }

    net_dev->mtu = new_mtu;

    return 0;
}

static s32 gmac_set_features(struct net_device *net_dev,
                             netdev_features_t features)
{
    /* Need to consider the case that do not support the features */
    net_dev->features = features;
    return 0;
}

const struct net_device_ops g_gmac_netdev_ops = {
    .ndo_open = gmac_open,
    .ndo_stop = gmac_stop,
    .ndo_start_xmit = gmac_eth_tx,
    .ndo_change_mtu = gmac_change_mtu,
    .ndo_validate_addr = eth_validate_addr,
    .ndo_tx_timeout = gmac_tx_timeout,
    .ndo_set_mac_address = gmac_set_mac_addr,
    .ndo_do_ioctl = gmac_ioctl,
    .ndo_set_features = gmac_set_features,
};

static void gmac_txrx_timer_handle(struct timer_list *tl)
{
    u8 i;
    struct net_device *net_dev = NULL;
    gmac_private_s *gmac_priv = NULL;
    u_cf_intrpt_msk int_mask;
    unsigned long flags;

    for (i = ETH2; i < MAX_MAC_PORT_NUM; i++) {
        net_dev = g_gmac_dev->mac_devs[i];
        gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
        if (!gmac_priv->opened) {
            continue;
        }

        /* Enable rx interrupt */
        spin_lock_irqsave(&gmac_priv->mac_lock, flags);
        gmac_reg_read((uintptr_t)(net_dev->base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), int_mask.val);
        int_mask.bits.cf_pkg_intrpt_msk = 1;
        gmac_reg_write((uintptr_t)(net_dev->base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), int_mask.val);
        spin_unlock_irqrestore(&gmac_priv->mac_lock, flags);
    }

    g_gmac_rx_timer.expires = (unsigned long)(jiffies_64 + msecs_to_jiffies(RX_TIMER_BLANK));
    add_timer(&g_gmac_rx_timer);

    return;
}

void gmac_rx_timer_close(void)
{
    (void)del_timer_sync(&g_gmac_rx_timer);
}

static void gmac_rx_timer_init(void)
{
    timer_setup(&g_gmac_rx_timer, gmac_txrx_timer_handle, 0);

    g_gmac_rx_timer.expires = (unsigned long)(jiffies_64 + msecs_to_jiffies(RX_TIMER_BLANK));
    add_timer(&g_gmac_rx_timer);
}

s32 gmac_check_speed_available(u32 id, u32 speed)
{
    s32 ret = 0;

    switch (id) {
        case ETH0:
        case ETH1:
            if ((speed != MAC_100M_MII) && (speed != MAC_10M_MII)) {
                ret = -EINVAL;
            }
            break;
        case ETH2:
        case ETH3:
            if ((speed != MAC_10M_SGMII) && (speed != MAC_100M_SGMII) && (speed != MAC_1000M_SGMII)) {
                ret = -EINVAL;
            }
            break;
        default:
            ret = -EINVAL;
            break;
    }

    return ret;
}

s32 gmac_adjustlink(u32 id, u32 speed, u32 duplex, u32 link)
{
    struct net_device *net_dev = NULL;
    gmac_private_s *gmac_priv = NULL;
    unsigned long base_addr;
    s32 ret;

    if (duplex > GMAC_DUPLEX_FULL) {
        return -EINVAL;
    }

    if (gmac_check_speed_available(id, speed)) {
        LOG(LOG_ERROR, "speed(%u) is invalid!(eth%u)", speed, id);
        return -EINVAL;
    }

    net_dev = g_gmac_dev->mac_devs[id];
    base_addr = net_dev->base_addr;
    gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    spin_lock(&gmac_priv->lock);
    ret = gmac_hw_set_port_mode(net_dev, speed, duplex);
    spin_unlock(&gmac_priv->lock);

    if (link && netif_running(net_dev)) {
        /* Configuring the enabled port */
        gmac_hw_rx_enable(base_addr, 1);
        gmac_hw_tx_enable(base_addr, 1);
    }

    return ret;
}

s32 gmac_get_link(u32 gmac_id, u32 *link)
{
    gmac_private_s *gmac_priv = NULL;
    struct net_device *net_dev = NULL;

    net_dev = g_gmac_dev->mac_devs[gmac_id];
    gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    if ((gmac_priv->phy_handle == NULL) || (gmac_priv->phy_handle->link == NULL)) {
        return -EINVAL;
    }

    return gmac_priv->phy_handle->link(gmac_priv->phy_addr, link);
}

s32 gmac_get_phy_speed_duplex(u32 gmac_id, u32 *speed, u32 *duplex)
{
    gmac_private_s *gmac_priv = NULL;
    struct net_device *net_dev = NULL;

    net_dev = g_gmac_dev->mac_devs[gmac_id];
    gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    if ((gmac_priv->phy_handle == NULL) || (gmac_priv->phy_handle->get_speed_duplex == NULL)) {
        return -EINVAL;
    }

    return gmac_priv->phy_handle->get_speed_duplex(gmac_priv->phy_addr, speed, duplex);
}

s32 phy1512_detect_recover_fiber_link(struct net_device *net_dev, gmac_private_s *gmac_priv)
{
    u32 value;
    u32 fiber_link;
    u32 count = 0;
    u_an_neg_state an_neg_state;
    int detect_fiber_link = 0;
    
    do {
        detect_fiber_link = 0;

        if (gmac_priv->phy_handle->read(gmac_priv->phy_addr, FIBER_1512_STATUS, FIBER_1512_PAGE, &fiber_link) != 0) {
            return -EIO;
        }

        if ((fiber_link & 0xffff) == 0xffff) {
            return -EIO;
        }

        gmac_reg_read((uintptr_t)(net_dev->base_addr + GMAC_CSR_AN_NEG_STATE_OFFSET_ADDR), an_neg_state.val);

        /*  Determine if auto-recovery measures are enabled. */
        if ((an_neg_state.bits.an_done == 0) && ((fiber_link & (1UL << BIT_SPECIFIC_FIBER_1512_LINK)) == 0)) {
            gmac_priv->phy_handle->read(gmac_priv->phy_addr, FIBER_1512_CONTROL, FIBER_1512_PAGE, &value);
            value |= (1UL << BIT_FIBER_1512_POWER_DOWN);
            gmac_priv->phy_handle->write(gmac_priv->phy_addr, FIBER_1512_CONTROL, FIBER_1512_PAGE, value);

            udelay(100);

            gmac_priv->phy_handle->read(gmac_priv->phy_addr, FIBER_1512_CONTROL, FIBER_1512_PAGE, &value);
            value &= ~(1UL << BIT_FIBER_1512_POWER_DOWN);
            gmac_priv->phy_handle->write(gmac_priv->phy_addr, FIBER_1512_CONTROL, FIBER_1512_PAGE, value);

            msleep(100);

            count++;

            if (count >= FIBER_1512_REPAIR_MAX_TIMES) {
                LOG(LOG_PRINT, "writing phy register to recovery failed, set bmc_reset flag!!");
                return 0;
            }

            detect_fiber_link = 1;
        }
    } while (detect_fiber_link != 0);

    return 0;
}

s32 phy_detect_recovery_fiber_link(struct net_device *net_dev, gmac_private_s *gmac_priv)
{
    u32 phy_id;

    if ((gmac_priv->phy_handle == NULL) || (gmac_priv->phy_handle->read == NULL)
        || (gmac_priv->phy_handle->write == NULL)) {
        return -EINVAL;
    }

    gmac_priv->phy_handle->get_phy_id(gmac_priv->phy_addr, &phy_id);
    if (phy_id != IDENTIFIER_MARVELL1512) {
        return 0;
    } else {
        return phy1512_detect_recover_fiber_link(net_dev, gmac_priv);
    }
}

void check_sgmii_neg_state(struct net_device *net_dev, u32 gmac_id, u32 phy_link)
{
    u_an_neg_state an_neg_state;

    if ((gmac_id == ETH2) || (gmac_id == ETH3)) {
        gmac_reg_read((uintptr_t)(net_dev->base_addr + GMAC_CSR_AN_NEG_STATE_OFFSET_ADDR), an_neg_state.val);
        if (an_neg_state.bits.an_done == 0) {
            LOG(LOG_INFO, "Warning: SGMII auto neg failed");
        }

        if ((phy_link == PHY_LINK_UP) && (an_neg_state.bits.np_link_ok == 0)) {
            LOG(LOG_INFO, "PHY link failed");
        }
    }

    return;
}

static s32 gmac_link_handle(u32 id)
{
    s32 ret;
    u32 link = 0;
    u32 speed = 0;
    u32 duplex = 0;
    gmac_private_s *gmac_priv = NULL;
    struct net_device *net_dev = NULL;

    if (g_gmac_dev->mac_devs[id] == NULL) {
        return 0;
    }
    net_dev = g_gmac_dev->mac_devs[id];
    gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    ret = gmac_get_link(id, &link);
    if (ret) {
        return ret;
    }

    ret = gmac_get_phy_speed_duplex(id, &speed, &duplex);
    if (ret) {
        return ret;
    }

    if (gmac_priv->oldlink != link) {
        if (link) {
            (void)gmac_adjustlink(id, speed, duplex, link);
            netif_carrier_on(net_dev);
            gmac_priv->update_jiffies = get_jiffies_64();
            LOG(LOG_PRINT, "%s link up!", net_dev->name);
        } else {
            netif_carrier_off(net_dev);
            LOG(LOG_PRINT, "%s link down!", net_dev->name);
        }
    }

    if (link) {
        if (((u32)gmac_priv->oldspeed != speed) || ((u32)gmac_priv->oldduplex != duplex)) {
            (void)gmac_adjustlink(id, speed, duplex, link);
        }

        (void)phy_detect_recovery_fiber_link(net_dev, gmac_priv);
    }

    gmac_priv->oldlink = (s32)link;
    gmac_priv->oldspeed = (s32)speed;
    gmac_priv->oldduplex = (s32)duplex;

    check_sgmii_neg_state(net_dev, id, gmac_priv->oldlink);

    return 0;
}

static void gmac_update_txrx_stats(u32 gmac_id)
{
    struct net_device *net_dev = NULL;
    gmac_private_s *gmac_priv = NULL;
    u64 cur_jiffies;

    net_dev = g_gmac_dev->mac_devs[gmac_id];
    if (net_dev == NULL) {
        return;
    }

    gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    cur_jiffies = get_jiffies_64();
    if (gmac_priv->oldlink && time_after64(cur_jiffies, gmac_priv->update_jiffies + HZ * 18)) { // HZ mul 18
        gmac_ethtool_get_estats(gmac_priv);
        gmac_priv->update_jiffies = get_jiffies_64();
    }

    return;
}

static void gmac_delaywork_handle(struct work_struct *work)
{
    u32 gmac_id;

    for (gmac_id = 0; gmac_id < MAX_MAC_PORT_NUM; gmac_id++) {
        (void)gmac_link_handle(gmac_id);
        gmac_update_txrx_stats(gmac_id);
    }

    (void)queue_delayed_work(g_gmac_wq, &g_gmac_delayed_work, msecs_to_jiffies(GMAC_DELAYED_WORK_TIMEOUT));

    return;
}

s32 gmac_delaywork_init(void)
{
    g_gmac_wq = create_workqueue("gmac_queue");
    INIT_DELAYED_WORK(&g_gmac_delayed_work, gmac_delaywork_handle);
    (void)queue_delayed_work(g_gmac_wq, &g_gmac_delayed_work, msecs_to_jiffies(GMAC_DELAYED_WORK_TIMEOUT));

    return 0;
}

s32 gmac_reset_chip(struct net_device *net_dev, u32 mac_num)
{
    gmac_hw_hp_clock_enable(mac_num);
    gmac_hw_rx_enable(net_dev->base_addr, DISABLE);
    gmac_hw_tx_enable(net_dev->base_addr, ENABLE);
    gmac_hw_set_bmu_close(net_dev);

    mdelay(1000);

    (void)gmac_hw_ptp_reset(net_dev->base_addr);
    (void)gmac_hw_reset(net_dev->base_addr, mac_num);

    return GMAC_OK;
}

static void gmac_chip_init_default_mac_set(const struct net_device *dev, s32 index)
{
    dev->dev_addr[0] = 0x08;        /* mac addr[0] is 0x08 */
    dev->dev_addr[1] = (u8)index;   /* mac addr[1] changes widh index */
    dev->dev_addr[2] = 0x08;        /* mac addr[2] is 0x08 */
    dev->dev_addr[3] = 0x08;        /* mac addr[3] is 0x08 */
    dev->dev_addr[4] = 0x08;        /* mac addr[4] is 0x08 */
    dev->dev_addr[5] = 0x08;        /* mac addr[5] is 0x08 */

    return;
}

static void gmac_chip_init_mac_config(const struct net_device *dev)
{
    u32 mac_addr_low;
    u32 mac_addr_high;
    unsigned long base_addr = dev->base_addr;

    mac_addr_high = (dev->dev_addr[0] << 8) | (dev->dev_addr[1]); /* dev_addr[0]: bit8~bit15 */
    mac_addr_low = ((u32)(dev->dev_addr[2]) << 24)   /* dev_addr[2]: bit24~bit31 */
                   | ((u32)(dev->dev_addr[3]) << 16) /* dev_addr[3]: bit16~bit23 */
                   | ((u32)(dev->dev_addr[4]) << 8)  /* dev_addr[4]: bit8~bit15 */
                   | ((u32)(dev->dev_addr[5]));      /* dev_addr[5]: bit0~bit7 */

    gmac_reg_write((uintptr_t)(base_addr + GMAC_CSR_STATION_ADDR_LOW_2_OFFSET_ADDR), mac_addr_low);
    gmac_reg_write((uintptr_t)(base_addr + GMAC_CSR_STATION_ADDR_HIGH_2_OFFSET_ADDR), mac_addr_high);
    gmac_reg_write((uintptr_t)(base_addr + GMAC_CSR_FD_FC_ADDR_LOW_OFFSET_ADDR), mac_addr_low);
    gmac_reg_write((uintptr_t)(base_addr + GMAC_CSR_FD_FC_ADDR_HIGH_OFFSET_ADDR), mac_addr_high);

    return;
}

static s32 gmac_chip_init_pkt_store_format(unsigned long base_addr, unsigned int id)
{
    gmac_pkt_store_ctrl_cfg_s pkt_store_ctrl;

    /*
     * Initialize the packet storage format parameters and buffer obtaining mode in the buff
     * based on the port number.(PCU_CSR_RX_CTRL_OFFSET_ADDR)
     */
    pkt_store_ctrl.cf_buf_1st_skip1 = GMAC_RX_SKIP1;
    pkt_store_ctrl.cf_buf_n1st_skip = 0;
    pkt_store_ctrl.cf_cache_line = 0;
    pkt_store_ctrl.cf_rxbuf_1st_skip_size2 = GMAC_RX_SKIP2;
    pkt_store_ctrl.cf_rx_align_num = NET_IP_ALIGN;

    return gmac_hw_set_pkt_store_format(base_addr, &pkt_store_ctrl, id);
}

static s32 gmac_chip_init_input_parse_mode(unsigned long base_addr)
{
    gmac_rx_pkt_mode_cfg_s rx_pkt_mode;

    /* Set the packet input mode and parsing mode. */
    rx_pkt_mode.cf_instr_head_mode = 0;
    rx_pkt_mode.cf_skip_len = 0;
    rx_pkt_mode.cf_parse_mode = 1;
    rx_pkt_mode.mac_skip_len = 0;

    return gmac_hw_set_pkt_input_parse_mode(base_addr, &rx_pkt_mode);
}

s32 gmac_chip_init1(const struct net_device *dev, s32 index)
{
    unsigned long base_addr = dev->base_addr;
    s32 ret;

    gmac_chip_init_default_mac_set(dev, index);

    gmac_chip_init_mac_config(dev);

    gmac_hw_txrx_fifo_thrsld_init(base_addr);
    gmac_hw_tx_flow_ctrl_config(base_addr);

    ret = gmac_chip_init_pkt_store_format(base_addr, index);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_pkt_store_format fail");
        return ret;
    }

    ret = gmac_chip_init_input_parse_mode(base_addr);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_pkt_input_parse_mode fail");
        return ret;
    }

    /* Set cf_endian to 3, control word to big endian, and data to big endian. */
    ret = gmac_hw_set_bus_ctrl(base_addr, 3);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_bus_ctrl fail");
        return ret;
    }

    ret = gmac_hw_set_rx_max_frame_len(base_addr, DEFUALT_MAX_FRAEM_LEN);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_rx_max_frame_len fail");
        return ret;
    }

    ret = gmac_hw_set_max_frame_len(base_addr, DEFUALT_MAX_FRAEM_LEN);
    if (ret) {
        LOG(LOG_ERROR, "GMAC MaxFrameLenSet fail");
        return ret;
    }

    /* Set the minimum frame length to 31. */
    ret = gmac_hw_set_short_frame_len(base_addr, 31);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_short_frame_len fail");
        return ret;
    }

    return 0;
}

s32 gmac_chip_init2(const struct net_device *dev, s32 index)
{
    unsigned long base_addr = dev->base_addr;
    s32 ret;

    ret = gmac_hw_set_tx_add_crc(base_addr, 1);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_tx_add_crc fail");
        return ret;
    }

    ret = gmac_hw_set_rx_strip_crc(base_addr, 1);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_rx_strip_crc fail");
        return ret;
    }

    ret = gmac_hw_set_rx_strip_pad(base_addr, 1);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_rx_strip_pad fail");
        return ret;
    }

    ret = gmac_hw_set_rx_short_frame_recv(base_addr, 1);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_rx_short_frame_recv fail");
        return ret;
    }

    ret = gmac_hw_set_port_mode_all(dev, index);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hw_set_port_mode_all failed");
        return ret;
    }

    /* Configure the address extension function to map the 32-bit address to the 34-bit address. */
    ret = gmac_hw_haddr_extern_config(index);
    if (ret) {
        LOG(LOG_ERROR, "gmac%d addr extern config failed!", index);
        return ret;
    }

    return 0;
}

s32 gmac_chip_init(const struct net_device *dev, s32 index)
{
    s32 ret;

    ret = gmac_chip_init1(dev, index);
    if (ret) {
        LOG(LOG_ERROR, "gmac_chip_init1 fail");
        return ret;
    }

    ret = gmac_chip_init2(dev, index);
    if (ret) {
        LOG(LOG_ERROR, "gmac_chip_init2 fail");
        return ret;
    }

    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
void gmac_tx_timeout(struct net_device *net_dev, u32 txqueue)
#else
void gmac_tx_timeout(struct net_device *net_dev)
#endif
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    gmac_priv->status.tx_errors++;
    netif_wake_queue(net_dev);

    return;
}

void gmac_interrupt_handle_rx_intrpt(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    /* Schedule NAPI Packet Receiving */
    napi_schedule(&gmac_priv->napi);

    return;
}

void gmac_interrupt_handle_buf_avl(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    gmac_priv->status.rx_fifo_errors++;

    return;
}

void gmac_interrupt_handle_tx_bd(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    tasklet_schedule(&gmac_priv->tx_free_tasklet);
    return;
}

void gmac_interrupt_handle_mac_fifo_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x) handle mac_fifo_err!", gmac_priv->id);

    (void)gmac_hw_core_reset(gmac_priv->id);
    return;
}

void gmac_interrupt_handle_tx_ahb_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Tx direction bus error", gmac_priv->id);
    return;
}

void gmac_interrupt_handle_tx_release_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Tx direction free BMU failed", gmac_priv->id);
    return;
}

void gmac_interrupt_handle_tx_drop_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Tx direction frame droped error", gmac_priv->id);
    return;
}

void gmac_interrupt_handle_rx_drop_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Rx direction frame droped error", gmac_priv->id);
    return;
}

void gmac_interrupt_handle_rx_ahb_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Rx direction bus error", gmac_priv->id);
    return;
}

void gmac_interrupt_handle_rbreq_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Alloc rx mem failed", gmac_priv->id);
    return;
}

void gmac_interrupt_handle_we_err(struct net_device *dev)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(dev);

    LOG(LOG_ERROR, "gmac(0x%x): Write work desc failed", gmac_priv->id);
    return;
}

irqreturn_t gmac_interrupt_handle(s32 irq, void *dev_id)
{
    struct net_device *net_dev = (struct net_device *)dev_id;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    u_cf_intrpt_stat intr_status;
    u_cf_intrpt_msk intr_mask;
    u_cf_intrpt_clr intr_clr;
    irqreturn_t irq_ret = IRQ_NONE;
    unsigned long base_addr = net_dev->base_addr;
    unsigned long flags;
    u32 i;

    gmac_reg_read((base_addr + PCU_CSR_CF_INTRPT_STAT_OFFSET_ADDR), intr_status.val);
    gmac_reg_read((base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), intr_mask.val);

    for (i = 0; i < sizeof(g_gmac_err_src) / sizeof(intr_disp_info_s); i++) {
        if ((intr_mask.val & g_gmac_err_src[i].intr_mask)
            && (intr_status.val & g_gmac_err_src[i].intr_mask)) {
            spin_lock_irqsave(&gmac_priv->mac_lock, flags);
            intr_mask.val = intr_mask.val & (~g_gmac_err_src[i].intr_mask);
            gmac_reg_write((base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), intr_mask.val);
            spin_unlock_irqrestore(&gmac_priv->mac_lock, flags);

            intr_clr.val = g_gmac_err_src[i].intr_mask;
            gmac_reg_write((base_addr + PCU_CSR_CF_INTRPT_CLR_OFFSET_ADDR), intr_clr.val);

            mb();

            if (g_gmac_err_src[i].intr_proc != NULL) {
                g_gmac_err_src[i].intr_proc(net_dev);
            }

            if ((g_gmac_err_src[i].intr_mask == BIT_MASK(27)) || /* bit27 rx interrupt */
                (g_gmac_err_src[i].intr_mask == BIT_MASK(24))) { /* bit24 tx waterline interupt */
                continue;
            }

            spin_lock_irqsave(&gmac_priv->mac_lock, flags);
            gmac_reg_read((base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), intr_mask.val);
            intr_mask.val = intr_mask.val | g_gmac_err_src[i].intr_mask;
            gmac_reg_write((base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), intr_mask.val);
            spin_unlock_irqrestore(&gmac_priv->mac_lock, flags);
        }

        if (g_gmac_err_src[i].intr_ret != IRQ_NONE) {
            irq_ret = g_gmac_err_src[i].intr_ret;
        }
    }

    return irq_ret;
}

s32 gmac_set_mac_addr(struct net_device *net_dev, void *pst_ptr)
{
    struct sockaddr *pst_addr = (struct sockaddr *)pst_ptr;
    u32 ul_mac_addr_low;
    u32 ul_mac_addr_hig;
    u8 mac_clr_flag;

    ul_mac_addr_hig = ((u8)(pst_addr->sa_data[0]) << 8) | ((u8)(pst_addr->sa_data[1])); /* sa_data[0]: bit8~bit15 */
    ul_mac_addr_low = ((u8)(pst_addr->sa_data[2]) << 24)   /* sa_data[2]: bit24~bit31 */
                   | ((u8)(pst_addr->sa_data[3]) << 16) /* sa_data[3]: bit16~bit23 */
                   | ((u8)(pst_addr->sa_data[4]) << 8)  /* sa_data[4]: bit8~bit15 */
                   | ((u8)(pst_addr->sa_data[5]));      /* sa_data[5]: bit0~bit7 */

    /* Obtains the MAC address clearance flag in the user-mode program. */
    mac_clr_flag = (u8)pst_addr->sa_data[MAC_ADDR_SIZE];

    if ((!is_valid_ether_addr((u8 *)pst_addr->sa_data)) && (mac_clr_flag != 0xcf)) {
        (void)dev_err(&(net_dev->dev), " Invalid ether addr\n");
        return -EADDRNOTAVAIL;
    }

    gmac_reg_write((net_dev->base_addr + GMAC_CSR_STATION_ADDR_LOW_2_OFFSET_ADDR), ul_mac_addr_low);
    gmac_reg_write((net_dev->base_addr + GMAC_CSR_STATION_ADDR_HIGH_2_OFFSET_ADDR), ul_mac_addr_hig);
    gmac_reg_write((net_dev->base_addr + GMAC_CSR_FD_FC_ADDR_LOW_OFFSET_ADDR), ul_mac_addr_low);
    gmac_reg_write((net_dev->base_addr + GMAC_CSR_FD_FC_ADDR_HIGH_OFFSET_ADDR), ul_mac_addr_hig);

    /* Write the MAC address into the net_device structure. */
    (void)memcpy_s(net_dev->dev_addr, net_dev->addr_len, pst_addr->sa_data, net_dev->addr_len);

    return GMAC_OK;
}

s32 gmac_open(struct net_device *net_dev)
{
    unsigned long base_addr = net_dev->base_addr;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    /* if phy link is not ready, wait for 1s.
     * fix bug: ifconfig up just after insmod .ko, the tx/rx may be not enabled.
     * to make sure link update done, the driver sleep longer than timer_func. */
    if (!gmac_priv->oldlink) {
        msleep(GMAC_DELAYED_WORK_TIMEOUT + 1);
    }

    napi_enable(&gmac_priv->napi);
    tasklet_enable(&gmac_priv->tx_free_tasklet);

    gmac_hw_enable_interrupt(gmac_priv->id, base_addr);
    gmac_hw_tx_enable(base_addr, 1);
    gmac_hw_rx_enable(base_addr, 1);

    netif_start_queue(net_dev);

    gmac_priv->opened = 1;

    return 0;
}

s32 gmac_stop(struct net_device *net_dev)
{
    unsigned long base_addr = net_dev->base_addr;
    u_cf_intrpt_msk un_int_msk;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    gmac_priv->opened = 0;

    netif_stop_queue(net_dev);
    gmac_hw_tx_enable(base_addr, 0);
    gmac_hw_rx_enable(base_addr, 0);

    gmac_reg_read((base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), un_int_msk.val);
    un_int_msk.bits.cf_pkg_intrpt_msk = 0;
    gmac_reg_write((base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), un_int_msk.val);
    tasklet_disable(&gmac_priv->tx_free_tasklet);

    napi_disable(&gmac_priv->napi);

    return 0;
}

s32 gmac_ioctl(struct net_device *net_dev, struct ifreq *ifr, s32 cmd)
{
    s32 ret = 0;
    u32 eth_cmd = 0;
    void __user *data;
    
    switch (cmd) {
        case SIOCDEVDRVCMD:
            mdelay(1);
            data = ifr->ifr_data;
            
            if (copy_from_user(&eth_cmd, data, sizeof(eth_cmd))) {
                return -EFAULT;
            }

            ret = gmac_ioctl_from_usr_space(net_dev, ifr, data, eth_cmd);
            break;

        default:
            return -EFAULT;
    }

    return ret;
}

s32 gmac_device_init(struct platform_device *platform_dev, struct net_device **net_dev)
{
    struct net_device *net_dev_temp = NULL;

    net_dev_temp = alloc_netdev(sizeof(gmac_private_s), "eth%d", NET_NAME_UNKNOWN, ether_setup);
    if (net_dev_temp == NULL) {
        LOG(LOG_ERROR, "alloc_netdev failed");
        return -ENOMEM;
    }

    SET_NETDEV_DEV(net_dev_temp, &platform_dev->dev);

    /* The GMAC-specific entries in the device structure. */
    net_dev_temp->netdev_ops = &g_gmac_netdev_ops;

    net_dev_temp->ethtool_ops = &g_gmac_ethtool_ops;

    net_dev_temp->watchdog_timeo = TX_TIMEOUT;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
    net_dev_temp->max_mtu = MAX_MTU_SIZE;
#endif

    LOG(LOG_INFO, "Enable GMAC TX checksum offload");
    net_dev_temp->features |= (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_RXCSUM);

    net_dev_temp->hw_features |= (NETIF_F_IP_CSUM | NETIF_F_IPV6_CSUM | NETIF_F_RXCSUM);

    *net_dev = net_dev_temp;

    return 0;
}

s32 gmac_priv_init(struct net_device *net_dev, gmac_private_s **gmac_priv, u32 mac_num)
{
    gmac_private_s *gmac_priv_temp = NULL;

    gmac_priv_temp = (gmac_private_s *)netdev_priv(net_dev);
    (void)memset_s(gmac_priv_temp, sizeof(gmac_private_s), 0, sizeof(gmac_private_s));

    gmac_priv_temp->gmac_loop_recv_hook = NULL;
    gmac_priv_temp->gmac_loop_cnt = 0;

    gmac_priv_temp->id = mac_num;
    gmac_priv_temp->opened = 0;

    spin_lock_init(&gmac_priv_temp->lock);

    gmac_priv_temp->oldautoneg = AUTONEG_ENABLE;
    *gmac_priv = gmac_priv_temp;

    return 0;
}

s32 gmac_baseaddr_init(struct platform_device *platform_dev, struct net_device *net_dev, u32 mac_num)
{
    struct device_node *node = platform_dev->dev.of_node;
    struct resource *regs;

    regs = platform_get_resource(platform_dev, IORESOURCE_MEM, mac_num);
    if (regs == NULL) {
        return -EINVAL;
    }
    net_dev->mem_start = regs->start;

    net_dev->base_addr = (unsigned long)of_iomap(node, mac_num);
    if (net_dev->base_addr == 0) {
        LOG(LOG_ERROR, "gmac(0x%x) ioremap failed", mac_num);
        return -EIO;
    }

    return 0;
}

struct gmac_phy_info *gmac_get_phy_info(u32 phy_addr)
{
    u32 i = 0;
    struct gmac_phy_info *info = NULL;
    u32 count = sizeof(g_hi_gmac_phy_info_table) / sizeof(gmac_phy_info_s);
    u32 phy_id = 0;

    while (count--) {
        info = &g_hi_gmac_phy_info_table[i];
        if ((info->driver != NULL) && (info->driver->get_phy_id != NULL)) {
            info->driver->get_phy_id(phy_addr, &phy_id);
            if (phy_id == info->phy_id) {
                return info;
            }
        }
        i++;
    }

    return NULL;
}

s32 gmac_phy_init(struct platform_device *pdev, gmac_private_s *gmac_priv, u32 mac_num)
{
    s32 ret = 0;
    u32 phy_addr = 0;
    u32 addr = 0;
    gmac_phy_info_s *info = NULL;
    struct device_node *np = pdev->dev.of_node;

#define INVALID_PHY_ADDR 0xFF

    if (gmac_priv == NULL) {
        return -EINVAL;
    }

    if (of_property_read_u32(np, "phy_addr", &phy_addr)) {
        LOG(LOG_ERROR, "cannot get phy_addr from dtb!");
        return -ENODEV;
    }

    phy_addr = (phy_addr >> (mac_num * 8)); /* 8 bits represent a PHY address. */
    /* Check if the corresponding MAC is using a PHY. */
    if ((phy_addr & INVALID_PHY_ADDR) != 0xFF) {
        addr = (phy_addr & GMAC_PHY_MAX_ADDR);
        LOG(LOG_PRINT, "mac(%u)'s phy_addr is %u.", mac_num, addr);
    } else {
        LOG(LOG_PRINT, "mac(%u) not used phy.", mac_num);
        return -EINVAL;
    }

    info = gmac_get_phy_info(addr);
    if (info) {
        gmac_priv->phy_handle = info->driver;
        gmac_priv->phy_addr = addr;
    } else {
        return -EINVAL;
    }

    if (gmac_priv->phy_handle->init) {
        ret = gmac_priv->phy_handle->init(addr);
        if (ret) {
            LOG(LOG_ERROR, "mac(%u) init phy failed(%d)!", mac_num, ret);
        }
    }

    return ret;
}

s32 gmac_txrx_init(struct net_device *net_dev, gmac_private_s *gmac_priv, u32 mac_num)
{
    s32 ret;

    ret = gmac_tx_init_ring(net_dev);
    if (ret) {
        LOG(LOG_ERROR, "gmac(%u) tx ring init failed(%d)!", mac_num, ret);
        return ret;
    }

    gmac_rx_init_ring(net_dev);
    gmac_rx_fill_all_buff(net_dev);
    spin_lock_init(&gmac_priv->mac_lock);

    if (g_rx_mode && ((mac_num == ETH2) || (mac_num == ETH3))) {
        netif_napi_add(net_dev, &gmac_priv->napi, gmac_eth_rx, 128); /* config budget 128 */
    } else {
        netif_napi_add(net_dev, &gmac_priv->napi, gmac_eth_rx, 64); /* config budget 64 */
    }

    tasklet_init(&gmac_priv->tx_free_tasklet, gmac_tx_free_bd, (unsigned long)(uintptr_t)net_dev);
    tasklet_disable(&gmac_priv->tx_free_tasklet);

    return 0;
}

s32 gmac_irq_init(struct platform_device *platform_dev, struct net_device *net_dev, u32 mac_num, u32 *irq)
{
    s32 ret;
    u32 irq_num;
    struct device_node *node = platform_dev->dev.of_node;

    irq_num = irq_of_parse_and_map(node, mac_num);
    if (!irq_num) {
        LOG(LOG_ERROR, "mac(0x%x) irq_of_parse_and_map failed", mac_num);
        return -EPERM;
    }

    ret = request_irq(irq_num, gmac_interrupt_handle, 0, platform_dev->name, net_dev);
    if (ret) {
        LOG(LOG_ERROR, "request_irq failed");
        return -EPERM;
    }

    *irq = irq_num;

    return GMAC_OK;
}

s32 gmac_hardware_init(struct platform_device *platform_dev, struct net_device *net_dev,
                       gmac_private_s *gmac_priv, u32 mac_num)
{
    s32 ret;

    ret = gmac_reset_chip(net_dev, mac_num);
    if (ret) {
        LOG(LOG_ERROR, "gmac_reset_chip failed");
        return ret;
    }

    ret = gmac_phy_init(platform_dev, gmac_priv, mac_num);
    if (ret) {
        LOG(LOG_INFO, "gmac_phy_init(%u) failed", mac_num);
        gmac_priv->phy_handle = NULL;
    }

    ret = gmac_chip_init(net_dev, mac_num);
    if (ret) {
        LOG(LOG_ERROR, "gmac_chip_init failed");
        return ret;
    }

    ret = gmac_txrx_init(net_dev, gmac_priv, mac_num);
    if (ret) {
        LOG(LOG_ERROR, "gmac_txrx_init failed");
        return ret;
    }

    return GMAC_OK;
}

s32 gmac_init(struct platform_device *platform_dev, gmac_devs_s *net_devs, u32 mac_num)
{
    s32 ret;
    u32 irq = 0;
    struct net_device *net_dev = NULL;
    gmac_private_s *gmac_priv = NULL;

    ret = gmac_device_init(platform_dev, &net_dev);
    if (ret) {
        LOG(LOG_ERROR, "gmac_device_init failed");
        return ret;
    }

    net_devs->mac_devs[mac_num] = net_dev;

    ret = gmac_priv_init(net_dev, &gmac_priv, mac_num);
    if (ret) {
        LOG(LOG_ERROR, "gmac_priv_init failed");
        goto INIT_FAILED;
    }

    ret = gmac_baseaddr_init(platform_dev, net_dev, mac_num);
    if (ret) {
        LOG(LOG_ERROR, "gmac_baseaddr_init failed");
        goto INIT_FAILED;
    }

    ret = gmac_hardware_init(platform_dev, net_dev, gmac_priv, mac_num);
    if (ret) {
        LOG(LOG_ERROR, "gmac_hardware_init failed");
        goto INIT_FAILED;
    }

    ret = gmac_irq_init(platform_dev, net_dev, mac_num, &irq);
    if (ret) {
        LOG(LOG_ERROR, "gmac_irq_init failed");
        goto INIT_FAILED;
    }

    ret = register_netdev(net_dev);
    if (ret) {
        LOG(LOG_ERROR, "register_netdev failed");
        goto INIT_FAILED;
    }

    return 0;

INIT_FAILED:
    if (net_dev->base_addr != 0) {
        iounmap((void __iomem *)(uintptr_t)net_dev->base_addr);
    }

    if (irq > 0) {
        free_irq(irq, net_dev);
    }

    free_netdev(net_dev);
    net_devs->mac_devs[mac_num] = NULL;

    return ret;
}

static s32 gmac_phy_reinit(struct net_device *dev, u32 addr)
{
    gmac_private_s *priv = (gmac_private_s *)netdev_priv(dev);
    gmac_phy_info_s *info = NULL;
    s32 ret;

    info = gmac_get_phy_info(addr);
    if (!info) {
        LOG(LOG_ERROR, "mac(%u) get phy info failed", priv->id);
        return -EIO;
    }

    priv->phy_handle = info->driver;
    priv->phy_addr = addr;

    if (priv->phy_handle->init) {
        ret = priv->phy_handle->init(addr);
        if (ret) {
            LOG(LOG_ERROR, "mac(%u) reinit phy failed!", priv->id);
            return ret;
        }
    }

    return 0;
}

s32 gmac_ioctl_from_usr_space(struct net_device *net_dev, const struct ifreq *ifr, void __user *data, u32 eth_cmd)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    eth_drv_loop_set_s eth_loop_set;
    eth_drv_auto_nego_set_s atuo_nego_set;
    eth_drv_speed_duplex_set_s speed_duplex_set;
    eth_serdes_power_ctr_s serdes_power_ctr;
    struct eth_drv_phy_info phy_info;
    struct eth_drv_rmii_config rmii_info;

    switch (eth_cmd) {
        case ETH_DRV_LOOP_SET:
            if (copy_from_user(&eth_loop_set, data, sizeof(eth_drv_loop_set_s))) {
                return -EFAULT;
            }

            return gmac_hw_debug_lb_test(net_dev, eth_loop_set.loop_mode);
        case ETH_DRV_PHY_REINIT:
            if (copy_from_user(&phy_info, data, sizeof(struct eth_drv_phy_info))) {
                return -EFAULT;
            }

            return gmac_phy_reinit(net_dev, phy_info.addr);
        case ETH_DRV_RMII_CLK_CONFIG:
            if (copy_from_user(&rmii_info, data, sizeof(struct eth_drv_rmii_config))) {
                return -EFAULT;
            }
            return gmac_hw_config_rmii_clk_info(net_dev, &rmii_info);
        case ETH_DRV_AUTONEGO_SET:
            if (copy_from_user(&atuo_nego_set, data, sizeof(atuo_nego_set))) {
                return -EFAULT;
            }
            return gmac_hw_set_autoneg(net_dev, &atuo_nego_set);

        case ETH_DRV_SPPED_DUPLEX_SET:
            if (copy_from_user(&speed_duplex_set, data, sizeof(speed_duplex_set))) {
                return -EFAULT;
            }
            return gmac_adjustlink(gmac_priv->id, speed_duplex_set.speed, speed_duplex_set.duplex, 1);

        case ETH_SERDES_POWER_SET:
            if (copy_from_user(&serdes_power_ctr, data, sizeof(eth_serdes_power_ctr_s))) {
                return -EFAULT;
            }
            return gmac_serdes_power_set(gmac_priv->id, serdes_power_ctr.serdes_power_mode);
        case ETH_SERDES_NEG_STATE:
            return gmac_get_serdes_neg_state(net_dev, gmac_priv->id, ifr->ifr_data);
        default:
            return -EINVAL;
    }
}

u32 gmac_cr_read(u32 id, u32 addr)
{
    if (id >= MAX_MAC_PORT_NUM) {
        LOG(LOG_ERROR, "cr read index out.");
        return 0;
    } else {
        return (u32)gmac_hw_serdes_read(id, addr);
    }
}
EXPORT_SYMBOL(gmac_cr_read);

s32 gmac_cr_write(u32 id, u32 addr, u32 data)
{
    if (id >= MAX_MAC_PORT_NUM) {
        LOG(LOG_ERROR, "cr write index out.");
        return -EINVAL;
    } else {
        return gmac_hw_serdes_write(id, addr, data);
    }
}
EXPORT_SYMBOL(gmac_cr_write);

s32 gmac_probe(struct platform_device *platform_dev)
{
    s32 ret;
    u32 mac_id;
    gmac_devs_s *net_devs = NULL;

    phy_lock_init_mar1512();

    (void)memset_s(&g_gmac_devices, sizeof(gmac_devs_s), 0, sizeof(gmac_devs_s));
    g_gmac_dev = &g_gmac_devices;
    net_devs = &g_gmac_devices;

    dma_set_mask(&platform_dev->dev, GMAC_DMA_MASK);
    dma_set_mask_and_coherent(&(platform_dev->dev), GMAC_DMA_MASK);

    for (mac_id = 0; mac_id < MAX_MAC_PORT_NUM; mac_id++) {
        if ((mac_id ==  ETH2) || (mac_id == ETH3)) {
            ret = gmac_serdes_init(mac_id);
            if (ret) {
                LOG(LOG_EMERG, "gmac%u serdes init failed!", mac_id);
                continue;
            }
        }

        ret = gmac_init(platform_dev, net_devs, mac_id);
        if (ret) {
            LOG(LOG_EMERG, "gmac%u init failed(%d)!", mac_id, ret);
            continue;
        }
    }

    platform_set_drvdata(platform_dev, net_devs);

    (void)gmac_delaywork_init();

    if (g_rx_mode) {
        gmac_rx_timer_init();
    }

    LOG(LOG_ERROR, "gmac init ok!");

    return 0;
}

s32 gmac_remove(struct platform_device *platform_dev)
{
    u32 virq;
    u32 index, mac_num;
    struct sk_buff *skb_buff = NULL;
    struct sk_buff **rx_bd_ring = NULL;
    struct net_device *pst_net_dev = NULL;
    struct device_node *node = platform_dev->dev.of_node;
    gmac_devs_s *pst_devs = (gmac_devs_s *)platform_get_drvdata(platform_dev);
    gmac_private_s *gmac_priv = NULL;

    if (g_gmac_wq) {
        cancel_rearming_delayed_workqueue(g_gmac_wq, &g_gmac_delayed_work);
        destroy_workqueue(g_gmac_wq);
        g_gmac_wq = NULL;
        msleep(1000);
    }

    if (g_rx_mode) {
        gmac_rx_timer_close();
    }

    for (mac_num = 0; mac_num < MAX_MAC_PORT_NUM; mac_num++) {
        pst_net_dev = pst_devs->mac_devs[mac_num];
        if (pst_net_dev == NULL) {
            continue;
        }

        unregister_netdev(pst_net_dev);

        virq = irq_of_parse_and_map(node, mac_num);
        if (virq > 0) {
            free_irq(virq, pst_net_dev);
        }

        gmac_priv = (gmac_private_s *)netdev_priv(pst_net_dev);
        netif_napi_del(&gmac_priv->napi);

        rx_bd_ring = gmac_priv->rx_bd_ring;
        for (index = 0; index < GMAC_RX_BD_RING_SIZE; index++) {
            if (rx_bd_ring[index] != NULL) {
                skb_buff = rx_bd_ring[index];
                dev_kfree_skb_any(skb_buff);
                rx_bd_ring[index] = NULL;
            }
        }

        tasklet_kill(&gmac_priv->tx_free_tasklet);

        gmac_tx_free_all_bd_force(pst_net_dev, gmac_priv);
        gmac_tx_free_ring(pst_net_dev);

        iounmap((void __iomem *)pst_net_dev->base_addr);

        free_netdev(pst_net_dev);
    }

    platform_set_drvdata(platform_dev, NULL);
    g_gmac_dev = NULL;

    return 0;
}

s32 gmac_suspend(struct platform_device *platform_dev, pm_message_t state)
{
    s32 ret = 0;
    u32 mac_id;
    struct net_device **ppst_devs = (struct net_device **)platform_get_drvdata(platform_dev);
    struct net_device *pst_net_dev = NULL;

    for (mac_id = 0; mac_id < MAX_MAC_PORT_NUM; mac_id++) {
        pst_net_dev = ppst_devs[mac_id];
        if (pst_net_dev != NULL) {
            if (netif_running(pst_net_dev)) {
                netif_device_detach(pst_net_dev);
                ret = gmac_stop(pst_net_dev);
            }
        }
    }

    return ret;
}

s32 gmac_resume(struct platform_device *platform_dev)
{
    s32 ret = 0;
    u32 mac_id;
    struct net_device *pst_net_dev = NULL;
    struct net_device **ppst_devs = (struct net_device **)platform_get_drvdata(platform_dev);

    for (mac_id = 0; mac_id < MAX_MAC_PORT_NUM; mac_id++) {
        pst_net_dev = ppst_devs[mac_id];
        if (pst_net_dev != NULL) {
            if (netif_running(pst_net_dev)) {
                netif_device_attach(pst_net_dev);
                ret = gmac_open(pst_net_dev);
            }
        }
    }

    return ret;
}

static const struct of_device_id g_gmac_of_match[] = {
    { .compatible = "hisilicon, hi1711_gmac_net" },
    {},
};

struct platform_driver g_gmac_platform_driver = {
    .driver = {
        .name = GMAC_DEV_NAME,
        .of_match_table = g_gmac_of_match,
        .owner = THIS_MODULE,
    },

    .probe = gmac_probe,
    .remove = gmac_remove,
};

module_platform_driver(g_gmac_platform_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("gmac driver");