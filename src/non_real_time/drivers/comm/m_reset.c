// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Chip module reset driver file
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "hp_subsys.h"
#include "hp_sub_csr_addr_define.h"
#include "sysctrl_csr_addr_define.h"
#include "sysctrl.h"
#include "m_reset.h"

static struct semaphore g_reset_access_sem[RESET_REG_NUM];

/* hp_reset mutex semaphore for reset. */
static struct semaphore g_hp_reset_access_sem;

/**
 * @brief: Module reset initialization function.
 * @param: void
 * @retval: 0
 */
s32 m_reset_init(void)
{
    u32 i;

    for (i = 0; i < RESET_REG_NUM; i++) {
        init_mutex(&g_reset_access_sem[i]);
    }

    init_mutex(&g_hp_reset_access_sem);

    return 0;
}

/**
 * @brief: Module reset removal function.
 * @param: void
 * @retval: 0
 */
s32 m_reset_exit(void)
{
    return 0;
}

/**
 * @brief: emmc reset function, performs reset on the specified module.
 * @param: reg_offser: Reset register offset address.
 *         mask: reset mask, 1 indicates the corresponding one or more modules to be reset.
 * @retval: 0 is success, other is failure.
 */
s32 m_emmc_reset(u32 reg_offset, u32 mask)
{
    s32 ret;
    u32 reg_value;

    if (reg_offset != 0xf8) {
        LOG(LOG_ERROR, "err reg_offset:%x", reg_offset);
        return -EINVAL;
    }

    LOG(LOG_INFO, "m_reset_handle: [0x%x]:[0x%x]\n", reg_offset, mask);
    ret = lp_subsys_reg_read(reg_offset, &reg_value);
    if (ret) {
        LOG(LOG_ERROR, "read lpsubsy reg err %d\n", ret);
        return ret;
    }

    /* reset the corresponding module. */
    reg_value = reg_value | mask;

    ret = lp_subsys_reg_write(reg_offset, reg_value);
    if (ret) {
        LOG(LOG_ERROR, "config lpsubsy srst err %d\n", ret);
        return ret;
    }

    udelay(20);

    /* write to the reset register corresponding to MASK. */
    reg_value &= ~mask;

    ret = lp_subsys_reg_write(reg_offset, reg_value);
    if (ret) {
        LOG(LOG_ERROR, "deassert lpsubsy srst err %d\n", ret);
        return ret;
    }

    return ret;
}
EXPORT_SYMBOL(m_emmc_reset);

/**
 * @brief: hp module reset function, performs reset on the specified module.
 * @param: reg_offser: register offser.
 *         reset mask, 1 indicates the corresponding one or more modules to be reset.
 * @retval: 0 is success, other is failure.
 */
s32 m_hp_reset(u32 reg_offset, u32 mask)
{
    s32 ret;
    u32 reg_addr;
    u32 reg_value;

    ret = down_interruptible(&g_hp_reset_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "down_interruptible g_hp_reset_access_sem err(%d).", ret);
        return ret;
    }

    reg_addr = reg_offset;
    reg_value = mask;

    /* reset request. */
    ret = hp_subsys_reg_write(reg_addr, reg_value);
    if (ret) {
        LOG(LOG_ERROR, "config hp_subsy rst enable reg_addr(0x%x) err(%d).\n", reg_addr, ret);
        goto release_sem;
    }

    /* delay 20us */
    udelay(20);

    reg_addr = HP_SUB_CSR_HP_SRST_DIS1_OFFSET_ADDR + (reg_offset - HP_SUB_CSR_HP_SRST_REQ1_OFFSET_ADDR);
    reg_value = mask;

    ret = hp_subsys_reg_write(reg_offset, reg_value);
    if (ret) {
        LOG(LOG_ERROR, "config hp_subsy rst disable reg_addr(0x%x) err(%d).\n", reg_addr, ret);
        goto release_sem;
    }

release_sem:
    up(&g_hp_reset_access_sem);

    return ret;
}
EXPORT_SYMBOL(m_hp_reset);

/**
 * @brief: Module reset function, performs reset on the specified module.
 *         This version does not include mutex, can be used in interrupt and soft interrupt environments.
 * @param: reg_offser: regsister offser.
 *         mask: reset mask, 1 indicates the corresponding one or more modules to be reset.
 * @retval: 0 is success, other is failure.
 */
s32 m_reset_handle_atomic(u32 reg_offset, u32 mask)
{
    s32 ret;

    LOG(LOG_INFO, "m_reset_handle_atomic: [0x%x]:0x%x\n", reg_offset, mask);

    ret = lp_subsys_srst(reg_offset, mask);
    if (ret) {
        LOG(LOG_ERROR, "lp_subsys_srst error(%d)", ret);
        return ret;
    }

    return 0;
}
EXPORT_SYMBOL(m_reset_handle_atomic);

/**
 * @brief: Module reset function, performs reset on the specified module.
 * @param: reg_offser: regsister offser.
 *         mask: reset mask, 1 indicates the corresponding one or more modules to be reset.
 * @retval: 0 is success, other is failure.
 */
s32 m_reset_handle(u32 reg_offset, u32 mask)
{
    s32 ret;
    u32 reg_index;

    /* validity check */
    if ((reg_offset < RESET_REG_MIN_OFFSET)
        || (reg_offset > RESET_REG_MAX_OFFSET)
        || ((reg_offset % (sizeof(u32))) != 0)) {
        LOG(LOG_ERROR, "err reg_offset:%x", reg_offset);
        return -EINVAL;
    }

    reg_index = (reg_offset - RESET_REG_MIN_OFFSET) / sizeof(u32);
    ret = down_interruptible(&g_reset_access_sem[reg_index]);
    if (ret) {
        LOG(LOG_ERROR, "down_interruptible err, reg_index:0x%x", reg_index);
        return ret;
    }

    LOG(LOG_INFO, "m_reset_handle: [0x%x]:0x%x\n", reg_offset, mask);

    ret = lp_subsys_srst(reg_offset, mask);
    if (ret) {
        LOG(LOG_ERROR, "lp_subsys_srst error(%d)", ret);
        goto out_release_sem;
    }

out_release_sem:
    up(&g_reset_access_sem[reg_index]);

    return ret;
}
EXPORT_SYMBOL(m_reset_handle);

/**
 * @brief: soft reset interface for each reset domain.
 * @param: rst_zone: see macro definition RST_ZONE_CHIP for specific reset domains.
 * @retval: 0 is success, other is failure.
 */
s32 subsys_sft_reset(u8 rst_zone)
{
    if (rst_zone > RST_ZONE_PCIE1) {
        LOG(LOG_ERROR, "subsys_sft_reset, rst_zone(0x%x) is inval", rst_zone);
        return -EINVAL;
    }

    /* request reset */
    (void)sysctrl_reg_write(SYSCTRL_CSR_SYS_RST_REQ_OFFSET_ADDR, (0x1U << rst_zone));

    udelay(20);

    /* release reset */
    (void)sysctrl_reg_write(SYSCTRL_CSR_SYS_RST_DREQ_OFFSET_ADDR, (0x1U << rst_zone));

    return 0;
}
EXPORT_SYMBOL(subsys_sft_reset);