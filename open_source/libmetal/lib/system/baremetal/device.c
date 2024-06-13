/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	baremetal/device.c
 * @brief	baremetal libmetal device definitions.
 */

#include <metal/device.h>
#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>

int metal_generic_dev_sys_open(struct metal_device *dev)
{
	return 0;
}
