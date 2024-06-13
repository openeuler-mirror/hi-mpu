/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	libck/alloc.h
 * @brief	libck assertion support.
 */
#ifndef __METAL_ALLOC__H__
#error "Include metal/alloc.h instead of metal/system/@PROJECT_SYSTEM@/alloc.h"
#endif

#include "sre_mem.h"

#ifndef __METAL_LIBCK_ALLOC__H__
#define __METAL_LIBCK_ALLOC__H__

#ifdef __cplusplus
extern "C" {
#endif

static inline void *metal_allocate_memory(unsigned int size)
{
	return SRE_MemAlloc(OS_MID_SYS, OS_MEM_DEFAULT_PT0, size);
}

static inline void metal_free_memory(void *ptr)
{
	SRE_MemFree(OS_MID_SYS, ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_LIBCK_ALLOC__H__ */
