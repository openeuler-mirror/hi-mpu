/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023, All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * @file	libck/sys.c
 * @brief	libck libmetal device definitions.
 */

#include <metal/io.h>
#include <metal/sys.h>
#include <metal/utilities.h>
#include <stdint.h>

void sys_irq_restore_enable(unsigned int flags)
{
}

unsigned int sys_irq_save_disable(void)
{
    return 0;
}

void sys_irq_enable(unsigned int vector)
{
}

void sys_irq_disable(unsigned int vector)
{
}

void metal_machine_cache_flush(void *addr, unsigned int len)
{
    (void *)addr;
}

void metal_machine_cache_invalidate(void *addr, unsigned int len)
{
    (void *)addr;
}

void metal_generic_default_poll(void)
{
	/* Add implementation here */
}

void *metal_machine_io_mem_map(void *va, metal_phys_addr_t pa,
			       size_t size, unsigned int flags)
{
	return va;
}
