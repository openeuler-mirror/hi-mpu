/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */
 
#include <asm/arch-hi309x/lp_subsys.h>

void lp_subsys_reg_read_write(unsigned int offset, unsigned int mask, unsigned int val)
{
    unsigned int value;

    value = readl((void *)LP_SUBSYS_CSR_BASE_ADDR + offset);
    value &= ~mask;
    value |= (val & mask);
    writel(value, (void *)LP_SUBSYS_CSR_BASE_ADDR + offset);
}

void lp_subsys_reg_set_bits(unsigned int offset, unsigned int mask, unsigned int set_bit)
{
    unsigned int value;

    value = readl((void *)LP_SUBSYS_CSR_BASE_ADDR + offset);
    value &= ~mask;

    if (set_bit) {
        value |= mask;
    }

    writel(value, (void *)LP_SUBSYS_CSR_BASE_ADDR + offset);
}