// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __ETHTOOL_H__
#define __ETHTOOL_H__

#include "gmac.h"

extern struct ethtool_ops g_gmac_ethtool_ops;

struct gmac_ethtool_stats *gmac_ethtool_get_estats(gmac_private_s *gmac_priv);

#endif  // __ETHTOOL_H__