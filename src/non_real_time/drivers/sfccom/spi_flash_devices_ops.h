// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * flash ops
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SPI_FLASH_DEV_OPS_H__
#define __SPI_FLASH_DEV_OPS_H__

#include "sfc0_core.h"

s32 sfc_spi_fs512s_entry_4addr(hisfc_spi *spi, s32 enable);
s32 sfc_spi_fs01gs_entry_4addr(hisfc_spi *spi, s32 enable);
s32 spi_flash_fs512s_spec_cfg(hisfc_spi *spi, u8 is_lock);
s32 spi_flash_fs01gs_spec_cfg(hisfc_spi *spi, u8 is_lock);
s32 sfc_spi_fs01gs_wait_ready(hisfc_spi *spi, u8 is_lock);
s32 sfc_spi_mt25quxxbb_protect_process(hisfc_spi *spi, protect_para *p_protect);
s32 sfc_spi_n25q256a_entry_4addr(hisfc_spi *spi, s32 enable);
s32 sfc_spi_s25fl256s_entry_4addr(hisfc_spi *spi, s32 enable);
s32 sfc_spi_issi_entry_4addr(hisfc_spi *spi, s32 enable);
s32 sfc_spi_general_entry_4addr(hisfc_spi *spi, s32 enable);
s32 sfc_spi_general_wait_ready(hisfc_spi *spi, u8 is_lock);
s32 sfc_spi_general_write_enable(hisfc_spi *spi);
s32 sfc_spi_general_bus_prepare(hisfc_spi *spi, s32 op);

#endif
