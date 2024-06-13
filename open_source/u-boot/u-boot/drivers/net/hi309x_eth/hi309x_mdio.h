/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
* Description: none
* Create: 2018-08-01
* Notes: none
*/

#ifndef __HI309x_MDIO_H_
#define __HI309x_MDIO_H_

#include <config.h>
#include <common.h>
#include "hi309x_types.h"

#define MDIO_MAX_DRV_NUM            1
#define MDIO_WAIT_READY_CNT         1000


/* BEGIN: Added by duzhenquan 00219026, 2012/8/17 */
typedef union tagMdioCmd {
    struct {
        u32    reg_addr                 : 5;  /* [4..0] */
        u32    phy_addr                 : 5;  /* [9..5] */
        u32    op_code                  : 2;  /* [11..10] */
        u32    mdio_st                  : 2;  /* [13..12] */
        u32    mdio_go                  : 1;  /* [14] */
        u32    mdio_clk_sel             : 1;  /* [15] */
        u32    mdio_auto_scan           : 1;  /* [16] */
        u32    mdio_clk_sel_exp         : 1;  /* [17] */
        u32    Reserved_0               : 14; /* [31..18] */
    } bits;
    /* Define an unsigned member */
    u32    u32;
} MDIO_CMD_U;

/* Define the union MDIO_RD_WR_DATA_U */
typedef union tagMdioRdWrData {
    struct {
        u32    mdio_wr_data     : 16;  /* [15..0] */
        u32    reserved         : 16;  /* [31..16] */
    } bits;
    /* Define an unsigned member */
    u32    u32;
} MDIO_RD_WR_DATA_U;
/* END:   Added by duzhenquan 00219026, 2012/8/17 */

/******************************************************************************/
/*                      HI1710MDIO PCU 寄存器定义                      */
/******************************************************************************/
#define MDIO_CMD_REG                 0x0
#define MDIO_ADDR_REG                0x4
#define MDIO_WDATA_REG               0x8
#define MDIO_RDATA_REG               0xC
#define MDIO_AUTO_CFG_2              0x5C
#define MDIO_AUTO_INTR               0x60
#define MDIO_AUTO_INTR_EN            0x64

#define MDIO_STAT_REG                0x10
#define MDIO_SOFT_RST_MDIO_REG       0x14
#define MDIO_PHY0_ADDR_REG           0x18
#define MDIO_PHY1_ADDR_REG           0x1C
#define MDIO_PHY2_ADDR_REG           0x20
#define MDIO_PHY3_ADDR_REG           0x24
#define MDIO_PHY4_ADDR_REG           0x28
#define MDIO_PHY_TYPE_REG            0x2C
#define MDIO_PHY0_CONTROL_REG        0x600
#define MDIO_PHY0_STATUS_REG         0x610
#define MDIO_PHY0_ID1_REG            0x620
#define MDIO_PHY0_ID2_REG            0x630
#define MDIO_AN0_ADVERTISEMENT_REG   0x640
#define MDIO_LINK0_BASE_PAGE_REG     0x650
#define MDIO_AN0_EXPANSION_REG       0x660
#define MDIO_AN0_NP_TX_REG           0x670
#define MDIO_PHY1_CONTROL_REG        0x700
#define MDIO_PHY1_STATUS_REG         0x710
#define MDIO_PHY1_ID1_REG            0x720
#define MDIO_PHY1_ID2_REG            0x730
#define MDIO_AN1_ADVERTISEMENT_REG   0x740
#define MDIO_LINK1_BASE_PAGE_REG     0x750
#define MDIO_AN1_EXPANSION_REG       0x760
#define MDIO_AN1_NP_TX_REG           0x770
#define MDIO_PHY2_CONTROL_REG        0x800
#define MDIO_PHY2_STATUS_REG         0x810
#define MDIO_PHY2_ID1_REG            0x820
#define MDIO_PHY2_ID2_REG            0x830
#define MDIO_AN2_ADVERTISEMENT_REG   0x840
#define MDIO_LINK2_BASE_PAGE_REG     0x850
#define MDIO_AN2_EXPANSION_REG       0x860
#define MDIO_AN2_NP_TX_REG           0x870
#define MDIO_PHY3_CONTROL_REG        0x900
#define MDIO_PHY3_STATUS_REG         0x910
#define MDIO_PHY3_ID1_REG            0x920
#define MDIO_PHY3_ID2_REG            0x930
#define MDIO_AN3_ADVERTISEMENT_REG   0x940
#define MDIO_LINK3_BASE_PAGE_REG     0x950
#define MDIO_AN3_EXPANSION_REG       0x960
#define MDIO_AN3_NP_TX_REG           0x970
#define MDIO_PHY4_CONTROL_REG        0xA00
#define MDIO_PHY4_STATUS_REG         0xA10
#define MDIO_PHY4_ID1_REG            0xA20
#define MDIO_PHY4_ID2_REG            0xA30
#define MDIO_AN4_ADVERTISEMENT_REG   0xA40
#define MDIO_LINK4_BASE_PAGE_REG     0xA50
#define MDIO_AN4_EXPANSION_REG       0xA60
#define MDIO_AN4_NP_TX_REG           0xA70

#define MDIO_BASE_ADDR          0x08747000
#define MDIO_REG_ADDR_LEN       0x10000
#define MDIO_PHY_GRP_LEN        0x100
#define MDIO_REG_LEN            0x10
#define MDIO_PHY_ADDR_NUM       5
#define MDIO_MAX_PHY_ADDR       0x1F
#define MDIO_MAX_PHY_REG_ADDR   0x30

#define MDIO_REG_ADDR(port, offset)      (MDIO_BASE_ADDR + (port) * MDIO_REG_ADDR_LEN + (offset))

#define MDIO_REG_WRITE(port, offset, data) (REG_WRITE(MDIO_REG_ADDR((port), (offset)), (BYTE_SWAP32(data))))
#define MDIO_REG_READ(port, offset, data)  do { \
                                            (REG_READ(MDIO_REG_ADDR((port), (offset)), (data))); \
                                            (data) = (BYTE_SWAP32(data)); \
                                            } while (0)

s32 hi309x_mdio_init(u32 mdio_id);
s32 hi309x_mdio_read(u32 ulMdioId, u32 ulPhyAddr, u32 ulRegAddr, u32 *pulValue);
s32 hi309x_mdio_write(u32 ulMdioId, u32 ulPhyAddr, u32 ulRegAddr, u32 ulValue);
void eth_mdio_mutil_write(u32 lsw_addr, u32 phy_num, u32 phy_reg, s32 data);
s32 eth_mdio_mutil_read(u32 lsw_addr, u32 phy_num, u32 phy_reg);

#endif
