/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#ifndef __LP_SUBSYS_H__
#define __LP_SUBSYS_H__

#include "io_config.h"

#define LP_SUBSYS_CSR_BASE_ADDR 0x871d000

#define LP_SUBSYS_CSR_LPSC_CLKEN_CTRL0_OFFSET_ADDR 0x10
#define LP_SUBSYS_CSR_LPSC_SMBUS_I2C_MUX_OFFSET_ADDR 0x100
#define LP_SUBSYS_CSR_LPSC_CLKSTA_CTRL0_OFFSET_ADDR 0x1BC

void lp_subsys_reg_read_write(unsigned int offset, unsigned int mask, unsigned int val);
void lp_subsys_reg_set_bits(unsigned int offset, unsigned int mask, unsigned int set_bit);

#endif