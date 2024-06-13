// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * PCIe hip02 register define
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __PCIE_HIP02_REG_H__
#define __PCIE_HIP02_REG_H__
#define HIP02_PCIE_PORT_LINK_CONTROL 0x710

#define HIP02_PORT_LINK_MODE_MASK    (0x3f << 16)
#define HIP02_PORT_LINK_MODE_1_LANES (0x1 << 16)
#define HIP02_PORT_LINK_MODE_2_LANES (0x3 << 16)
#define HIP02_PORT_LINK_MODE_4_LANES (0x7 << 16)
#define HIP02_PORT_LINK_MODE_8_LANES (0xf << 16)

#define HIP02_PCIE_LINK_WIDTH_SPEED_CONTROL 0x80C
#define HIP02_PORT_LOGIC_SPEED_CHANGE       BIT(17)
#define HIP02_PORT_LOGIC_LINK_WIDTH_MASK    (0x1f << 8)
#define HIP02_PORT_LOGIC_LINK_WIDTH_1_LANES BIT(8)
#define HIP02_PORT_LOGIC_LINK_WIDTH_2_LANES (0x2 << 8)
#define HIP02_PORT_LOGIC_LINK_WIDTH_4_LANES (0x4 << 8)
#define HIP02_PORT_LOGIC_LINK_WIDTH_8_LANES (0x8 << 8)

#define HIP02_PCIE_MSI_ADDR_LO      0x820
#define HIP02_PCIE_MSI_ADDR_HI      0x824
#define HIP02_PCIE_MSI_INTR0_ENABLE 0x828
#define HIP02_PCIE_MSI_INTR0_MASK   0x82C
#define HIP02_PCIE_MSI_INTR0_STATUS 0x830

#define HIP02_PCIE_ATU_VIEWPORT        0x900
#define HIP02_PCIE_ATU_REGION_INBOUND  (0x1 << 31)
#define HIP02_PCIE_ATU_REGION_OUTBOUND (0x0 << 31)
#define HIP02_PCIE_ATU_REGION_INDEX2   (0x2 << 0)
#define HIP02_PCIE_ATU_REGION_INDEX1   (0x1 << 0)
#define HIP02_PCIE_ATU_REGION_INDEX0   (0x0 << 0)
#define HIP02_PCIE_ATU_CR1             0x904
#define HIP02_PCIE_ATU_TYPE_MEM        (0x0 << 0)
#define HIP02_PCIE_ATU_TYPE_IO         (0x2 << 0)
#define HIP02_PCIE_ATU_TYPE_CFG0       (0x4 << 0)
#define HIP02_PCIE_ATU_TYPE_CFG1       (0x5 << 0)
#define HIP02_PCIE_ATU_CR2             0x908
#define HIP02_PCIE_ATU_ENABLE          (0x1 << 31)
#define HIP02_PCIE_ATU_BAR_MODE_ENABLE (0x1 << 30)
#define HIP02_PCIE_ATU_LOWER_BASE      0x90C
#define HIP02_PCIE_ATU_UPPER_BASE      0x910
#define HIP02_PCIE_ATU_LIMIT           0x914
#define HIP02_PCIE_ATU_LOWER_TARGET    0x918
#define HIP02_PCIE_ATU_BUS(x)          (((x) & 0xff) << 24)
#define HIP02_PCIE_ATU_DEV(x)          (((x) & 0x1f) << 19)
#define HIP02_PCIE_ATU_FUNC(x)         (((x) & 0x7) << 16)
#define HIP02_PCIE_ATU_UPPER_TARGET    0x91C

#define PCIE_SLV_NONE_DBI_MODE      0x0
#define PCIE_SLV_DBI_MODE           0x1
#define PCIE_LTSSM_RCVRY_LOCK_STATE 0x0d
#define PCIE_LTSSM_RCVRY_IDLE_STATE 0x10
#define PCIE_LTSSM_RCVRY_EQ0_STATE  0x20
#define PCIE_LTSSM_RCVRY_EQ3_STATE  0x23
#define PCIE_LTSSM_LINKUP_STATE     0x11
#define PCIE_LTSSM_STATE_MASK       0x3F

#define PCIE_MSI_LOW_ADDRESS        0x1b4
#define PCIE_MSI_HIGH_ADDRESS       0x1c4

#define PCIE_CFG_BAR0BASE     0x10
#define PCIE_CFG_BAR1BASE     0x14
#define PCIE_DB2_ENABLE_SHIFT BIT(13)
#define PCIE_DBI_CS2_ENABLE   0x1
#define PCIE_DBI_CS2_DISABLE  0x0

#define PCIE_LTSSM_ENABLE_SHIFT                BIT(11)
#define PCIE_SUBCTRL_RESET_REQ_REG             0xA00
#define PCIE0_2_SUBCTRL_RESET_REQ_REG(port_id) \
    (PCIE_SUBCTRL_RESET_REQ_REG + ((port_id) << 3))
#define PCIE3_SUBCTRL_RESET_REQ_REG 0xA68

#define PCIE_SUBCTRL_DRESET_REQ_REG             0xA04
#define PCIE0_2_SUBCTRL_DRESET_REQ_REG(port_id) \
    (PCIE_SUBCTRL_DRESET_REQ_REG + ((port_id) << 3))
#define PCIE3_SUBCTRL_DRESET_REQ_REG 0xA6C

#define PCIE_SUBCTRL_SC_PCIE0_CLK_DIS_REG            0x304
#define PCIE_SUBCTRL_SC_PCIE0_2_CLK_DIS_REG(port_id) \
    (PCIE_SUBCTRL_SC_PCIE0_CLK_DIS_REG + ((port_id) << 3))
#define PCIE_SUBCTRL_SC_PCIE3_CLK_DIS_REG 0x324

#define PCIE_SUBCTRL_SC_PCIE0_CLK_ST_REG            0x5300
#define PCIE_SUBCTRL_SC_PCIE0_2_CLK_ST_REG(port_id) \
    (PCIE_SUBCTRL_SC_PCIE0_CLK_ST_REG + ((port_id) << 2))
#define PCIE_SUBCTRL_SC_PCIE3_CLK_ST_REG 0x5310

#define PCIE_SUBCTRL_SC_PCIE0_CLK_EN_REG            0x300
#define PCIE_SUBCTRL_SC_PCIE0_2_CLK_EN_REG(port_id) \
    (PCIE_SUBCTRL_SC_PCIE0_CLK_EN_REG + ((port_id) << 3))
#define PCIE_SUBCTRL_SC_PCIE3_CLK_EN_REG 0x320

#define PCIE_LINK_WIDTH_SPEED_CONTROL 0x80C
#define PORT_LOGIC_SPEED_CHANGE       (0x1 << 17)

#define DS_API(lane)        ((0x1FF6c + 8 * (15 - (lane))) * 2)
#define PCIE_DFE_ENABLE_VAL 0x3851

/* *****PCIE MISC***** */
#define PCIE_CTRL0       0x00
#define PCIE_CTRL1       0x04
#define PCIE_CTRL2       0x08
#define PCIE_CTRL3       0x0c
#define PCIE_CTRL4       0x10
#define PCIE_CTRL5       0x14
#define PCIE_CTRL6       0x18
#define PCIE_CTRL7       0x1c
#define PCIE_CTRL8       0x20
#define PCIE_CTRL9       0x24
#define PCIE_CTRL10      0x28
#define PCIE_CTRL11      0x2c
#define PCIE_ECC_INT     0x50
#define PCIE_ECC_INT_MSK 0x54

#define PCIE_LINKDOWN_STATUS 0x70
#define PCIE_STATUS0  0x80
#define PCIE_STATUS1  0x84
#define PCIE_STATUS2  0x8c
#define PCIE_STATUS3  0x90
#define PCIE_STATUS4  0x94
#define PCIE_STATUS5  0x98
#define PCIE_STATUS6  0x9c
#define PCIE_STATUS7  0xa0
#define PCIE_STATUS8  0xa4
#define PCIE_STATUS9  0xa8
#define PCIE_STATUS10 0xac

#define PCIE_SC_SLV_DBI_ENABLE       BIT(21)
#define PCIE_SC_SLV_DEVICE_TYPE_MASK 0xF0000000
#define PCIE_SC_SLV_DEVICE_TYPE_HOST (0x4 << 28)

#define HIP_PCIE_CORE_RESET (0x1 << 12)

#define PCIE_ERR_RRESP_ENABLE           BIT(31)
#define PCIE_ERR_BRESP_ENABLE           (0x1 << 30)
#define PCIE_LINKDOWN_AUTO_RSTN_DISABLE (0x1 << 29)

#define PCIE_BRIDGE_FLUSH_NOT_CLR (0x1 << 11)

#define SHIFT_LINK_REQ_RST_NOT_MASK 12
#define SHIFT_SLV_ERR_INT           14
#define SHIFT_LTSSM_ENABLE          11
#define SHIFT_BRIDGE_FLUSH_NREG     20
#define SHIFT_BRIDGE_FLUSH_NCLR     11
#define SHIFT_POWER_UP_RSTEN        12

#endif