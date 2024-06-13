// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/errno.h>
#include "comm_knl_api.h"
#include "com_phy.h"

#define EXT_ADDR_OFFSET_REG     0x1E
#define EXT_DATA_REG            0x1F

#define PHY_SPEC_STATUS         0x11
#define MASK_YT85XX_SPEED       0xc000

/* yt8521 common register */
#define SMI_SDS_PHY_ADDR        0xa000
#define BROAD_CFG_ADDR          0xa005
#define LED0_CFG_ADDR           0xa00c
#define LED1_CFG_ADDR           0xa00d
#define LED2_CFG_ADDR           0xa00e
#define SDS_LINK_TIMER_ADDR     0xa5

/* yt8521 sds mii registers */
#define SDS_RESET_ADDR          0x0
#define SDS_STATUS_ADDR         0x11
#define SDS_RESET_BIT           BIT(15)
#define SDS_RX_LPI_ACTIVE_BIT   BIT(9)

#define SDS_ACCESS_MODE         0x2
#define PHY_ACCESS_MODE         0x0

static s32 phy_yt85xx_get_id(u32 phy_addr, u32 *identifier);

static s32 phy_yt85xx_write(u32 phy_addr, u32 reg_num, u32 page, u32 value)
{
    s32 ret;

    if (phy_addr > GMAC_PHY_MAX_ADDR) {
        LOG(LOG_ERROR, "phy_addr(0x%x) is error!", phy_addr);
        return -EINVAL;
    }

    ret = com_phy_write(phy_addr, reg_num, value);
    if (ret) {
        LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
    }

    return ret;
}

static s32 phy_yt85xx_read(u32 phy_addr, u32 reg_num, u32 page, u32 *pvalue)
{
    s32 ret;

    if (phy_addr > GMAC_PHY_MAX_ADDR) {
        LOG(LOG_ERROR, "phy_addr(0x%x) is error!", phy_addr);
        return -EINVAL;
    }

    ret = com_phy_read(phy_addr, reg_num, pvalue);
    if (ret) {
        LOG(LOG_ERROR, "phy_read(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
    }

    return ret;
}

static s32 phy_yt85xx_read_ext_reg(u32 phy_addr, u32 reg_num, u32 *pvalue)
{
    s32 ret;

    ret = com_phy_write(phy_addr, EXT_ADDR_OFFSET_REG, reg_num);
    ret += com_phy_read(phy_addr, EXT_DATA_REG, pvalue);
    if (ret) {
        LOG(LOG_ERROR, "phy_read ext(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
    }

    return ret;
}

static s32 phy_yt85xx_write_ext_reg(u32 phy_addr, u32 reg_num, u32 value)
{
    s32 ret;

    ret = com_phy_write(phy_addr, EXT_ADDR_OFFSET_REG, reg_num);
    ret += com_phy_write(phy_addr, EXT_DATA_REG, value);
    if (ret) {
        LOG(LOG_ERROR, "phy_write ext(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
    }

    return ret;
}

static s32 phy_yt8512_fix_old_version(u32 phy_addr)
{
    s32 ret;
    u32 value;

    ret = phy_yt85xx_read_ext_reg(phy_addr, 0xf, &value);
    if (ret) {
        return ret;
    }

    if ((value == 0x10) || (value == 0x12) || (value == 0x14)) {
        ret = phy_yt85xx_write_ext_reg(phy_addr, SMI_SDS_PHY_ADDR, SDS_ACCESS_MODE);
        if (ret) {
            return ret;
        }

        ret = phy_yt85xx_write_ext_reg(phy_addr, 0x3, 0x400);
        if (ret) {
            return ret;
        }
    }

    return 0;
}

static s32 phy_yt8521_recover_ext_reg0_val(s32 phy_addr)
{
    u32 value;
    s32 ret;

    ret = phy_yt85xx_read_ext_reg(phy_addr, 0, &value);
    if (ret) {
        return ret;
    }

    /* 0x1c8d is the default value of ext reg0 and cannot be modified. If it is modified, restore it. */
    if (value == 0x1c8d) {
        return 0;
    }

    ret = phy_yt85xx_write_ext_reg(phy_addr, 0, 0x1c8d);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_read(phy_addr, 0, 0, &value);
    if (ret) {
        return ret;
    }
    value |= PHY_RESET;

    return phy_yt85xx_write(phy_addr, 0, 0, value);
}

static s32 phy_yt8521_init(u32 phy_addr)
{
    s32 ret;
    u32 value = 0;

    ret = phy_yt8512_fix_old_version(phy_addr);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_write_ext_reg(phy_addr, SMI_SDS_PHY_ADDR, SDS_ACCESS_MODE);
    /* It is recommended that the negotiated timer be set to the maximum value 0x1f. */
    ret += phy_yt85xx_write_ext_reg(phy_addr, SDS_LINK_TIMER_ADDR, 0x1f);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_write_ext_reg(phy_addr, SMI_SDS_PHY_ADDR, PHY_ACCESS_MODE);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_read_ext_reg(phy_addr, BROAD_CFG_ADDR, &value);
    value &= ~(BIT(6)); /* If bit 6 is set to 0, the broadcast function is disabled. */
    ret += phy_yt85xx_write_ext_reg(phy_addr, BROAD_CFG_ADDR, value);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_write_ext_reg(phy_addr, LED0_CFG_ADDR, 0x1870);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_write_ext_reg(phy_addr, LED2_CFG_ADDR, 0x2600);
    if (ret) {
        return ret;
    }

    return phy_yt8521_recover_ext_reg0_val(phy_addr);
}

static bool check_yt8521_phy_id(u32 phy_addr)
{
    u32 id;
    s32 ret;

    ret = phy_yt85xx_get_id(phy_addr, &id);
    if (ret) {
        return false;
    }

    if (id == IDENTIFIER_YT8521) {
        return true;
    }

    return false;
}

static void phy_yt8521_fix_chip_buf(u32 phy_addr)
{
    u32 value;
    s32 ret;

    ret = phy_yt85xx_write_ext_reg(phy_addr, SMI_SDS_PHY_ADDR, SDS_ACCESS_MODE);
    if (ret) {
        return;
    }

    ret = phy_yt85xx_read(phy_addr, SDS_STATUS_ADDR, 0, &value);
    if (ret) {
        goto exit;
    }

    /* Determine whether to enter the RX LPI mode. */
    if ((value & SDS_RX_LPI_ACTIVE_BIT) == 0) {
        goto exit;
    }

    LOG(LOG_PRINT, "YT8521 SDS enter rx LPI mode");
    /* SDS RESET */
    ret = phy_yt85xx_read(phy_addr, SDS_RESET_ADDR, 0, &value);
    if (ret) {
        goto exit;
    }
    value |= SDS_RESET_BIT;
    (void)phy_yt85xx_write(phy_addr, SDS_RESET_ADDR, 0, value);

    msleep(100);

exit:
    (void)phy_yt85xx_write_ext_reg(phy_addr, SMI_SDS_PHY_ADDR, PHY_ACCESS_MODE);
    return;
}

static s32 phy_yt85xx_link(u32 phy_addr, u32 *status)
{
    u32 reg_value = 0;
    s32 ret;

    if (!status) {
        return -EINVAL;
    }

    ret = phy_yt85xx_read(phy_addr, PHY_SPEC_STATUS, 0x0, &reg_value);
    if (ret) {
        return ret;
    }

    if ((reg_value & 0xffff) == 0xffff) {
        return -EIO;
    }

    *status = (reg_value & BIT(10)) ? PHY_LINK_UP : PHY_LINK_DOWN; // bit 10

    if (*status == PHY_LINK_UP && check_yt8521_phy_id(phy_addr)) {
        phy_yt8521_fix_chip_buf(phy_addr);
    }

    return 0;
}

static s32 phy_yt8521_get_speed_duplex(u32 phy_addr, u32 *speed, u32 *duplex)
{
    s32 ret;
    u32 value = 0;

    ret = phy_yt85xx_read(phy_addr, PHY_SPEC_STATUS, 0x0, &value);
    if (ret) {
        return ret;
    }

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    if ((value & MASK_YT85XX_SPEED) == PHY_SPEED_1000M_MODE) {
        *speed = MAC_1000M_SGMII;
    } else if ((value & MASK_YT85XX_SPEED) == PHY_SPEED_100M_MODE) {
        *speed = MAC_100M_SGMII;
    } else if ((value & MASK_YT85XX_SPEED) == PHY_SPEED_10M_MODE) {
        *speed = MAC_10M_SGMII;
    } else {
        *speed = MAC_1000M_SGMII;
    }

    *duplex = (value & BIT(13)) ? GMAC_DUPLEX_FULL : GMAC_DUPLEX_HALF; // bit 13

    return 0;
}

static s32 phy_yt8512_get_speed_duplex(u32 phy_addr, u32 *speed, u32 *duplex)
{
    u32 value = 0;
    s32 ret;

    ret = phy_yt85xx_read(phy_addr, PHY_SPEC_STATUS, 0x0, &value);
    if (ret) {
        return ret;
    }

    if ((value & MASK_YT85XX_SPEED) == PHY_SPEED_1000M_MODE) {
        *speed = MAC_1000M_GMII;
    } else if ((value & MASK_YT85XX_SPEED) == PHY_SPEED_100M_MODE) {
        *speed = MAC_100M_MII;
    } else if ((value & MASK_YT85XX_SPEED) == PHY_SPEED_10M_MODE) {
        *speed = MAC_10M_MII;
    } else {
        *speed = MAC_100M_MII;
    }

    *duplex = (value & BIT(13)) ? GMAC_DUPLEX_FULL : GMAC_DUPLEX_HALF; // bit 13

    return 0;
}

static s32 phy_yt85xx_restart_autoneg(u32 phy_addr)
{
    s32 ret;
    u32 value = 0;

    ret = phy_yt85xx_read(phy_addr, REG_PHY_CTRL, 0, &value);
    if (ret) {
        LOG(LOG_ERROR, "read control reg error %u", phy_addr);
        return -EIO;
    }

    value |= MASK_PHY_CTRL_RESTART_AUTONEG;
    ret = phy_yt85xx_write(phy_addr, REG_PHY_CTRL, 0, value);
    if (ret) {
        LOG(LOG_ERROR, "write control reg error %u", phy_addr);
        return -EIO;
    }

    mdelay(10); /* delay 10ms */

    return 0;
}

static s32 phy_yt85xx_get_id(u32 phy_addr, u32 *identifier)
{
    s32 ret;
    u32 identifier_l32 = 0;
    u32 identifier_h32 = 0;

    ret = phy_yt85xx_read(phy_addr, MII_PHY_ID0_REG, 0, &identifier_h32);
    if (ret) {
        LOG(LOG_ERROR, "yt8521(0x%x) read reg[0x%x] error!", phy_addr, MII_PHY_ID0_REG);
        return ret;
    }

    ret = phy_yt85xx_read(phy_addr, MII_PHY_ID1_REG, 0, &identifier_l32);
    if (ret) {
        LOG(LOG_ERROR, "yt8521(0x%x) read reg[0x%x] error!", phy_addr, MII_PHY_ID1_REG);
        return ret;
    }

    *identifier = ((identifier_h32 << 16) | (identifier_l32 & IDENTIFIER_LOW_MASK)); // << 16

    return ret;
}

static s32 phy_yt8512_init(u32 phy_addr)
{
    u32 val = 0;
    s32 ret;

    ret = phy_yt85xx_restart_autoneg(phy_addr);
    if (ret) {
        return ret;
    }

    ret = phy_yt85xx_read_ext_reg(phy_addr, 0, &val);
    if (ret) {
        return ret;
    }
    val &= ~BIT(6); /* BIT6: 0 Disable Broadcast addr  */

    return phy_yt85xx_write_ext_reg(phy_addr, 0, val);
}

phy_ops_s g_phy_yt8521_ops = {
    .init = phy_yt8521_init,
    .read = phy_yt85xx_read,
    .write = phy_yt85xx_write,
    .link = phy_yt85xx_link,
    .get_speed_duplex = phy_yt8521_get_speed_duplex,
    .restart_autoneg = phy_yt85xx_restart_autoneg,
    .get_phy_id = phy_yt85xx_get_id,
};

phy_ops_s g_phy_yt8512_ops = {
    .init = phy_yt8512_init,
    .read = phy_yt85xx_read,
    .write = phy_yt85xx_write,
    .link = phy_yt85xx_link,
    .get_speed_duplex = phy_yt8512_get_speed_duplex,
    .restart_autoneg = phy_yt85xx_restart_autoneg,
    .get_phy_id = phy_yt85xx_get_id,
};