/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	libck/sleep.h
 * @brief	libck sleep primitives for libmetal.
 */

#ifndef __METAL_SLEEP__H__
#error "Include metal/sleep.h instead of metal/system/@PROJECT_SYSTEM@/sleep.h"
#endif

#ifndef __METAL_LIBCK_SLEEP__H__
#define __METAL_LIBCK_SLEEP__H__

#include "sre_task.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int __metal_sleep_usec(unsigned int usec)
{
	return SRE_TaskDelay(usec);
}

#ifdef __cplusplus
}
#endif

#endif /* __METAL_LIBCK_SLEEP__H__ */
