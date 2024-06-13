// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * flash info parse
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "sfc0_core.h"

#define HISFC351_CRG31_CLK_25M (3 << 12)
#define HISFC351_CRG31_CLK_50M (2 << 12)
#define HISFC351_CRG31_CLK_75M (1 << 12)

#ifdef CONFIG_HISFC351_SHOW_CYCLE_TIMING
#define HISFC351_CFG31_CLK_SRC_OFFSET 16
#endif

void hisfc351_set_system_clock(hisfc_core_host *host, struct spi_operation *op, s32 clk_en)
{
    return;
}

void hisfc351_get_best_clock(u32 *clock)
{
    s32 ix;
    u32 clk;
    u32 sysclk[] = {
        25,
        HISFC351_CRG31_CLK_25M,
        50,
        HISFC351_CRG31_CLK_50M,
        75,
        HISFC351_CRG31_CLK_75M,
        0,
        0,
    };

    clk = HISFC351_CRG31_CLK_25M;
    for (ix = 0; sysclk[ix]; ix += 2) { // offset 2
        if ((*clock) < sysclk[ix]) {
            break;
        }
        clk = sysclk[ix + 1];
    }
    (*clock) = clk;

    return;
}

#ifdef CONFIG_HISFC351_SHOW_CYCLE_TIMING
s8 *hisfc351_get_clock_str(u32 clk_reg)
{
    s32 ix;
#define MAX_CLK_STR_LEN 40
    static s8 buffer[MAX_CLK_STR_LEN] = { 0 };

    static u32 clk_str[] = {
        4,
        50, /* 0x100 : 50M */
        5,
        25, /* 0x101 : 25M */
        6,
        75, /* 0x110 : 75M */
        7,
        75, /* 0x111 : 75M */
        0,
        0,
    };

    clk_reg = ((clk_reg & 0x70000) >> HISFC351_CFG31_CLK_SRC_OFFSET);

    for (ix = 0; clk_str[ix] != 0; ix += 2) { // offset 2
        if (clk_reg == clk_str[ix]) {
            (void)snprintf_s(buffer, MAX_CLK_STR_LEN, MAX_CLK_STR_LEN - 1, "%dM", clk_str[ix + 1]);
            break;
        }
    }

    return buffer;
}
#endif
