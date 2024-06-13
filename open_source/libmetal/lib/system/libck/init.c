/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	libck/init.c
 * @brief	libck libmetal initialization.
 */
#include <metal/device.h>
#include <metal/sys.h>

struct metal_state _metal;

int metal_sys_init(const struct metal_init_params *params)
{
	(void)params;
	metal_bus_register(&metal_generic_bus);
	return 0;
}

void metal_sys_finish(void)
{
	metal_bus_unregister(&metal_generic_bus);
}
