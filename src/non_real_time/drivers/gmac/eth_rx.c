// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * ETH RX interface function.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <uapi/linux/icmpv6.h>
#include <uapi/linux/ipv6.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/of_address.h>
#include <linux/io.h>

#include "pcu_csr_addr_define.h"
#include "pcu_c_union_define.h"
#include "comm_knl_api.h"
#include "gmac.h"
#include "gmac_hw.h"
#include "eth_rx.h"

static s32 gmac_rx_reuse_skb(struct net_device *net_dev, const struct sk_buff *pst_rx_skbuff)
{
    u32 buff_addr;
    dma_addr_t absolutely_dma_addr;
    unsigned long base_addr = net_dev->base_addr;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    (void)memset_s(pst_rx_skbuff->data, PACKET_HEAD_SIZE, 0x0, PACKET_HEAD_SIZE);

    absolutely_dma_addr = dma_map_single(net_dev->dev.parent, pst_rx_skbuff->data, RX_BUFFER_SIZE, DMA_TO_DEVICE);

    buff_addr = gmac_hw_get_rx_ddr_relative_addr(gmac_priv->id, absolutely_dma_addr);
    gmac_reg_write((base_addr + PCU_CSR_RX_CFF_ADDR_OFFSET_ADDR), buff_addr);

    return 0;
}

static s32 gmac_rx_ndisc_smac_is_myself(const char *pbuf, const struct net_device *pnetdev)
{
    u8 srcaddr[MAC_ADDR_SIZE];
    struct vlan_hdr__ *pvlanhdr = NULL;
    struct ethhdr *pethhdr = NULL;
    struct ipv6hdr *pip6hdr = NULL;
    struct icmp6hdr *picmp6hdr = NULL;
    u16 proto;
    s32 offset;

    if (!pbuf || !pnetdev) {
        return -1;
    }

    (void)memcpy_s(srcaddr, MAC_ADDR_SIZE, pbuf + MAC_ADDR_SIZE, MAC_ADDR_SIZE);

    if (memcmp(pnetdev->dev_addr, srcaddr, MAC_ADDR_SIZE)) {
        return 0;
    }

    pethhdr = (struct ethhdr *)pbuf;
    if (pethhdr->h_proto == ETH_P_8021Q_V) {
        pvlanhdr = (struct vlan_hdr__ *)pbuf;
        proto = pvlanhdr->eth_proto;
        offset = sizeof(struct vlan_hdr__);
    } else {
        proto = pethhdr->h_proto;
        offset = sizeof(struct ethhdr);
    }

    if (proto != ETH_P_IPV6_V) {
        return 0;
    }

    pip6hdr = (struct ipv6hdr *)(pbuf + offset);
    if (pip6hdr->nexthdr != NEXTHDR_ICMP) {
        return 0;
    }
    picmp6hdr = (struct icmp6hdr *)((char *)pip6hdr + sizeof(struct ipv6hdr));
    if (picmp6hdr->icmp6_type != NDISC_NEIGHBOUR_SOLICITATION) {
        return 0;
    }
    return 1;
}

static s32 gmac_rx_fill_one_buff(struct net_device *net_dev, const u32 rx_skb_curr)
{
    u32 dma_addr;
    dma_addr_t absolutely_dma_addr;
    u64 base_addr = net_dev->base_addr;
    struct sk_buff *rx_skb_buff;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    rx_skb_buff = dev_alloc_skb(RX_BUFFER_SIZE);
    if (unlikely(rx_skb_buff == NULL)) {
        LOG(LOG_ERROR, "dev_alloc_skb failed");
        gmac_priv->rx_bd_ring[rx_skb_curr] = NULL;
        return -ENOMEM;
    }

    /* Clears the entire SKB data area. */
    (void)memset_s(rx_skb_buff->data, PACKET_HEAD_SIZE, 0x0, PACKET_HEAD_SIZE);

    /* Obtain the physical address. */
    gmac_priv->rx_bd_ring[rx_skb_curr] = rx_skb_buff;
    absolutely_dma_addr = dma_map_single(net_dev->dev.parent, rx_skb_buff->data, RX_BUFFER_SIZE, DMA_TO_DEVICE);
    if (unlikely((absolutely_dma_addr % CPU_CACHE_LINE_SIZE) != 0)) {
        LOG(LOG_ERROR, "rx buffer addr(0x%x) cache align problem!", absolutely_dma_addr);
    }

    /* The physical address is translated into the relative address of the GMAC in the DDR. */
    dma_addr = gmac_hw_get_rx_ddr_relative_addr(gmac_priv->id, absolutely_dma_addr);
    gmac_reg_write((base_addr + PCU_CSR_RX_CFF_ADDR_OFFSET_ADDR), dma_addr);

    return 0;
}

static void gmac_rx_desc_update_l3_err(u8 *buff_descrip, struct gmac_ethtool_stats *pges)
{
    u8 l3_err;

    l3_err = (buff_descrip[GMAC_DESC_L3_ERR] & GMAC_DESC_L3_ERR_MASK) >> 1;
    if (unlikely(l3_err)) {
        switch (l3_err) {
            case GMAC_L3_WRONG_HEAD:
                pges->rx_desc_l3_wrong_head_cnt++;
                break;
            case GMAC_L3_CSUM_ERR:
                pges->rx_desc_l3_csum_err_cnt++;
                break;
            case GMAC_L3_LEN_ERR:
                pges->rx_desc_l3_len_err_cnt++;
                break;
            case GMAC_L3_ZERO_TTL:
                pges->rx_desc_l3_zero_ttl_cnt++;
                break;
            default:
                pges->rx_desc_l3_rsvd_cnt++;
                break;
        }
    }

    return;
}

static void gmac_rx_desc_update_l4_err(u8 *buff_descrip, struct gmac_ethtool_stats *pges)
{
    u8 l4_err;

    l4_err = (buff_descrip[GMAC_DESC_L4_ERR_LOW] & GMAC_DESC_L4_ERR_LOW_MASK) >> GMAC_DESC_L4_ERR_SHIFT;
    l4_err |= ((buff_descrip[GMAC_DESC_L4_ERR_HIGH] & GMAC_DESC_L4_ERR_HIGH_MASK) << 1);
    if (unlikely(l4_err)) {
        switch (l4_err) {
            case GMAC_L4_WRONG_HEAD:
                pges->rx_desc_l4_wrong_head_cnt++;
                break;
            case GMAC_L4_LEN_ERR:
                pges->rx_desc_l4_len_err_cnt++;
                break;
            case GMAC_L4_CSUM_ERR:
                pges->rx_desc_l4_csum_err_cnt++;
                break;
            case GMAC_L4_ZERO_PORT_NUM:
                pges->rx_desc_l4_zero_port_num_cnt++;
                break;
            default:
                pges->rx_desc_l4_rsvd_cnt++;
                break;
        }
    }

    return;
}

static void gmac_rx_desc_update_ip_pkt(u8 *buff_descrip, struct gmac_ethtool_stats *pges)
{
    u8 ip_ver_err;
    u8 ipvx;
    u8 pkt_type;

    ip_ver_err = (buff_descrip[GMAC_DESC_IP_VER_ERR] & GMAC_DESC_IP_VER_ERR_MASK) >> GMAC_DESC_IP_VER_ERR_SHIFT;
    if (unlikely(ip_ver_err)) {
        pges->rx_desc_ip_ver_err_cnt++;
    }

    ipvx = (buff_descrip[GMAC_DESC_IPVX] & GMAC_DESC_IPVX_MASK) >> GMAC_DESC_IPVX_SHIFT;
    if (ipvx) {
        pges->rx_desc_ipv6_pkt_cnt++;
    } else {
        pges->rx_desc_ipv4_pkt_cnt++;
    }

    pkt_type = (buff_descrip[GMAC_DESC_IP_TCP_UDP] & GMAC_DESC_IP_TCP_UDP_MASK) >> GMAC_DESC_IP_TCP_UDP_SHIFT;
    if (likely(pkt_type)) {
        if (pkt_type == GMAC_IP_PKT) {
            pges->rx_desc_ip_pkt_cnt++;
        } else if (pkt_type == GMAC_TCP_PKT) {
            pges->rx_desc_tcp_pkt_cnt++;
        } else {
            pges->rx_desc_udp_pkt_cnt++;
        }
    } else {
        pges->rx_desc_no_ip_pkt_cnt++;
    }

    return;
}

static void gmac_rx_desc_update_vlan_pkt(u8 *buff_descrip, struct gmac_ethtool_stats *pges)
{
    u8 vlan;

    vlan = (buff_descrip[GMAC_DESC_VLAN_FLAG] & GMAC_DESC_VLAN_FLAG_MASK) >> GMAC_DESC_VLAN_FLAG_SHIFT;
    if (vlan) {
        pges->rx_desc_vlan_pkt_cnt++;
    }

    return;
}

static void gmac_rx_desc_update_ip_proto(u8 *buff_descrip, struct gmac_ethtool_stats *pges)
{
    u8 icmp;
    u8 rarp;
    u8 arp;
    u8 multicast;
    u8 broadcast;
    u8 ipsec;
    u8 ip_opt;

    icmp = (buff_descrip[GMAC_DESC_ICMP] & GMAC_DESC_ICMP_MASK) >> GMAC_DESC_ICMP_SHIFT;
    if (icmp) {
        pges->rx_desc_icmp_pkt_cnt++;
    }

    rarp = (buff_descrip[GMAC_DESC_RARP] & GMAC_DESC_RARP_MASK);
    if (rarp) {
        pges->rx_desc_rarp_pkt_cnt++;
    }

    arp = (buff_descrip[GMAC_DESC_ARP] & GMAC_DESC_ARP_MASK) >> 1;
    if (arp) {
        pges->rx_desc_arp_pkt_cnt++;
    }

    multicast = (buff_descrip[GMAC_DESC_MUL_CST] & GMAC_DESC_MUL_CST_MASK) >> GMAC_DESC_MUL_CST_SHIFT;
    if (multicast) {
        pges->rx_desc_multicast_pkt_cnt++;
    }

    broadcast = (buff_descrip[GMAC_DESC_BRD_CST] & GMAC_DESC_BRD_CST_MASK) >> GMAC_DESC_BRD_CST_SHIFT;
    if (broadcast) {
        pges->rx_desc_broadcast_pkt_cnt++;
    }

    ipsec = (buff_descrip[GMAC_DESC_IPSEC] & GMAC_DESC_IPSEC_MASK) >> GMAC_DESC_IPSEC_SHIFT;
    if (ipsec) {
        pges->rx_desc_ipsec_pkt_cnt++;
    }

    ip_opt = (buff_descrip[GMAC_DESC_OPT_FLAG] & GMAC_DESC_OPT_FLAG_MASK) >> GMAC_DESC_OPT_SHIFT;
    if (ip_opt) {
        pges->rx_desc_ip_opt_pkt_cnt++;
    }

    return;
}

static void gmac_eth_update_rx_desc_stats(u8 *buff_descrip, struct gmac_ethtool_stats *pges)
{
    u8 frag;
    u8 key_match;

    gmac_rx_desc_update_l3_err(buff_descrip, pges);

    frag = (buff_descrip[GMAC_DESC_FRAG_FLAG] & GMAC_DESC_FRAG_FLAG_MASK);
    if (unlikely(frag)) {
        pges->rx_desc_frag_cnt++;
    } else {
        gmac_rx_desc_update_l4_err(buff_descrip, pges);
    }

    gmac_rx_desc_update_ip_pkt(buff_descrip, pges);
    gmac_rx_desc_update_vlan_pkt(buff_descrip, pges);
    gmac_rx_desc_update_ip_proto(buff_descrip, pges);

    key_match = (buff_descrip[GMAC_DESC_KEY_MATCH] & GMAC_DESC_KEY_MATCH_MASK) >> GMAC_DESC_KEY_SHFIT;
    if (key_match) {
        pges->rx_desc_key_match_cnt++;
    }

    return;
}

/*
 * When the db in the FIFO is less than half, the forcible pause frame transmission is enabled.
 */
static void gmac_force_pause_enable(struct net_device *net_dev)
{
    unsigned long base_addr = net_dev->base_addr;
    u_cf_cff_data_num cff_data_num;

    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_CF_CFF_DATA_NUM_OFFSET_ADDR), cff_data_num.val);
    if (cff_data_num.bits.cf_rx_cfg_num <= (CHIP_RX_FIFO_NUM / 2)) { // divide 2
        gmac_reg_write((uintptr_t)(base_addr + PCU_CSR_CF_TX_PAUSE_OFFSET_ADDR), PAUSE_FORCE_ENABLE);
    }

    return;
}

static void gmac_auto_pause_enable(struct net_device *net_dev)
{
    unsigned long base_addr = net_dev->base_addr;

    /* Disable the forcible sending pause function and adjust the sending mode to automatic sending. */
    gmac_reg_write((uintptr_t)(base_addr + PCU_CSR_CF_TX_PAUSE_OFFSET_ADDR), PAUSE_AUTO_ENABLE);

    return;
}

static s32 gmac_eth_rx_pkt_check(struct gmac_hi1711_private_t *gmac_priv, struct net_device *net_dev,
                                 struct sk_buff *rx_skb_buff, u32 *length)
{
    struct gmac_ethtool_stats *pges = &gmac_priv->ges;
    u8 *buff_descrip = NULL;
    dma_addr_t skb_dma_addr;
    u32 l2_err;
    u32 drop;
    u32 gmac_num;

    skb_dma_addr = dma_map_single(net_dev->dev.parent, rx_skb_buff->data, RX_BUFFER_SIZE, DMA_FROM_DEVICE);
    dma_sync_single_for_cpu(net_dev->dev.parent, skb_dma_addr, RX_BUFFER_SIZE, DMA_FROM_DEVICE);

    /* Obtains the data of the SKB. */
    buff_descrip = rx_skb_buff->data;

    /* Obtains the packet length. */
    *length = (buff_descrip[GMAC_DESC_PKT_LEN_HIGH] << GMAC_PKT_LEN_H_OFFSET) + buff_descrip[GMAC_DESC_PKT_LEN_LOW];
    if (*length == 0) {
        dma_unmap_single(net_dev->dev.parent, skb_dma_addr, RX_BUFFER_SIZE, DMA_TO_DEVICE);
        return 0;
    }

    if (unlikely(*length > PACKET_DATA_SIZE)) {
        net_dev->stats.rx_length_errors++;
        net_dev->stats.rx_dropped++;
        goto RX_CHECK_ERR;
    }

    /* Obtains the MAC_num value of packets. */
    gmac_num = (buff_descrip[GMAC_DESC_PORT_NUM] & GMAC_DESC_PORT_NUM_MASK) >> GMAC_NUM_BIT_MASK;
    if (unlikely(gmac_num != gmac_priv->id)) {
        net_dev->stats.rx_dropped++;
        goto RX_CHECK_ERR;
    }

    /* check word5.drop field */
    drop = buff_descrip[GMAC_DESC_DROP] & GMAC_DESC_DROP_MASK;
    if (unlikely(drop)) {
        net_dev->stats.rx_dropped++;
        goto RX_CHECK_ERR;
    }

    /* check word5.L2_error field */
    l2_err = buff_descrip[GMAC_DESC_L2_ERR] & GMAC_DESC_L2_ERR_MASK;
    if (unlikely(l2_err)) {
        net_dev->stats.rx_errors++;
        goto RX_CHECK_ERR;
    }

    gmac_eth_update_rx_desc_stats(buff_descrip, pges);

    if (unlikely(gmac_priv->gmac_loop_recv_hook != NULL)) {
        gmac_priv->gmac_loop_recv_hook(net_dev, rx_skb_buff);
        goto RX_CHECK_ERR;
    }

    if (unlikely(gmac_rx_ndisc_smac_is_myself((char *)(rx_skb_buff->data) + ETH_HEAD_OFFSET, net_dev)) > 0) {
        goto RX_CHECK_ERR;
    }

    dma_unmap_single(net_dev->dev.parent, skb_dma_addr, RX_BUFFER_SIZE, DMA_TO_DEVICE);
    return 0;

RX_CHECK_ERR:
    (void)gmac_rx_reuse_skb(net_dev, rx_skb_buff);
    dma_unmap_single(net_dev->dev.parent, skb_dma_addr, RX_BUFFER_SIZE, DMA_TO_DEVICE);
    return -EINVAL;
}

static s32 gmac_rx_pkt_max_handle(struct napi_struct *recv_napi, struct sk_buff *rx_skb_buff, s32 packet_done)
{
    u8 *buff_descrip = NULL;
    struct net_device *net_dev = recv_napi->dev;
    u32 gmac_num;

    if (rx_skb_buff == NULL) {
        LOG(LOG_ERROR, "rx_skb_buff is NULL");
        return packet_done;
    }

    buff_descrip = (u8 *)rx_skb_buff->data;
    gmac_num = (buff_descrip[GMAC_DESC_PORT_NUM] & GMAC_DESC_PORT_NUM_MASK) >> GMAC_NUM_BIT_MASK;
    if (g_rx_mode && ((gmac_num == ETH2) || (gmac_num == ETH3))) {
        napi_complete(recv_napi);
        gmac_auto_pause_enable(net_dev);
        return 0;
    }

    return packet_done;
}

static void gmac_eth_rx_int_en(struct gmac_hi1711_private_t *gmac_priv, struct net_device *net_dev)
{
    unsigned long base_addr = net_dev->base_addr;
    u_cf_intrpt_msk un_int_msk;
    unsigned long flags;

    spin_lock_irqsave(&gmac_priv->mac_lock, flags);
    gmac_reg_read((uintptr_t)(base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), un_int_msk.val);
    un_int_msk.bits.cf_pkg_intrpt_msk = 1;
    gmac_reg_write((uintptr_t)(base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), un_int_msk.val);
    spin_unlock_irqrestore(&gmac_priv->mac_lock, flags);

    return;
}

s32 gmac_eth_rx(struct napi_struct *recv_napi, s32 budget)
{
    gmac_private_s *gmac_priv = container_of(recv_napi, gmac_private_s, napi);
    struct net_device *net_dev = recv_napi->dev;
    struct sk_buff **rx_bd_ring = gmac_priv->rx_bd_ring;
    struct sk_buff *rx_skb_buff = NULL;
    u32 rx_skb_curr = gmac_priv->rx_skb_curr;
    u32 length = 0;
    s32 packet_done = 0;

    /* Obtains the number of data packets to be processed. */
    while (packet_done < budget) {
        gmac_force_pause_enable(net_dev);

        rx_skb_buff = rx_bd_ring[rx_skb_curr];
        if (rx_skb_buff == NULL) {
            goto RX_FILL_SKB;
        }

        if (gmac_eth_rx_pkt_check(gmac_priv, net_dev, rx_skb_buff, &length)) {
            goto RX_FILL_DONE;
        }

        if (length == 0) {
            break;
        }

        skb_reserve(rx_skb_buff, PACKET_HEAD_SIZE + NET_IP_ALIGN);
        skb_put(rx_skb_buff, length);
        rx_skb_buff->protocol = eth_type_trans(rx_skb_buff, net_dev);

        netif_receive_skb(rx_skb_buff);

        net_dev->stats.rx_bytes += length;
        net_dev->stats.rx_packets++;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
        net_dev->last_rx = jiffies;
#endif

RX_FILL_SKB:
        if (gmac_rx_fill_one_buff(net_dev, rx_skb_curr)) {
            return budget;
        }

RX_FILL_DONE:
        /* Calculate and record the next skb index to be processed. */
        rx_skb_curr++;
        if (rx_skb_curr == GMAC_RX_BD_RING_SIZE) {
            rx_skb_curr = 0;
        }
        gmac_priv->rx_skb_curr = rx_skb_curr;

        /* Check whether the number of processed packets exceeds the maximum. */
        packet_done++;
        if (packet_done >= budget) {
            return gmac_rx_pkt_max_handle(recv_napi, rx_skb_buff, packet_done);
        }
    }

    napi_complete(recv_napi);

    /* Start the automatic sending pause. */
    gmac_auto_pause_enable(net_dev);

    /* Enable Rx Interrupt */
    gmac_eth_rx_int_en(gmac_priv, net_dev);

    return packet_done;
}

void gmac_rx_fill_all_buff(struct net_device *net_dev)
{
    u32 index;
    u32 dma_addr;
    dma_addr_t absolutely_dma_addr;
    unsigned long base_addr = net_dev->base_addr;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    struct sk_buff **rx_bd_ring = gmac_priv->rx_bd_ring;

    for (index = 0; index < GMAC_RX_BD_RING_SIZE; index++) {
        /* Clears the entire SKB data area. */
        (void)memset_s(rx_bd_ring[index]->data, PACKET_HEAD_SIZE, 0x0, PACKET_HEAD_SIZE);

        /* Obtain the physical address. */
        absolutely_dma_addr = dma_map_single(net_dev->dev.parent, rx_bd_ring[index]->data,
                                             RX_BUFFER_SIZE, DMA_TO_DEVICE);
        if ((absolutely_dma_addr % CPU_CACHE_LINE_SIZE) != 0) {
            LOG(LOG_ERROR, "rx buffer addr(0x%x) cache align problem!", absolutely_dma_addr);
            break;
        }

        dma_addr = gmac_hw_get_rx_ddr_relative_addr(gmac_priv->id, absolutely_dma_addr);
        gmac_reg_write((base_addr + PCU_CSR_RX_CFF_ADDR_OFFSET_ADDR), dma_addr);
    }

    return;
}

static void gmac_rx_free_all_buff(const struct net_device *net_dev)
{
    u32 index;
    struct sk_buff *rx_skb_buff = NULL;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    struct sk_buff **rx_bd_ring = gmac_priv->rx_bd_ring;

    for (index = 0; index < GMAC_RX_BD_RING_SIZE; index++) {
        if (rx_bd_ring[index] != NULL) {
            rx_skb_buff = rx_bd_ring[index];

            dev_kfree_skb_any(rx_skb_buff);
            rx_bd_ring[index] = NULL;
        }
    }

    return;
}

s32 gmac_rx_init_ring(const struct net_device *net_dev)
{
    u32 index;
    struct sk_buff *rx_skb_buff = NULL;
    unsigned long base_addr = net_dev->base_addr;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    struct sk_buff **rx_bd_ring = gmac_priv->rx_bd_ring;

    /* Initialize the RX buffer ring. */
    for (index = 0; index < GMAC_RX_BD_RING_SIZE; index++) {
        rx_skb_buff = dev_alloc_skb(RX_BUFFER_SIZE);
        if (rx_skb_buff == NULL) {
            LOG(LOG_ERROR, "dev_alloc_skb index(0x%x) failed!\n", index);
            gmac_rx_free_all_buff(net_dev);
            return -ENOMEM;
        }

        rx_bd_ring[index] = rx_skb_buff;
    }

    /* Initializes the index for receiving the SKB. */
    gmac_priv->rx_skb_curr = 0;

    gmac_reg_write((base_addr + PCU_CSR_RX_BUF_SIZE_OFFSET_ADDR), RX_BUFFER_SIZE);

    return 0;
}
