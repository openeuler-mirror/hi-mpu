/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	baremetal/sys.h
 * @brief	baremetal system primitives for libmetal.
 */

#ifndef __METAL_SYS__H__
#error "include metal/sys.h instead of metal/system/@PROJECT_SYSTEM@/sys.h"
#endif

#ifndef __METAL_BAREMETAL_SYS__H__
#define __METAL_BAREMETAL_SYS__H__

#ifdef __cplusplus
extern "C" {
#endif

#define METAL_INIT_DEFAULTS				\
{							\
	.log_handler	= metal_default_log_handler,	\
	.log_level	= METAL_LOG_INFO,		\
}

/** Structure of libmetal runtime state. */
struct metal_state {
	/** Common (system independent) data. */
	struct metal_common_state common;
};

#ifdef __cplusplus
}
#endif

#endif /* __METAL_BAREMETALL_SYS__H__ */
