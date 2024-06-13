// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spi flash ids
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "sfc0_core.h"
#include "sfc_cfi.h"
#include "spi_flash_devices_ops.h"

/*****************************************************************************/
#define set_read_std(_dummy_, _size_, _clk_)                          \
    static struct spi_operation read_std_##_dummy_##_size_##_clk_ = { \
        SPI_IF_READ_STD, SPI_CMD_READ, _dummy_, _size_, _clk_         \
    }

#define read_std(_dummy_, _size_, _clk_) read_std_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_read_fast(_dummy_, _size_, _clk_)                          \
    static struct spi_operation read_fast_##_dummy_##_size_##_clk_ = { \
        SPI_IF_READ_FAST, SPI_CMD_FAST_READ, _dummy_, _size_, _clk_    \
    }

#define read_fast(_dummy_, _size_, _clk_) read_fast_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_read_dual(_dummy_, _size_, _clk_)                          \
    static struct spi_operation read_dual_##_dummy_##_size_##_clk_ = { \
        SPI_IF_READ_DUAL, SPI_CMD_READ_DUAL, _dummy_, _size_, _clk_    \
    }

#define read_dual(_dummy_, _size_, _clk_) read_dual_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_read_quad(_dummy_, _size_, _clk_)                          \
    static struct spi_operation read_quad_##_dummy_##_size_##_clk_ = { \
        SPI_IF_READ_QUAD, SPI_CMD_READ_QUAD, _dummy_, _size_, _clk_    \
    }

#define read_quad(_dummy_, _size_, _clk_) read_quad_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_read_dual_addr(_dummy_, _size_, _clk_)                            \
    static struct spi_operation read_dual_addr_##_dummy_##_size_##_clk_ = {   \
        SPI_IF_READ_DUAL_ADDR, SPI_CMD_READ_DUAL_ADDR, _dummy_, _size_, _clk_ \
    }

#define read_dual_addr(_dummy_, _size_, _clk_) read_dual_addr_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_read_quad_addr(_dummy_, _size_, _clk_)                            \
    static struct spi_operation read_quad_addr_##_dummy_##_size_##_clk_ = {   \
        SPI_IF_READ_QUAD_ADDR, SPI_CMD_READ_QUAD_ADDR, _dummy_, _size_, _clk_ \
    }

#define read_quad_addr(_dummy_, _size_, _clk_) read_quad_addr_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_write_std(_dummy_, _size_, _clk_)                          \
    static struct spi_operation write_std_##_dummy_##_size_##_clk_ = { \
        SPI_IF_WRITE_STD, SPI_CMD_PP, _dummy_, _size_, _clk_           \
    }

#define write_std(_dummy_, _size_, _clk_) write_std_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_write_dual(_dummy_, _size_, _clk_)                          \
    static struct spi_operation write_dual_##_dummy_##_size_##_clk_ = { \
        SPI_IF_WRITE_DUAL, SPI_CMD_WRITE_DUAL, _dummy_, _size_, _clk_   \
    }

#define write_dual(_dummy_, _size_, _clk_) write_dual_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_write_quad(_dummy_, _size_, _clk_)                          \
    static struct spi_operation write_quad_##_dummy_##_size_##_clk_ = { \
        SPI_IF_WRITE_QUAD, SPI_CMD_WRITE_QUAD, _dummy_, _size_, _clk_   \
    }

#define write_quad(_dummy_, _size_, _clk_) write_quad_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_write_dual_addr(_dummy_, _size_, _clk_)                             \
    static struct spi_operation write_dual_addr_##_dummy_##_size_##_clk_ = {    \
        SPI_IF_WRITE_DUAL_ADDR, SPI_CMD_WRITE_DUAL_ADDR, _dummy_, _size_, _clk_ \
    }

#define write_dual_addr(_dummy_, _size_, _clk_) write_dual_addr_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_write_quad_addr(_dummy_, _size_, _clk_)                             \
    static struct spi_operation write_quad_addr_##_dummy_##_size_##_clk_ = {    \
        SPI_IF_WRITE_QUAD_ADDR, SPI_CMD_WRITE_QUAD_ADDR, _dummy_, _size_, _clk_ \
    }

#define write_quad_addr(_dummy_, _size_, _clk_) write_quad_addr_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_write_quad_addr_144(_dummy_, _size_, _clk_)                             \
    static struct spi_operation write_quad_addr_144_##_dummy_##_size_##_clk_ = {    \
        SPI_IF_WRITE_QUAD_ADDR, SPI_CMD_WRITE_QUAD_ADDR_144, _dummy_, _size_, _clk_ \
    }

#define write_quad_addr_144(_dummy_, _size_, _clk_) write_quad_addr_144_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#define set_erase_sector_4k(_dummy_, _size_, _clk_)                          \
    static struct spi_operation erase_sector_4k_##_dummy_##_size_##_clk_ = { \
        SPI_IF_ERASE_SECTOR_4K, SPI_CMD_SE_4K, _dummy_, _size_, _clk_        \
    }

#define erase_sector_4k(_dummy_, _size_, _clk_) erase_sector_4k_##_dummy_##_size_##_clk_
// -----------------------------------------------------------------------------
#ifdef SFC_32K_ERASE_SECTOR_SUPPORT
#define set_erase_sector_32k(_dummy_, _size_, _clk_)                          \
    static struct spi_operation erase_sector_32k_##_dummy_##_size_##_clk_ = { \
        SPI_IF_ERASE_SECTOR_32K, SPI_CMD_SE_32K, _dummy_, _size_, _clk_       \
    }

#define erase_sector_32k(_dummy_, _size_, _clk_) erase_sector_32k_##_dummy_##_size_##_clk_
#endif
// -----------------------------------------------------------------------------
#define set_erase_sector_64k(_dummy_, _size_, _clk_)                    \
    struct spi_operation erase_sector_64k_##_dummy_##_size_##_clk_ = {  \
        SPI_IF_ERASE_SECTOR_64K, SPI_CMD_SE_64K, _dummy_, _size_, _clk_ \
    }

#define erase_sector_64k(_dummy_, _size_, _clk_) erase_sector_64k_##_dummy_##_size_##_clk_

// -----------------------------------------------------------------------------
#define set_erase_sector_256k(_dummy_, _size_, _clk_)                          \
    static struct spi_operation erase_sector_256k_##_dummy_##_size_##_clk_ = { \
        SPI_IF_ERASE_SECTOR_256K, SPI_CMD_SE_256K, _dummy_, _size_, _clk_      \
    }

#define erase_sector_256k(_dummy_, _size_, _clk_) erase_sector_256k_##_dummy_##_size_##_clk_

#define set_erase_sector_s25_256k(_dummy_, _size_, _clk_)                          \
    static struct spi_operation erase_sector_s25_256k_##_dummy_##_size_##_clk_ = { \
        SPI_IF_ERASE_SECTOR_S25_256K, SPI_CMD_SE_256K, _dummy_, _size_, _clk_      \
    }

#define erase_sector_s25_256k(_dummy_, _size_, _clk_) erase_sector_s25_256k_##_dummy_##_size_##_clk_

/*****************************************************************************/
set_read_std(0, INFINITE, 0);
set_read_std(0, INFINITE, 20);
set_read_std(0, INFINITE, 33);
set_read_std(0, INFINITE, 40);
set_read_std(0, INFINITE, 50);
set_read_std(0, INFINITE, 54);
set_read_std(0, INFINITE, 66);
set_read_std(0, INFINITE, 80);
set_read_std(0, INFINITE, 90);
set_read_std(0, INFINITE, 108);

set_read_fast(1, INFINITE, 50);
set_read_fast(1, INFINITE, 64);
set_read_fast(1, INFINITE, 80);
set_read_fast(1, INFINITE, 104);
set_read_fast(1, INFINITE, 108);
set_read_fast(1, INFINITE, 133);
set_read_fast(1, INFINITE, 166);

set_read_dual(1, INFINITE, 54);
set_read_dual(1, INFINITE, 64);
set_read_dual(1, INFINITE, 80);
set_read_dual(1, INFINITE, 104);
set_read_dual(1, INFINITE, 108);
set_read_dual(1, INFINITE, 133);
set_read_dual(1, INFINITE, 152);

set_read_quad(1, INFINITE, 80);
set_read_quad(1, INFINITE, 108);
set_read_quad(1, INFINITE, 134);

set_read_dual_addr(1, INFINITE, 54);
set_read_dual_addr(1, INFINITE, 64);
set_read_dual_addr(0, INFINITE, 80);
set_read_dual_addr(1, INFINITE, 80);
set_read_dual_addr(2, INFINITE, 108);
set_read_dual_addr(1, INFINITE, 133);
set_read_dual_addr(2, INFINITE, 134);
set_read_dual_addr(1, INFINITE, 134);

set_read_quad_addr(2, INFINITE, 80);
set_read_quad_addr(5, INFINITE, 108);
set_read_quad_addr(3, INFINITE, 125);

// -----------------------------------------------------------------------------
set_write_std(0, 256, 0);
set_write_std(0, 256, 50);
set_write_std(0, 256, 64);
set_write_std(0, 256, 80);
set_write_std(0, 256, 104);
set_write_std(0, 256, 108);
set_write_std(0, 256, 133);
set_write_std(0, 256, 166);

set_write_dual(0, 256, 108);
set_write_dual(0, 256, 90);

set_write_quad(0, 256, 80);
set_write_quad(0, 256, 108);
set_write_quad(0, 256, 90);

set_write_dual_addr(0, 256, 108);
set_write_dual_addr(0, 256, 90);

set_write_quad_addr(0, 256, 108);
set_write_quad_addr_144(0, 256, 90);

set_erase_sector_64k(0, _64K, 0);
set_erase_sector_64k(0, _64K, 50);
set_erase_sector_64k(0, _64K, 64);
set_erase_sector_64k(0, _64K, 66);
set_erase_sector_64k(0, _64K, 75);
set_erase_sector_64k(0, _64K, 80);
set_erase_sector_64k(0, _64K, 86);
set_erase_sector_64k(0, _64K, 104);
set_erase_sector_64k(0, _64K, 108);
set_erase_sector_64k(0, _64K, 133);
set_erase_sector_64k(0, _64K, 166);

set_erase_sector_256k(0, _256K, 133);
set_erase_sector_256k(0, _256K, 104);
set_erase_sector_s25_256k(0, _256K, 133);

/*****************************************************************************/
static struct spi_driver g_spi_driver_general = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_general_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .read_process = NULL,
    .erase_process = NULL,
    .init_flash_mem = NULL,
    .spec_cfg = NULL,
    .write_any_reg = NULL,
    .read_any_reg = NULL,
    .modify_any_reg = NULL,
    .driver_test = {
        .protect_process = NULL,
    },
};

static struct spi_driver g_spi_issi_driver_gen = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_issi_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .read_process = NULL,
    .erase_process = NULL,
    .init_flash_mem = NULL,
    .spec_cfg = NULL,
    .read_any_reg = NULL,
    .write_any_reg = NULL,
    .modify_any_reg = NULL,
    .driver_test = {
        .protect_process = NULL,
    },
};

static struct spi_driver g_spi_driver_s25fl256s = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_s25fl256s_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .erase_process = NULL,
    .read_process = NULL,
    .init_flash_mem = NULL,
    .spec_cfg = NULL,
    .write_any_reg = NULL,
    .read_any_reg = NULL,
    .modify_any_reg = NULL,
    .driver_test = {
        .protect_process = NULL,
    },
};

static struct spi_driver g_spi_driver_n25q256a = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_n25q256a_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .erase_process = NULL,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .read_process = NULL,
    .init_flash_mem = NULL,
    .spec_cfg = NULL,
    .write_any_reg = NULL,
    .read_any_reg = NULL,
    .modify_any_reg = NULL,
    .driver_test = {
        .protect_process = sfc_spi_mt25quxxbb_protect_process,
    },
};

/* for MX */
static struct spi_driver g_spi_driver_mx66u = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_n25q256a_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .erase_process = NULL,
    .read_process = NULL,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .init_flash_mem = NULL,
    .spec_cfg = NULL,
    .write_any_reg = NULL,
    .read_any_reg = NULL,
    .modify_any_reg = NULL,
    .driver_test = {
        .protect_process = NULL,
    },
};

/* S70fs01gs This flash does not support commands such as RDSR. */
static struct spi_driver g_spi_driver_s70fs01gs = {
    .support_dma_bus_flag = (SUPPORT_DMA_RD | SUPPORT_BUS_RD | SUPPORT_REG_WR | SUPPORT_REG_RD),
    .wait_ready = sfc_spi_fs01gs_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_fs01gs_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .erase_process = sfc_core_spansionfsxx_erase,     /* The erase of the flash memory requires special processing. */
    .read_process = sfc_core_s70fs01gs_reg_read,      /* The read of the flash memory requires special processing. */
    /* The read of the flash memory requires special initialization. */
    .init_flash_mem = sfc_core_fs01gs_init_flash_mem,
    .spec_cfg = spi_flash_fs01gs_spec_cfg,  /* Depends on read write modify anreg */
    .write_any_reg = sfc_core_write_any_reg,
    .read_any_reg = sfc_core_read_any_reg,
    .modify_any_reg = sfc_core_modify_any_reg,
    .driver_test = {
        .protect_process = NULL,
    },
};

static struct spi_driver g_spi_driver_s25fs512s = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_fs512s_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .erase_process = sfc_core_spansionfsxx_erase, /* Special processing is required for erasing the flash memory. */
    .read_process = NULL,
    /* The read of the flash memory requires special initialization. sfc_core_fs512s_init_flash_mem */
    .init_flash_mem = NULL,
    .spec_cfg = spi_flash_fs512s_spec_cfg, /* Depends on read write modify anreg */
    .write_any_reg = sfc_core_write_any_reg,
    .read_any_reg = sfc_core_read_any_reg,
    .modify_any_reg = sfc_core_modify_any_reg,
    .driver_test = {
        .protect_process = NULL,
    },
};

/* spansion general driver ex.s25fs128s */
static struct spi_driver g_spi_driver_spansion_general = {
    .support_dma_bus_flag = SUPPORT_ALL_MOD,
    .wait_ready = sfc_spi_general_wait_ready,
    .write_enable = sfc_spi_general_write_enable,
    .entry_4addr = sfc_spi_general_entry_4addr,
    .bus_prepare = sfc_spi_general_bus_prepare,
    .spi_opr_finish_call = sfc_core_release_spi_lock,
    .read_process = NULL,
    .erase_process = sfc_core_spansionfsxx_erase,
    .init_flash_mem = NULL,
    .spec_cfg = NULL,
    .write_any_reg = NULL,
    .read_any_reg = NULL,
    .modify_any_reg = NULL,
    .driver_test = {
        .protect_process = NULL,
    },
};

struct spi_info g_hisfc_spi_info_table[] = {
    {
        "EN25QH256", { 0x1C, 0x70, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 50),
            &read_dual(1, INFINITE, 54),
            &read_dual_addr(1, INFINITE, 54), 0
        },
        {   &write_std(0, 256, 104), 0 },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "N25Q128", { 0x20, 0xba, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 54),
            &read_fast(1, INFINITE, 108),
            &read_dual(1, INFINITE, 108),
            &read_quad(1, INFINITE, 108),
            &read_dual_addr(2, INFINITE, 108),
            &read_quad_addr(5, INFINITE, 108),
            0
        },
        {   &write_std(0, 256, 108),
            &write_dual(0, 256, 108),
            &write_quad(0, 256, 108),
            &write_dual_addr(0, 256, 108),
            &write_quad_addr(0, 256, 108), 0
        },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_general,
    },
    {
        /* 171x */
        "N25Q256A", { 0x20, 0xba, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 54),
            &read_fast(1, INFINITE, 108),
            &read_dual(1, INFINITE, 108),
            &read_quad(1, INFINITE, 108),
            &read_dual_addr(2, INFINITE, 108),
            &read_quad_addr(5, INFINITE, 108), 0
        },
        {   &write_std(0, 256, 108),
            &write_dual(0, 256, 108),
            &write_quad(0, 256, 108),
            &write_dual_addr(0, 256, 108),
            &write_quad_addr(0, 256, 108),
            0
        },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_n25q256a,
    },
    {
        "MT25QU128AB", { 0x20, 0xbb, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 66),
            0
        },
        {   &write_std(0, 256, 133),
            0
        },
        {   &erase_sector_64k(0, _64K, 133), 0 },
        &g_spi_driver_n25q256a,
    },
    {
        /* 1711 and 1710 */
        "MT25QU512ABB", { 0x20, 0xbb, 0x20 }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 80),
            0
        },
        {   &write_std(0, 256, 166),
            0
        },
        {   &erase_sector_64k(0, _64K, 166), 0 },
        &g_spi_driver_n25q256a,
    },
    {
        /* 1711 */
        "MT25Q_QLKT_L_512_ABB_0", { 0x20, 0xba, 0x20 }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 80), 0 },
        {   &write_std(0, 256, 166), 0 },
        {   &erase_sector_64k(0, _64K, 166), 0 },
        &g_spi_driver_n25q256a,
    },
    {
        /* 1711 add */
        "MT25QU01GBBB", { 0x20, 0xbb, 0x21 }, 3, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 90),
            &read_fast(1, INFINITE, 166), /* 0x0B 1-1-1 iftype 0x2  ok */
            &read_dual(1, INFINITE, 152), /* 0x3B 1-1-2 iftype 0x4 okok */
            &read_dual_addr(2, INFINITE, 134), /* 0xBB 1-2-2 iftype 0x8  ok */
            0
        },
        {   &write_std(0, 256, 166),
            &write_dual(0, 256, 90), /* 0xA2 1-1-2 iftype 0x2   ok */
            &write_dual_addr(0, 256, 90), /* 0xD2 1-2-2 iftype 0x4  ok */
            0
        },
        {   &erase_sector_64k(0, _64K, 166), 0 },
        &g_spi_driver_n25q256a,
    },
    {
        "mx66u51235f", { 0xc2, 0x25, 0x3a }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 108),
            &read_fast(1, INFINITE, 166),      /* 0x0B 1-1-1  iftype */
            &read_dual(1, INFINITE, 152),      /* 0x3B 1-1-2  iftype 0x4 */
            &read_quad(1, INFINITE, 134),      /* 0x6B 1-1-4  iftype 0x10 */
            &read_quad_addr(3, INFINITE, 125), /* 0xEB 1-4-4  iftype 0x20 */
            &read_dual_addr(1, INFINITE, 134), /* 0xBB 1-2-2  iftype 0x8 */
            0
        },
        {   &write_std(0, 256, 108),
            /* The dual-wire write command is not supported. */
            &write_quad(0, 256, 90),            /* 0x32 1-1-4 iftype 0x8 */
            &write_quad_addr_144(0, 256, 90), 0 /* 0x38 1-4-4 iftype 0x10  */
        },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        "mx66u1g45g", { 0xc2, 0x25, 0x3b }, 3, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 108),
            0
        },
        {   &write_std(0, 256, 108),
            0
        },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        "mx25u12835f", { 0xc2, 0x25, 0x38 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 108), 0 },
        {   &write_std(0, 256, 108), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        /* 1711 add */
        "mx25l51245g", { 0xc2, 0x20, 0x1a }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 108),
            &read_fast(1, INFINITE, 166),      /* 0x0B 1-1-1 iftype 0x2 */
            &read_dual(1, INFINITE, 152),      /* 0x3B 1-1-2 iftype 0x4 */
            &read_dual_addr(1, INFINITE, 134), /* 0xBB 1-2-2 iftype 0x8 */
            &read_quad(1, INFINITE, 134),      /* 0x6B 1-1-4 iftype 0x10 */
            &read_quad_addr(3, INFINITE, 125), /* 0xEB 1-4-4 iftype 0x20 */
            0
        },
        {   &write_std(0, 256, 108),
            &write_quad_addr_144(0, 256, 90),  /* 0x38 1-4-4 iftype 0x10 */
            &write_quad(0, 256, 90), 0         /* 0x32 1-1-4 iftype 0x8 */
        },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        /* 1711 add */
        "mx66l1g45g", { 0xc2, 0x20, 0x1b }, 3, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 108),
            &read_fast(1, INFINITE, 166),      /* 0x0B  1-1-1 iftype 0x2 source 0xc2 0x25 0x3a */
            &read_dual(1, INFINITE, 152),      /* 0x3B  1-1-2 iftype 0x4 */
            &read_dual_addr(1, INFINITE, 134), /* 0xBB  1-2-2 iftype 0x8 */
            &read_quad_addr(3, INFINITE, 125), /* 0xEB  1-4-4 iftype 0x20 */
            &read_quad(1, INFINITE, 134),      /* 0x6B  1-1-4 iftype 0x10 */
            0
        },
        {   &write_std(0, 256, 108), &write_quad(0, 256, 90), &write_quad_addr_144(0, 256, 90), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        /* 1711 add spansion */
        "s70fs01gs", { 0x01, 0x02, 0x21, 0x4d, 0x00, 0x81 }, SPANSION_ID_LEN, _128M, _256K, 4,
        {   &read_std(0, INFINITE, 50),
            0
        },
        {   &write_std(0, 256, 133),
            0
        },
        {   &erase_sector_256k(0, _256K, 133), 0 },
        &g_spi_driver_s70fs01gs,
    },
    {
        /* 1711 C+ */
        "IS25WP128F", { 0x9D, 0x70, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 50), 0 },
        {   &write_std(0, 256, 133), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_issi_driver_gen,
    },
    {
        "IS25WP256E", { 0x9D, 0x70, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 50), 0 },
        {   &write_std(0, 256, 133), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_issi_driver_gen,
    },
    {
        "IS25LP512M", { 0x9D, 0x60, 0x1a }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 50), 0 },
        {   &write_std(0, 256, 133), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_issi_driver_gen,
    },
    /* The follow  chips W25Q64C and W25Q64FVSIG have the same chipid, but command have some difference */
    {
        "W25Q128BV", { 0xEF, 0x40, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 33),
            &read_fast(1, INFINITE, 104),
            &read_dual(1, INFINITE, 104),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "W25Q256FV", { 0xEF, 0x40, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 33),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "W25Q256JW",
        { 0xEF, 0x60, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 33),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "EN25Q128", { 0x1c, 0x30, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 80),
            &read_dual(1, INFINITE, 64),
            &read_dual_addr(1, INFINITE, 64),
            0
        },
        {   &write_std(0, 256, 80),
            0
        },
        {   &erase_sector_64k(0, _64K, 80), 0 },
        &g_spi_driver_general,
    },
    {
        "S25FL256S", { 0x01, 0x02, 0x19, 0x4D, 0x01, 0x80 }, 6, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 50),
            0
        },
        {   &write_std(0, 256, 133),
            0
        },
        {   &erase_sector_64k(0, _64K, 133), 0 },
        &g_spi_driver_s25fl256s,
    },
    {
        "S25FL128S", { 0x01, 0x20, 0x18, 0x4D, 0x01, 0x80 }, 6, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 133),
            &read_dual(1, INFINITE, 133),
            &read_dual_addr(1, INFINITE, 133),
            0
        },
        {   &write_std(0, 256, 133),
            0
        },
        {   &erase_sector_64k(0, _64K, 133), 0 },
        &g_spi_driver_general,
    },
    {
        /* 171x */
        "S25FL512S", { 0x01, 0x02, 0x20, 0x4d, 0x00, 0x80 }, 6, _64M, _256K, 4,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 104),
            &read_dual(1, INFINITE, 104),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_s25_256k(0, _256K, 133), 0 },
        &g_spi_driver_s25fl256s,
    },
    {
        "S25FS512S", { 0x01, 0x02, 0x20, 0x4d, 0x00, 0x81 }, 6, _64M, _256K, 4,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 133),
            &read_dual(1, INFINITE, 133),
            0
        },
        {   &write_std(0, 256, 133),
            0
        },
        {   &erase_sector_s25_256k(0, _256K, 133), 0 },
        &g_spi_driver_s25fs512s,
    },
    {
        "S25FS128S", { 0x01, 0x20, 0x18, 0x4d, 0x01, 0x81 }, 6, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 133),
            &read_dual(1, INFINITE, 133),
            0
        },
        {   &write_std(0, 256, 133),
            0
        },
        {   &erase_sector_64k(0, _64K, 133), 0 },
        &g_spi_driver_spansion_general,
    },
    /* Spansion */
    {
        "S25FL128P-0", { 0x01, 0x20, 0x18, 0x03, 0x00 }, 5, _16M, _256K, 3,
        {   &read_std(0, INFINITE, 40),
            &read_fast(1, INFINITE, 104),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_256k(0, _256K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "S25FL128P-1", { 0x01, 0x20, 0x18, 0x03, 0x01 }, 5, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 40),
            &read_fast(1, INFINITE, 104),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "S25FL129P0", { 0x01, 0x20, 0x18, 0x4d, 0x00 }, 5, _16M, _256K, 3,
        {   &read_std(0, INFINITE, 40),
            &read_fast(1, INFINITE, 104),
            &read_dual(1, INFINITE, 80),
            &read_quad(1, INFINITE, 80),
            &read_dual_addr(0, INFINITE, 80),
            &read_quad_addr(2, INFINITE, 80),
            0
        },
        {   &write_std(0, 256, 104),
            &write_quad(0, 256, 80),
            0
        },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "S25FL129P1", { 0x01, 0x20, 0x18, 0x4d, 0x01 }, 5, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 40),
            &read_fast(1, INFINITE, 104),
            &read_dual(1, INFINITE, 64),
            0
        },
        {   &write_std(0, 256, 104),
            0
        },
        {   &erase_sector_64k(0, _64K, 104), 0 },
        &g_spi_driver_general,
    },
    {
        "MX25L128", { 0xc2, 0x20, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 50),
            &read_fast(1, INFINITE, 108),
            0
        },
        {   &write_std(0, 256, 108),
            0
        },
        {   &erase_sector_64k(0, _64K, 75), 0 },
        &g_spi_driver_general,
    },
    {
        /* 171x */
        "MX25L25645G", { 0xc2, 0x20, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 40),
            &read_fast(1, INFINITE, 64),
            &read_dual_addr(1, INFINITE, 80),
            0
        },
        {   &write_std(0, 256, 64),
            0
        },
        {   &erase_sector_64k(0, _64K, 80), 0 },
        &g_spi_driver_general,
    },
    {
        "MX25U25645G", { 0xc2, 0x25, 0x39 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 40),
            &read_fast(1, INFINITE, 64),
            &read_dual_addr(1, INFINITE, 80),
            0
        },
        {   &write_std(0, 256, 64),
            0
        },
        {   &erase_sector_64k(0, _64K, 80), 0 },
        &g_spi_driver_general,
    },
    {
        "MX25U12832F", { 0xc2, 0x25, 0x38 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 40),
            0
        },
        {   &write_std(0, 256, 64),
            0
        },
        {   &erase_sector_64k(0, _64K, 80), 0 },
        &g_spi_driver_general,
    },
    {
        "M25P128", { 0x20, 0x20, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 20),
            &read_fast(1, INFINITE, 50),
            0
        },
        {   &write_std(0, 256, 50),
            0
        },
        {   &erase_sector_64k(0, _64K, 50), 0 },
        &g_spi_driver_general,
    },
    {
        "mx25l12855e", { 0xc2, 0x26, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 0),
            0
        },
        {   &write_std(0, 256, 0),
            0
        },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "GD25LE128E", { 0xc8, 0x60, 0x18 }, 3, _16M, _64K, 3,
        {   &read_std(0, INFINITE, 40), 0 },
        {   &write_std(0, 256, 0), 0 },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "GD25Q256DFFRY", { 0xc8, 0x40, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 40), 0 },
        {   &write_std(0, 256, 0), 0 },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "GD55B512ME", { 0xc8, 0x40, 0x1a }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 40), 0 },
        {   &write_std(0, 256, 0), 0 },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "GD25LB256E", { 0xc8, 0x67, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 40), 0 },
        {   &write_std(0, 256, 0), 0 },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "MX66U51235F", { 0xc2, 0x25, 0x3a }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 40),
            0
        },
        {   &write_std(0, 256, 0),
            0
        },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "XT25W512B", { 0x0b, 0x65, 0x1a }, 3, _64M, _64K, 4,
        {   &read_std(0, INFINITE, 40), 0 },
        {   &write_std(0, 256, 0), 0 },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "XT25F256B", { 0x0b, 0x40, 0x19 }, 3, _32M, _64K, 4,
        {   &read_std(0, INFINITE, 40), 0 },
        {   &write_std(0, 256, 0), 0 },
        {   &erase_sector_64k(0, _64K, 0), 0 },
        &g_spi_driver_general,
    },
    {
        "MICRON default", { CFI_MFR_MICRON }, 1, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 80), 0 },
        {   &write_std(0, 256, 166), 0 },
        {   &erase_sector_64k(0, _64K, 166), 0 },
        &g_spi_driver_n25q256a,
    },
    {
        "MACRONIX default", { CFI_MFR_MACRONIX }, 1, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 108), 0 },
        {   &write_std(0, 256, 108), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        "Spansion256 default", { CFI_MFR_SPANSION, CFI_PSA_256KB_SPANSION_VAL }, SPANSION_SPEC_ID_LEN, _128M, _256K, 4,
        {   &read_std(0, INFINITE, 50), 0 },
        {   &write_std(0, 256, 133), 0 },
        {   &erase_sector_s25_256k(0, _256K, 133), 0 },
        &g_spi_driver_s25fs512s,
    },
    {
        "Spansion64 default", { CFI_MFR_SPANSION, CFI_PSA_64KB_SPANSION_VAL }, SPANSION_SPEC_ID_LEN, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 50), 0 },
        {   &write_std(0, 256, 133), 0 },
        {   &erase_sector_64k(0, _64K, 133), 0 },
        &g_spi_driver_spansion_general,
    },
    {
        "ISSI default", { CFI_MFR_ISSI }, 1, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 50), 0 },
        {   &write_std(0, 256, 133), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_issi_driver_gen,
    },
    /* It must be placed in the last one. If this one is matched, assign a value before it is found. */
    {
        "default default", { 0xff }, 1, _128M, _64K, 4,
        {   &read_std(0, INFINITE, 108), 0 },
        {   &write_std(0, 256, 108), 0 },
        {   &erase_sector_64k(0, _64K, 108), 0 },
        &g_spi_driver_mx66u,
    },
    {
        0, { 0 }, 0, 0, 0, 0, { 0 }, { 0 }, { 0 },
    },
};
