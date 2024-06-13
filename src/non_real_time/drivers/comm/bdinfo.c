// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Public module bdinfo interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sysctrl_csr_addr_define.h"
#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "sysctrl.h"
#include "io_config.h"
#include "board_info.h"
#include "bdinfo.h"

io_multi_cfg_s g_chip_io_cfg_table[] = {
    {
        .module_id   = IO_MULTI_I2C0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x34, 0x38},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x00, 0x00},
    },
    {
        .module_id   = IO_MULTI_I2C1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x3c, 0x40},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x00, 0x00},
    },
    {
        .module_id   = IO_MULTI_I2C2,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x44, 0x48},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x00, 0x00},
    },
    {
        .module_id   = IO_MULTI_I2C3,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x4c, 0x50},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x00, 0x00},
    },
    {
        .module_id   = IO_MULTI_I2C4,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x54, 0x58},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x00, 0x00},
    },
    {
        .module_id   = IO_MULTI_I2C5,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x5c, 0x60},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x00, 0x00},
    },
    {
        .module_id   = IO_MULTI_I2C6,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x64},
        .reg_mask    = {0x03},
        .reg_val     = {0x00},
    },
    {
        .module_id   = IO_MULTI_I2C7,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x68},
        .reg_mask    = {0x03},
        .reg_val     = {0x00},
    },
    {
        .module_id   = IO_MULTI_I2C8,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x70, 0x74},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x03, 0x03},
    },
    {
        .module_id   = IO_MULTI_I2C9,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x78, 0x7c},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x03, 0x03},
    },
    {
        .module_id   = IO_MULTI_I2C10,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x80, 0x84},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x03, 0x03},
    },
    {
        .module_id   = IO_MULTI_I2C11,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x88, 0x8c},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x03, 0x03},
    },
    {
        .module_id   = IO_MULTI_I2C12,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x90, 0x94},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x03, 0x03},
    },
    {
        .module_id   = IO_MULTI_I2C13,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0xc8},
        .reg_mask    = {0x03},
        .reg_val     = {0x01},
    },
    {
        .module_id   = IO_MULTI_I2C14,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x8c, 0x90},
        .reg_mask    = {0x03, 0x03},
        .reg_val     = {0x03, 0x03},
    },
    {
        .module_id   = IO_MULTI_I2C15,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x54},
        .reg_mask    = {0x03},
        .reg_val     = {0x01},
    },
    {
        .module_id   = IO_MULTI_SPI0_CS0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x01c, 0x028, 0x02c, 0x020},
        .reg_mask    = {0x007, 0x007, 0x007, 0x007},
        .reg_val     = {0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SPI0_CS1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x01c, 0x028, 0x02c, 0x024},
        .reg_mask    = {0x007, 0x007, 0x007, 0x007},
        .reg_val     = {0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SPI1_CS0_R_EMMC,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x5,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x334, 0x030, 0x03c, 0x040, 0x034},
        .reg_mask    = {0x001, 0x007, 0x007, 0x007, 0x007},
        .reg_val     = {0x000, 0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SPI1_CS1_R_EMMC,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x5,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x334, 0x030, 0x03c, 0x040, 0x038},
        .reg_mask    = {0x001, 0x007, 0x007, 0x007, 0x007},
        .reg_val     = {0x000, 0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SPI1_CS0_T_SFC,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x018, 0x024, 0x01c},
        .reg_mask    = {0x001, 0x001, 0x003, 0x003},
        .reg_val     = {0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SPI1_CS1_T_SFC,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x018, 0x024, 0x020},
        .reg_mask    = {0x001, 0x001, 0x003, 0x003},
        .reg_val     = {0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SFC0_CS0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x008, 0x00c, 0x010},
        .reg_mask    = {0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SFC0_CS1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x008, 0x00c, 0x014},
        .reg_mask    = {0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SFC1_CS0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x5,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x018, 0x01c, 0x020, 0x024, 0x028},
        .reg_mask    = {0x001, 0x003, 0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SFC1_CS1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x5,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x018, 0x01c, 0x020, 0x024, 0x02c},
        .reg_mask    = {0x001, 0x003, 0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_IPMB0_T_I2C,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x034, 0x038},
        .reg_mask    = {0x010, 0x007, 0x007},
        .reg_val     = {0x010, 0x003, 0x003},
    },
    {
        .module_id   = IO_MULTI_IPMB1_T_I2C,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x03C, 0x040},
        .reg_mask    = {0x008, 0x007, 0x007},
        .reg_val     = {0x008, 0x003, 0x003},
    },
    {
        .module_id   = IO_MULTI_IPMB2_T_I2C,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x044, 0x048},
        .reg_mask    = {0x004, 0x007, 0x007},
        .reg_val     = {0x004, 0x003, 0x003},
    },
    {
        .module_id   = IO_MULTI_IPMB3_T_I2C,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x04C, 0x050},
        .reg_mask    = {0x002, 0x007, 0x007},
        .reg_val     = {0x002, 0x003, 0x003},
    },
    {
        .module_id   = IO_MULTI_IPMB0_R_LOCALBUS,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x334, 0x044},
        .reg_mask    = {0x010, 0x007},
        .reg_val     = {0x000, 0x003},
    },
    {
        .module_id   = IO_MULTI_IPMB1_R_LOCALBUS,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x334, 0x048},
        .reg_mask    = {0x008, 0x007},
        .reg_val     = {0x000, 0x003},
    },
    {
        .module_id   = IO_MULTI_IPMB2_R_LOCALBUS,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x334, 0x088, 0x08C},
        .reg_mask    = {0x004, 0x007, 0x007},
        .reg_val     = {0x000, 0x004, 0x004},
    },
    {
        .module_id   = IO_MULTI_IPMB3_R_LOCALBUS,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x334, 0x090, 0x094},
        .reg_mask    = {0x002, 0x007, 0x007},
        .reg_val     = {0x000, 0x004, 0x004},
    },
    {
        .module_id   = IO_MULTI_PF_PAD0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x070},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x074},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD2,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x078},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD3,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x07C},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD4,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x080},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD5,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x084},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD6,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x088},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD7,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x08c},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD8,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x090},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD9,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x094},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD10,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x098},
        .reg_mask    = {0x003},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_PF_PAD11,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x088},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD12,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x08c},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD13,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x090},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD14,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x094},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD15,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x098},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD16,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x09C},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD17,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x0a0},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD18,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x0a4},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD19,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x0a8},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_PF_PAD20,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x038},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD21,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x01c},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD22,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x020},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD23,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x024},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD24,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x028},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD25,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x02c},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD26,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x030},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_PF_PAD27,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x034},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_EMMC,
        .debug_flag  = 0x1,
        .reg_cnt     = 0xa,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x01c, 0x020, 0x024, 0x028, 0x02c, 0x030, 0x034, 0x038, 0x03c, 0x040},
        .reg_mask    = {0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SDIO,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x5,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x0b0, 0x0b4, 0x0b8, 0x0bc, 0x0c0},
        .reg_mask    = {0x007, 0x007, 0x007, 0x007, 0x007},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_MDIO,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x084},
        .reg_mask    = {0x003},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_RMII0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x7,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x058, 0x05c, 0x060, 0x064, 0x8a0, 0x8a4, 0x8a8},
        .reg_mask    = {0x007, 0x007, 0x007, 0x007, 0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x002, 0x002, 0x002},
    },
    {
        .module_id   = IO_MULTI_RMII1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x3,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x068, 0x06c, 0x8c4},
        .reg_mask    = {0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x002},
    },
    {
        .module_id   = IO_MULTI_JLC1_R_RMII,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_SYSCTL, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x218, 0x058, 0x05c, 0x060},
        .reg_mask    = {0x010, 0x003, 0x007, 0x007},
        .reg_val     = {0x000, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_JLC1_T_SDIO,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_SYSCTL, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x218, 0x0b8, 0x0bc, 0x0c0},
        .reg_mask    = {0x010, 0x007, 0x007, 0x007},
        .reg_val     = {0x010, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_PECI,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x09C},
        .reg_mask    = {0x003},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_SGPIO0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x000, 0x004, 0x008, 0x00C},
        .reg_mask    = {0x003, 0x003, 0x003, 0x003},
        .reg_val     = {0x001, 0x001, 0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_SGPIO1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x010, 0x014},
        .reg_mask    = {0x003, 0x003},
        .reg_val     = {0x001, 0x001},
    },
    {
        /* localbus chip select 0 multiplexing */
        .module_id   = IO_MULTI_LOCALBUS_CS0,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x004},
        .reg_mask    = {0x003},
        .reg_val     = {0x000},
    },
    {
        /* localbus chip select 1 multiplexing */
        .module_id   = IO_MULTI_LOCALBUS_CS1,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x008},
        .reg_mask    = {0x003},
        .reg_val     = {0x000},
    },
    {
        /* localbus chip selct 2 multiplexing */
        .module_id   = IO_MULTI_LOCALBUS_CS2,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x094},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        /* localbus chip selct 3 multiplexing */
        .module_id   = IO_MULTI_LOCALBUS_CS3,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x070},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        /* the pins that should be configured for pin multiplexing when reused for CPLD (excluding chip select pins) */
        .module_id   = IO_MULTI_LOCALBUS_CPLD,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x5,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x000, 0x00C, 0x010, 0x014, 0x018},
        .reg_mask    = {0x003, 0x003, 0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x000},
    },
    {
    /* the pins that should be configured for pin multiplexing when reused for nonflash (excluding chip select pins) */
        .module_id   = IO_MULTI_LOCALBUS_NORFLASH,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x13,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_IOCFG_R,
                        IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x000, 0x00C, 0x010, 0x014, 0x018, 0x044, 0x048, 0x04C, 0x050, 0x054,
                        0x074, 0x078, 0x07C, 0x080, 0x084, 0x088, 0x08C, 0x090, 0x098},
        .reg_mask    = {0x003, 0x003, 0x003, 0x003, 0x003, 0x007, 0x007, 0x007, 0x007, 0x007,
                        0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x007, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
                        0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_BMC_UNSE_RST0_N,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x004},
        .reg_mask    = {0x003},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_INT,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x000},
        .reg_mask    = {0x003},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_VGA_SYNC,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_SYSCTL},
        .reg_offset  = {0xcc, 0x20c},
        .reg_mask    = {0x07, 0x1fffffff},
        .reg_val     = {0x00, 0x0800820f}, /* vga io, dac init value */
    },
    {
        .module_id   = IO_MULTI_VGA_SCL_SDA,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0xc8},
        .reg_mask    = {0x07},
        .reg_val     = {0x00},
    },
    {
        .module_id   = IO_MULTI_HOSTRTC,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0xac},
        .reg_mask    = {0x07},
        .reg_val     = {0x00},
    },
    {
        .module_id   = IO_MULTI_VCE_PIN_CAPTURE,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x30},
        .reg_mask    = {0x03},
        .reg_val     = {0x00},
    },
    {
        .module_id   = IO_MULTI_UART0,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x044},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_UART1,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x054},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_UART2,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x06c, 0x070},
        .reg_mask    = {0x003, 0x003},
        .reg_val     = {0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_UART3,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x074, 0x078},
        .reg_mask    = {0x003, 0x003},
        .reg_val     = {0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_UART4,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x07c, 0x080},
        .reg_mask    = {0x003, 0x003},
        .reg_val     = {0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_UART5,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x04c},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_UART6,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R},
        .reg_offset  = {0x050},
        .reg_mask    = {0x007},
        .reg_val     = {0x003},
    },
    {
        .module_id   = IO_MULTI_UART7,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x088, 0x08c},
        .reg_mask    = {0x007, 0x007},
        .reg_val     = {0x001, 0x001},
    },
    {
        .module_id   = IO_MULTI_CANBUS0,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x064},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_CANBUS1,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x068},
        .reg_mask    = {0x007},
        .reg_val     = {0x001},
    },
    {
        .module_id   = IO_MULTI_ESPI,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_LP},
        .reg_offset  = {0x0A0, 0x024},
        .reg_mask    = {0x003, 0x007},
        .reg_val     = {0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_LPC,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_R, IO_MULTI_REG_TYPE_LP},
        .reg_offset  = {0x0A0, 0x024},
        .reg_mask    = {0x003, 0x007},
        .reg_val     = {0x001, 0x004},
    },
    {
        .module_id   = IO_MULTI_USB_VBUS,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x0B0, 0x0B4},
        .reg_mask    = {0x007, 0x007},
        .reg_val     = {0x003, 0x003},
    },
    {
        .module_id   = IO_MULTI_CHIP_ACTIVE,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x1,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x0c4},
        .reg_mask    = {0x007},
        .reg_val     = {0x000},
    },
    {
        .module_id   = IO_MULTI_SFC0_CS0_NO_WPHOLD,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x008, 0x010},
        .reg_mask    = {0x003, 0x003},
        .reg_val     = {0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SFC0_CS1_NO_WPHOLD,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x008, 0x014},
        .reg_mask    = {0x003, 0x003},
        .reg_val     = {0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SFC1_CS0_NO_WPHOLD,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x018, 0x01c, 0x020, 0x028},
        .reg_mask    = {0x001, 0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_SFC1_CS1_NO_WPHOLD,
        .debug_flag  = 0x1,
        .reg_cnt     = 0x4,
        .reg_type    = {IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T,
                        IO_MULTI_REG_TYPE_IOCFG_T, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x018, 0x01c, 0x020, 0x02c},
        .reg_mask    = {0x001, 0x003, 0x003, 0x003},
        .reg_val     = {0x000, 0x000, 0x000, 0x000},
    },
    {
        .module_id   = IO_MULTI_UART12,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x0B0},
        .reg_mask    = {0x200, 0x007},
        .reg_val     = {0x000, 0x001},
    },
    {
        .module_id   = IO_MULTI_UART13,
        .debug_flag  = 0x0,
        .reg_cnt     = 0x2,
        .reg_type    = {IO_MULTI_REG_TYPE_LP, IO_MULTI_REG_TYPE_IOCFG_T},
        .reg_offset  = {0x334, 0x0B4},
        .reg_mask    = {0x400, 0x007},
        .reg_val     = {0x000, 0x001},
    },
};

/* Board pin multiplexing adaptation: 0 is card insertion board, 1 is EVB board */
bd_cfg_info g_board_cfg_info[] = {
    {
        .board_type = BOARD_TYPE_BUCKLE,
        .plat_id    = BOARD_PLATM_BUCKLE,
        .board_id   = 0,
        .pcb_ver    = 0,
        /* mid: 0 1 2 3 5 9 10 11 12 13 14 15 24 70 73 79 80 82 83 84 86 87 88 91 92 94 97 */
        .use_module_mask = { 0x100fe2f, 0x0, 0x59dd8240, 0x2 },
    }, {
        .board_type = BOARD_TYPE_BUCKLE,
        .plat_id    = BOARD_PLATM_SKYLAKE_BUCKLE,
        .board_id   = EVB_V100_PCB_VERSION,
        /* mid: 2 3 4 8 9 10 11 12 13 35 36 58 59 60 69 70 77 78 79 82 83 84 87 88 91 93 94 95 96 98 104 105 */
        .use_module_mask = { 0x3f1c, 0x1c000018, 0xe99ce060, 0x305 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_BUCKLE,
        .board_id   = EVB_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 4 9 10 12 13 14 15 23 24 27 51 52 69 70 71 72 73 79 80 82 83 84 86
         * 87 88 89 90 91 92 94 97 99 */
        .use_module_mask = { 0x980f61d, 0x180000, 0x5fdd83e0, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_SMMA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 2 3 8 9 10 11 12 13 14 15 16 19 35 79 82 91 92 94 95 97 */
        .use_module_mask = { 0x9ff0d, 0x8, 0xd8048000, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_GPUA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 4 5 7 9 10 11 12 13 14 15 16 24 79 83 84 86 87 88 90 91 92 94 95 97 99 101 */
        .use_module_mask = { 0x101febe, 0x0, 0xddd88000, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90BMCBB,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 2 8 9 10 11 12 13 14 15 16 19 35 36 79 84 94 97 */
        .use_module_mask = { 0x9ff05, 0x18, 0x40108000, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90BMCBBA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 2 8 9 10 11 12 13 14 15 16 19 35 36 79 84 94 97 */
        .use_module_mask = { 0x9ff05, 0x18, 0x40108000, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90GPUB,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 4 5 9 10 11 12 13 14 15 16 24 70 79 86 87 88 94 97 */
        .use_module_mask = { 0x101FE3E, 0x0, 0x41C08040, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN91SPUA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 4 5 9 10 11 12 13 14 15 16 17 19 20 24 35 36 70 79 83 84 85 87 88 94 97 */
        .use_module_mask = { 0x11BFE3E, 0x18, 0x41B88040, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90BMCC,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 6 9 10 11 12 13 14 15 16 17 19 20 24 70 73 79 86 87 88 91 94 97 99 */
        .use_module_mask = { 0x11bfe4e, 0x0, 0x49c08240, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN91LIUAA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 5 9 10 11 12 13 14 15 16 17 19 20 24 36 70 73 79 83 84 85 87 88 90 91 94 97 99 */
        .use_module_mask = { 0x11bfe2e, 0x10, 0x4db88240, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90GPUGA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 20 24 70 73 79 82 84 86 87 88 91 94 97 99 */
        .use_module_mask = { 0x11bfe0e, 0x0, 0x49d48240, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90GPUD,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 6 9 10 11 12 13 14 15 16 17 19 20 24 70 73 79 86 87 88 91 94 97 99 */
        .use_module_mask = { 0x11bfe4e, 0x0, 0x49c08240, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90GPUE,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 6 9 10 11 12 13 14 15 16 17 19 20 24 70 73 79 86 87 88 91 94 97 99 */
        .use_module_mask = { 0x11bfe4e, 0x0, 0x49c08240, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90ESWD,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 10 15 20 79 82 84 91 94 97 99 */
        .use_module_mask = { 0x10840c, 0x0, 0x48148000, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CN90GPUB_VB,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 4 5 9 10 11 12 13 14 15 16 24 79 86 91 92 94 97 99 101 */
        .use_module_mask = { 0x101fe3e, 0x0, 0x58408000, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_CM91LIUAA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 4 5 9 10 11 12 13 14 15 16 17 19 20 24 36 73 79 83 84 86 87 88 89 90 91 92 94 97 99 101 */
        .use_module_mask = { 0x11bfe3e, 0x10, 0x5fd88200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TCE,
        .board_id   = TCE_BOARD_ID_ARM241,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 5 6 9 10 11 12 13 14 15 16 17 19 20 24 36 79 82 84 87 91 92 94 97 99 101 */
        .use_module_mask = { 0x11bfe6e, 0x10, 0x58948000, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = TAISHAN_BOARD_ID_5290,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 4 5 9 10 11 12 13 14 15 16 24 79 83 84 86 87 88 90 91 92 94 95 97 99 101 */
        .use_module_mask = { 0x101fe3e, 0x0, 0xddd88000, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = FUSIONPOD_BC82MMC,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 5 9 10 11 12 13 14 15 16 24 70 79 82 83 84 86 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x101fe2e, 0x0, 0x5ddc8040, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = TAISHAN_BOARD_ID_2480,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 9 10 11 12 13 14 15 24 41 42 43 44 45 47 48 49 50 51 52 53 54 55
         * 56 57 59 60 69 71 75 79 82 83 84 86 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x100fe2f, 0x1bffbe00, 0x5ddc88a0, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = FUSION_DA140V2,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 9 10 11 12 13 14 15 24 41 42 43 44 45 47 48 49 50 51 52 53 54 55
         * 56 57 59 60 69 71 75 79 82 83 84 86 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x100fe2f, 0x1bffbe00, 0x5ddc88a0, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = FUSION_DA140V6,
        .pcb_ver    = PCB_COMMON_VER2,
        /* mid: 1 2 3 5 9 10 11 12 13 14 15 24 41 42 43 44 45 47 48 49 50 51 52 53 54 55 56 57 58 70
                75 79 83 84 85 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x100FE2E, 0x7FFBE00, 0x5DB88840, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = PLATM_X86_2288_V6_BC13MBSC,
        .pcb_ver    = PCB_COMMON_VER2,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 24 52 53 54 55 56 57 70 73 75 79 82 83 84 85 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x101fe0f, 0x3f00000, 0x5dbc8a40, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_2288H_V5_BC11SPSCB_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 24 73 79 82 83 84 85 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x100fe2d, 0x0, 0x59bc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_2288H_V5_BC11SPCCA_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 24 73 79 82 83 84 85 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x100fe2d, 0x0, 0x59bc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_1288H_V5_BC11SPSC_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 24 73 79 82 83 84 85 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x100fe2d, 0x0, 0x59bc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_1288H_V5_BC11SPSCC_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 24 73 79 82 83 84 85 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x100fe2d, 0x0, 0x59bc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_2288X_V5_BC11SPSG_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 19 24 73 79 82 83 84 86 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x108fe2d, 0x0, 0x59dc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_2288X_V5_BC11SPSGA_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 19 24 73 79 82 83 84 86 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x108fe2d, 0x0, 0x59dc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X86,
        .board_id   = RH_2288X_V5_BC11SPSGB_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 3 5 9 10 11 12 13 14 15 19 24 73 79 82 83 84 86 87 88 91 92 94 97 99 101 */
        .use_module_mask = { 0x108fe2d, 0x0, 0x59dc8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSPCE_SAS,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 24 35 69 70 75 79 82 84 86 91 92 94 97 99  */
        .use_module_mask = { 0x10bfe0e, 0x8, 0x58548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSPCE_PALM,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 24 35 69 70 75 79 82 84 86 91 92 94 97 99  */
        .use_module_mask = { 0x10bfe0e, 0x8, 0x58548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSPC_TEST,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 8 9 10 11 12 13 14 15 16 17 18 19 49 50 51 79 84 91 99 */
        .use_module_mask = { 0xfff04, 0xe0000, 0x8108000, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_800_BOARD_ID_9000,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 11 12 13 14 15 16 17 19 20 24 25 26 52 53 54 55 56 57 69 71 72 75
         * 79 82 83 84 86 87 88 90 91 92 94 97 99
         */
        .use_module_mask = { 0x71bf82f, 0x3f00000, 0x5ddc89a0, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = FUSION_RM210,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 8 9 10 12 13 45 46 47 77 78 79 82 84 */
        .use_module_mask = { 0x3705, 0xe000, 0x14e000, 0 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X_SERIES,
        .board_id   = FUSION_RM220,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 12 79 82 84 91 99 */
        .use_module_mask = { 0x1005, 0x0, 0x8108000, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = TAISHAN_BOARD_ID_2280E,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 4 5 9 10 11 12 13 14 15 16 24 79 83 84 86 87 88 90 91 92 94 95 97 99 101 */
        .use_module_mask = { 0x101fe3e, 0x0, 0xddd88000, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = KUNPENG_S920S03,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 7 9 10 11 12 13 14 15 16 24 79 83 84 86 87 88 91 92 94 95 97 101 102 */
        .use_module_mask = { 0x101feaf, 0x0, 0xd9d88000, 0x62 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_TAISHAN,
        .board_id   = RM211_IT31RMUC,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 8 9 10 12 45 46 47 77 78 79 82 84 91 99 103 */
        .use_module_mask = { 0x1705, 0xe000, 0x814e000, 0x88 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6EXCN,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 8 9 10 11 12 13 14 15 16 17 18 19 35 75 79 82 84 91 92 94 97 99 */
        .use_module_mask = { 0xfff0c, 0x8, 0x58148800, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_HIGH_SERVE,
        .board_id   = HAIYAN_BOARD_ID_5,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 9 10 11 12 13 14 15 24 41 42 43 44 45 47 48 49 50 51 52 53 54
           55 56 57 58 70 73 75 79 82 83 84 85 87 88 90 91 92 94 97 */
        .use_module_mask = { 0x100fe2f, 0x7ffbe00, 0x5dbc8a40, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_MANAGEBOARD,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 8 13 35 36 79 84 */
        .use_module_mask = { 0x2105, 0x18, 0x108000, 0x0 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCD,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 20 21 24 70 75 79 86 94 97 99 */
        .use_module_mask = { 0x13bfe0e, 0x0, 0x40408840, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSPCM_PACI,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 19 24 69 70 75 79 82 84 86 91 92 94 97 */
        .use_module_mask = { 0x10bfe0f, 0x0, 0x58548860, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSP_PACI_T,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 19 24 69 70 75 79 82 84 86 91 92 94 97 */
        .use_module_mask = { 0x10bfe0f, 0x0, 0x58548860, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSPCM_ARCT,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 19 24 69 70 75 79 82 84 86 91 92 94 97 */
        .use_module_mask = { 0x10bfe0f, 0x0, 0x58548860, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSP_ARCT_T,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 19 24 69 70 75 79 82 84 86 91 92 94 97 */
        .use_module_mask = { 0x10bfe0f, 0x0, 0x58548860, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6TSPCM_ATLA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 19 24 69 70 75 79 82 84 86 91 92 94 97 */
        .use_module_mask = { 0x10bfe0f, 0x0, 0x58548860, 0x2 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_500_IT21EKMA_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 7 9 10 11 12 13 14 15 16 24 79 83 84 86 87 88 91 92 94 95 97 101 102 */
        .use_module_mask = { 0x101feaf, 0x0, 0xd9d88000, 0x62 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_CDLS_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER3,
        /* mid: 2 3 8 9 10 11 12 13 17 19 21 41 49 50 51 52 53 54 55 56 57 59 60 69 70 75 77 78 79 84 91 92 94 97 99 */
        .use_module_mask = { 0x2a3f0c, 0x1bfe0200, 0x5810e860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_2_3_0_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 19 75 79 84 87 91 92 94 97 99 101 */
        .use_module_mask = { 0xbfe0f, 0x0, 0x58908800, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6ICBP,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 35 36 79 84 91 99  */
        .use_module_mask = { 0x4, 0x18, 0x8108000, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_800_BOARD_ID_9010,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 5 11 12 13 14 15 16 17 19 20 24 25 26 73 75 79 82 83 84 86 87 88 90 91 92 94 97 101 102 */
        .use_module_mask = { 0x71bf82e, 0x0, 0x5ddc8a00, 0x62 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_T1_5_0_POD_IT21SK4B,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 5 11 12 13 14 15 16 17 19 20 24 25 26 52 53 54 55 56 57 75 79 82 83 84 86 87 88 90 91 92
                94 97 99 100 102 */
        .use_module_mask = { 0x71bf82f, 0x3f00000, 0x5ddc8800, 0x5a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_T1_5_0_POD_IT21SK4C,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 5 11 12 13 14 15 16 17 19 20 24 25 26 52 53 54 55 56 57 75 79 82 83 84 86 87 88 90 91 92
                94 97 99 100 102 */
        .use_module_mask = { 0x71bf82f, 0x3f00000, 0x5ddc8800, 0x5a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_T1_5_0_BOX_IT21SDHA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 5 12 13 14 15 16 19 24 25 26 27 79 84 90 91 94 97 99 */
        .use_module_mask = { 0xf09f02e, 0x0, 0x4c108000, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_800_IT21MNNA_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 5 8 9 10 11 12 13 14 15 16 35 36 43 44 45 47 48 49 50 51 52 53 54 55 75 79 82 83 84 90 91
                92 94 97 99 100 101 */
        .use_module_mask = { 0x1ff2f, 0xffb818, 0x5c1c8800, 0x3a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_ATLAS,
        .board_id   = ATLAS_800_IT21MNCB_BOARD_ID,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 8 9 10 11 12 13 14 15 16 35 36 75 79 84 86 91 92 94 97 99 100 101 */
        .use_module_mask = { 0x1ff2f, 0x18, 0x58508800, 0x3a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6DESC_SAS,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 35 69 70 75 79 82 83 84 86 91 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x8, 0x485c8860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6DESC_PALM,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 35 69 70 75 79 82 83 84 86 91 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x8, 0x485c8860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCI,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 35 69 70 75 79 82 84 86 91 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x8, 0x48548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCU01,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 35 69 70 75 79 82 84 86 91 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x8, 0x48548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCU03,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 35 69 70 75 79 82 84 86 91 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x8, 0x48548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCK,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 35 69 70 75 79 82 84 86 91 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x8, 0x48548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_KUNPENG,
        .board_id   = KUNPENG_BOARD_ID_BC82AMDJ,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 10 11 12 13 14 15 16 17 19 20 24 25 59 60 69 71 75 79 82 83 84 86 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x31bfc2f, 0x18000000, 0x5ddc88a0, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_KUNPENG,
        .board_id   = KUNPENG_BOARD_ID_BC82AMDL,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 5 9 10 11 12 13 14 15 16 17 19 20 24 59 60 70 75 79 82 83 84 86 87 88 90 91 92 94 97 99  */
        .use_module_mask = { 0x11bfe2f, 0x18000000, 0x5ddc8840, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_KUNPENG,
        .board_id   = KUNPENG_BOARD_ID_BC82AMDR,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 1 2 3 5 8 9 10 11 12 13 14 15 16 17 19 20 70 75 79 82 83 84 86 87 88 90 91 92 94 97 99 */
        .use_module_mask = { 0x1bff2e, 0x0, 0x5ddc8840, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6EXCN01,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 8 9 10 11 12 13 14 15 16 17 18 19 35 75 79 82 84 91 92 94 97 99 */
        .use_module_mask = { 0xfff0c, 0x8, 0x58148800, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCQ_T,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 8 9 10 11 12 13 14 15 16 19 35 36 69 70 75 79 82 84 86 91 92 94 97 99 */
        .use_module_mask = { 0x9ff0e, 0x18, 0x58548860, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCQ,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 8 9 10 11 12 13 14 15 16 17 19 35 36 69 70 75 79 82 84 86 91 92 99 */
        .use_module_mask = { 0xbff0e, 0x18, 0x18548860, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCO,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 16 17 19 24 35 69 75 77 78 79 82 84 86 91 92 94 97 99 */
        .use_module_mask = { 0x10b3e0f, 0x8, 0x5854e820, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCO12,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 9 10 11 12 13 16 17 19 24 35 69 75 77 78 79 82 84 86 91 92 94 97 99 */
        .use_module_mask = { 0x10b3e0f, 0x8, 0x5854e820, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCQ10,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 8 9 10 11 12 13 14 15 16 17 19 35 36 69 70 75 79 82 84 86 91 92 99 */
        .use_module_mask = { 0xbff0e, 0x18, 0x18548860, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCQ20,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 8 9 10 11 12 13 14 15 16 17 19 35 36 69 70 75 79 82 84 86 91 92 99 */
        .use_module_mask = { 0xbff0e, 0x18, 0x18548860, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X_SERIES,
        .board_id   = X_SERIES_X6000_V6_BC23SPSA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 17 24 70 73 75 79 82 83 84 85 87 91 92 94 97 99 */
        .use_module_mask = { 0x102fe0e, 0x0, 0x58bc8a40, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_X_SERIES,
        .board_id   = X_SERIES_X6000_V6_BC23SPSB,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 17 24 70 73 75 79 82 83 84 85 87 91 92 94 97 99 */
        .use_module_mask = { 0x102fe0e, 0x0, 0x58bc8a40, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_BUCKLE_OSCA,
        .board_id   = OSCA_E9000_V6_IT11SWCA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 9 10 11 12 13 14 15 16 24 70 73 79 80 85 90 91 92 94 97 99 */
        .use_module_mask = { 0x101fe0c, 0x0, 0x5c218240, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCJ,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 21 24 35 36 69 70 75 79 86 91 92 99 */
        .use_module_mask = { 0x12bfe0e, 0x18, 0x18408860, 0x8 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6MGTD10,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 2 8 13 35 36 79 84 */
        .use_module_mask = { 0x2105, 0x18, 0x108000, 0x0 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_BC13MBSBF,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 9 10 11 12 13 14 15 16 17 24 36 73 79 82 83 86 90 91 92 94 97 99 101 */
        .use_module_mask = { 0x103fe0c, 0x10, 0x5c4c8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_BC13MBSBF2,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 9 10 11 12 13 14 15 16 17 24 36 73 79 82 83 86 90 91 92 94 97 99 101 */
        .use_module_mask = { 0x103fe0c, 0x10, 0x5c4c8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_BC82ADDRF,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 9 10 11 12 13 14 15 16 17 24 36 73 79 82 83 86 90 91 92 94 97 99 101 */
        .use_module_mask = { 0x103fe0c, 0x10, 0x5c4c8200, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6ICBA01,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 19 24 35 36 75 79 84 86 91 92 93 94 97 99 101 */
        .use_module_mask = { 0x109fe0e, 0x18, 0x78508800, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6ICBA02,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 19 24 35 36 75 79 84 86 91 92 93 94 97 99 101 */
        .use_module_mask = { 0x109fe0e, 0x18, 0x78508800, 0x2a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCY,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 24 35 36 75 79 83 84 86 87 88 91 92 99 101 */
        .use_module_mask = { 0x10bfe0e, 0x18, 0x19d88800, 0x28 },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCW_S,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 24 35 36 75 79 82 84 86 91 92 94 97 99 100 101 */
        .use_module_mask = { 0x10bfe0e, 0x18, 0x58548800, 0x3a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCW_C,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 15 16 17 19 24 35 36 75 79 84 86 91 92 94 97 99 100 101 */
        .use_module_mask = { 0x10bfe0e, 0x18, 0x58508800, 0x3a },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_PANGEA_V6,
        .board_id   = PANGEA_V6_BOARD_ID_STL6SPCR,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 1 2 3 9 10 11 12 13 14 19 24 70 75 79 84 86 91 92 93 94 97 99 */
        .use_module_mask = { 0x1087e0e, 0x0, 0x78508840, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_CHH,
        .board_id   = CHH_BOARD_ID_SPGPBA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 4 5 9 10 11 12 13 14 15 16 17 19 20 24 36 70 79 82 83 84 86 91 92 94 97 99 */
        .use_module_mask = { 0x11bfe3f, 0x10, 0x585c8040, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_CHH,
        .board_id   = CHH_BOARD_ID_SPSWBA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 4 5 8 9 10 11 12 13 14 15 16 17 19 20 36 70 79 82 83 84 86 91 92 94 97 99 */
        .use_module_mask = { 0x1bff3f, 0x10, 0x585c8040, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_CHH,
        .board_id   = CHH_BOARD_ID_CHH1FPFMBA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 2 3 4 5 8 9 10 11 12 13 14 15 17 19 20 36 41 42 52 53 70 79 84 91 92 94 97 99 */
        .use_module_mask = { 0x1aff3c, 0x300610, 0x58108040, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_CHH,
        .board_id   = CHH_BOARD_ID_CHH1SPMNTA,
        .pcb_ver    = PCB_COMMON_VER0,
        /* mid: 0 1 2 3 4 5 8 17 18 19 20 36 41 42 52 53 54 55 56 57 58 59 60 70 77 78 79 82 83 84 91 92 94 97 99 */
        .use_module_mask = { 0x1e013f, 0x1ff00610, 0x581ce040, 0xa },
    }, {
        .board_type = BOARD_TYPE_HOST,
        .plat_id    = BOARD_PLATM_CA2200,
        .board_id   = CA2200_BOARD_ID_V7_CM15MBCA,
        .pcb_ver    = PCB_COMMON_VER1,
        /* mid: 0 1 2 3 9 10 11 12 13 14 15 16 17 18 19 20 69 70 71 73 75 79 83 84 85 87 88 90 91 94 97 99 */
        .use_module_mask = { 0x1ffe0f, 0x0, 0x4db88ae0, 0xa },
    },
};


/* boot cfg pin information */
boot_cfg_info_s g_boot_cfg_info;

/**
 * @brief: configuration mouldes's IO multiplexing related registers.
 * @param: configuration parameters
 * @retval: 0 is success, other is failure
 */
static s32 bdinfo_set_iocfg_reg(io_multi_cfg_s *module_cfg)
{
    u32 i;
    s32 ret = 0;
    u32 type;

    /* Judgment of input parameters. */
    if (module_cfg->reg_cnt > IO_MULTI_REG_MAX_CNT) {
        LOG(LOG_ERROR, "bdifo cfg module(0x%x)'s reg_cnt(0x%x) is inval!",
            module_cfg->module_id, module_cfg->reg_cnt);

        return -EINVAL;
    }

    for (i = 0; i < module_cfg->reg_cnt; i++) {
        type = module_cfg->reg_type[i];
        switch (type) {
            case IO_MULTI_REG_TYPE_IOCFG_R:
                ret = io_config_r_reg_read_write(module_cfg->reg_offset[i], module_cfg->reg_mask[i],
                                                 module_cfg->reg_val[i]);
                break;

            case IO_MULTI_REG_TYPE_IOCFG_T:
                ret = io_config_t_reg_read_write(module_cfg->reg_offset[i], module_cfg->reg_mask[i],
                                                 module_cfg->reg_val[i]);
                break;

            case IO_MULTI_REG_TYPE_LP:
                ret = lp_subsys_reg_read_write(module_cfg->reg_offset[i], module_cfg->reg_mask[i],
                                               module_cfg->reg_val[i]);
                break;

            case IO_MULTI_REG_TYPE_SYSCTL:
                ret = sysctrl_reg_read_write(module_cfg->reg_offset[i], module_cfg->reg_mask[i],
                                             module_cfg->reg_val[i]);
                break;

            default:
                LOG(LOG_ERROR, "bdifo cfg module(0x%x)'s reg_type(0x%x) is inval!",
                    module_cfg->module_id, module_cfg->reg_type[i]);
                return -EINVAL;
        }

        if (ret) {
            LOG(LOG_PRINT, "bdifo cfg module(0x%x)'s reg_type(0x%x),offset(0x%x),mask(0x%x),val(0x%x) %s!",
                module_cfg->module_id, module_cfg->reg_type[i], module_cfg->reg_offset[i],
                module_cfg->reg_mask[i], module_cfg->reg_val[i], ((ret != 0) ? "failed" : "success"));
            break;
        }
    }

    return ret;
}

/**
 * @brief: get the configuration information of the module.
 * @param: u16 module_id: module id for IO multiplexing, corresponding to IO_MULTI_MODULE_ID.
 * @retval: io_multi_cfg_s.
 */
static io_multi_cfg_s *bdinfo_get_module_cfginfo(u16 module_id)
{
    u16 i;
    u32 io_config_num;
    io_multi_cfg_s *module_cfg = NULL;

    if (module_id >= IO_MULTI_MODULE_MAX) {
        LOG(LOG_ERROR, "Bdinfo iocfg module(0x%x) is inval!", module_id);
        return NULL;
    }

    io_config_num = sizeof(g_chip_io_cfg_table) / sizeof(io_multi_cfg_s);
    for (i = 0; i < io_config_num; i++) {
        module_cfg = &g_chip_io_cfg_table[i];
        if (module_id == module_cfg->module_id) {
            break;
        }
    }

    if (i >= io_config_num) {
        LOG(LOG_ERROR, "Can't find module(0x%x)'s ioconfig info!", module_id);
        return NULL;
    }

    return module_cfg;
}

/**
 * @brief: get the value of the corresponding module's register based on the register index.
 * @param: u16 module_id: module ID; u32 reg_index: regiser index; u32 *val: value of register.
 * @retval: 0 is success, other is failure.
 */
s32 bdinfo_get_ioreg_val_by_index(u16 module_id, u32 reg_index, u32 *val)
{
    s32 ret = 0;
    u32 type;
    io_multi_cfg_s *module_cfg = NULL;

    if (!val) {
        LOG(LOG_ERROR, "Error: parameter val is NULL");
        return -EINVAL;
    }

    module_cfg = bdinfo_get_module_cfginfo(module_id);
    if (!module_cfg) {
        return -EINVAL;
    }

    if ((module_cfg->reg_cnt > IO_MULTI_REG_MAX_CNT) || (reg_index >= module_cfg->reg_cnt)) {
        LOG(LOG_ERROR, "bdifo cfg module(0x%x)'s reg_cnt(0x%x) index(0x%x)is inval!",
            module_cfg->module_id, module_cfg->reg_cnt, reg_index);

        return -EINVAL;
    }

    type = module_cfg->reg_type[reg_index];
    switch (type) {
        case IO_MULTI_REG_TYPE_IOCFG_R:
            ret = io_config_r_reg_read(module_cfg->reg_offset[reg_index], val);
            break;

        case IO_MULTI_REG_TYPE_IOCFG_T:
            ret = io_config_t_reg_read(module_cfg->reg_offset[reg_index], val);
            break;

        case IO_MULTI_REG_TYPE_LP:
            ret = lp_subsys_reg_read(module_cfg->reg_offset[reg_index], val);
            break;

        case IO_MULTI_REG_TYPE_SYSCTL:
            ret = sysctrl_reg_read(module_cfg->reg_offset[reg_index], val);
            break;

        default:
            LOG(LOG_ERROR, "bdifo cfg module(0x%x)'s reg_type(0x%x) is inval!",
                module_cfg->module_id, module_cfg->reg_type[reg_index]);
            return -EINVAL;
    }

    if (ret || module_cfg->debug_flag) {
        LOG(LOG_PRINT, "bdifo cfg module(0x%x)'s reg_type(0x%x),index(0x%x),val(0x%p) %s!",
            module_cfg->module_id, type, reg_index, val, ((ret != 0) ? "failed" : "success"));
    }

    return ret;
}
EXPORT_SYMBOL(bdinfo_get_ioreg_val_by_index);

/**
 * @brief: configure module's IO pin multiplexing.
 * @param: u16 moudle_id: module id for IO multiplexing, corresponding to IO_MULTI_MODULE_ID.
 * @retval: 0 is success, other is failure.
 */
s32 bdinfo_set_module_iocfg(u16 module_id)
{
    s32 ret;
    io_multi_cfg_s *module_cfg = NULL;

    module_cfg = bdinfo_get_module_cfginfo(module_id);
    if (!module_cfg) {
        LOG(LOG_ERROR, "module id 0x%x not cfg!", module_id);
        return -EFAULT;
    }

    ret = bdinfo_set_iocfg_reg(module_cfg);
    if (ret) {
        LOG(LOG_ERROR, "board_reg_cfg_in_seq cfg err!");
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(bdinfo_set_module_iocfg);

/**
 * @brief: get board id
 * @param: void
 * @retval: board id
 */
u32 bdinfo_get_board_id(void)
{
    return g_boot_cfg_info.bits.board_id;
}
EXPORT_SYMBOL(bdinfo_get_board_id);

/**
 * @brief: get platform id
 * @param: void
 * @retval: platform id
 */
u32 bdinfo_get_platform_id(void)
{
    return g_boot_cfg_info.bits.platform_id;
}
EXPORT_SYMBOL(bdinfo_get_platform_id);

/**
 * @brief: get pcb version
 * @param: void
 * @retval: pcb version
 */
u32 bdinfo_get_pcb_ver(void)
{
    return g_boot_cfg_info.bits.pcb_ver;
}
EXPORT_SYMBOL(bdinfo_get_pcb_ver);

/**
 * @brief: get board type: card insertion / chip placement
 * @param: void
 * @retval: board type
 */
u32 bdinfo_get_board_type(void)
{
    return g_boot_cfg_info.bits.board_type;
}
EXPORT_SYMBOL(bdinfo_get_board_type);

/**
 * @brief: get if the board uses eSPI or LPC
 * @parm: void
 * @retval: HOST PCH type
 */
s32 bdinfo_get_host_pchproto(void)
{
    if (g_boot_cfg_info.bits.is_espi) {
        return BDINFO_HOST_PCHPROTO_ESPI;
    } else {
        return BDINFO_HOST_PCHPROTO_LPC;
    }
}
EXPORT_SYMBOL(bdinfo_get_host_pchproto);

/**
 * @brief: get board system type
 * @param: void
 * @retval: system type
 */
u32 bdinfo_get_system_type(void)
{
    return g_boot_cfg_info.bits.system_type;
}
EXPORT_SYMBOL(bdinfo_get_system_type);

/**
 * @brief: get board boot cfg infomation
 * @param: void
 * @retval: cfg info
 */
u32 bdinfo_get_boot_cfg_info(void)
{
    return g_boot_cfg_info.val;
}
EXPORT_SYMBOL(bdinfo_get_boot_cfg_info);

/**
 * @brief: Check if the IO pin multiplexing relationship corresponding to the module is configured correctly.
 * @param: u16 module_id: IOCFG module_id.
 * @retval: 0-The pin and the expected function of the module do not match (or there is an internal error).
 *          1-The pin and the expected function of the module match.
 */
s32 bdinfo_iocfg_check(u16 module_id)
{
    u32 i;
    s32 ret;
    u32 reg_val = 0;
    io_multi_cfg_s *cfg = NULL;

    cfg = bdinfo_get_module_cfginfo(module_id);
    if (cfg == NULL) {
        return 0;
    }

    for (i = 0; i < cfg->reg_cnt; i++) {
        ret = bdinfo_get_ioreg_val_by_index(module_id, cfg->reg_offset[i], &reg_val);
        if (ret) {
            return 0;
        }

        if (cfg->reg_val[i] != reg_val) {
            return 0;
        }
    }

    return 1;
}
EXPORT_SYMBOL(bdinfo_iocfg_check);

/**
 * @brief: get current board configuration information.
 * @param: void
 * @retval: void
 */
static bd_cfg_info *bdinfo_get_cfg_info(void)
{
    u32 i;
    u32 board_type = bdinfo_get_board_type();
    u32 plat_id = bdinfo_get_platform_id();
    u32 board_id = bdinfo_get_board_id();

    for (i = 0; i < (sizeof(g_board_cfg_info) / sizeof(g_board_cfg_info[0])); i++) {
        if (board_type == BOARD_TYPE_BUCKLE) {
            if (plat_id == g_board_cfg_info[i].plat_id) {
                return &g_board_cfg_info[i];
            }
        } else {
            if ((plat_id == g_board_cfg_info[i].plat_id) && (plat_id == BOARD_PLATM_SKYLAKE_BUCKLE)) {
                return &g_board_cfg_info[i];
            }

            if ((plat_id == g_board_cfg_info[i].plat_id) && (board_id == g_board_cfg_info[i].board_id)) {
                return &g_board_cfg_info[i];
            }
        }
    }

    LOG(LOG_ERROR, "plat 0x%x board 0x%x not support, use default!", plat_id, board_id);

    return &g_board_cfg_info[0];
}

static u32 bdinfo_module_id_is_used(u32 module_id, bd_cfg_info *cur_board)
{
    if (BIT_MASK(module_id % VALID_MASK_BIT_NUM) & cur_board->use_module_mask[module_id / VALID_MASK_BIT_NUM]) {
        return 1;
    }

    return 0;
}

static void bdinfo_fix_lpc_espi_ioconfig(bd_cfg_info *cur_board)
{
    s32 pch_protocol = bdinfo_get_host_pchproto();

    /*
     * The pins are not configured for LPC or eSPI.
     * It is possible that the pins are being used for other purposes,
     * and therefore neither eSPI nor LPC can be configured.
     */
    if (!bdinfo_module_id_is_used(IO_MULTI_LPC,  cur_board) &&
        !bdinfo_module_id_is_used(IO_MULTI_ESPI, cur_board)) {
        return;
    }

    /*
     * If the pins are configured for LPC or eSPI, then the pins can only be used by eSPI or LPC.
     * Adjust the pin configuration based on the hardware signals.
     */
    if (pch_protocol == BDINFO_HOST_PCHPROTO_ESPI) {
        cur_board->use_module_mask[IO_MULTI_LPC  / VALID_MASK_BIT_NUM] &=
                ~BIT_MASK(IO_MULTI_LPC  % VALID_MASK_BIT_NUM);
        cur_board->use_module_mask[IO_MULTI_ESPI / VALID_MASK_BIT_NUM] |=
                BIT_MASK(IO_MULTI_ESPI  % VALID_MASK_BIT_NUM);
    } else if (pch_protocol == BDINFO_HOST_PCHPROTO_LPC) {
        cur_board->use_module_mask[IO_MULTI_ESPI / VALID_MASK_BIT_NUM] &=
                ~BIT_MASK(IO_MULTI_ESPI % VALID_MASK_BIT_NUM);
        cur_board->use_module_mask[IO_MULTI_LPC  / VALID_MASK_BIT_NUM] |=
                BIT_MASK(IO_MULTI_LPC   % VALID_MASK_BIT_NUM);
    } else {
        cur_board->use_module_mask[IO_MULTI_ESPI / VALID_MASK_BIT_NUM] &=
                ~BIT_MASK(IO_MULTI_ESPI % VALID_MASK_BIT_NUM);
        cur_board->use_module_mask[IO_MULTI_LPC  / VALID_MASK_BIT_NUM] &=
                ~BIT_MASK(IO_MULTI_LPC  % VALID_MASK_BIT_NUM);
    }
}

static void bdinfo_fix_ioconfig(bd_cfg_info *cur_board)
{
    /* adjusting eSPI/LPC pin multiplexing configuration */
    bdinfo_fix_lpc_espi_ioconfig(cur_board);
}

s32 bdinfo_cur_board_moduleid_is_used(u32 module_id)
{
    bd_cfg_info *cfg_info = NULL;

    if (module_id >= IO_MULTI_MODULE_MAX) {
        LOG(LOG_ERROR, "check if board_module id is used failed. invalid param:%u", module_id);
        return -EINVAL;
    }

    cfg_info = bdinfo_get_cfg_info();

    return bdinfo_module_id_is_used(module_id, cfg_info);
}
EXPORT_SYMBOL(bdinfo_cur_board_moduleid_is_used);

/**
 * @brief: For card insertion platform id conversion, according to product requirements,
 *         the platform ID is reduced to 2 digits, with corresponding values seen in the function implementation.
 * @param: u32 platform_id: the 2-digit pin id read.
 * @retval: void
 */
static u32 bdinfo_convert_platform_id(u32 platform_id)
{
    switch (platform_id) {
        case 0x0:
            return 0x0; /* 0x0 ->> 0x0  */

        case 0x1:
            return 0x7; /* 0x1 ->> 0x7  */

        case 0x2:
            return 0xa; /* 0x2 ->> 0xa  */

        case 0x3:
            return 0xf; /* 0x3 ->> 0xd  */

        default:
            return 0x0;
    }
}

static void bdinfo_boot_cfg_base_get(u32 strap_info, u32 initial_info, u32 initial_info2, u32 initial_info3,
                                     boot_cfg_info_s *board_cfg_info)
{
    /* hostrtc_clk -> boot_cfg35 */
    board_cfg_info->bits.single_chip = bit_value(initial_info2, 3);

    /* lb_data[2] -> boot_cfg56 */
    /* sdio_clk -> boot_cfg36 */
    /* lb_ale -> boot_cfg8 */
    board_cfg_info->bits.pcb_ver = (bit_value(initial_info2, 24)) |       /* initial_info2.bit24 as pcb_ver.bit0 */
                                   (bit_value(initial_info2, 4) << 0x1) | /* initial_info2.bit4 as pcb_ver.bit1 */
                                   (bit_value(initial_info, 8) << 0x2);   /* initial_info.bit8 as pcb_ver.bit2 */

    /* lb_data[6]->strap5 */
    /* lb_data[7]->strap6 */
    /* lb_data[8]->strap7 */
    /* UTXD4 ->boot_cfg76 */
    /* URXD4 ->boot_cfg75 */
    /* PF_17 ->boot_cfg32 */
    /* PF_18 ->boot_cfg33 */
    /* PF_19 ->boot_cfg34 */
    board_cfg_info->bits.board_id = (bit_value(strap_info, 5)) |            /* strap_sts.bit5 as board_id.bit0 */
                                    (bit_value(strap_info, 6) << 0x1) |     /* strap_sts.bit6 as board_id.bit1 */
                                    (bit_value(strap_info, 7) << 0x2) |     /* strap_sts.bit7 as board_id.bit2 */
                                    (bit_value(initial_info3, 12) << 0x3) | /* initial_info3.bit12 as board_id.bit3 */
                                    (bit_value(initial_info3, 11) << 0x4) | /* initial_info3.bit11 as board_id.bit4 */
                                    (bit_value(initial_info2, 0) << 0x5) |  /* initial_info3.bit0 as board_id.bit5 */
                                    (bit_value(initial_info2, 1) << 0x6) |  /* initial_info3.bit1 as board_id.bit6 */
                                    (bit_value(initial_info2, 2) << 0x7);   /* initial_info3.bit2 as board_id.bit7 */

    /* vga_hync_out->boot_cfg46 */
    /* vga_vync_out->boot_cfg47 */
    /* sfc0_soi->boot_cfg60 */
    /* sfc0_sio->boot_cfg59 */
    board_cfg_info->bits.platform_id = (bit_value(initial_info2, 14)) |        /* info2.bit14 as platform_id.bit0 */
                                       (bit_value(initial_info2, 15) << 0x1) | /* info2.bit15 as platform_id.bit1 */
                                       (bit_value(initial_info2, 28) << 0x2) | /* info2.bit28 as platform_id.bit2 */
                                       (bit_value(initial_info2, 27) << 0x3);  /* info2.bit27 as platform_id.bit3 */

    /* pf_11->boot_cfg26 */
    /* pf_12->boot_cfg27 */
    board_cfg_info->bits.ddr_size = (bit_value(initial_info, 26)) |       /* initial_info.bit26 as ddr_size.bit0 */
                                    (bit_value(initial_info, 27) << 0x1); /* initial_info.bit27 as ddr_size.bit1 */

    /* pf_13->boot_cfg28 */
    board_cfg_info->bits.ddr_bits = bit_value(initial_info, 28);

    /* pf_14->boot_cfg29 */
    board_cfg_info->bits.debug_gmac = bit_value(initial_info, 29);

    /* pf_15->boot_cfg30 */
    board_cfg_info->bits.system_type = bit_value(initial_info, 30);

    /* pf_16->boot_cfg31 */
    board_cfg_info->bits.ddr_ecc = bit_value(initial_info, 31);

    /* lb_data[24]->boot_cfg20 */
    board_cfg_info->bits.is_espi = bit_value(initial_info, 20);

    board_cfg_info->bits.board_type = BOARD_TYPE_HOST;

    return;
}

static u32 bdinfo_sub_platform_id_get(u32 initial_info2)
{
    /* sdio_data[2]->boot_cfg42 */
    /* sdio_data[3]->boot_cfg43 */
    return (bit_value(initial_info2, 10)) |          /* initial_info2.bit10 as platform_id.bit0 */
           (bit_value(initial_info2, 11) << 0x1);    /* initial_info2.bit11 as platform_id.bit1 */
}

static u32 bdinfo_buckle_board_id_get(u32 strap_info, u32 initial_info2, u32 initial_info3)
{
    /* UTXD4->boot_cfg76 */
    /* URXD4->boot_cfg75 */
    /* lb_data[6]->strap5 */
    /* lb_data[7]->strap6 */
    /* lb_data[8]->strap7 */
    /* PF_17->boot_cfg32 */
    /* PF_18->boot_cfg33 */
    /* PF_19->boot_cfg34 */
    return (bit_value(initial_info3, 12)) |         /* info3.bit12 as board_id.bit0 */
           (bit_value(initial_info3, 11) << 0x1) |  /* info3.bit11 as board_id.bit1 */
           (bit_value(strap_info, 5) << 0x2) |      /* strap.bit5  as board_id.bit2 */
           (bit_value(strap_info, 6) << 0x3) |      /* strap.bit6  as board_id.bit3 */
           (bit_value(strap_info, 7) << 0x4) |      /* strap.bit7  as board_id.bit4 */
           (bit_value(initial_info2, 0) << 0x5) |   /* info2.bit0  as board_id.bit5 */
           (bit_value(initial_info2, 1) << 0x6) |   /* info2.bit1  as board_id.bit6 */
           (bit_value(initial_info2, 2) << 0x7);    /* info2.bit2  as board_id.bit7 */
}

static u32 bdinfo_new_buckle_board_id_get(u32 strap_info, u32 initial_info2)
{
    /* sdio_data[2]->boot_cfg42 */
    /* sdio_data[3]->boot_cfg43 */
    /* lb_data[6]->strap5 */
    /* lb_data[7]->strap6 */
    /* lb_data[8]->strap7 */
    /* PF_17->boot_cfg32 */
    /* PF_18->boot_cfg33 */
    /* PF_19->boot_cfg34 */
    return (bit_value(initial_info2, 10)) |         /* initial_info2.bit10 as board_id.bit0 */
           (bit_value(initial_info2, 11) << 0x1) |  /* initial_info2.bit11 as board_id.bit1 */
           (bit_value(strap_info, 5) << 0x2) |      /* strap.bit5  as board_id.bit2 */
           (bit_value(strap_info, 6) << 0x3) |      /* strap.bit6  as board_id.bit3 */
           (bit_value(strap_info, 7) << 0x4) |      /* strap.bit7  as board_id.bit4 */
           (bit_value(initial_info2, 0) << 0x5) |   /* info2.bit0  as board_id.bit5 */
           (bit_value(initial_info2, 1) << 0x6) |   /* info2.bit1  as board_id.bit6 */
           (bit_value(initial_info2, 2) << 0x7);    /* info2.bit2  as board_id.bit7 */
}

static u32 bdinfo_buckle_pcb_ver_get(u32 initial_info2)
{
    u32 pcb_ver;

    /* lb_data[2] -> boot_cfg56 */
    /* sdio_data[0]->boot_cfg40 */
    /* sdio_data[1]->boot_cfg41 */
    pcb_ver = (bit_value(initial_info2, 24)) |        /* initial_info2.bit24 as board_id.bit0 */
            (bit_value(initial_info2, 8) << 0x1) |    /* initial_info2.bit8  as board_id.bit1 */
            (bit_value(initial_info2, 9) << 0x2);     /* initial_info2.bit9  as board_id.bit2 */
    return (~pcb_ver) & 0x7;
}

static void bdinfo_boot_cfg_fix(u32 strap_info, u32 initial_info, u32 initial_info2, u32 initial_info3,
                                boot_cfg_info_s *board_cfg_info)
{
    u32 sub_platform_id = bdinfo_sub_platform_id_get(initial_info2);

    if (board_cfg_info->bits.platform_id == BOARD_PLATM_BUCKLE) {
        /* special adaptation for EVB, treated as a placement board. */
        if (board_cfg_info->bits.board_id == EVB_BOARD_ID) {
            board_cfg_info->bits.board_type = BOARD_TYPE_HOST;
            if (board_cfg_info->bits.pcb_ver != EVB_V110_PCB_VERSION) {
                board_cfg_info->bits.pcb_ver = PCB_COMMON_VER0;
                board_cfg_info->bits.is_espi = 0x0;
                board_cfg_info->bits.ddr_size = 0x2;
                board_cfg_info->bits.ddr_bits = 0x1;
                board_cfg_info->bits.ddr_ecc = 0x0;
            }
            board_cfg_info->bits.single_chip = 0x0;
            board_cfg_info->bits.debug_gmac = 0x1;
            board_cfg_info->bits.system_type = 0x0;
            LOG(LOG_PRINT, "[WARN]This is Hi1711 EVB Board, please check!!!");
        } else {
            board_cfg_info->bits.platform_id = bdinfo_convert_platform_id(sub_platform_id);
            board_cfg_info->bits.board_id = bdinfo_buckle_board_id_get(strap_info, initial_info2, initial_info3);
            board_cfg_info->bits.pcb_ver = bdinfo_buckle_pcb_ver_get(initial_info2);
            board_cfg_info->bits.board_type = BOARD_TYPE_BUCKLE;
        }
    } else if (board_cfg_info->bits.platform_id == BOARD_PLATM_BUCKLE_NEW) {
        if (sub_platform_id == BOARD_SUB_PLATM_ID_OLD) {
            board_cfg_info->bits.board_id = bdinfo_buckle_board_id_get(strap_info, initial_info2, initial_info3);
        } else {
            board_cfg_info->bits.board_id = bdinfo_new_buckle_board_id_get(strap_info, initial_info2);
        }

        board_cfg_info->bits.pcb_ver = bdinfo_buckle_pcb_ver_get(initial_info2);
    } else if (board_cfg_info->bits.platform_id == BOARD_PLATM_SKYLAKE_BUCKLE) {
        board_cfg_info->bits.pcb_ver = bdinfo_buckle_pcb_ver_get(initial_info2);
    }

    return;
}

/**
 * @brief: get board boot_cfg pin information
 * @param: void
 * @retval: void
 */
static void bdinfo_boot_cfg_init(void)
{
    u32 strap_info = 0;
    u32 initial_info = 0;
    u32 initial_info2 = 0;
    u32 initial_info3 = 0;
    boot_cfg_info_s board_cfg_info;

    board_cfg_info.val = 0;

    /* get pin information */
    (void)sysctrl_reg_read(SYSCTRL_CSR_STRAP_STATUS_REG_OFFSET_ADDR, &strap_info);
    (void)sysctrl_reg_read(SYSCTRL_CSR_INITIAL_INF_REG_OFFSET_ADDR, &initial_info);
    (void)sysctrl_reg_read(SYSCTRL_CSR_INITIAL_INF_REG2_OFFSET_ADDR, &initial_info2);
    (void)sysctrl_reg_read(SYSCTRL_CSR_INITIAL_INF_REG3_OFFSET_ADDR, &initial_info3);
    mb();

    bdinfo_boot_cfg_base_get(strap_info, initial_info, initial_info2, initial_info3, &board_cfg_info);

    bdinfo_boot_cfg_fix(strap_info, initial_info, initial_info2, initial_info3, &board_cfg_info);

    (void)memcpy_s(&g_boot_cfg_info, sizeof(boot_cfg_info_s), &board_cfg_info, sizeof(boot_cfg_info_s));

    LOG(LOG_PRINT, "board_type:0x%x, platform_id:0x%x, board_id:0x%x, pcb_ver:0x%x.",
        g_boot_cfg_info.bits.board_type, g_boot_cfg_info.bits.platform_id,
        g_boot_cfg_info.bits.board_id, g_boot_cfg_info.bits.pcb_ver);

    LOG(LOG_PRINT, "ddr_size:0x%x, ddr_bits:0x%x, ddr_ecc:0x%x.",
        g_boot_cfg_info.bits.ddr_size, g_boot_cfg_info.bits.ddr_bits, g_boot_cfg_info.bits.ddr_ecc);

    LOG(LOG_PRINT, "single_chip:0x%x, debug_gmac:0x%x, vga_type:0x%x, is_espi:0x%x.",
        g_boot_cfg_info.bits.single_chip, g_boot_cfg_info.bits.debug_gmac,
        g_boot_cfg_info.bits.system_type, g_boot_cfg_info.bits.is_espi);

    return;
}

u32 bdinfo_is_not_skip_mid(u32 mid)
{
    if ((mid == IO_MULTI_JLC1_T_SDIO) || (mid == IO_MULTI_JLC1_R_RMII)) {
        return 0;
    }

    return 1;
}

static void set_hi3093_board_pcie_rst(void)
{
    u32 io_config_t_offset = 0xb8;    /* gpio 117 */
    u32 io_config_t_mask = 0x3;
    u32 io_config_t_val = 0x2;
    u32 gpio_base_addr = 0x874d000;
    u32 gpio_reg_size = 0x60;
    u32 reg_gpio_output = 0x0;    /* gpio output register */
    u32 reg_gpio_dir = 0x4;    /* gpio direction register, 0-input 1-output */
    u32 gpio_val = 0x200000;    /* gpio 117 */

    void __iomem *gpio_virt_base = ioremap(gpio_base_addr, gpio_reg_size);
    if (!gpio_virt_base) {
        LOG(LOG_ERROR, "goio reg remap failed");
        return;
    }

    io_config_t_reg_read_write(io_config_t_offset, io_config_t_mask, io_config_t_val);
    iowrite32(gpio_val, gpio_virt_base + reg_gpio_dir);
    iowrite32(gpio_val, gpio_virt_base + reg_gpio_output);
    iounmap(gpio_virt_base);
}

/**
 * @brief: unifed configuration of board pin multiplexing.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 bdinfo_cfg_init(void)
{
    u32 i;
    s32 ret = 0;
    u32 m_id;
    u32 cfged_mask[USE_MODULE_MASK_NUM] = { 0 };
    bd_cfg_info *cur_board = NULL;

    /* get boot_cfg pin information. */
    bdinfo_boot_cfg_init();

    /* configure pin io multiplexing. */
    cur_board = bdinfo_get_cfg_info();
    if (cur_board == NULL) {
        return -EINVAL;
    }

    bdinfo_fix_ioconfig(cur_board);

    for (i = 0; i < (sizeof(g_chip_io_cfg_table) / sizeof(g_chip_io_cfg_table[0])); i++) {
        m_id = g_chip_io_cfg_table[i].module_id;
        if (!bdinfo_module_id_is_used(m_id, cur_board)) {
            continue;
        }

        if (bdinfo_is_not_skip_mid(m_id)) {
            ret = bdinfo_set_iocfg_reg(&g_chip_io_cfg_table[i]);
            if (ret) {
                LOG(LOG_ERROR, "bdinfo_set_iocfg_reg cfg module 0x%x failed!", m_id);
                return ret;
            }
        }
        cfged_mask[m_id / VALID_MASK_BIT_NUM] |= BIT_MASK(m_id % VALID_MASK_BIT_NUM);
    }

    if (cur_board->plat_id == BOARD_PLATM_SKYLAKE_BUCKLE && cur_board->board_id == EVB_V100_PCB_VERSION) {
        set_hi3093_board_pcie_rst();
    }

    /* check cfg. */
    for (i = 0; i < USE_MODULE_MASK_NUM; i++) {
        if (cfged_mask[i] != cur_board->use_module_mask[i]) {
            LOG(LOG_ERROR, "cfg index 0x%x, need cfg 0x%x, actual cfg 0x%x, not fuuly configured!", i,
                cur_board->use_module_mask[i], cfged_mask[i]);
        }
    }

    return ret;
}

/**
 * @brief: module reset removal function.
 * @param: void
 * @retval: 0
 */
s32 bdinfo_cfg_exit(void)
{
    return 0;
}
