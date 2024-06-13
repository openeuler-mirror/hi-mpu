/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#include <linux/errno.h>
#include <asm/io.h>
#include <asm/arch-hi309x/io_config.h>

static unsigned long g_io_config_base[IO_CONFIG_REG_NUM] = {IO_CONFIG_R_BASE_ADDR, IO_CONFIG_T_BASE_ADDR};

static int io_config_read_write(unsigned int type, unsigned int offset, unsigned int mask, unsigned int val)
{
    if (type > IO_CONFIG_T) {
        return -EINVAL;
    }

    unsigned int temp_val;

    temp_val = readl(g_io_config_base[type] + offset);
    temp_val &= ~mask;
    temp_val |= (val & mask);
    writel(temp_val, (void *)(g_io_config_base[type] + offset));

    return 0;
}

int io_config_r_read_write(unsigned int offset, unsigned int mask, unsigned int val)
{
    return io_config_read_write(IO_CONFIG_R, offset, mask, val);
}

int io_config_t_read_write(unsigned int offset, unsigned int mask, unsigned int val)
{
    return io_config_read_write(IO_CONFIG_T, offset, mask, val);
}