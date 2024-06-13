// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc chip diff
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SFC_SYC_FUNC__
#define __SFC_SYC_FUNC__

#define SFC0_CS0_2480_ATLAS880_DRIVE (0x31)

#define SFC1_ATLAS800_CLOCK 8
#define MAX_SFC_CLK_MHZ     50
#define DEFAULT_SFC_CLK_MHZ 25

s32 sfc_sys_set_controller_clk(u32 bus_id, u32 clk_mhz);
s32 sfc_sys_set_iocfg(u32 bus, u32 cs, u32 enable);
void sfc_sys_init_extend_addr_mode(void);
u32 sfc_sys_get_dma_cfg_ddr(u32 bus_id, dma_addr_t phy_addr);
s32 sfc_sys_clock_operator(u32 bus_id, u32 operator);
s32 sfc_sys_set_clk(u32 bus_id, u32 clk_mhz);
s32 sfc_sys_reset(u32 bus);

#endif
