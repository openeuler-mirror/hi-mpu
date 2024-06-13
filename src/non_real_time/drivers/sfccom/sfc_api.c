// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spi flash api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "sfc0_core.h"
#include "sfc350.h"
#include "sfc_api.h"

hisfc_mgr g_sfc_mgr = { 0 };

s32 hisfc_bus_check(u32 bus)
{
    if (bus >= SFC_CTRL_CNT) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "bus %u out of range!", bus);
        return -EINVAL;
    }

    return 0;
}

hisfc_core_host *hisfc_get_core_host(u32 bus)
{
    if (bus >= SFC_CTRL_CNT) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "bus id out of range!");
        bus = 0;
    }

    return &g_sfc_mgr.mgr_core_host[bus];
}

void *hisfc_get_plat_data(const hisfc_core_host *c_host)
{
    return c_host->platform_data;
}

static s32 hisfc_plat_get_cfg_cs(hisfc_core_host *c_host, u32 *cfg_cs)
{
    if (!c_host->plat_ops->plat_get_cfg_cs) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_get_cfg_cs is NULL!");
        return -EFAULT;
    }

    return c_host->plat_ops->plat_get_cfg_cs(c_host, cfg_cs);
}

static void __iomem *hisfc_plat_get_reg_base(hisfc_core_host *c_host)
{
    if (!c_host->plat_ops->plat_get_reg_base) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_get_reg_base not support!");
        return NULL;
    }

    return c_host->plat_ops->plat_get_reg_base(c_host);
}

static void __iomem *hisfc_plat_get_io_base(hisfc_core_host *c_host)
{
    if (!c_host->plat_ops->plat_get_io_base) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_get_io_base not support!");
        return NULL;
    }

    return c_host->plat_ops->plat_get_io_base(c_host);
}

static s8 *hisfc_plat_mem_zalloc(hisfc_core_host *c_host, u32 mtype, u32 size)
{
    if (!c_host->plat_ops->plat_mem_zalloc) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_mem_zalloc not support!");
        return 0;
    }

    return c_host->plat_ops->plat_mem_zalloc(c_host, mtype, size);
}

static void hisfc_plat_mem_free(hisfc_core_host *c_host, u32 mtype, const u8 *ptr)
{
    if (!c_host->plat_ops->plat_mem_free) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_mem_free not support!");
        return;
    }

    c_host->plat_ops->plat_mem_free(c_host, mtype, ptr);

    return;
}

static dma_addr_t hisfc_plat_dma_map(const hisfc_core_host *c_host, u8 *dma_buf, size_t size,
                                     enum sfc_dma_type type)
{
    if (!c_host->plat_ops->plat_dma_map) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_dma_map not support!");
        return 0;
    }

    return c_host->plat_ops->plat_dma_map(c_host, dma_buf, size, type);
}

static void hisfc_plat_dma_unmap(const hisfc_core_host *c_host, dma_addr_t addr,
                                 size_t size, enum sfc_dma_type type)
{
    if (!c_host->plat_ops->plat_dma_unmap) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_dma_unmap not support!");
        return;
    }

    c_host->plat_ops->plat_dma_unmap(c_host, addr, size, type);

    return;
}

static void hisfc_plat_modify_mtd_cb(const hisfc_core_host *c_host, u8 *plat_data, u32 cs, u32 is_write, u32 mtd_flag)
{
    if (!c_host->plat_ops->plat_modify_mtd_cb) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "plat_modify_mtd_cb not support!");
        return;
    }

    c_host->plat_ops->plat_modify_mtd_cb(plat_data, c_host->bus, cs, is_write, mtd_flag);

    return;
}

void hisfc_core_register_chip_ops(u32 bus, const sfc_chip_operations *ops)
{
    hisfc_get_core_host(bus)->chip_ops = ops;

    return;
}

void hisfc_core_register_plat_ops(u32 bus, const sfc_plat_operations *ops)
{
    hisfc_get_core_host(bus)->plat_ops = ops;

    return;
}

static s32 hisfc_core_check_plat_callback(u32 bus)
{
    hisfc_core_host *c_host = NULL;
    const sfc_plat_operations *p_ops = NULL;

    c_host = hisfc_get_core_host(bus);
    if (c_host == NULL) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_core_check_plat_callback sfc%u is NULL!", bus);
        return -EFAULT;
    }
    p_ops = c_host->plat_ops;
    if ((p_ops->plat_mem_zalloc == NULL) || (p_ops->plat_mem_free == NULL) || (p_ops->plat_modify_mtd_cb == NULL)) {
        sfc_log(LOG_PRINT_F, SFC_API_C, "warning:mem opr sfc%u not support!", bus);
    }

    if ((p_ops->plat_get_cfg_cs == NULL) || (p_ops->plat_get_reg_base == NULL) || (p_ops->plat_get_io_base == NULL) ||
        (p_ops->plat_dma_map == NULL) || (p_ops->plat_dma_unmap == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%u plat some opr need cfg, plese check!", bus);
        return -EFAULT;
    }

    return 0;
}

static s32 hisfc_core_check_chip_callback(u32 bus)
{
    hisfc_core_host *c_host = NULL;
    const sfc_chip_operations *c_ops = NULL;

    c_host = hisfc_get_core_host(bus);
    c_ops = c_host->chip_ops;
    if (c_ops == NULL) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "chip_ops sfc%u not set!", bus);
        return -EFAULT;
    }
    if ((c_ops->sfc_clock_switch_ctrl == NULL) || (c_ops->sfc_set_clock == NULL)) {
        sfc_log(LOG_PRINT_F, SFC_API_C, "warning:some clock operator sfc%u not support!", bus);
    }

    if ((c_ops->sfc_reset_handle == NULL) || (c_ops->sfc_set_iodrive == NULL)) {
        sfc_log(LOG_PRINT_F, SFC_API_C, "warning:reset or iodrive sfc%u not support!", bus);
    }

    if (c_ops->sfc_init_extend_addr_mode == NULL) {
        sfc_log(LOG_PRINT_F, SFC_API_C, "warning:extend_addr_mode sfc%u not init!", bus);
    }

    if (c_ops->sfc_check_permission == NULL) {
        sfc_log(LOG_PRINT_F, SFC_API_C, "warning:sfc_check_permission sfc%u not init!", bus);
    }

    if ((c_ops->sfc_get_dma_cfg_addr == NULL) || (c_ops->sfc_set_iocfg == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%u iocfg or dma_cfg not set,plese check!", bus);
        return -EINVAL;
    }

    return 0;
}

static s32 hisfc_core_check_callback(u32 bus)
{
    s32 ret;

    ret = hisfc_core_check_chip_callback(bus);
    if (ret) {
        return ret;
    }

    return hisfc_core_check_plat_callback(bus);
}

static s32 hisfc_set_iocfg(hisfc_core_host *c_host, u32 cs, u32 enable)
{
    if (!c_host->chip_ops->sfc_set_iocfg) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%ucs%u sfc_set_iocfg not support!", c_host->bus, cs);
        return -EINVAL;
    }

    return c_host->chip_ops->sfc_set_iocfg(c_host->bus, cs, enable);
}

static void hisfc_set_iodrive(hisfc_core_host *c_host, u32 cs)
{
    if (!c_host->chip_ops->sfc_set_iodrive) {
        sfc_log(LOG_WARN_F, SFC_API_C, "sfc%ucs%u sfc_set_iodrive not support!", c_host->bus, cs);
        return;
    }

    c_host->chip_ops->sfc_set_iodrive(c_host->bus, cs);

    return;
}

static s32 hisfc_sfc_check_permission(hisfc_core_host *c_host, u32 bus)
{
    if (c_host->chip_ops == NULL) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%u no permission!", c_host->bus);
        return -EACCES;
    }

    if (c_host->chip_ops->sfc_check_permission == NULL) {
        /* If this parameter is not set, the check is not performed by default. */
        return 0;
    }

    return c_host->chip_ops->sfc_check_permission(c_host->bus);
}

s32 hisfc_set_clk(u32 bus_id, u32 sfc_clk)
{
    s32 ret;
    hisfc_core_host *c_host = NULL;

    ret = hisfc_bus_check(bus_id);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "clk sfc_spec para_check fail!");
        return ret;
    }

    c_host = hisfc_get_core_host(bus_id);
    if (!c_host->chip_ops->sfc_set_clock) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%u sfc_set_clock not support!", bus_id);
        return -1;
    }

    return c_host->chip_ops->sfc_set_clock(bus_id, sfc_clk);
}

s32 hisfc_module_reset(u32 bus)
{
    hisfc_core_host *c_host = NULL;

    c_host = hisfc_get_core_host(bus);
    if (!c_host->chip_ops->sfc_reset_handle) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%u sfc_reset_handle not support!", bus);
        return -1;
    }

    return c_host->chip_ops->sfc_reset_handle(bus);
}

static void hisfc_init_extend_addr_mode(hisfc_core_host *c_host)
{
    if (!c_host->chip_ops->sfc_init_extend_addr_mode) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc_init_extend_addr_mode not support!");
        return;
    }

    c_host->chip_ops->sfc_init_extend_addr_mode();

    return;
}

u32 hisfc_get_dma_cfg_ddr(u32 bus_id, dma_addr_t phy_addr)
{
    hisfc_core_host *c_host = NULL;

    c_host = hisfc_get_core_host(bus_id);
    if (!c_host->chip_ops->sfc_get_dma_cfg_addr) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc_get_dma_cfg_addr not cfg");
        return (u32)phy_addr;
    }

    return c_host->chip_ops->sfc_get_dma_cfg_addr(bus_id, phy_addr);
}

u32 hisfc_get_com_res_init_sta(void)
{
    return g_sfc_mgr.com_res_init_flag;
}

void hisfc_set_com_res_init_sta(u32 init_flag)
{
    g_sfc_mgr.com_res_init_flag = init_flag;

    return;
}

s32 hisfc_reg_read(sfc_bus_info info, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    s32 ret;
    hisfc_spi *spi = NULL;
    hisfc_core_host *c_host = NULL;

    *retlen = 0;
    c_host = hisfc_get_core_host(info.bus);

    ret = hisfc_sfc_check_permission(c_host, info.bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_reg_read bus %u no permission", info.bus);
        return ret;
    }

    spi = &c_host->spi[info.cs];

    sfc_osadp_mutex_lock(&c_host->lock);
    hisfc_addr_mode_set_support_test(spi, c_host);
    ret = sfc_core_reg_read_for_all(spi, from, len, retlen, buf);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc_core_reg_read failed %d", ret);
    }
    sfc_osadp_mutex_unlock(&c_host->lock);

    return ret;
}

s32 hisfc_reg_write(sfc_bus_info info, loff_t to, size_t len, size_t *retlen, const u8 *data)
{
    s32 ret;
    hisfc_spi *spi = NULL;
    hisfc_core_host *c_host = NULL;

    *retlen = 0;
    c_host = hisfc_get_core_host(info.bus);

    ret = hisfc_sfc_check_permission(c_host, info.bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_reg_write bus %u no permission", info.bus);
        return ret;
    }

    spi = &(c_host->spi[info.cs]);

    sfc_osadp_mutex_lock(&c_host->lock);
    hisfc_addr_mode_set_support_test(spi, c_host);
    ret = sfc_core_reg_write(spi, to, len, retlen, data);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc_core_reg_write failed %d", ret);
    }
    sfc_osadp_mutex_unlock(&c_host->lock);

    return ret;
}

s32 hisfc_bus_read(sfc_bus_info info, loff_t addr, size_t len, size_t *retlen, u8 *buf)
{
    s32 ret;
    hisfc_spi *spi = NULL;
    hisfc_core_host *c_host = NULL;

    *retlen = 0;
    c_host = hisfc_get_core_host(info.bus);

    ret = hisfc_sfc_check_permission(c_host, info.bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_bus_read bus %u no permission", info.bus);
        return ret;
    }

    spi = &c_host->spi[info.cs];
    if ((buf == NULL) || (sfc_core_bus_addr_align_detect(addr))) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_bus_read input err, data is NULL");
        return -EINVAL;
    }

    ret = sfc_core_check_addr_len(spi, addr, len, 0);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_bus_read sfc_core_check_addr_len failed %d", ret);
        if (ret == SFC_RW_LEN_ZERO) {
            return 0;
        }

        return ret;
    }

    sfc_osadp_mutex_lock(&c_host->lock);
    hisfc_addr_mode_set_support_test(spi, c_host);
    ret = spi->driver->wait_ready(spi, SPI_OPR_NOLOCK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "SPIFlash is WIP!");
        goto exit;
    }

    (void)spi->driver->bus_prepare(spi, SFC_SERACH_READ);
    ret = memcpy_s((void *)buf, len, (void *)(spi->iobase + addr), len);
    if (ret != 0) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "mem cpy error,%d", ret);
        goto exit;
    }

    *retlen = len;

exit:
    sfc_osadp_mutex_unlock(&c_host->lock);

    return ret;
}

s32 hisfc_bus_write(sfc_bus_info info, loff_t to, size_t len, size_t *retlen, const u8 *buf)
{
    s32 result;
    hisfc_spi *spi = NULL;
    hisfc_core_host *c_host = NULL;

    *retlen = 0;
    c_host = hisfc_get_core_host(info.bus);

    result = hisfc_sfc_check_permission(c_host, info.bus);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_bus_write bus %u no permission", info.bus);
        return result;
    }

    spi = &(c_host->spi[info.cs]);
    if ((buf == NULL) || (sfc_core_bus_addr_align_detect(to))) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "bus write buf is NULL");
        return -EINVAL;
    }

    result = sfc_core_check_addr_len(spi, to, len, 0);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_bus_write sfc_core_check_addr_len failed %d", result);
        if (result == SFC_RW_LEN_ZERO) {
            return 0;
        }

        return result;
    }

    sfc_osadp_mutex_lock(&c_host->lock);
    hisfc_addr_mode_set_support_test(spi, c_host);
    result = spi->driver->wait_ready(spi, SPI_OPR_NOLOCK);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_bus_write wait ready failed %d", result);
        goto exit;
    }

    (void)spi->driver->bus_prepare(spi, SFC_SERACH_WRITE);

    sfc_core_bus_write_enable(c_host, SFC_ENABLE);
    result = memcpy_s((void *)(spi->iobase + to), len, (void *)buf, len);
    if (result != 0) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "mem cpy error,%d", result);
        goto exit;
    }
    sfc_core_bus_write_enable(c_host, SFC_DISABLE);

    *retlen = len;

exit:
    sfc_osadp_mutex_unlock(&c_host->lock);

    return result;
}

void hisfc_addr_mode_set_support_test(hisfc_spi *spi, hisfc_core_host *c_host)
{
    sfc_core_addr_mode_set(spi);

    return;
}

s32 hisfc_reg_erase(sfc_bus_info info, unsigned long long offset, unsigned long long length)
{
    hisfc_spi *spi = NULL;
    s32 ret;
    hisfc_core_host *c_host = NULL;

    c_host = hisfc_get_core_host(info.bus);

    ret = hisfc_sfc_check_permission(c_host, info.bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_reg_erase bus %u no permission", info.bus);
        return ret;
    }

    spi = &(c_host->spi[info.cs]);

    sfc_osadp_mutex_lock(&c_host->lock);
    hisfc_addr_mode_set_support_test(spi, c_host);
    if (spi->driver->erase_process != NULL) {
        ret = spi->driver->erase_process(spi, offset, length, SPI_OPR_NOLOCK);
    } else {
        ret = sfc_core_reg_erase(spi, offset, length);
    }

    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc_core_reg_erase failed %d", ret);
    }

    sfc_osadp_mutex_unlock(&c_host->lock);

    return ret;
}

void hisfc_dma_cmd_mode2rw_mode(u32 cmd_mode, u8 *is_read)
{
    *is_read = SFC_SERACH_WRITE;

    if ((cmd_mode == SFC_DMA_READ) || (cmd_mode == SFC_DMA_SEC_READ)) {
        *is_read = SFC_SERACH_READ;
    }

    return;
}

static void hisfc_dma_transfer(const hisfc_core_host *c_host, u32 spi_start_addr,
                               u8 *dma_buffer, u8 is_read, u32 size, u8 cs)
{
    dma_addr_t addr;
    u32 bus = c_host->bus;
    dma_rw_addr rm_addr;

    if (is_read) {
        addr = hisfc_plat_dma_map(c_host, dma_buffer, HISFC350_DMA_MAX_SIZE, SFC_DMA_FROM_DEVICE);
    } else {
        addr = hisfc_plat_dma_map(c_host, dma_buffer, HISFC350_DMA_MAX_SIZE, SFC_DMA_TO_DEVICE);
    }

    rm_addr = (dma_rw_addr){hisfc_get_dma_cfg_ddr(bus, addr), spi_start_addr,
                                  size, is_read, cs};
    sfc_core_dma_rw_config(c_host, rm_addr);
    sfc_osadp_mb();
    sfc_core_dma_wait_cpu_finish(c_host);
    if (is_read) {
        hisfc_plat_dma_unmap(c_host, addr, HISFC350_DMA_MAX_SIZE, SFC_DMA_FROM_DEVICE);
    } else {
        hisfc_plat_dma_unmap(c_host, addr, HISFC350_DMA_MAX_SIZE, SFC_DMA_TO_DEVICE);
    }

    return;
}

static void hisfc_dma_buffer_process(u8 *src_buf, u8 **dest_buf, u32 dma_op_mode, u32 op_num)
{
#ifdef RUNNING_ENV_LINUX
    int ret;

    if (dma_op_mode == SFC_DMA_WRITE) {
        ret = memcpy_s(*dest_buf, HISFC350_DMA_MAX_SIZE, src_buf, op_num);
    } else if (dma_op_mode == SFC_DMA_READ) {
        ret = memcpy_s(src_buf, op_num, *dest_buf, op_num);
    } else {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_dma_buffer_process opmode err!");
        return;
    }
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "buffer_process opmode %u failed!", dma_op_mode);
    }
#else
    if (dma_op_mode == SFC_DMA_WRITE) {
        *dest_buf = src_buf;
    }
#endif

    return;
}

static s32 hisfc_reg_rw_for_dma(hisfc_spi *spi, loff_t addr, size_t len, size_t *retlen, u8 *buf, u8 is_read)
{
    s32 ret;

    if (is_read) {
        ret = sfc_core_reg_read_for_all(spi, addr, len, retlen, buf);
    } else {
        ret = sfc_core_reg_write(spi, addr, len, retlen, buf);
    }

    return ret;
}

static s32 hisfc_base_dma_rw_xfer_data(hisfc_core_host *c_host, u32 cs, loff_t to,
                                       size_t len, size_t *retlen, u8 *buf, u8 is_read)
{
    u32 num;
    s32 ret = 0;
    size_t retlen_tmp = 0;
    u8 *ptr = (u8 *)buf;
    hisfc_spi *spi = NULL;

    spi = (hisfc_spi *)(&(c_host->spi[cs]));

    if ((u32)to & HISFC350_DMA_ALIGN_MASK) {
        /* Remaining space of the first 256 bytes */
        num = HISFC350_DMA_ALIGN_SIZE - ((u32)to & HISFC350_DMA_ALIGN_MASK);
        /* The remaining space is greater than the actual number of bytes to be operated. */
        if (num > len) {
            num = len;
        }

        ret = hisfc_reg_rw_for_dma(spi, to, num, retlen, ptr, is_read);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "dma_xfer_data isread:%u failed %d.", is_read, ret);
            return ret;
        }
        to += num;
        ptr += num;
        len -= num;
    }

    while (len >= HISFC350_DMA_MIN_SIZE) {
        num = ((len >= HISFC350_DMA_MAX_SIZE) ? HISFC350_DMA_MAX_SIZE : len);
        num = num - (num % HISFC350_DMA_MIN_SIZE);
        if (is_read == 0) {
            hisfc_dma_buffer_process(ptr, (u8 **)(&c_host->buffer), SFC_DMA_WRITE, num);
        }
        hisfc_dma_transfer(c_host, (u32)to, (u8 *)c_host->buffer, is_read, num, (u8)spi->chipselect);
        if (is_read != 0) {
            hisfc_dma_buffer_process(ptr, (u8 **)(&c_host->buffer), SFC_DMA_READ, num);
        }
        to += num;
        ptr += num;
        len -= num;
    }

    if (len > 0) {
        ret = hisfc_reg_rw_for_dma(spi, to, len, &retlen_tmp, ptr, is_read);
        if (ret) {
            *retlen = (size_t)(ptr - buf) + retlen_tmp;
            sfc_log(LOG_ERROR_F, SFC_API_C, "dma_xfer_data isread:%u failed %d.", is_read, ret);
            return ret;
        }
        ptr += len;
    }

    *retlen = (size_t)(ptr - buf);

    return ret;
}

static s32 hisfc_base_dma_rw_xfer(hisfc_core_host *c_host, u32 cs, loff_t to,
                                  size_t len, size_t *retlen, u8 *buf, u32 dma_op_mode)
{
    s32 result;
    u8 is_read = SFC_SERACH_WRITE;
    hisfc_spi *spi = NULL;

    if (c_host == NULL) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_base_dma_rw_xfer para failed");
        return -EINVAL;
    }

    result = hisfc_sfc_check_permission(c_host, c_host->bus);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "dma opr %u bus %u no permission", dma_op_mode, c_host->bus);
        return result;
    }

    spi = (hisfc_spi *)(&(c_host->spi[cs]));
    hisfc_dma_cmd_mode2rw_mode(dma_op_mode, &is_read);
    if (sfc_core_check_addr_len(spi, to, len, 0)) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc_core_check_addr_len err. mode %u", is_read);
        return -EINVAL;
    }

    *retlen = 0;
    if (!len) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "wr length is 0. mode %u", is_read);
        return 0;
    }

    sfc_osadp_mutex_lock((void *)(&c_host->lock));
    hisfc_addr_mode_set_support_test(spi, c_host);
    if (spi->driver->wait_ready(spi, SPI_OPR_NOLOCK)) {
        goto exit;
    }

    if (dma_op_mode == SFC_DMA_WRITE) {
        (void)spi->driver->write_enable(spi);
    }

    /*
     * The DMA operation and bus operation share the register,
     * which is used to configure the read/write parameters and iftype.
     */
    (void)spi->driver->bus_prepare(spi, is_read);
    result = hisfc_base_dma_rw_xfer_data(c_host, cs, to, len, retlen, buf, is_read);

exit:
    sfc_osadp_mutex_unlock((void *)(&c_host->lock));

    return result;
}

s32 hisfc_dma_read(sfc_bus_info info, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    return hisfc_base_dma_rw_xfer(hisfc_get_core_host(info.bus), info.cs, from, len, retlen, buf, SFC_DMA_READ);
}

s32 hisfc_dma_write(sfc_bus_info info, loff_t to, size_t len, size_t *retlen, const u8 *buf)
{
    return hisfc_base_dma_rw_xfer(hisfc_get_core_host(info.bus), info.cs, to, len, retlen, (u8 *)buf, SFC_DMA_WRITE);
}

static void hisfc_lock_destroy(u32 bus)
{
    if (sfc_core_bus_is_used(bus, hisfc_get_core_host(bus)->cfg_use_cs)) {
        sfc_osadp_mutex_destroy(&hisfc_get_core_host(bus)->lock);
    }

    return;
}

void hisfc_res_unmap(u32 bus)
{
    hisfc_core_host *c_host = NULL;

    c_host = hisfc_get_core_host(bus);
    if (c_host->regbase) {
        sfc_osadp_iounmap(c_host->regbase);
    }

    if (c_host->iobase) {
        sfc_osadp_iounmap(c_host->iobase);
    }

    return;
}

static void hisfc_core_host_init(u32 bus)
{
    s32 ret;
#ifdef RUNNING_ENV_SCM3
    const s8 temp_name[SFC_CTRL_CNT][MAX_SFC_NAME_LEN] = { "sfc0", "sfc1" };
#endif
    if (hisfc_get_com_res_init_sta() == COM_NOT_INIT) {
        if (memset_s(&g_sfc_mgr, sizeof(hisfc_mgr), 0, sizeof(hisfc_mgr))) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "memset_s g_sfc_mgr init failed");
        }
        hisfc_set_com_res_init_sta(COM_INIT_OK);
    }
#ifdef RUNNING_ENV_SCM3
    ret = memcpy_s(g_sfc_mgr.mgr_core_host[bus].sfc_name, MAX_SFC_NAME_LEN, temp_name[bus], strlen(temp_name[bus]));
    if (ret != 0) {
        sfc_log(LOG_WARN_F, SFC_API_C, "sfc%u name cpy error", bus);
    }
#else
    ret = snprintf_s(g_sfc_mgr.mgr_core_host[bus].sfc_name, MAX_SFC_NAME_LEN, SFC_NAME_LEN, "sfc%u", bus);
    if (ret < 0) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "sfc%u name init error", bus);
    }
#endif

    return;
}

s32 hisfc_module_res_init(u32 bus, platform_init_callback plat_init, void *plat_data)
{
    s32 result;
    u32 cfg_cs_id = 0;
    hisfc_core_host *c_host = NULL;

    hisfc_core_host_init(bus);
    /* platform ressource init */
    if (plat_init) {
        plat_init(plat_data);
    }

    c_host = hisfc_get_core_host(bus);
    result = hisfc_plat_get_cfg_cs(c_host, &cfg_cs_id);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_plat_get_cfg_cs id failed. ret %d", result);
        return result;
    }
    hisfc_init_extend_addr_mode(c_host);
    c_host->cfg_use_cs = cfg_cs_id;
    c_host->bus = bus;
    if (sfc_core_bus_is_used(bus, cfg_cs_id)) {
        sfc_osadp_init_waitqueue_head(&c_host->sfc_wait_queue);
        sfc_osadp_mutex_init(&c_host->lock);
        if (hisfc_core_check_callback(bus)) {
            goto FAIL;
        }

        c_host->regbase = hisfc_plat_get_reg_base(c_host);
        if (!c_host->regbase) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "spi base reg ioremap failed.");
            goto FAIL;
        }

        /* Flash space: 256 MB */
        c_host->iobase = hisfc_plat_get_io_base(c_host);
        if (!c_host->iobase) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "spi buffer ioremap failed.");
            goto FAIL;
        }

        c_host->buffer = hisfc_plat_mem_zalloc(c_host, SFC_TYPE_HOST_BUF, HISFC350_DMA_MAX_SIZE);
#ifdef RUNNING_ENV_LINUX
        if (c_host->buffer == NULL) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "spi alloc dma buffer failed.");
            goto FAIL;
        }
#endif
        c_host->set_system_clock = hisfc351_set_system_clock;
        c_host->set_host_addr_mode = sfc_core_set_host_addr_mode;
    }

    return 0;

FAIL:
    hisfc_lock_destroy(bus);
    hisfc_res_unmap(bus);

    return result;
}

void hisfc_module_res_exit(u32 bus)
{
    hisfc_core_host *c_host = NULL;

    c_host = hisfc_get_core_host(bus);
    hisfc_lock_destroy(bus);
    hisfc_res_unmap(bus);
    if (c_host->buffer != NULL) {
        hisfc_plat_mem_free(c_host, SFC_TYPE_HOST_BUF, c_host->buffer);
    }

    return;
}

static s32 hisfc_base_probe(hisfc_core_host *c_host)
{
    hisfc351_set_system_clock(c_host, NULL, SFC_TRUE);
    sfc_core_timing_cfg(c_host);
    if (!sfc_core_spi_probe(c_host)) {
        return -1;
    }

    return 0;
}

s32 hisfc_flash_scan_init(u32 bus)
{
    s32 result = 0;
    u32 cs;
    hisfc_core_host *c_host = NULL;

    c_host = hisfc_get_core_host(bus);
    if (sfc_core_bus_is_used(bus, c_host->cfg_use_cs)) {
        sfc_log(LOG_PRINT_F, SFC_API_C, "hisfc_probe start bus %u", bus);
        for (cs = 0; cs < SFC_CS_CNT; cs++) {
            if (!sfc_core_cs_is_used(bus, cs, c_host->cfg_use_cs)) {
                continue;
            }
            (void)hisfc_set_iocfg(c_host, cs, 1);
            hisfc_set_iodrive(c_host, cs);
        }

        /* The default working clock of the SFC controller is 25 MHz. */
        if (hisfc_set_clk(bus, hisfc_get_config_clk_frequency())) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "sfc(%u) set clock error", bus);
            return -EFAULT;
        }

        if (hisfc_base_probe(c_host)) {
            sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_base_probe failed. bus %u", bus);
            return -EFAULT;
        }
        if (c_host->mtd_init) {
            result = c_host->mtd_init(c_host->mtd_data);
            if (result) {
                sfc_log(LOG_ERROR_F, SFC_API_C, "warning:mtd init faild bus %u ret %d", bus, result);
            }
        }
    }

    return result;
}

s32 hisfc_std_init(u32 bus, platform_init_callback plat_init, u8 *plat_data)
{
    s32 result;

    if (bus >= SFC_CTRL_CNT) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_std_init bus %u err", bus);
        return -EFAULT;
    }

    result = hisfc_module_res_init(bus, plat_init, plat_data);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_module_res_init bus %u failed. ret %d", bus, result);
        return result;
    }

    result = hisfc_flash_scan_init(bus);
    if (result) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_flash_scan_init bus %u failed %d", bus, result);
        goto FAIL;
    }

    return 0;
FAIL:
    hisfc_module_res_exit(bus);

    return result;
}

static void hisfc_flash_scan_release(u32 bus)
{
    return;
}

void hisfc_std_release(u32 bus)
{
    hisfc_flash_scan_release(bus);
    hisfc_module_res_exit(bus);

    return;
}

void hisfc_shutdown_single_controler(hisfc_core_host *c_host)
{
    s32 ret;

    ret = hisfc_sfc_check_permission(c_host, c_host->bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "shutdown_single_controler bus %u no permission", c_host->bus);
        return;
    }

    sfc_core_suspend_shutdown_com_process(c_host, 0);

    return;
}

void hisfc_suspend_single_controller(hisfc_core_host *c_host)
{
    s32 ret;

    ret = hisfc_sfc_check_permission(c_host, c_host->bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc suspend controler bus %u no permission", c_host->bus);
        return;
    }

    sfc_core_suspend_shutdown_com_process(c_host, 1);

    return;
}

s32 hisfc_resume(hisfc_core_host *c_host)
{
    s32 ret;

    ret = hisfc_sfc_check_permission(c_host, c_host->bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_resume bus %u no permission", c_host->bus);
        return ret;
    }

    return sfc_core_resume(c_host);
}

s32 hisfc_cmd_set_iftype(sfc_set_iftype *io_iftype)
{
    s32 ret;
    u32 bus_id;
    u32 cs_id;
    hisfc_core_host *c_host = NULL;

    if (io_iftype == NULL) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "hisfc_cmd_set_iftype para is NULL!");
        return -EINVAL;
    }

    ret = sfc_core_com_para_check(&io_iftype->com_info);
    if (ret) {
        return ret;
    }

    ret = sfc_core_iftype_check(io_iftype->com_info.if_type, io_iftype->is_write);
    if (ret) {
        return ret;
    }

    bus_id = io_iftype->com_info.bus;
    cs_id = io_iftype->com_info.cs;

    c_host = hisfc_get_core_host(bus_id);

    ret = hisfc_sfc_check_permission(c_host, bus_id);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_API_C, "set_iftype bus %u no permission", bus_id);
        return ret;
    }
    /* Search first and then map */
    ret = sfc_core_spi_iftype_update(c_host, cs_id, io_iftype->is_write, io_iftype->com_info.if_type);
    /* Modify the mtd debug parameter. */
    if (io_iftype->mtd_flag) {
        c_host->bus = bus_id;
        hisfc_plat_modify_mtd_cb(c_host, c_host->platform_data, cs_id, io_iftype->is_write, io_iftype->mtd_flag);
    }

    return ret;
}
