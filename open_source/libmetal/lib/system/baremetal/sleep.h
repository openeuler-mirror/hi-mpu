/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	baremetal/sleep.h
 * @brief	baremetal sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/system/@PROJECT_SYSTEM@/sleep.h"
#endif

#ifndef __METAL_BAREMETAL_SLEEP__H__
#define __METAL_BAREMETAL_SLEEP__H__

#include "bm_common.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int __metal_sleep_usec(unsigned int usec)
{
	udelay(usec);
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_BAREMETAL_SLEEP__H__ */
