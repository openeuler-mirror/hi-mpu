/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#ifndef __IO_CONFIG_H__
#define __IO_CONFIG_H__

#include <asm/io.h>

#define IO_CONFIG_R 0
#define IO_CONFIG_T 1
#define IO_CONFIG_REG_NUM 2
#define IO_CONFIG_R_BASE_ADDR 0x876e000
#define IO_CONFIG_T_BASE_ADDR 0x8745000

#define IO_CONFIG_R_IOMG000_OFFSET_ADDR 0x0
#define IO_CONFIG_R_IOMG001_OFFSET_ADDR 0x4
#define IO_CONFIG_R_IOMG002_OFFSET_ADDR 0x8
#define IO_CONFIG_R_IOMG003_OFFSET_ADDR 0xC
#define IO_CONFIG_R_IOMG004_OFFSET_ADDR 0x10
#define IO_CONFIG_R_IOMG005_OFFSET_ADDR 0x14
#define IO_CONFIG_R_IOMG006_OFFSET_ADDR 0x18
#define IO_CONFIG_R_IOMG007_OFFSET_ADDR 0x1C
#define IO_CONFIG_R_IOMG008_OFFSET_ADDR 0x20
#define IO_CONFIG_R_IOMG009_OFFSET_ADDR 0x24
#define IO_CONFIG_R_IOMG010_OFFSET_ADDR 0x28
#define IO_CONFIG_R_IOMG011_OFFSET_ADDR 0x2C
#define IO_CONFIG_R_IOMG012_OFFSET_ADDR 0x30
#define IO_CONFIG_R_IOMG013_OFFSET_ADDR 0x34
#define IO_CONFIG_R_IOMG014_OFFSET_ADDR 0x38
#define IO_CONFIG_R_IOMG015_OFFSET_ADDR 0x3C
#define IO_CONFIG_R_IOMG016_OFFSET_ADDR 0x40
#define IO_CONFIG_R_IOMG017_OFFSET_ADDR 0x44
#define IO_CONFIG_R_IOMG018_OFFSET_ADDR 0x48
#define IO_CONFIG_R_IOMG019_OFFSET_ADDR 0x4C
#define IO_CONFIG_R_IOMG020_OFFSET_ADDR 0x50
#define IO_CONFIG_R_IOMG021_OFFSET_ADDR 0x54
#define IO_CONFIG_R_IOMG022_OFFSET_ADDR 0x58
#define IO_CONFIG_R_IOMG023_OFFSET_ADDR 0x5C
#define IO_CONFIG_R_IOMG024_OFFSET_ADDR 0x60
#define IO_CONFIG_R_IOMG025_OFFSET_ADDR 0x64
#define IO_CONFIG_R_IOMG026_OFFSET_ADDR 0x68
#define IO_CONFIG_R_IOMG027_OFFSET_ADDR 0x6C
#define IO_CONFIG_R_IOMG028_OFFSET_ADDR 0x70
#define IO_CONFIG_R_IOMG029_OFFSET_ADDR 0x74
#define IO_CONFIG_R_IOMG030_OFFSET_ADDR 0x78
#define IO_CONFIG_R_IOMG031_OFFSET_ADDR 0x7C
#define IO_CONFIG_R_IOMG032_OFFSET_ADDR 0x80
#define IO_CONFIG_R_IOMG033_OFFSET_ADDR 0x84
#define IO_CONFIG_R_IOMG034_OFFSET_ADDR 0x88
#define IO_CONFIG_R_IOMG035_OFFSET_ADDR 0x8C
#define IO_CONFIG_R_IOMG036_OFFSET_ADDR 0x90
#define IO_CONFIG_R_IOMG037_OFFSET_ADDR 0x94
#define IO_CONFIG_R_IOMG038_OFFSET_ADDR 0x98
#define IO_CONFIG_R_IOMG039_OFFSET_ADDR 0x9C
#define IO_CONFIG_R_IOMG040_OFFSET_ADDR 0xA0

#define IO_CONFIG_T_IOMG000_OFFSET_ADDR 0x0
#define IO_CONFIG_T_IOMG001_OFFSET_ADDR 0x4
#define IO_CONFIG_T_IOMG002_OFFSET_ADDR 0x8
#define IO_CONFIG_T_IOMG003_OFFSET_ADDR 0xC
#define IO_CONFIG_T_IOMG004_OFFSET_ADDR 0x10
#define IO_CONFIG_T_IOMG005_OFFSET_ADDR 0x14
#define IO_CONFIG_T_IOMG006_OFFSET_ADDR 0x18
#define IO_CONFIG_T_IOMG007_OFFSET_ADDR 0x1C
#define IO_CONFIG_T_IOMG008_OFFSET_ADDR 0x20
#define IO_CONFIG_T_IOMG009_OFFSET_ADDR 0x24
#define IO_CONFIG_T_IOMG010_OFFSET_ADDR 0x28
#define IO_CONFIG_T_IOMG011_OFFSET_ADDR 0x2C
#define IO_CONFIG_T_IOMG012_OFFSET_ADDR 0x30
#define IO_CONFIG_T_IOMG013_OFFSET_ADDR 0x34
#define IO_CONFIG_T_IOMG014_OFFSET_ADDR 0x38
#define IO_CONFIG_T_IOMG015_OFFSET_ADDR 0x3C
#define IO_CONFIG_T_IOMG016_OFFSET_ADDR 0x40
#define IO_CONFIG_T_IOMG017_OFFSET_ADDR 0x44
#define IO_CONFIG_T_IOMG018_OFFSET_ADDR 0x48
#define IO_CONFIG_T_IOMG019_OFFSET_ADDR 0x4C
#define IO_CONFIG_T_IOMG020_OFFSET_ADDR 0x50
#define IO_CONFIG_T_IOMG021_OFFSET_ADDR 0x54
#define IO_CONFIG_T_IOMG022_OFFSET_ADDR 0x58
#define IO_CONFIG_T_IOMG023_OFFSET_ADDR 0x5C
#define IO_CONFIG_T_IOMG024_OFFSET_ADDR 0x60
#define IO_CONFIG_T_IOMG025_OFFSET_ADDR 0x64
#define IO_CONFIG_T_IOMG026_OFFSET_ADDR 0x68
#define IO_CONFIG_T_IOMG027_OFFSET_ADDR 0x6C
#define IO_CONFIG_T_IOMG028_OFFSET_ADDR 0x70
#define IO_CONFIG_T_IOMG029_OFFSET_ADDR 0x74
#define IO_CONFIG_T_IOMG030_OFFSET_ADDR 0x78
#define IO_CONFIG_T_IOMG031_OFFSET_ADDR 0x7C
#define IO_CONFIG_T_IOMG032_OFFSET_ADDR 0x80
#define IO_CONFIG_T_IOMG033_OFFSET_ADDR 0x84
#define IO_CONFIG_T_IOMG034_OFFSET_ADDR 0x88
#define IO_CONFIG_T_IOMG035_OFFSET_ADDR 0x8C
#define IO_CONFIG_T_IOMG036_OFFSET_ADDR 0x90
#define IO_CONFIG_T_IOMG037_OFFSET_ADDR 0x94
#define IO_CONFIG_T_IOMG038_OFFSET_ADDR 0x98
#define IO_CONFIG_T_IOMG039_OFFSET_ADDR 0x9C
#define IO_CONFIG_T_IOMG040_OFFSET_ADDR 0xA0
#define IO_CONFIG_T_IOMG041_OFFSET_ADDR 0xA4
#define IO_CONFIG_T_IOMG042_OFFSET_ADDR 0xA8
#define IO_CONFIG_T_IOMG043_OFFSET_ADDR 0xAC
#define IO_CONFIG_T_IOMG044_OFFSET_ADDR 0xB0
#define IO_CONFIG_T_IOMG045_OFFSET_ADDR 0xB4
#define IO_CONFIG_T_IOMG046_OFFSET_ADDR 0xB8
#define IO_CONFIG_T_IOMG047_OFFSET_ADDR 0xBC
#define IO_CONFIG_T_IOMG048_OFFSET_ADDR 0xC0
#define IO_CONFIG_T_IOMG049_OFFSET_ADDR 0xC4
#define IO_CONFIG_T_IOMG050_OFFSET_ADDR 0xC8
#define IO_CONFIG_T_IOMG051_OFFSET_ADDR 0xCC

int io_config_r_read_write(unsigned int offset, unsigned int mask, unsigned int val);
int io_config_t_read_write(unsigned int offset, unsigned int mask, unsigned int val);

#endif