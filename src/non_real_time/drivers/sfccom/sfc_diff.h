// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc diff
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _SFC_DIFF_H_
#define _SFC_DIFF_H_
/* Specification Differences */
#define SFC_CTRL_CNT (2)
#define SFC_CS_CNT   (2)
#define SFC_TOTAL_CS (SFC_CTRL_CNT * SFC_CS_CNT)

#define SFC_INPUT_CLK         (400) /* 400M */
#define SFC_CLK_DIV_MAX       (0x1fUL)
#define SFC_CLK_DIV_OFFSET1 5
#define SFC_CLK_DIV_OFFSET2 10
static inline u32 sfc_clk_div_mask(u32 bus)
{
    return (SFC_CLK_DIV_MAX << ((bus)*SFC_CLK_DIV_OFFSET1 + SFC_CLK_DIV_OFFSET2));
}

#define SFC0_CS0_MEM_ADDR 0x60000000
#define SFC0_CS1_MEM_ADDR 0x68000000
#define SFC1_CS0_MEM_ADDR 0x70000000
#define SFC1_CS1_MEM_ADDR 0x78000000
#define SFC_CS_SIZE       0x8000000
#define CFG_OFF_ADDR      0x80000000

typedef struct {
    unsigned int reg_base;
    unsigned int bus_buf_base;
    unsigned int busid;
    unsigned int sfccsid;
    unsigned int intnum;
} sfc_dev_info;

typedef struct {
    sfc_dev_info dev_info[SFC_CTRL_CNT];
} sfc_diff_mgr;

#endif
