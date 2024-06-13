// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * ETH TX interface function.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/io.h>
#include "pcu_csr_addr_define.h"
#include "pcu_c_union_define.h"
#include "comm_knl_api.h"
#include "gmac.h"
#include "gmac_hw.h"
#include "eth_tx.h"

static void gmac_recycle_tx_buffer_process(struct net_device *net_dev, gmac_private_s *gmac_priv,
                                           gmac_queue_node_t *node, s32 free_skb)
{
    gmac_tx_buffer_s *tx_buff;
    struct sk_buff *skb;
    dma_addr_t skb_dma_addr;

    tx_buff = &node->tx_buf;
    skb = tx_buff->tx_skb_buff;
    skb_dma_addr = tx_buff->skb_data_dma_addr;
    tx_buff->tx_skb_buff = NULL;
    tx_buff->tx_state = GMAC_TX_IDLE;

    gmac_queue_enque(&gmac_priv->gmac_txbd_unused, node, NULL, NULL, NULL);

    dma_unmap_single(net_dev->dev.parent, skb_dma_addr, skb->len, DMA_FROM_DEVICE);

    if (free_skb) {
        dev_kfree_skb_any(skb);
    }

    return;
}

static inline void gmac_recycle_one_tx_buffer(struct net_device *net_dev, gmac_private_s *gmac_priv)
{
    gmac_queue_node_t *node = NULL;

    node = gmac_queue_deque(&gmac_priv->gmac_txbd_inused);
    if (!node) {
        return;
    }

    gmac_recycle_tx_buffer_process(net_dev, gmac_priv, node, true);
}

void gmac_tx_free_all_bd_force(struct net_device *net_dev, gmac_private_s *gmac_priv)
{
    while (!gmac_queue_check_empty(&gmac_priv->gmac_txbd_inused)) {
        gmac_recycle_one_tx_buffer(net_dev, gmac_priv);
    }

    return;
}

void gmac_tx_free_bd(unsigned long dev)
{
    struct net_device *net_dev = (struct net_device *)(uintptr_t)dev;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    u_cf_cff_data_num un_cf_cff_data_num;
    gmac_queue_node_t *node = NULL;
    gmac_tx_buffer_s *tx_buff = NULL;
    u_cf_intrpt_msk int_mask;
    unsigned long flags;
    u32 count = 0;
    u32 inused, limit;

    limit = gmac_queue_get_count(&gmac_priv->gmac_txbd_inused);
    gmac_reg_read((uintptr_t)(net_dev->base_addr + PCU_CSR_CF_CFF_DATA_NUM_OFFSET_ADDR), un_cf_cff_data_num.val);
    inused = un_cf_cff_data_num.bits.cf_tx_cfg_num / 4; // divide 4
    limit = (limit > inused) ? (limit - inused) : 0;

    if (!limit) {
        goto EXIT;
    }

    while (count++ < limit - 1) {
        gmac_recycle_one_tx_buffer(net_dev, gmac_priv);
    }

    node = gmac_queue_head(&gmac_priv->gmac_txbd_inused);
    tx_buff = node ? &node->tx_buf : NULL;

    if ((!tx_buff) || (tx_buff->tx_state != GMAC_TX_END)) {
        goto EXIT;
    }

    gmac_recycle_one_tx_buffer(net_dev, gmac_priv);

EXIT:
    /* Enable tx interrupt */
    spin_lock_irqsave(&gmac_priv->mac_lock, flags);
    gmac_reg_read((uintptr_t)(net_dev->base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), int_mask.val);
    int_mask.bits.cf_txcfg_avl_msk = 1;
    gmac_reg_write((uintptr_t)(net_dev->base_addr + PCU_CSR_CF_INTRPT_MSK_OFFSET_ADDR), int_mask.val);
    spin_unlock_irqrestore(&gmac_priv->mac_lock, flags);

    return;
}

static inline gmac_tx_buffer_s *gmac_tx_buffers_accquire(gmac_private_s *gmac_priv)
{
    gmac_queue_t *q = &gmac_priv->gmac_txbd_unused;
    gmac_queue_node_t *node;

    node = gmac_queue_deque(q);

    return node ? &node->tx_buf : NULL;
}

static s32 gmac_tx_bd_write(gmac_queue_node_t *node, void *dev, void *cmd)
{
    struct net_device *net_dev = dev;
    fe_pcu_cmd_s *st_pcu_cmd = cmd;
    u_cf_cff_data_num un_cf_cff_data_num;
    struct sk_buff *tx_skb = NULL;

    tx_skb = node->tx_buf.tx_skb_buff;

    gmac_reg_read((uintptr_t)(net_dev->base_addr + PCU_CSR_CF_CFF_DATA_NUM_OFFSET_ADDR), un_cf_cff_data_num.val);
    if (un_cf_cff_data_num.bits.cf_tx_cfg_num >= ((CHIP_TX_FIFO_NUM - 1) * 4)) { // mul 4
        return -1;
    }

    /* Write the packet descriptor to the chip. */
    gmac_reg_write((uintptr_t)(net_dev->base_addr + PCU_CSR_TX_CFF_ADDR_0_OFFSET_ADDR), st_pcu_cmd->word0.u_32);
    gmac_reg_write((uintptr_t)(net_dev->base_addr + PCU_CSR_TX_CFF_ADDR_1_OFFSET_ADDR), st_pcu_cmd->word1.u_32);
    gmac_reg_write((uintptr_t)(net_dev->base_addr + PCU_CSR_TX_CFF_ADDR_2_OFFSET_ADDR), st_pcu_cmd->pkt_addr);
    gmac_reg_write((uintptr_t)(net_dev->base_addr + PCU_CSR_TX_CFF_ADDR_3_OFFSET_ADDR), st_pcu_cmd->clear_addr);

    /* Updating network packet sending statistics */
    net_dev->stats.tx_bytes += tx_skb->len;
    net_dev->stats.tx_packets++;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    net_dev->trans_start = jiffies;
#endif

    return 0;
}

static s32 gmac_tx_add_bd(struct net_device *net_dev, struct sk_buff *tx_skb_buff)
{
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    gmac_tx_buffer_s *tx_bd_buff = NULL;
    fe_pcu_cmd_s st_pcu_cmd;
    dma_addr_t pkt_addr;
    gmac_queue_node_t *node = NULL;
    s32 ret;

    if (unlikely(gmac_priv->opened == 0)) {
        LOG(LOG_ERROR, "xmit ok, free skb!");
        dev_kfree_skb_any(tx_skb_buff);
        return NETDEV_TX_OK;
    }

    if (unlikely(tx_skb_buff->len == 0) || unlikely(tx_skb_buff->len > GMAC_MAX_PKT_LEN)) {
        LOG(LOG_ERROR, "tx_skb_buff length(0x%x) is inval!", tx_skb_buff->len);
        dev_kfree_skb_any(tx_skb_buff);
        net_dev->stats.tx_errors++;
        return NETDEV_TX_OK;
    }

    /* Organize packet descriptors */
    (void)memset_s((void *)&st_pcu_cmd, sizeof(fe_pcu_cmd_s), 0, sizeof(fe_pcu_cmd_s));

    st_pcu_cmd.word0.bits.ipoff = tx_skb_buff->network_header - tx_skb_buff->mac_header;
    if (tx_skb_buff->ip_summed == CHECKSUM_PARTIAL) {
        LOG(LOG_INFO, "csum_offload hdr_len:%d\n", tx_skb_buff->network_header - tx_skb_buff->mac_header);
        st_pcu_cmd.word0.bits.l3_cs = 1;
        st_pcu_cmd.word0.bits.l4_cs = 1;
    }

    st_pcu_cmd.word0.bits.wb = 1;                 /* Supports writeback tests. */
    st_pcu_cmd.word1.bits.len = tx_skb_buff->len; /* Send Length */

    /* Obtains the physical address of the data. */
    pkt_addr = dma_map_single(net_dev->dev.parent, tx_skb_buff->data, tx_skb_buff->len, DMA_TO_DEVICE);
    st_pcu_cmd.pkt_addr = gmac_hw_get_tx_ddr_relative_addr(gmac_priv->id, pkt_addr);

    tx_bd_buff = gmac_tx_buffers_accquire(gmac_priv);
    if (!tx_bd_buff) {
        LOG(LOG_INFO, "accquire tx bd failed!");
        return NETDEV_TX_BUSY;
    }

    /* Hooking data to the software data link */
    tx_bd_buff->tx_skb_buff = tx_skb_buff;
    tx_bd_buff->skb_data_dma_addr = pkt_addr;

    /* Obtains the physical address of the data status. */
    tx_bd_buff->tx_state = GMAC_TX_START;
    st_pcu_cmd.clear_addr = gmac_hw_get_tx_ddr_relative_addr(gmac_priv->id, tx_bd_buff->tx_state_dma_addr);
    node = container_of(tx_bd_buff, gmac_queue_node_t, tx_buf);

    ret = gmac_queue_enque(&(gmac_priv->gmac_txbd_inused), node, gmac_tx_bd_write, net_dev, &st_pcu_cmd);
    if (ret) {
        gmac_recycle_tx_buffer_process(net_dev, gmac_priv, node, false);
        return NETDEV_TX_BUSY;
    }

    return NETDEV_TX_OK;
}

s32 gmac_eth_tx(struct sk_buff *tx_skb_buff, struct net_device *net_dev)
{
    return gmac_tx_add_bd(net_dev, tx_skb_buff);
}

void gmac_tx_free_ring(const struct net_device *net_dev)
{
    u32 size;
    u8 *vaddr = NULL;
    dma_addr_t dma_addr;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);

    size = GMAC_TX_BD_RING_SIZE * sizeof(gmac_queue_node_t);
    vaddr = gmac_priv->tx_bd_start_vaddr;
    dma_addr = gmac_priv->tx_bd_start_dma_addr;

    if (vaddr) {
        dma_free_coherent(net_dev->dev.parent, size, vaddr, dma_addr);
    }

    return;
}

s32 gmac_tx_init_ring(const struct net_device *net_dev)
{
    u32 i;
    u8 *start_addr = NULL;
    u32 size;
    dma_addr_t dma_addr = 0;
    gmac_private_s *gmac_priv = (gmac_private_s *)netdev_priv(net_dev);
    gmac_queue_node_t *node = NULL;

    gmac_queue_init(&gmac_priv->gmac_txbd_unused);
    gmac_queue_init(&gmac_priv->gmac_txbd_inused);

    size = GMAC_TX_BD_RING_SIZE * sizeof(gmac_queue_node_t);
    /* The start_addr parameter needs to be saved and will be used for release later. */
    start_addr = (u8 *)dma_alloc_coherent(net_dev->dev.parent, size, &dma_addr, GFP_KERNEL);
    if (start_addr == NULL) {
        LOG(LOG_ERROR, "tx malloc bd_ring buffer failed!");
        gmac_priv->tx_bd_start_vaddr = NULL;
        gmac_priv->tx_bd_start_dma_addr = 0;
        return -ENOMEM;
    }

    gmac_priv->tx_bd_start_vaddr = start_addr;
    gmac_priv->tx_bd_start_dma_addr = dma_addr;

    for (i = 0; i < GMAC_TX_BD_RING_SIZE; i++) {
        node = (gmac_queue_node_t *)(start_addr + (i * sizeof(gmac_queue_node_t)));
        node->tx_buf.tx_state = GMAC_TX_IDLE;
        node->tx_buf.tx_state_dma_addr = dma_addr + (i * sizeof(gmac_queue_node_t));

        gmac_queue_handle_enque(&gmac_priv->gmac_txbd_unused, node);
    }

    return 0;
}
