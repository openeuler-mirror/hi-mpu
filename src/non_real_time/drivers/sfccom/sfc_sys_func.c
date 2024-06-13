// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys_c_union_define.h"
#include "io_config_t_addr_define.h"
#include "io_config.h"
#include "lp_subsys.h"
#include "sfc_diff.h"
#include "bdinfo.h"
#include "board_info.h"
#include "sfc_sys_func.h"

s32 sfc_sys_set_iocfg(u32 bus, u32 cs, u32 enable)
{
    return 0;
}

static bool sfc_sys_board_is_2480(u32 plat_id, u32 board_id)
{
    if ((plat_id == BOARD_PLATM_TAISHAN) && (board_id == TAISHAN_BOARD_ID_2480)) {
        return true;
    }

    return false;
}

static bool sfc_sys_board_is_atlas880(u32 plat_id, u32 board_id)
{
    if ((plat_id == BOARD_PLATM_ATLAS) && (board_id == ATLAS_800_BOARD_ID_9000)) {
        return true;
    }

    return false;
}

static bool sfc_sys_board_is_9010(u32 plat_id, u32 board_id)
{
    if ((plat_id == BOARD_PLATM_ATLAS) && (board_id == ATLAS_800_BOARD_ID_9010)) {
        return true;
    }

    return false;
}

static bool sfc_sys_board_is_fusion_da140v2(u32 plat_id, u32 board_id)
{
    if ((plat_id == BOARD_PLATM_TAISHAN) && (board_id == FUSION_DA140V2)) {
        return true;
    }

    return false;
}

static void sfc_sys_set_cs_drive(u32 bus, u32 cs, u32 drive_val)
{
    if ((bus >= SFC_CTRL_CNT) || (cs >= SFC_CS_CNT)) {
        sfc_log(LOG_WARN_F, SFC_SYS_FUNC_C, "sfc bus %u cs %u err!", bus, cs);
        return;
    }

    if (bus == SFC_BUSID0) {
        (void)io_config_t_reg_write(IO_CONFIG_T_PADMG007_OFFSET_ADDR + cs * 4, drive_val); /* cs off 4 */
    } else if (bus == SFC_BUSID1) {
        (void)io_config_t_reg_write(IO_CONFIG_T_PADMG014_OFFSET_ADDR + cs * 4, drive_val); /* cs off 4 */
    }

    return;
}

static void sfc_sys_board_iodrive_adapter(u32 bus, u32 cs)
{
    u32 plat_id;
    u32 board_id;

    if ((bus == SFC_BUSID0) && (cs == SFC_CSID0)) {
        plat_id = bdinfo_get_platform_id();
        board_id = bdinfo_get_board_id();
        if (sfc_sys_board_is_2480(plat_id, board_id) ||
            sfc_sys_board_is_atlas880(plat_id, board_id) ||
            sfc_sys_board_is_fusion_da140v2(plat_id, board_id)) {
            sfc_sys_set_cs_drive(bus, cs, SFC0_CS0_2480_ATLAS880_DRIVE);
        }
    }

    return;
}

static void sfc_sys_set_io_drive(u32 bus, u32 cs)
{
    sfc_sys_board_iodrive_adapter(bus, cs);

    return;
}

void sfc_sys_init_extend_addr_mode(void)
{
    s32 ret;

    ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR, 0xc30, 0x410);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "init  extend_addr_mode failed(%d)!", ret);
    }

    return;
}

u32 sfc_sys_get_dma_cfg_ddr(u32 bus_id, dma_addr_t phy_addr)
{
    s32 ret;
    u32 input_addr = (u32)phy_addr;
    u32 cfg_off;
    u32 remap_mode, lp_haddr_extend;
    u32 hi_addr;
    u32 sel_mask;

    if (bus_id >= SFC_CTRL_CNT) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "sfc %u bus err!", bus_id);
        return (u32)phy_addr;
    }

    ret = lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR, &lp_haddr_extend);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "bus %u rx get ddr relative addr failed(%d)!", bus_id, ret);
        return (u32)phy_addr;
    }
    remap_mode = ((lp_haddr_extend >> (bus_id * 6 + 4)) & 0x3); /* bit off 6,4, mask 0x3 */
    switch (remap_mode) {
        case 0x1:
            input_addr = (u64)(phy_addr - CFG_OFF_ADDR);
            cfg_off = ((CFG_OFF_ADDR >> 28) << (bus_id * 6)); /* cfg off 28, bus off 6 */
            /* bitmask 0xf off 6 */
            ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR, (0xfu << (bus_id * 6)),
                                           (u32)cfg_off);
            if (ret) {
                sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "bus%u get dma cfg failed(%d)!", bus_id, ret);
                return (u32)phy_addr;
            }
            break;
        case 0x2:
            hi_addr = (u32)(((u64)phy_addr >> 32) & 0x3); /* low 32bit mask 3 */
            input_addr = (u32)phy_addr;
            if (bus_id == 0) {
                sel_mask = (0x3 << 24); /* bit mask 3 off 24 */
                hi_addr <<= 24;         /* bit off 24 */
            } else {
                sel_mask = (0x3 << 6); /* bit mask 3 off 6 */
                hi_addr <<= 6;         /* bit off 6 */
            }
            ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_ADDR_SEL_OFFSET_ADDR, sel_mask, hi_addr);
            if (ret) {
                sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "bus%u LP_SUBSYS_CSR_LPSC_ADDR_SEL_OFFSET_ADDR failed(%d)!",
                        bus_id, ret);
                return (u32)phy_addr;
            }
            break;
        default:
            break;
    }

    return input_addr;
}

s32 sfc_sys_clock_operator(u32 bus_id, u32 opr)
{
    u32 clk_mask;
    s32 ret;
    u32 clk_en_ctl0 = 0;

    if (opr > 1) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "Input operator param is wrong!");
        return -EINVAL;
    }

    if (bus_id >= SFC_CTRL_CNT) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "clk sfc bus(%u) out of range[0,15]", bus_id);
        return EINVAL;
    }

    /* acoord bus off 5 set bit 1 */
    clk_mask = (1UL << (bus_id + 5));
    ret = lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_CLKSTA_CTRL0_OFFSET_ADDR, &clk_en_ctl0);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "sfc lp_subsys_reg_read clk0 sta fail,op %u", opr);
        return ret;
    }

    if (opr == 0) {
        if (clk_en_ctl0 & clk_mask) {
            /* Turn off the clock */
            ret = lp_subsys_reg_set_bits(LP_SUBSYS_CSR_LPSC_CLKEN_CTRL0_OFFSET_ADDR, clk_mask, opr);
            if (ret) {
                sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "sfc set clk enable err, bus %u op %u!", bus_id, opr);
                return ret;
            }
        }
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "CLOSE 1711 SFC Clock Success!");
    } else if (opr == 1) {
        if ((clk_en_ctl0 & clk_mask) == 0) {
            /* Need to open */
            ret = lp_subsys_reg_set_bits(LP_SUBSYS_CSR_LPSC_CLKEN_CTRL0_OFFSET_ADDR, clk_mask, opr);
            if (ret) {
                sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "sfc set clk enable err, bus %u,op %u!", bus_id, opr);
                return ret;
            }
        }
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "Open 1711 SFC Clock Success!");
    }

    return ret;
}

s32 sfc_sys_set_controller_clk(u32 bus_id, u32 clk_mhz)
{
    s32 ret;
    u32 clk_div_cnt; /* 1-31 */

    if (clk_mhz == 0 || clk_mhz > MAX_SFC_CLK_MHZ) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "clk_mhz(%u) is invalid", clk_mhz);
        return -EINVAL;
    }

    /* Divided by 2 */
    clk_div_cnt = (SFC_INPUT_CLK / 2) / clk_mhz;
    if (clk_div_cnt > SFC_CLK_DIV_MAX) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "clk %u is out of range!", clk_mhz);
        return -EINVAL;
    }

    sfc_log(LOG_PRINT_F, SFC_SYS_FUNC_C, "bus %u clock frequency of is all set!", bus_id);
    ret = sfc_sys_clock_operator(bus_id, 0);
    if (ret) {
        goto CLK_OUT;
    }
    ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_CRG_DIV_CTRL_OFFSET_ADDR,
                                   sfc_clk_div_mask(bus_id), (clk_div_cnt << ((bus_id)*5 + 10))); /* idoff 5, 10 */
    if (ret) {
        goto CLK_OUT;
    }
    ret = sfc_sys_clock_operator(bus_id, 1);
CLK_OUT:

    return ret;
}

static s32 sfc_sys_board_clock_adapter(u32 bus, u32 clk_mhz)
{
    u32 plat_id;
    u32 board_id;

    if (bus == SFC_BUSID1) {
        plat_id = bdinfo_get_platform_id();
        board_id = bdinfo_get_board_id();
        if (sfc_sys_board_is_9010(plat_id, board_id) ||
            sfc_sys_board_is_atlas880(plat_id, board_id)) {
            return sfc_sys_set_controller_clk(bus, SFC1_ATLAS800_CLOCK);
        }
    }

    return sfc_sys_set_controller_clk(bus, clk_mhz);
}

s32 sfc_sys_set_clk(u32 bus_id, u32 clk_mhz)
{
    return sfc_sys_board_clock_adapter(bus_id, clk_mhz);
}

s32 sfc_sys_reset(u32 bus)
{
    s32 ret;
    u_lpsc_srst_req0 sc_per8;

    if (bus > SFC_CTRL_CNT) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "bus err %u!", bus);
        return -EFAULT;
    }

    sc_per8.val = 0;
    if (bus == 0) {
        sc_per8.bits.sfc0_srst_req = 1;
    } else if (bus == 1) {
        sc_per8.bits.sfc1_srst_req = 1;
    }
    ret = m_reset_handle(LP_SUBSYS_CSR_LPSC_SRST_REQ0_OFFSET_ADDR, sc_per8.val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "sfc resset err bus %u ret %d", bus, ret);
        return -EFAULT;
    }

    sfc_log(LOG_ERROR_F, SFC_SYS_FUNC_C, "sfc %u reset!", bus);

    return 0;
}

sfc_chip_operations g_sfc_chip_ops = {
    .sfc_clock_switch_ctrl = sfc_sys_clock_operator,
    .sfc_set_clock = sfc_sys_set_clk,
    .sfc_reset_handle = sfc_sys_reset,
    .sfc_init_extend_addr_mode = sfc_sys_init_extend_addr_mode,
    .sfc_get_dma_cfg_addr = sfc_sys_get_dma_cfg_ddr,
    .sfc_set_iocfg = sfc_sys_set_iocfg,
    .sfc_set_iodrive = sfc_sys_set_io_drive,
    .sfc_check_permission = NULL,
};
