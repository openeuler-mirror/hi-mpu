/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	baremetal/io.h
 * @brief	baremetal specific io definitions.
 */

#ifndef __METAL_IO__H__
#error "Include metal/io.h instead of metal/system/@PROJECT_SYSTEM@/io.h"
#endif

#ifndef __METAL_BAREMETAL_IO__H__
#define __METAL_BAREMETAL_IO__H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef METAL_INTERNAL
/**
 * @brief memory mapping for an I/O region
 */
static inline void metal_sys_io_mem_map(struct metal_io_region *io)
{
	(void)io;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __METAL_BAREMETAL_IO__H__ */
