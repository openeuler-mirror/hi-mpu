// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __PHY_H__
#define __PHY_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define GMAC_PHY_MAX_ADDR 0x1F

#define PHY_LINK_DOWN 0
#define PHY_LINK_UP   1

#define REG_PHY_PAGE0 0
#define REG_PHY_CTRL  0

/* page any */
#define MII_PAGE_ADDR_REG   0x16
#define MASK_STATUS_LINK    0x04
#define MASK_STATUS_AUTONEG 0x20

#define REG_1512_SPEED_DUPLEX 17
#define MASK_1512_SPEED       0xc000
#define MASK_1512_DUPLEX      0x2000

#define MAC_10M_MII    0
#define MAC_100M_MII   1
#define MAC_1000M_GMII 2

#define MAC_10M_RGMII   3
#define MAC_100M_RGMII  4
#define MAC_1000M_RGMII 5

#define MAC_10M_SGMII   6
#define MAC_100M_SGMII  7
#define MAC_1000M_SGMII 8

/* Duplex, half or full. */
#define GMAC_DUPLEX_HALF 0x00
#define GMAC_DUPLEX_FULL 0x01

#define REG_1512_STATUS          1
#define MASK_STATUS_1512_LINK    0x04
#define MASK_STATUS_1512_AUTONEG 0x20
#define REG_3019_STATUS          1
#define MASK_STATUS_3019_LINK    0x04
#define MASK_STATUS_3019_AUTONEG 0x20

#define IDENTIFIER_MARVELL1512 0x01410dd1
#define IDENTIFIER_RELTEK8211  0x001cc916
#define IDENTIFIER_YT8521      0x0000011a
#define IDENTIFIER_YT8512      0x00000128
#define IDENTIFIER_RELTEK8201  0x001cc816

#define IDENTIFIER_LOW_MASK     0xFFFF

#define REG_8201_STATUS       1
#define SELECT_8201_RMII_MODE BIT(3)
#define SGMII_RTL_PAGE_REG    0x1f
#define ENABLE_BROADCAST_FUN  BIT(13)

#define MASK_8211_DUPLEX BIT(3)
#define PHY_RESET        BIT(15)

#define MASK_PHY_CTRL_RESTART_AUTONEG BIT(9)
#define AUTO_NEGOTIATION_ENABLE       BIT(12)

/* Standard MII Registers */
#define MII_CTRL_REG     0x00 /* MII Control Register */
#define MII_PHY_ID0_REG  0x02 /* MII PHY ID register */
#define MII_PHY_ID1_REG  0x03 /* MII PHY ID register */

#define phy_reg_page(x) (x)
#define phy_reg_num(n)  (n)

#define RTL8211_LEDON_VALUE    0xc00b
#define BYPASS_MODE_STEP_VALUE 0xdc88

#define MASK_PHY_CTRL_ANENABLE  0x1000
#define MASK_PHY_CTRL_ANRESTART 0x0200

#define FIBER_1512_STATUS            0x11
#define FIBER_1512_CONTROL           0x0
#define FIBER_1512_PAGE              1
#define BIT_SPECIFIC_FIBER_1512_LINK 10
#define FIBER_1512_REPAIR_MAX_TIMES  3
#define BIT_FIBER_1512_POWER_DOWN    11

#define PHY_SPEED_1000M_MODE        0x8000
#define PHY_SPEED_100M_MODE         0x4000
#define PHY_SPEED_10M_MODE          0x0

typedef struct tag_phy_ops_s {
    s32 (*init)(u32 phy_addr);
    s32 (*read)(u32 phy_addr, u32 reg_num, u32 page, u32 *reg_value);
    s32 (*write)(u32 phy_addr, u32 reg_num, u32 page, u32 value);
    s32 (*link)(u32 phy_addr, u32 *status);
    s32 (*get_speed_duplex)(u32 phy_addr, u32 *speed, u32 *duplex);
    s32 (*restart_autoneg)(u32 phy_addr);
    s32 (*get_phy_id)(u32 phy_addr, u32 *identifier);
} phy_ops_s;

extern phy_ops_s g_phy_1512_ops;
extern phy_ops_s g_phy_8211_ops;
extern phy_ops_s g_phy_8201_ops;
extern phy_ops_s g_phy_yt8521_ops;
extern phy_ops_s g_phy_yt8512_ops;

void phy_lock_init_mar1512(void);
extern s32 mdio_read_22(u32 mdio_id, u32 phy_addr, u32 reg_addr, u32 *val);
extern s32 mdio_write_22(u32 mdio_id, u32 phy_addr, u32 reg_addr, u32 value);
extern void phy_8211_disable_broadcast(u32 port);

static inline s32 com_phy_read(u32 phy_addr, u32 regnum, u32 *pvalue)
{
    if (!pvalue) {
        return -EINVAL;
    }

    return mdio_read_22(0, phy_addr, regnum, pvalue);
}

static inline s32 com_phy_write(u32 phy_addr, u32 regnum, u32 wr_value)
{
    return mdio_write_22(0, phy_addr, regnum, wr_value);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PHY_H__ */

