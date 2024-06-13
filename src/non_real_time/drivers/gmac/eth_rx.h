// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _ETH_RX_H_
#define _ETH_RX_H_

s32 gmac_eth_rx(struct napi_struct *recv_napi, s32 budget);
void gmac_rx_fill_all_buff(struct net_device *net_dev);
s32 gmac_rx_init_ring(const struct net_device *net_dev);

#endif  // _ETH_RX_H_

