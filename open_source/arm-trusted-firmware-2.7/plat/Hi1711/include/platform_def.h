/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: plat_def
 * Author: xxx
 * Create: 2022-11-16
 */

#ifndef __PLATFORM_DEF_H__
#define __PLATFORM_DEF_H__

#include <arch.h>
#include "../hi1711_def.h"

/*
 * Generic platform constants
 */
#define PLATFORM_STACK_SIZE 0x800 /* Size of cacheable stacks */

#define FIRMWARE_WELCOME_STR "Booting Trusted Firmware\n"

#define PLATFORM_CACHE_LINE_SIZE        64
#define PLATFORM_CLUSTER_COUNT          1
#define PLATFORM_CORE_COUNT_PER_CLUSTER 4
#define PLATFORM_CORE_MAX_NUM           4

#define PLAT_MAX_RET_STATE          1
#define PLAT_MAX_OFF_STATE          2
#define PLAT_MAX_CLUSTER_IDLE_STATE 3

#define PLATFORM_CORE_COUNT (PLATFORM_CLUSTER_COUNT * PLATFORM_CORE_COUNT_PER_CLUSTER)
#define PLAT_MAX_PWR_LVL     MPIDR_AFFLVL2

#define MAX_IO_DEVICES 3
#define MAX_IO_HANDLES 4
/* UFS RPMB and UFS User Data */
#define MAX_IO_BLOCK_DEVICES 2

/*
 * BL31 specific defines.
 */
#define BL31_BASE           0x87100000
#define BL31_PROGBITS_LIMIT 0x87150000
#define BL31_LIMIT          (0x87100000 + 0x100000)

/*******************************************************************************
 * Load address of BL3-3 in the Hi1910 port
 ******************************************************************************/
#define NS_IMAGE_OFFSET 0x1000
#define NS_IMAGE_SIZE   0x04800000

#define SCP_MEM_BASE 0x89C80000
#define SCP_MEM_SIZE 0x00040000

#define PLAT_PHY_ADDR_SPACE_SIZE    (1ULL << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE   (1ULL << 32)

#define CACHE_WRITEBACK_SHIFT   6
#define CACHE_WRITEBACK_GRANULE (1 << CACHE_WRITEBACK_SHIFT)

#if IMAGE_BL1 || IMAGE_BL2 || IMAGE_BL31
#define MAX_XLAT_TABLES 8
#endif

#define MAX_MMAP_REGIONS 16

#endif /* __PLATFORM_DEF_H__ */
