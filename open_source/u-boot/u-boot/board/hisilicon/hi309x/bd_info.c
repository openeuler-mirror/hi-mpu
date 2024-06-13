/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x board configuration APIs
 * Create: 2018-05-10
 */

#include <common.h>
#include <asm/io.h>
#include <hi309x_common.h>
#include "bd_info.h"

#define ECC_ENABLE 0x1
#define DDR_START_ADDR 0x80000000
#define BIT_VALUE(val, offset) (((val) >> (offset)) & 0x1)

/* boot cfg管脚信息 */
static boot_cfg_info_s g_boot_cfg_info;

/*****************************************************************************
 函 数 名  : bdinfo_get_board_type
 功能描述  : 获取单板类型: 插卡/贴片卡
 输入参数  : void
 输出参数  : board type
 返 回 值  : unsigned int
*****************************************************************************/
unsigned int bdinfo_get_board_type(void)
{
    return g_boot_cfg_info.bits.board_type;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_platform_id
 功能描述  : 获取单板platform_id
 输入参数  : void
 输出参数  : platform id
 返 回 值  : unsigned int
*****************************************************************************/
unsigned int bdinfo_get_platform_id(void)
{
    return g_boot_cfg_info.bits.platform_id;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_board_id
 功能描述  : 获取单板board_id
 输入参数  : void
 输出参数  : board id
 返 回 值  : unsigned int
*****************************************************************************/
unsigned int bdinfo_get_board_id(void)
{
    return g_boot_cfg_info.bits.board_id;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_pcb_ver
 功能描述  : 获取pcb版本号
 输入参数  : void
 输出参数  : pcb ver
 返 回 值  : unsigned int
*****************************************************************************/
unsigned int bdinfo_get_pcb_ver(void)
{
    return g_boot_cfg_info.bits.pcb_ver;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_pch_type
 功能描述  : 获取单板使用的是eSPI还是LPC
 输入参数  : void
 输出参数  : pch type
 返 回 值  : HOST PCH 类型
*****************************************************************************/
unsigned int bdinfo_get_pch_type(void)
{
    return g_boot_cfg_info.bits.is_espi;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_boot_cfg_info
 功能描述  : 获取单板boot cfg信息
 输入参数  : void
 输出参数  : cfg info
 返 回 值  : HOST PCH 类型
*****************************************************************************/
unsigned int bdinfo_get_boot_cfg_info(void)
{
    return g_boot_cfg_info.u32;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_vce_state
 功能描述  : 获取VCE状态信息
 输入参数  : void
 输出参数  : void
 返 回 值  : 开启关闭状态
*****************************************************************************/
unsigned int bdinfo_get_vce_state(void)
{
    return g_boot_cfg_info.bits.system_type;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_debug_gmac_type
 功能描述  : 获取调试网口信息, 0代码RMII0,1代表SGMII0
 输入参数  : void
 输出参数  : gmac type
 返 回 值  : 调试网口信息
*****************************************************************************/
unsigned int bdinfo_get_debug_gmac_type(void)
{
    return g_boot_cfg_info.bits.debug_gmac;
}

/*****************************************************************************
 函 数 名  : bdinfo_get_ddr_size
 功能描述  : 获取单板ddr总空间大小
 输入参数  : void
 输出参数  : ddr size
 返 回 值  : 空间大小, 单位:Byte
*****************************************************************************/
unsigned long bdinfo_get_ddr_size(void)
{
    unsigned long ddr_size;

    switch (g_boot_cfg_info.bits.ddr_size) {
        case 0: /* 0 表示 4G */
            ddr_size = 0x100000000;
            break;

        case 1: /* 1 表示 512M */
            ddr_size = 0x20000000;
            break;

        case 2: /* 2 表示 1G */
            ddr_size = 0x40000000;
            break;

        case 3: /* 3 表示 2G */
            ddr_size = 0x80000000;
            break;

        default:
            ddr_size = 0x40000000;
            break;
    }

    return ddr_size;
}

unsigned int bdinfo_get_ddr_ecc(void)
{
    return g_boot_cfg_info.bits.ddr_ecc;
}

u32 bdinfo_convert_platform_id(u32 platform_id)
{
    switch (platform_id) {
        case 0x0:
            return 0x0; /* 0x0 ->> 0x0 */

        case 0x1:
            return 0x7; /* 0x1 ->> 0x7 */

        case 0x2:
            return 0xa; /* 0x2 ->> 0xa */

        case 0x3:
            return 0xf; /* 0x3 ->> 0xf */

        default:
            return 0x0;
    }
}

u32 board_info_raw_platform_id_get(void)
{
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);

    return (BIT_VALUE(initial_info2, 14) << 0x0) | /* initial_info2.bit14 as platform_id.bit0 */
           (BIT_VALUE(initial_info2, 15) << 0x1) | /* initial_info2.bit15 as platform_id.bit1 */
           (BIT_VALUE(initial_info2, 28) << 0x2) | /* initial_info2.bit28 as platform_id.bit2 */
           (BIT_VALUE(initial_info2, 27) << 0x3);  /* initial_info2.bit27 as platform_id.bit3 */
}

u32 board_info_raw_board_id_get(void)
{
    u32 strap_sts = readl(SYSCTL_STRAP_STATUS_BASE);
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);
    u32 initial_info3 = readl(SYSCTL_INITIAL_INF3_BASE);

    return (BIT_VALUE(strap_sts, 5) << 0x0) |      /* strap_sts.bit5 as board_id.bit0 */
           (BIT_VALUE(strap_sts, 6) << 0x1) |      /* strap_sts.bit6 as board_id.bit1 */
           (BIT_VALUE(strap_sts, 7) << 0x2) |      /* strap_sts.bit7 as board_id.bit2 */
           (BIT_VALUE(initial_info3, 12) << 0x3) | /* initial_info3.bit12 as board_id.bit3 */
           (BIT_VALUE(initial_info3, 11) << 0x4) | /* initial_info3.bit11 as board_id.bit4 */
           (BIT_VALUE(initial_info2, 0) << 0x5) |  /* initial_info3.bit0 as board_id.bit5 */
           (BIT_VALUE(initial_info2, 1) << 0x6) |  /* initial_info3.bit1 as board_id.bit6 */
           (BIT_VALUE(initial_info2, 2) << 0x7);   /* initial_info3.bit2 as board_id.bit7 */
}

static u32 board_info_raw_pcb_ver_get(void)
{
    u32 initial_info = readl(SYSCTL_INITIAL_INF_BASE);
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);

    return (BIT_VALUE(initial_info2, 24) << 0x0) | /* initial_info2.bit24 as pcb_ver.bit0 */
           (BIT_VALUE(initial_info2, 4) << 0x1) |  /* initial_info2.bit4 as pcb_ver.bit1 */
           (BIT_VALUE(initial_info, 8) << 0x2);    /* initial_info.bit8 as pcb_ver.bit2 */
}

static u32 board_info_buckle_board_id_get(void)
{
    u32 strap_sts = readl(SYSCTL_STRAP_STATUS_BASE);
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);
    u32 initial_info3 = readl(SYSCTL_INITIAL_INF3_BASE);

    return (BIT_VALUE(initial_info3, 12) << 0x0) | /* initial_info3.bit12 as board_id.bit0 */
           (BIT_VALUE(initial_info3, 11) << 0x1) | /* initial_info3.bit11 as board_id.bit1 */
           (BIT_VALUE(strap_sts, 5) << 0x2) |      /* strap_sts.bit5      as board_id.bit2 */
           (BIT_VALUE(strap_sts, 6) << 0x3) |      /* strap_sts.bit6      as board_id.bit3 */
           (BIT_VALUE(strap_sts, 7) << 0x4) |      /* strap_sts.bit7      as board_id.bit4 */
           (BIT_VALUE(initial_info2, 0) << 0x5) |  /* initial_info2.bit0  as board_id.bit5 */
           (BIT_VALUE(initial_info2, 1) << 0x6) |  /* initial_info2.bit1  as board_id.bit6 */
           (BIT_VALUE(initial_info2, 2) << 0x7);   /* initial_info2.bit2  as board_id.bit7 */
}

static u32 board_info_new_buckle_board_id_get(void)
{
    u32 strap_sts = readl(SYSCTL_STRAP_STATUS_BASE);
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);

    return (BIT_VALUE(initial_info2, 10)) |        /* initial_info2.bit10 as board_id.bit0 */
           (BIT_VALUE(initial_info2, 11) << 0x1) | /* initial_info2.bit11 as board_id.bit1 */
           (BIT_VALUE(strap_sts, 5) << 0x2) |      /* strap_sts.bit5      as board_id.bit2 */
           (BIT_VALUE(strap_sts, 6) << 0x3) |      /* strap_sts.bit6      as board_id.bit3 */
           (BIT_VALUE(strap_sts, 7) << 0x4) |      /* strap_sts.bit7      as board_id.bit4 */
           (BIT_VALUE(initial_info2, 0) << 0x5) |  /* initial_info2.bit0  as board_id.bit5 */
           (BIT_VALUE(initial_info2, 1) << 0x6) |  /* initial_info2.bit1  as board_id.bit6 */
           (BIT_VALUE(initial_info2, 2) << 0x7);   /* initial_info2.bit2  as board_id.bit7 */
}

u32 board_info_platform_id_get(void)
{
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);
    u32 platform_id = board_info_raw_platform_id_get();
    u32 board_id = board_info_raw_board_id_get();

    if (platform_id == BOARD_PLATM_BUCKLE) {
        if (board_id != EVB_BOARD_ID) {
            /* sdio_data[2]->boot_cfg42 */
            /* sdio_data[3]->boot_cfg43 */
            platform_id = (BIT_VALUE(initial_info2, 10)) |       /* initial_info2.bit10 as platform_id.bit0 */
                          (BIT_VALUE(initial_info2, 11) << 0x1); /* initial_info2.bit11 as platform_id.bit1 */
            platform_id = bdinfo_convert_platform_id(platform_id);
        }
    }

    return platform_id;
}

u32 board_info_board_id_get(void)
{
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);
    u32 platform_id = board_info_raw_platform_id_get();
    u32 board_id = board_info_raw_board_id_get();
    u32 sub_platform_id = (BIT_VALUE(initial_info2, 10)) |       /* initial_info2.bit10 as platform_id.bit0 */
                          (BIT_VALUE(initial_info2, 11) << 0x1); /* initial_info2.bit11 as platform_id.bit1 */

    if (platform_id == BOARD_PLATM_BUCKLE) {
        if (board_id != EVB_BOARD_ID) {
            board_id = board_info_buckle_board_id_get();
        }
    } else if (platform_id == BOARD_PLATM_BUCKLE_NEW) {
        if (sub_platform_id == BOARD_SUB_PLATM_ID_OLD) {
            board_id = board_info_buckle_board_id_get();
        } else {
            board_id = board_info_new_buckle_board_id_get();
        }
    }

    return board_id;
}

u32 board_info_pcb_ver_get(void)
{
    u32 initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);
    u32 platform_id = board_info_raw_platform_id_get();
    u32 board_id = board_info_raw_board_id_get();
    u32 pcb_ver = board_info_raw_pcb_ver_get();

    if ((platform_id == BOARD_PLATM_BUCKLE_NEW) ||
        ((platform_id == BOARD_PLATM_BUCKLE) && (board_id != EVB_BOARD_ID))) {
        /* lb_data[2] -> boot_cfg56 */
        /* sdio_data[0]->boot_cfg40 */
        /* sdio_data[1]->boot_cfg41 */
        pcb_ver = (BIT_VALUE(initial_info2, 24)) |       /* initial_info2.bit24 as board_id.bit0 */
                  (BIT_VALUE(initial_info2, 8) << 0x1) | /* initial_info2.bit8  as board_id.bit1 */
                  (BIT_VALUE(initial_info2, 9) << 0x2);  /* initial_info2.bit9  as board_id.bit2 */
        pcb_ver = (~pcb_ver) & 0x7;
    }

    return pcb_ver;
}

/*****************************************************************************
 函 数 名  : bdinfo_boot_cfg_init
 功能描述  : 获取boot_cfg管脚信息
 输入参数  : void
 输出参数  : 无
 返 回 值  : static void
*****************************************************************************/
static void bdinfo_boot_cfg_init(void)
{
    u32 initial_info, initial_info2;
    boot_cfg_info_s board_cfg_info;

    /* 获取管脚信息 */
    initial_info = readl(SYSCTL_INITIAL_INF_BASE);
    initial_info2 = readl(SYSCTL_INITIAL_INF2_BASE);

    board_cfg_info.u32 = 0;

    /* hostrtc_clk -> boot_cfg35 */
    board_cfg_info.bits.single_chip = BIT_VALUE(initial_info2, 3);
    board_cfg_info.bits.pcb_ver = board_info_pcb_ver_get();
    board_cfg_info.bits.board_id = board_info_board_id_get();
    board_cfg_info.bits.platform_id = board_info_platform_id_get();
    /* pf_11->boot_cfg26 */
    /* pf_12->boot_cfg27 */
    board_cfg_info.bits.ddr_size = (BIT_VALUE(initial_info, 26)) |
                                   (BIT_VALUE(initial_info, 27) << 0x1);
    /* pf_13->boot_cfg28 */
    board_cfg_info.bits.ddr_bits = BIT_VALUE(initial_info, 28);
    /* pf_14->boot_cfg29 */
    board_cfg_info.bits.debug_gmac = BIT_VALUE(initial_info, 29);
    /* pf_15->boot_cfg30 */
    board_cfg_info.bits.system_type = BIT_VALUE(initial_info, 30);
    /* pf_16->boot_cfg31 */
    board_cfg_info.bits.ddr_ecc = BIT_VALUE(initial_info, 31);
    /* lb_data[24]->boot_cfg20 */
    board_cfg_info.bits.is_espi = BIT_VALUE(initial_info, 20);
    board_cfg_info.bits.board_type = BOARD_TYPE_HOST;

    /* EVB特殊适配 */
    if (board_cfg_info.bits.platform_id == BOARD_PLATM_BUCKLE) {
        if (board_cfg_info.bits.board_id == EVB_BOARD_ID) {
            board_cfg_info.bits.single_chip = 0x0;
            if (board_cfg_info.bits.pcb_ver != EVB_V110_PCB_VERSION) {
                board_cfg_info.bits.ddr_size = 0x2;
                board_cfg_info.bits.ddr_ecc = 0x0;
                board_cfg_info.bits.ddr_bits = 0x1;
            }
            board_cfg_info.bits.debug_gmac = 0x1;
            board_cfg_info.bits.system_type = 0x0;
            board_cfg_info.bits.is_espi = 0x0;
        } else {
            board_cfg_info.bits.board_type = BOARD_TYPE_BUCKLE;
        }
    }

    (void)memcpy(&g_boot_cfg_info, &board_cfg_info, sizeof(boot_cfg_info_s));

    return;
}

/*****************************************************************************
 函 数 名  : bdinfo_init
 功能描述  : 单板硬件信息初始化
 输入参数  : void
 输出参数  : 无
 返 回 值  : s32
*****************************************************************************/
void bdinfo_init(void)
{
    /* 获取boot_cfg管脚信息 */
    bdinfo_boot_cfg_init();

    /* 配置管脚复用 */
    return;
}
