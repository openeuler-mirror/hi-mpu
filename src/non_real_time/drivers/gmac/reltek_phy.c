// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include "comm_knl_api.h"
#include "com_phy.h"

s32 phy_rtl82xx_write(u32 phy_addr, u32 reg_num, u32 page, u32 value)
{
    s32 ret;

    if (phy_addr > GMAC_PHY_MAX_ADDR) {
        LOG(LOG_ERROR, "phy_addr(0x%x) is error!", phy_addr);
        return -EINVAL;
    }

    if ((ret = com_phy_write(phy_addr, SGMII_RTL_PAGE_REG, page & 0xFFFF)) != 0) {
        LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, SGMII_RTL_PAGE_REG);
        return -EINVAL;
    }

    mdelay(1);

    if ((ret = com_phy_write(phy_addr, reg_num & 0x1F, value)) != 0) {
        LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
        return -EINVAL;
    }

    return ret;
}

s32 phy_rtl82xx_read(u32 phy_addr, u32 reg_num, u32 page, u32 *reg_value)
{
    s32 ret = 0;

    if (phy_addr > GMAC_PHY_MAX_ADDR) {
        LOG(LOG_ERROR, "phy_addr(0x%x) is error!", phy_addr);
        return -EINVAL;
    }

    if ((ret = com_phy_read(phy_addr, SGMII_RTL_PAGE_REG, reg_value)) != 0) {
        LOG(LOG_ERROR, "phy_read(addr 0x%x reg 0x%x) failed", phy_addr, SGMII_RTL_PAGE_REG);
        return -EINVAL;
    }

    if (*reg_value != (page & 0xFFFF)) {
        if ((ret = com_phy_write(phy_addr, SGMII_RTL_PAGE_REG, page & 0xFFFF)) != 0) {
            LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, SGMII_RTL_PAGE_REG);
            return -EINVAL;
        }
    }

    if ((ret = com_phy_read(phy_addr, reg_num & 0x1F, reg_value)) != 0) {
        LOG(LOG_ERROR, "phy_read(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
        return -EINVAL;
    }

    return ret;
}

void phy_8211_disable_broadcast(u32 phy_addr)
{
    u32 value = 0;
    s32 ret;

    ret = phy_rtl82xx_read(phy_addr, 0x18, phy_reg_page(0xa43), &value);
    if (ret) {
        return;
    }
    value &= (~ENABLE_BROADCAST_FUN);
    ret = phy_rtl82xx_write(phy_addr, 0x18, phy_reg_page(0xa43), value);
    if (ret) {
        return;
    }

    return;
}

s32 phy_rtl8211_bypass_mode(u32 phy_addr)
{
    u32 reg_value = 0;
    s32 ret;

    /* write phy reg: page 0xa43, offset 0x1b */
    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(0x1b), phy_reg_page(0xa43), BYPASS_MODE_STEP_VALUE);
    if (ret) {
        return ret;
    }

    /* read phy reg: page 0xa43, offset 0x1c */
    ret = phy_rtl82xx_read(phy_addr, phy_reg_num(0x1c), phy_reg_page(0xa43), &reg_value);
    if (ret) {
        return ret;
    }

    reg_value |= BIT(1); /* need left shift 1 bit */

    /* write phy reg: page 0xa43, offset 0x1b */
    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(0x1b), phy_reg_page(0xa43), BYPASS_MODE_STEP_VALUE);
    if (ret) {
        return ret;
    }

    /* write phy reg: page 0xa43, offset 0x1c */
    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(0x1c), phy_reg_page(0xa43), reg_value);
    if (ret) {
        return ret;
    }

    return 0;
}

s32 phy_rtl8211_led_on(u32 phy_addr)
{
    s32 ret;
    u32 reg_value = 0;

    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(0x11), phy_reg_page(0xd04), 0x0);
    if (ret) {
        return ret;
    }

    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(0x10), phy_reg_page(0xd04), RTL8211_LEDON_VALUE);
    if (ret) {
        LOG(LOG_ERROR, "RTL8211 write reg error!\n");
        return ret;
    }

    ret = phy_rtl82xx_read(phy_addr, phy_reg_num(0x10), phy_reg_page(0xd04), &reg_value);
    if (ret) {
        LOG(LOG_ERROR, "RTL8211 read reg error!\n");
        return ret;
    }

    LOG(LOG_INFO, "RTL8211 read phy led value = 0x%x!\n", reg_value);

    return 0;
}

s32 phy_8211_init(u32 phy_addr)
{
    u32 value = 0;
    s32 ret;

    ret = phy_rtl8211_bypass_mode(phy_addr);
    if (ret) {
        return ret;
    }

    ret = phy_rtl82xx_write(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), PHY_RESET);
    if (ret) {
        return ret;
    }
    mdelay(2);

    phy_8211_disable_broadcast(phy_addr);

    ret = phy_rtl82xx_read(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), &value);
    if (ret) {
        return ret;
    }
    value |= AUTO_NEGOTIATION_ENABLE;
    ret = phy_rtl82xx_write(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), value);
    if (ret) {
        return ret;
    }

    ret = phy_rtl8211_led_on(phy_addr);
    if (ret) {
        return ret;
    }

    return 0;
}

s32 phy_8211_link(u32 phy_addr, u32 *status)
{
    s32 ret;
    u32 value = 0;

    ret = phy_rtl82xx_read(phy_addr, 0x1A, phy_reg_page(0xa43), &value);
    if (ret) {
        return -EIO;
    }

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    *status = (value & MASK_STATUS_LINK) == MASK_STATUS_LINK ? PHY_LINK_UP : PHY_LINK_DOWN;

    return 0;
}

s32 phy_8211_get_speed_duplex(u32 phy_addr, u32 *speed, u32 *duplex)
{
    s32 ret;
    u32 value = 0;

    ret = phy_rtl82xx_read(phy_addr, 0x1a, phy_reg_page(0xa43), &value);
    if (ret) {
        return -EIO;
    }

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    *duplex = (value & MASK_8211_DUPLEX) ? GMAC_DUPLEX_FULL : GMAC_DUPLEX_HALF;

    /* bit5~bit4 indicate the speed */
    value = (value >> 4) & 0x3;

    /* translate the speed to enum value */
    switch (value) {
        case 0: /* 0 means 10M, ref nManager */
            *speed = MAC_10M_SGMII;
            break;
        case 1: /* 1 means 100M, ref nManager */
            *speed = MAC_100M_SGMII;
            break;
        case 2: /* 2 means 1000M, ref nManager */
            *speed = MAC_1000M_SGMII;
            break;
        default:
            *speed = MAC_1000M_SGMII;
            break;
    }

    return 0;
}

s32 phy_rtl82xx_restart_autoneg(u32 phy_addr)
{
    s32 ret;
    u32 value = 0;

    ret = phy_rtl82xx_read(phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, &value);
    if (ret) {
        LOG(LOG_ERROR, "read control reg error %u", phy_addr);
        return -EIO;
    }

    value |= MASK_PHY_CTRL_RESTART_AUTONEG; /* Restart autoneg */
    ret = phy_rtl82xx_write(phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, value);
    if (ret) {
        LOG(LOG_ERROR, "write control reg error %u", phy_addr);
        return -EIO;
    }

    return 0;
}

s32 phy_rtl82xx_get_id(u32 phy_addr, u32 *identifier)
{
    s32 ret;
    u32 identifier_l32 = 0;
    u32 identifier_h32 = 0;

    ret = phy_rtl82xx_read(phy_addr, MII_PHY_ID0_REG, 0, &identifier_h32);
    if (ret) {
        LOG(LOG_ERROR, "rtl8211(0x%x) read reg[0x%x:0x%x] error!", phy_addr, 0, MII_PHY_ID0_REG);
        return ret;
    }

    ret = phy_rtl82xx_read(phy_addr, MII_PHY_ID1_REG, 0, &identifier_l32);
    if (ret) {
        LOG(LOG_ERROR, "rtl8211(0x%x) read reg[0x%x:0x%x] error!", phy_addr, 0, MII_PHY_ID1_REG);
        return ret;
    }

    *identifier = ((identifier_h32 << 16) | (identifier_l32 & IDENTIFIER_LOW_MASK)); // << 16

    return ret;
}

/* Realtek RTL8211 operation set */
phy_ops_s g_phy_8211_ops = {
    .init = phy_8211_init,
    .read = phy_rtl82xx_read,
    .write = phy_rtl82xx_write,
    .link = phy_8211_link,
    .get_speed_duplex = phy_8211_get_speed_duplex,
    .restart_autoneg = phy_rtl82xx_restart_autoneg,
    .get_phy_id = phy_rtl82xx_get_id,
};

s32 phy_8201_init(u32 phy_addr)
{
    u32 value = 0;
    s32 ret;

    ret = phy_rtl82xx_write(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), PHY_RESET);
    if (ret) {
        return ret;
    }
    mdelay(2);

    /* Selecting the RMII mode */
    ret = phy_rtl82xx_read(phy_addr, phy_reg_num(16), phy_reg_page(0x7), &value); /* reg 16, page 7 */
    if (ret) {
        return ret;
    }
    value |= SELECT_8201_RMII_MODE;
    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(16), phy_reg_page(0x7), value); /* reg 16, page 3 */
    if (ret) {
        return ret;
    }

    /*
     * Eables the broadcast address function.
     * When the PHY ID is 0x0, the system does not respond to the command sent from this address.
     */
    ret = phy_rtl82xx_read(phy_addr, phy_reg_num(20), phy_reg_page(0x7), &value); /* reg 20, page 7 */
    if (ret) {
        return ret;
    }
    value &= (~ENABLE_BROADCAST_FUN);
    ret = phy_rtl82xx_write(phy_addr, phy_reg_num(20), phy_reg_page(0x7), value); /* reg 20, page 7 */
    if (ret) {
        return ret;
    }

    /* Set the PHY to auto-negotiation. */
    ret = phy_rtl82xx_read(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), &value);
    if (ret) {
        return ret;
    }
    value |= AUTO_NEGOTIATION_ENABLE;
    ret = phy_rtl82xx_write(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), value);
    if (ret) {
        return ret;
    }

    return 0;
}

s32 phy_8201_link(u32 phy_addr, u32 *status)
{
    u32 value = 0;
    s32 ret;

    ret = phy_rtl82xx_read(phy_addr, REG_8201_STATUS, phy_reg_page(0x0), &value);
    if (ret) {
        return ret;
    }

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    *status = (value & MASK_STATUS_LINK) == MASK_STATUS_LINK ? PHY_LINK_UP : PHY_LINK_DOWN;

    return 0;
}

s32 phy_8201_get_speed_duplex(u32 phy_addr, u32 *speed, u32 *duplex)
{
    s32 ret;
    u32 value = 0;

    ret = phy_rtl82xx_read(phy_addr, 0, 0, &value);
    if (ret) {
        return -EIO;
    }

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    *speed = (value & BIT(13)) ? MAC_100M_MII : MAC_10M_MII; // bit 13

    *duplex = (value & BIT(8)) ? GMAC_DUPLEX_FULL : GMAC_DUPLEX_HALF; // bit 8

    return 0;
}

/* Realtek RTL8201 operation set */
phy_ops_s g_phy_8201_ops = {
    .init = phy_8201_init,
    .read = phy_rtl82xx_read,
    .write = phy_rtl82xx_write,
    .link = phy_8201_link,
    .get_speed_duplex = phy_8201_get_speed_duplex,
    .restart_autoneg = phy_rtl82xx_restart_autoneg,
    .get_phy_id = phy_rtl82xx_get_id,
};
