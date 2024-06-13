// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * mdio interface header files
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _MDIO_H_
#define _MDIO_H_


#define DEV_MDIO_KER_NAME               "mdio"
#define MDIO_MAX_DRV_NUM                0x1

#define MDIO_CLUSE22_MAX_PHY_ADDR       0x1F
#define MDIO_CLUSE22_MAX_PHY_REG_ADDR   0x1F
#define MDIO_CLUSE45_MAX_PHY_ADDR       0x1F
#define MDIO_CLUSE45_MAX_PHY_DEV_ADDR   0x1F
#define MDIO_CLUSE45_MAX_PHY_REG_ADDR   0xFFFF
#define MDIO_MAX_WRITE_VALUE            0xFFFF

#define MDIO_ST_45               0x0
#define MDIO_ST_22               0x1

#define MDIO_ST45_WRITE_ADDR     0x0
#define MDIO_ST45_WRITE_DATA     0x1
#define MDIO_ST45_CONTINUE_READ  0x2
#define MDIO_ST45_READ           0x3

#define MDIO_ST22_WRITE          0x1
#define MDIO_ST22_READ           0x2

#define MAX_WAIT_CNT    1000

s32 mdio_read_22(u32 mdio_id, u32 phy_addr, u32 reg_addr, u32 *val);
s32 mdio_write_22(u32 mdio_id, u32 phy_addr, u32 reg_addr, u32 value);
s32 mdio_read_45(u32 mdio_id, u32 prt_addr, u32 dev_addr, u32 reg_addr, u32 *value);
s32 mdio_write_45(u32 mdio_id, u32 prt_addr, u32 dev_addr, u32 reg_addr, u32 value);

#endif

