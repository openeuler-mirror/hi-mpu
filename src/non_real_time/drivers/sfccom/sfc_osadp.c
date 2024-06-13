// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc os diff
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"

void sfc_osadp_mutex_init(void *lock_info)
{
#ifdef RUNNING_ENV_LINUX
    struct mutex *lock = (struct mutex *)(lock_info);
    mutex_init(lock);
#else
#endif
}

void sfc_osadp_mutex_lock(void *lock_info)
{
#ifdef RUNNING_ENV_LINUX
    struct mutex *lock = (struct mutex *)(lock_info);
    mutex_lock(lock);
#else
#endif
}

void sfc_osadp_mutex_unlock(void *lock_info)
{
#ifdef RUNNING_ENV_LINUX
    struct mutex *lock = (struct mutex *)(lock_info);
    mutex_unlock(lock);
#else
#endif
}

void sfc_osadp_mutex_destroy(void *lock_info)
{
#ifdef RUNNING_ENV_LINUX
    struct mutex *lock = (struct mutex *)(lock_info);
    mutex_destroy(lock);
#else
#endif
}

void sfc_osadp_iounmap(void __iomem *addr)
{
#ifdef RUNNING_ENV_LINUX
    iounmap(addr);
#else
    addr = NULL;
    (void)addr;
#endif
}

#ifdef RUNNING_ENV_LINUX
static struct device_node *hisfc_drv_get_device_node(const hisfc_core_host *c_host)
{
    struct platform_device *pdev = NULL;
    pdev = hisfc_drv_get_platform_device(c_host);
    if (pdev == NULL) {
        sfc_log(LOG_ERROR_F, SFC_OSADP_C, "hisfc_drv_get_device_node err");
        return NULL;
    }

    return pdev->dev.of_node;
}

s32 hisfc_drv_plat_get_cfg_cs(hisfc_core_host *c_host, u32 *cfg_cs)
{
    s32 ret;
    ret = of_property_read_u32_index(hisfc_drv_get_device_node(c_host), "sfccsid", 0, cfg_cs);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_OSADP_C, "of_property_read_u32_index id failed. ret %d", ret);
        return ret;
    }

    return ret;
}

void __iomem *hisfc_drv_plat_get_reg_base(hisfc_core_host *c_host)
{
    return of_iomap(hisfc_drv_get_device_node(c_host), 0);
}

void __iomem *hisfc_drv_plat_get_io_base(hisfc_core_host *c_host)
{
    return of_iomap(hisfc_drv_get_device_node(c_host), 1);
}

s8 *hisfc_drv_plat_mem_zalloc(hisfc_core_host *c_host, u32 mtype, u32 size)
{
    return (s8 *)kzalloc(size, GFP_KERNEL);
}

void hisfc_drv_plat_mem_free(hisfc_core_host *c_host, u32 mtype, const u8 *ptr)
{
    kfree(ptr);

    return;
}

dma_addr_t hisfc_drv_plat_dma_map(const hisfc_core_host *c_host, u8 *dma_buf,
                                  size_t size, enum sfc_dma_type type)
{
    struct platform_device *pdev = NULL;
    struct device *cdev = NULL;
    s32 dma_type;

    dma_type = (type == SFC_DMA_TO_DEVICE) ? DMA_TO_DEVICE : DMA_FROM_DEVICE;
    pdev = hisfc_drv_get_platform_device(c_host);
    if (pdev == NULL) {
        return 0;
    }
    cdev = &pdev->dev;

    return sfc_osadp_dma_map_single(cdev, dma_buf, size, dma_type);
}

void hisfc_drv_plat_dma_unmap(const hisfc_core_host *c_host, dma_addr_t addr,
                              size_t size, enum sfc_dma_type type)
{
    struct platform_device *pdev = NULL;
    struct device *cdev = NULL;
    s32 dma_type;

    dma_type = (type == SFC_DMA_TO_DEVICE) ? DMA_TO_DEVICE : DMA_FROM_DEVICE;
    pdev = hisfc_drv_get_platform_device(c_host);
    if (pdev == NULL) {
        return;
    }
    cdev = &pdev->dev;

    sfc_osadp_dma_unmap_single(cdev, addr, size, dma_type);

    return;
}

#else

const sfc_diff_mgr g_diff_mgr = {
    .dev_info = {
        { 0x08640000, 0x60000000, 0x0, 0x3, 76 },
        { 0x08600000, 0x70000000, 0x1, 0x3, 29 }
    },
};

s32 hisfc_drv_plat_get_cfg_cs(hisfc_core_host *c_host, u32 *cfg_cs)
{
    if ((c_host == NULL) || (c_host->bus >= SFC_CTRL_CNT)) {
        sfc_log(LOG_ERROR_F, SFC_OSADP_C, "hisfc_drv_plat_get_cfg_cs err");
        return -EFAULT;
    }

    *cfg_cs = g_diff_mgr.dev_info[c_host->bus].sfccsid;

    return 0;
}

void __iomem *hisfc_drv_plat_get_reg_base(hisfc_core_host *c_host)
{
    if ((c_host == NULL) || (c_host->bus >= SFC_CTRL_CNT)) {
        sfc_log(LOG_ERROR_F, SFC_OSADP_C, "hisfc_drv_plat_get_reg_base err");
        return NULL;
    }

    return (void *)g_diff_mgr.dev_info[c_host->bus].reg_base;
}

void __iomem *hisfc_drv_plat_get_io_base(hisfc_core_host *c_host)
{
    if ((c_host == NULL) || (c_host->bus >= SFC_CTRL_CNT)) {
        sfc_log(LOG_ERROR_F, SFC_OSADP_C, "hisfc_drv_plat_get_reg_base err");
        return NULL;
    }

    return (void *)g_diff_mgr.dev_info[c_host->bus].bus_buf_base;
}

s8 *hisfc_drv_plat_mem_zalloc(hisfc_core_host *c_host, u32 mtype, u32 size)
{
    return NULL;
}

void hisfc_drv_plat_mem_free(hisfc_core_host *c_host, u32 mtype, const u8 *ptr)
{
    return;
}

dma_addr_t hisfc_drv_plat_dma_map(const hisfc_core_host *c_host, u8 *dma_buf,
                                  size_t size, enum sfc_dma_type type)
{
    return sfc_osadp_dma_map_single(NULL, dma_buf, size, dma_type);
}

void hisfc_drv_plat_dma_unmap(const hisfc_core_host *c_host, dma_addr_t addr,
                              size_t size, enum sfc_dma_type type)
{
    sfc_osadp_dma_unmap_single(NULL, addr, size, dma_type);

    return;
}

#endif

sfc_plat_operations g_sfc_platdrv_ops = {
    .plat_get_cfg_cs = hisfc_drv_plat_get_cfg_cs,
    .plat_get_irq_num = NULL,
    .plat_get_reg_base = hisfc_drv_plat_get_reg_base,
    .plat_get_io_base = hisfc_drv_plat_get_io_base,
    .plat_mem_zalloc = hisfc_drv_plat_mem_zalloc,
    .plat_mem_free = hisfc_drv_plat_mem_free,
    .plat_dma_map = hisfc_drv_plat_dma_map,
    .plat_dma_unmap = hisfc_drv_plat_dma_unmap,
    .plat_ioremap_wc = NULL,
    .plat_modify_mtd_cb = NULL,
};
