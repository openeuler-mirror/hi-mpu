/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	libck/cache.h
 * @brief	libck cache operation primitives for libmetal.
 */

#ifndef __METAL_CACHE__H__
#error "include metal/cache.h instead of metal/system/@PROJECT_SYSTEM@/cache.h"
#endif

#ifndef __METAL_LIBCK_CACHE__H__
#define __METAL_LIBCK_CACHE__H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void __metal_cache_flush(void *addr, unsigned int len)
{
	(void)addr;
	(void)len;
}

static inline void __metal_cache_invalidate(void *addr, unsigned int len)
{
	(void)addr;
	(void)len;
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_LIBCK_CACHE__H__ */
