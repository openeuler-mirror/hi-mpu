// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __ETH_TX_H__
#define __ETH_TX_H__

#include "gmac.h"

s32 gmac_eth_tx(struct sk_buff *tx_skb_buff, struct net_device *net_dev);
void gmac_tx_free_bd(unsigned long dev);
void gmac_tx_free_ring(const struct net_device *net_dev);
s32 gmac_tx_init_ring(const struct net_device *net_dev);
void gmac_tx_free_all_bd_force(struct net_device *net_dev, gmac_private_s *gmac_priv);

#endif  // __ETH_TX_H__
