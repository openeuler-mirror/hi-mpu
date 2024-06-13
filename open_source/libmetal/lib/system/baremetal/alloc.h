/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	baremetal/alloc.h
 * @brief	baremetal assertion support.
 */
#ifndef __METAL_ALLOC__H__
#error "Include metal/alloc.h instead of metal/system/@PROJECT_SYSTEM@/alloc.h"
#endif

#include "bm_mem.h"

#ifndef __METAL_BAREMETAL_ALLOC__H__
#define __METAL_BAREMETAL_ALLOC__H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void *metal_allocate_memory(unsigned int size)
{
	return bm_memory_alloc(size);
}

static inline void metal_free_memory(void *ptr)
{
	bm_memory_free(ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_BAREMETAL_ALLOC__H__ */
