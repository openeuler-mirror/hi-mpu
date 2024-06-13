// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _SFC_API_H_
#define _SFC_API_H_
#include "sfc_osadp.h"
#include "sfc0_core.h"

/* Enables the interface for switching the address mode. */
#define EN_ADDRMODE_SWITCH

typedef struct {
    u32 bus;
    u32 cs;
} sfc_bus_info;

s32 hisfc_bus_check(u32 bus);
s32 hisfc_reg_read(sfc_bus_info info, loff_t from, size_t len, size_t *retlen, u8 *buf);
s32 hisfc_reg_write(sfc_bus_info info, loff_t to, size_t len, size_t *retlen, const u8 *data);
s32 hisfc_bus_read(sfc_bus_info info, loff_t addr, size_t len, size_t *retlen, u8 *buf);
s32 hisfc_bus_write(sfc_bus_info info, loff_t to, size_t len, size_t *retlen, const u8 *buf);
s32 hisfc_dma_read(sfc_bus_info info, loff_t from, size_t len, size_t *retlen, u8 *buf);
s32 hisfc_dma_write(sfc_bus_info info, loff_t to, size_t len, size_t *retlen, const u8 *buf);
s32 hisfc_reg_erase(sfc_bus_info info, unsigned long long offset, unsigned long long length);

typedef s32 (*mtd_init_callback)(u8 *);
typedef s32 (*platform_init_callback)(u8 *);

void hisfc_core_register_chip_ops(u32 bus, const sfc_chip_operations *ops);
void hisfc_core_register_plat_ops(u32 bus, const sfc_plat_operations *ops);

s32 hisfc_module_reset(u32 bus);
s32 hisfc_set_clk(u32 bus_id, u32 sfc_clk);
s32 hisfc_std_init(u32 bus, platform_init_callback plat_init, u8 *plat_data);
void hisfc_std_release(u32 bus);
void hisfc_shutdown_single_controler(hisfc_core_host *c_host);
void hisfc_suspend_single_controller(hisfc_core_host *c_host);
void hisfc_addr_mode_set_support_test(hisfc_spi *spi, hisfc_core_host *c_host);
hisfc_core_host *hisfc_get_core_host(u32 bus);
void *hisfc_get_plat_data(const hisfc_core_host *c_host);
void hisfc_dma_cmd_mode2rw_mode(u32 cmd_mode, u8 *is_read);
u32 hisfc_get_dma_cfg_ddr(u32 bus_id, dma_addr_t phy_addr);
s32 hisfc_resume(hisfc_core_host *c_host);

typedef struct {
    sfc_com_info com_info;
    u32 is_write; /* 1-write，0-read */
    u32 mtd_flag; /* USE_DMA_FLAG BIT(0) USE_REG_FLAG BIT(1) USE_BUS_FLAG BIT(2) */
} sfc_set_iftype;

typedef enum {
    COM_NOT_INIT = 0,
    COM_INIT_OK = 1,
} comres_init_e;

s32 hisfc_cmd_set_iftype(sfc_set_iftype *io_iftype);
u32 hisfc_get_com_res_init_sta(void);
void hisfc_set_com_res_init_sta(u32 init_flag);

#endif
