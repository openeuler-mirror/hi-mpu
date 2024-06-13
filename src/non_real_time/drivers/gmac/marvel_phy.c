// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include "comm_knl_api.h"
#include "com_phy.h"
#include "bdinfo.h"
#include "board_info.h"

struct semaphore g_sem_page;
u32 g_init_flag = 0;

void phy_lock_init_mar1512(void)
{
    if (g_init_flag == 0) {
        sema_init(&g_sem_page, 1);
        g_init_flag = 1;
    }

    return;
}

s32 phy_1512_write(u32 phy_addr, u32 reg_num, u32 page, u32 value)
{
    s32 ret;

    if (phy_addr > GMAC_PHY_MAX_ADDR) {
        LOG(LOG_ERROR, "the phy_addr(0x%x) to write is error!\n", phy_addr);
        return -EINVAL;
    }

    ret = down_interruptible(&g_sem_page);
    if (ret) {
        return ret;
    }

    if ((ret = com_phy_write(phy_addr, MII_PAGE_ADDR_REG, page & 0xFF)) != 0) {
        LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, MII_PAGE_ADDR_REG);
        goto write_exit;
    }

    mdelay(1);

    if ((ret = com_phy_write(phy_addr, reg_num & 0x1F, value)) != 0) {
        LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
        goto write_exit;
    }

write_exit:
    up(&g_sem_page);
    return ret;
}

s32 phy_1512_read(u32 phy_addr, u32 reg_num, u32 page, u32 *pvalue)
{
    s32 ret;

    if (phy_addr > GMAC_PHY_MAX_ADDR) {
        LOG(LOG_ERROR, "the phy_addr(0x%x) to read is error!\n", phy_addr);
        return -EINVAL;
    }

    ret = down_interruptible(&g_sem_page);
    if (ret) {
        return ret;
    }

    if ((ret = com_phy_read(phy_addr, MII_PAGE_ADDR_REG, pvalue)) != 0) {
        LOG(LOG_ERROR, "phy_read(addr 0x%x reg 0x%x) failed", phy_addr, MII_PAGE_ADDR_REG);
        goto read_exit;
    }

    if (*pvalue != (page & 0xFF)) {
        if ((ret = com_phy_write(phy_addr, MII_PAGE_ADDR_REG, page & 0xFF)) != 0) {
            LOG(LOG_ERROR, "phy_write(addr 0x%x reg 0x%x) failed", phy_addr, MII_PAGE_ADDR_REG);
            goto read_exit;
        }
        udelay(1000); /* just delay 1ms, 1000us check again */
    }

    if ((ret = com_phy_read(phy_addr, reg_num & 0x1F, pvalue)) != 0) {
        LOG(LOG_ERROR, "phy_read(addr 0x%x reg 0x%x) failed", phy_addr, reg_num);
        goto read_exit;
    }

read_exit:
    up(&g_sem_page);
    return ret;
}

s32 phy_reset_mar1512(u32 phy_addr)
{
    u32 value = 0;
    s32 ret;

    ret = phy_1512_read(phy_addr, phy_reg_num(20), phy_reg_page(18), &value); // num is 20, page 18
    if (ret) {
        LOG(LOG_ERROR, "MAR1512(%u) read reg error(%d)!", phy_addr, ret);
        return ret;
    }
    value |= 0x8000;
    ret = phy_1512_write(phy_addr, phy_reg_num(20), phy_reg_page(18), value); // num is 20, page 18
    if (ret) {
        LOG(LOG_ERROR, "MAR1512(%u) write reg error(%d)!", phy_addr, ret);
        return ret;
    }

    mdelay(2);

    return ret;
}

static s32 phy_1512_copper_set(u32 phy_addr)
{
    s32 ret;
    u32 ul_reg_value = 0;

    ret = phy_1512_read(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), &ul_reg_value);
    if (ret) {
        return ret;
    }

    ul_reg_value |= AUTO_NEGOTIATION_ENABLE;
    return phy_1512_write(phy_addr, MII_CTRL_REG, phy_reg_page(0x0), ul_reg_value);
}

static s32 phy_1512_fiber_set(u32 phy_addr)
{
    s32 ret;
    u32 ul_reg_value = 0;

    ret = phy_1512_read(phy_addr, MII_CTRL_REG, FIBER_1512_PAGE, &ul_reg_value);
    if (ret) {
        return ret;
    }

    ul_reg_value |= AUTO_NEGOTIATION_ENABLE;
    return phy_1512_write(phy_addr, MII_CTRL_REG, FIBER_1512_PAGE, ul_reg_value);
}

static s32 phy_1512_sgmii_mode_set(u32 phy_addr)
{
    s32 ret;
    u32 ul_reg_value = 0;

    ret = phy_1512_read(phy_addr, phy_reg_num(20), phy_reg_page(18), &ul_reg_value); /* reg 20, page 18 */
    if (ret) {
        return ret;
    }

    ul_reg_value &= 0xFFF8;
    ul_reg_value |= 1;
    return phy_1512_write(phy_addr, phy_reg_num(20), phy_reg_page(18), ul_reg_value); /* reg 20, page 18 */
}

static s32 phy_1512_ledctrl_set(u32 phy_addr)
{
    s32 ret;
    u32 ul_reg_value = 0;

    ret = phy_1512_read(phy_addr, phy_reg_num(16), phy_reg_page(3), &ul_reg_value); /* reg 16, page 3 */
    if (ret) {
        return ret;
    }

    ul_reg_value &= 0xff00;
    ul_reg_value |= 0x0040;
    return phy_1512_write(phy_addr, phy_reg_num(16), phy_reg_page(3), ul_reg_value); /* reg 16, page 3 */
}

s32 phy_1512_init(u32 phy_addr)
{
    s32 ret;

    ret = phy_reset_mar1512(phy_addr);
    if (ret) {
        LOG(LOG_ERROR, "phy_init_mar1512:Error reset Phy! phy = %u", phy_addr);
        return ret;
    }

    ret = phy_1512_copper_set(phy_addr);
    if (ret) {
        LOG(LOG_ERROR, "phy_init_mar1512:Error set copper! phy = %u", phy_addr);
        return ret;
    }

    ret = phy_1512_fiber_set(phy_addr);
    if (ret) {
        LOG(LOG_ERROR, "phy_init_mar1512:Error set fiber! phy = %u", phy_addr);
        return ret;
    }

    ret = phy_1512_sgmii_mode_set(phy_addr);
    if (ret) {
        LOG(LOG_ERROR, "phy_init_mar1512:Error set sgmii! phy = %u", phy_addr);
        return ret;
    }

    /* Perform a soft reset again after the SGMII mode is configured. */
    ret = phy_reset_mar1512(phy_addr);
    if (ret) {
        LOG(LOG_ERROR, "phy_init_mar1512:Error reset Phy! phy = %u", phy_addr);
        return ret;
    }

    ret = phy_1512_ledctrl_set(phy_addr);
    if (ret) {
        LOG(LOG_ERROR, "phy_init_mar1512:Error ledctrl Phy! phy = %u", phy_addr);
        return ret;
    }

    return 0;
}

s32 phy_1512_link(u32 phy_addr, u32 *status)
{
    u32 reg_value = 0;
    s32 ret;

    ret = phy_1512_read(phy_addr, REG_1512_STATUS, REG_PHY_PAGE0, &reg_value);
    if (ret) {
        return ret;
    }

    if ((reg_value & 0xffff) == 0xffff) {
        return -EIO;
    }

    *status = (reg_value & MASK_STATUS_LINK) == MASK_STATUS_LINK ? PHY_LINK_UP : PHY_LINK_DOWN;

    return 0;
}

s32 phy_1512_get_speed_duplex(u32 phy_addr, u32 *speed, u32 *duplex)
{
    s32 ret;
    u32 value = 0;

    ret = phy_1512_read(phy_addr, REG_1512_SPEED_DUPLEX, REG_PHY_PAGE0, &value);
    if (ret) {
        return -EIO;
    }

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    if ((value & MASK_1512_SPEED) == PHY_SPEED_1000M_MODE) {
        *speed = MAC_1000M_SGMII;
    } else if ((value & MASK_1512_SPEED) == PHY_SPEED_100M_MODE) {
        *speed = MAC_100M_SGMII;
    } else if ((value & MASK_1512_SPEED) == PHY_SPEED_10M_MODE) {
        *speed = MAC_10M_SGMII;
    } else {
        *speed = MAC_1000M_SGMII;
    }

    *duplex = (value & MASK_1512_DUPLEX) ? GMAC_DUPLEX_FULL : GMAC_DUPLEX_HALF;

    return 0;
}

s32 phy_1512_restart_autoneg(u32 phy_addr)
{
    s32 ret;
    u32 value = 0;

    ret = phy_1512_read(phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, &value);
    if (ret) {
        LOG(LOG_ERROR, "read control reg error %u", phy_addr);
        return -EIO;
    }

    value |= MASK_PHY_CTRL_RESTART_AUTONEG; /* Restart autoneg */
    ret = phy_1512_write(phy_addr, REG_PHY_CTRL, REG_PHY_PAGE0, value);
    if (ret) {
        LOG(LOG_ERROR, "write control reg error %u", phy_addr);
        return -EIO;
    }

    return 0;
}

s32 phy_1512_get_id(u32 phy_addr, u32 *identifier)
{
    s32 ret;
    u32 identifier_l32 = 0;
    u32 identifier_h32 = 0;
    u32 plat_id, board_id;

    plat_id = bdinfo_get_platform_id();
    board_id = bdinfo_get_board_id();
    if (plat_id == BOARD_PLATM_BUCKLE && board_id == EVB_BOARD_ID) {
        /* The address of the PHY connected to the SGMII1 of the 3093 board is always 0x7. */
        phy_8211_disable_broadcast(0x7);
    }

    ret = phy_1512_read(phy_addr, MII_PHY_ID0_REG, 0, &identifier_h32);
    if (ret) {
        LOG(LOG_ERROR, "mar1512(0x%x) read reg[0x%x:0x%x] error!", phy_addr, 0, MII_PHY_ID0_REG);
        return ret;
    }

    ret = phy_1512_read(phy_addr, MII_PHY_ID1_REG, 0, &identifier_l32);
    if (ret) {
        LOG(LOG_ERROR, "mar1512(0x%x) read reg[0x%x:0x%x] error!", phy_addr, 0, MII_PHY_ID1_REG);
        return ret;
    }

    *identifier = ((identifier_h32 << 16) | (identifier_l32 & IDENTIFIER_LOW_MASK)); // << 16

    return ret;
}

phy_ops_s g_phy_1512_ops = {
    .init = phy_1512_init,
    .read = phy_1512_read,
    .write = phy_1512_write,
    .link = phy_1512_link,
    .get_speed_duplex = phy_1512_get_speed_duplex,
    .restart_autoneg = phy_1512_restart_autoneg,
    .get_phy_id = phy_1512_get_id,
};
