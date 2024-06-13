// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc os diff header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _SFC_OSADP_H_
#define _SFC_OSADP_H_

#define CHIP_HI1711       0x1711
#define LINUX_VER_4_1_19  0x4119
#define RUNNING_ENV_LINUX (LINUX_VER_4_1_19)

#ifdef RUNNING_ENV_LINUX
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/string_helpers.h>
#include <linux/compiler_types.h>
#include <linux/compiler_attributes.h>

#include <asm/io.h>
#include <asm/setup.h>
#include <asm/uaccess.h>
#include "comm_knl_api.h"
#include "sfc_diff.h"
#else
#include "typedef.h"
#include "io_defs.h"
#include "err_num.h"
#include "print.h"
#include "securec.h"
#include "jiffies.h"
#include "delay.h"
#include "typecheck.h"
#include "sfc_m3_addr_define.h"
#include "sfc_m3_cfi.h"
#include "sfc_m3_diff.h"
#endif

#ifdef RUNNING_ENV_SCM3

struct device_node {
    u32 rsv;
};

struct device {
    u32 rsv;
};

struct mutex {
    u32 rsv;
};
typedef struct {
    u32 rsv;
} wait_queue_head_t;

static inline int time_after_eq(unsigned long a, unsigned long b)
{
    return ((long)((a) - (b)) >= 0);
}
#endif

typedef enum SFC_LOG_LEVEL {
    LOG_EMERG_F = 0,
    LOG_ERROR_F,
    LOG_WARN_F,
    LOG_PRINT_F,
    LOG_INFO_F
} sfc_log_level_enum;

#define SFC_API_C               0x2f
#define SFC_OSADP_C             0x30
#define SFC_SYS_FUNC_C          0x31
#define SFC_CORE_C              0x32
#define SFC_PLAT_C              0x33
#define SFC_GODBOX_C            0x34
#define SFC_SPI_IDS_C           0x35
#define SFC_FLASH_DEVICES_OPS_C 0x36
#define SFC_IDS_C               0x37

#ifdef RUNNING_ENV_LINUX
#define lev2linux(sfc_lv)                         (sfc_lv)
#define sfc_log(sfc_lev, file_code, X, format...) LOG(lev2linux(sfc_lev), X, ##format)
#define sfc_osadp_dma_map_single(d, a, s, r)      dma_map_single(d, a, s, r)
#define sfc_osadp_dma_unmap_single(d, a, s, r)    dma_unmap_single(d, a, s, r)
#define sfc_osadp_mb()                            mb()
#define sfc_osadp_init_waitqueue_head(q)          init_waitqueue_head(q)
#include "sfc0_core.h"
extern struct platform_device *hisfc_drv_get_platform_device(const hisfc_core_host *c_host);
#elif defined(RUNNING_ENV_SCM3)
#define lev2m3(sfc_lv)                            (sfc_lv)
#define sfc_log(sfc_lev, file_code, X, format...) LOG(lev2m3(sfc_lev), file_code, X, ##format)
#define sfc_osadp_dma_map_single(d, a, s, r)      (u32)(a)
#define sfc_osadp_dma_unmap_single(d, a, s, r)
#define sfc_osadp_mb()
#define cond_resched()
#define kzalloc(size, flags) NULL
#define sfc_osadp_init_waitqueue_head(q)
#include "sfc_m3_core.h"
#endif

void sfc_osadp_mutex_init(void *lock_info);
void sfc_osadp_mutex_lock(void *lock_info);
void sfc_osadp_mutex_unlock(void *lock_info);
void sfc_osadp_mutex_destroy(void *lock_info);
void sfc_osadp_iounmap(void __iomem *addr);

s32 hisfc_drv_plat_get_cfg_cs(hisfc_core_host *c_host, u32 *cfg_cs);
void __iomem *hisfc_drv_plat_get_reg_base(hisfc_core_host *c_host);
void __iomem *hisfc_drv_plat_get_io_base(hisfc_core_host *c_host);

s8 *hisfc_drv_plat_mem_zalloc(hisfc_core_host *c_host, u32 mtype, u32 size);
void hisfc_drv_plat_mem_free(hisfc_core_host *c_host, u32 mtype, const u8 *ptr);
dma_addr_t hisfc_drv_plat_dma_map(const hisfc_core_host *c_host, u8 *dma_buf, size_t size, enum sfc_dma_type type);
void hisfc_drv_plat_dma_unmap(const hisfc_core_host *c_host, dma_addr_t addr, size_t size, enum sfc_dma_type type);

#endif
