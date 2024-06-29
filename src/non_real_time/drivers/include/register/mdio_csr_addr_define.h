// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __MDIO_ADDR_DEFINE_H__
#define __MDIO_ADDR_DEFINE_H__

#define MDIO_CSR_BASE_ADDR                0x8747000

#define MDIO_CSR_COMMAND_REG_OFFSET_ADDR  0x0
#define MDIO_CSR_ADDR_REG_OFFSET_ADDR     0x4
#define MDIO_CSR_WDATA_REG_OFFSET_ADDR    0x8
#define MDIO_CSR_RDATA_REG_OFFSET_ADDR    0xC
#define MDIO_CSR_STA_REG_OFFSET_ADDR      0x10
#define MDIO_CSR_AUTO_CFG_0_0_OFFSET_ADDR 0x20
#define MDIO_CSR_AUTO_CFG_0_1_OFFSET_ADDR 0x24
#define MDIO_CSR_AUTO_CFG_0_2_OFFSET_ADDR 0x28
#define MDIO_CSR_AUTO_CFG_0_3_OFFSET_ADDR 0x2C
#define MDIO_CSR_AUTO_CFG_0_4_OFFSET_ADDR 0x30
#define MDIO_CSR_AUTO_CFG_1_0_OFFSET_ADDR 0x40
#define MDIO_CSR_AUTO_CFG_1_1_OFFSET_ADDR 0x44
#define MDIO_CSR_AUTO_CFG_1_2_OFFSET_ADDR 0x48
#define MDIO_CSR_AUTO_CFG_1_3_OFFSET_ADDR 0x4C
#define MDIO_CSR_AUTO_CFG_1_4_OFFSET_ADDR 0x50
#define MDIO_CSR_AUTO_CFG_2_OFFSET_ADDR   0x5C
#define MDIO_CSR_AUTO_INTR_OFFSET_ADDR    0x60
#define MDIO_CSR_AUTO_INTR_EN_OFFSET_ADDR 0x64

#define MDIO_CSR_COMMAND_REG_ADDR  (MDIO_CSR_BASE_ADDR + MDIO_CSR_COMMAND_REG_OFFSET_ADDR)
#define MDIO_CSR_ADDR_REG_ADDR     (MDIO_CSR_BASE_ADDR + MDIO_CSR_ADDR_REG_OFFSET_ADDR)
#define MDIO_CSR_WDATA_REG_ADDR    (MDIO_CSR_BASE_ADDR + MDIO_CSR_WDATA_REG_OFFSET_ADDR)
#define MDIO_CSR_RDATA_REG_ADDR    (MDIO_CSR_BASE_ADDR + MDIO_CSR_RDATA_REG_OFFSET_ADDR)
#define MDIO_CSR_STA_REG_ADDR      (MDIO_CSR_BASE_ADDR + MDIO_CSR_STA_REG_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_0_0_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_0_0_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_0_1_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_0_1_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_0_2_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_0_2_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_0_3_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_0_3_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_0_4_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_0_4_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_1_0_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_1_0_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_1_1_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_1_1_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_1_2_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_1_2_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_1_3_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_1_3_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_1_4_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_1_4_OFFSET_ADDR)
#define MDIO_CSR_AUTO_CFG_2_ADDR   (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_CFG_2_OFFSET_ADDR)
#define MDIO_CSR_AUTO_INTR_ADDR    (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_INTR_OFFSET_ADDR)
#define MDIO_CSR_AUTO_INTR_EN_ADDR (MDIO_CSR_BASE_ADDR + MDIO_CSR_AUTO_INTR_EN_OFFSET_ADDR)

#endif // _MDIO_ADDR_DEFINE_H__