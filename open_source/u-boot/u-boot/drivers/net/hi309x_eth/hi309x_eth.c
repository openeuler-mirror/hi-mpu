/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
* Description: none
* Create: 2018-08-01
* Notes: none
*/

#include "hi309x_eth.h"
#include <config.h>
#include <common.h>
#include <net.h>
#include <watchdog.h>
#include <dm.h>
#include <errno.h>
#include <memalign.h>
#ifndef CONFIG_DM_ETH
#include <netdev.h>
#endif
#include <exports.h>
#include <command.h>
#include <asm/io.h>
#include "hi309x_mdio.h"
#include "gmac_addr_reg_define.h"
#include "../../../board/hisilicon/hi309x/bd_info.h"

u8 packet_buf[RX_BUFFER_DEFAULT_SIZE];
s32 g_eth_rx_print = 0;

gmac_phy_info_s hi_gmac_phy_info_table[] = {
    {
        "RTL8201",
        IDENTIFIER_RELTEK8201,
        gmac_8201phy_init,
        phy_8201_get_speed,
    },

    {
        "MAR1512",
        IDENTIFIER_MARVELL1512,
        gmac_1512phy_init,
        phy_1512_get_speed,
    },

    {
        "RTL8211",
        IDENTIFIER_RELTEK8211,
        gmac_8211phy_init,
        phy_8211_get_speed,
    },
};

u32 gmac_get_rx_ddr_relative_addr(u32 gmac_id, dma_addr_t phy_addr)
{
    u32 reg_addr;
    dma_addr_t mac_haddr;
    dma_addr_t relative_addr;
    u_hp_mac_haddr_extend hp_mac_haddr_extend;

    reg_addr = HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR + (gmac_id * 0x4);
    REG_READ(reg_addr, hp_mac_haddr_extend.u32);

    switch (hp_mac_haddr_extend.bits.m0_addr_remap_mode) {
        case 0x1:
            /* 左移28获取物理地址 */
            mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m0_haddr_offset)) << 28);
            break;

        case 0x2:
            /* 左移32获取物理地址 */
            mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m0_haddr_hi)) << 32);
            break;

        default:
            mac_haddr = 0;
            break;
    }

    if (phy_addr < mac_haddr) {

        return (u32)phy_addr;
    }

    /*lint -save -e587*/
    relative_addr = phy_addr - mac_haddr;
    /* 判定右移32后是否是0 */
    if ((relative_addr >> 32) != 0) {

        return (u32)phy_addr;
    }
    /*lint -restore */

    return (u32)relative_addr;
}

u32 gmac_get_tx_ddr_relative_addr(u32 gmac_id, dma_addr_t phy_addr)
{
    u32 reg_addr;
    dma_addr_t mac_haddr;
    dma_addr_t relative_addr;
    u_hp_mac_haddr_extend hp_mac_haddr_extend;

    reg_addr = HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR + (gmac_id * 0x4);
    REG_READ(reg_addr, hp_mac_haddr_extend.u32);

    switch (hp_mac_haddr_extend.bits.m1_addr_remap_mode) {
        case 0x1:
            /* 左移28获取物理地址 */
            mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m1_haddr_offset)) << 28);
            break;

        case 0x2:
            /* 左移32获取物理地址 */
            mac_haddr = (((u64)(hp_mac_haddr_extend.bits.m1_haddr_hi)) << 32);
            break;

        default:
            mac_haddr = 0;
            break;
    }

    if (phy_addr < mac_haddr) {

        return (u32)phy_addr;
    }

    /*lint -save -e587*/
    relative_addr = phy_addr - mac_haddr;
    if ((relative_addr >> 32) != 0) { /* high 32 bits */

        return (u32)phy_addr;
    }
    /*lint -restore */

    return (u32)relative_addr;
}

void gmac_init_extend_addr(u32 gmac_id)
{
    u32 reg_addr;
    u_hp_mac_haddr_extend hp_mac_haddr_extend;

    reg_addr = HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR + (gmac_id * 0x4);

    /* tx&rx默认采用地址映射模式1, 地址偏移为0x80000000. */
    hp_mac_haddr_extend.u32 = 0;
    hp_mac_haddr_extend.bits.m0_haddr_offset = 0x8;
    hp_mac_haddr_extend.bits.m0_addr_remap_mode = 0x1;
    hp_mac_haddr_extend.bits.m1_haddr_offset = 0x8;
    hp_mac_haddr_extend.bits.m1_addr_remap_mode = 0x1;
    REG_WRITE(reg_addr, hp_mac_haddr_extend.u32);

    return;
}

s32 gmac_set_hw_mac_addr(u32 ulPort)
{
    char *ulAddr = NULL;
    u8 ucAddr[MAC_ADDR_LEN] = { 0x08, 0x08, 0x08, 0x08, 0x08, 0x08 };
    s32 index = 0;
    u32 ulMacAddrLow, ulMacAddrHi;

    /* 判断参数的有效性 */
    if (ulPort >= FE_MAX_DEVS) {
        return -1;
    }

    ulAddr = env_get("ethaddr");

    while ((ulAddr != NULL) && (*ulAddr != '\0') && (index < MAC_ADDR_LEN)) {
        if (*ulAddr == ':') {
            ulAddr++;
            continue;
        }
        ucAddr[index] = simple_strtoul(ulAddr, NULL, 16); /* 16进制获取参数 */
        index++;
        ulAddr += 2; /* 获取下个mac地址字段偏移2个字节 */
    }

    /* 高地址:add[0]左移8 bits + add[1] */
    ulMacAddrHi = (ucAddr[0] << 8) | (ucAddr[1]);
    /* 高地址:add[2]左移24 bits + add[3]左移16 +  add[4]左移8 + add[5] */
    ulMacAddrLow = (ucAddr[2] << 24) | (ucAddr[3] << 16) | (ucAddr[4] << 8) | (ucAddr[5]);

    /* 设置端口MAC地址 */
    MAC_REG_WRITE(ulPort, MAC_CORE_STATION_ADDR_HIGH_2_REG, ulMacAddrHi);
    MAC_REG_WRITE(ulPort, MAC_CORE_STATION_ADDR_LOW_2_REG, ulMacAddrLow);

    MAC_REG_WRITE(ulPort, MAC_CORE_STATION_ADDR_LOW_MSK_0_REG, 0xFFFFFFFF);
    MAC_REG_WRITE(ulPort, MAC_CORE_STATION_ADDR_HIGH_MSK_0_REG, 0xFFFFFFFF);

    MAC_REG_WRITE(ulPort, MAC_CORE_STATION_ADDR_LOW_MSK_1_REG, 0xFFFFFFFF);
    MAC_REG_WRITE(ulPort, MAC_CORE_STATION_ADDR_HIGH_MSK_1_REG, 0xFFFFFFFF);

    return 0;
}

s32 gmac_hw_set_tx_pad_add(u32 ulPort, u32 ulEnValue)
{
    u_transmit_control unTxCtrl;
    u32 ulCmp;

    /* 判断参数的有效性 */
    if (ulPort >= FE_MAX_DEVS) {
        return -1;
    }

    if (ulEnValue > 1) {
        return -1;
    }

    MAC_REG_READ(ulPort, MAC_CORE_TRANSMIT_CONTROL_REG, unTxCtrl.u32);
    unTxCtrl.bits.pad_enable = ulEnValue;
    MAC_REG_WRITE(ulPort, MAC_CORE_TRANSMIT_CONTROL_REG, unTxCtrl.u32);

    MAC_REG_READ(ulPort, MAC_CORE_TRANSMIT_CONTROL_REG, ulCmp);
    if (ulCmp != unTxCtrl.u32) {
        return -1;
    }

    return 0;
}

static s32 gmac_rx_init_ring(struct hi309x_gmac_priv *priv)
{
    u8 i;
    u8 *tmp_buf[CHIP_RX_FIFO_NUM] = { NULL };

    for (i = 0; i < CHIP_RX_FIFO_NUM; i++) {
        if (priv->rx_data_ring[i] == NULL) {
            tmp_buf[i] = malloc(RX_BUFFER_DEFAULT_SIZE);
            if (tmp_buf[i] == NULL) {
                printf("error: malloc rx ring buffer failed %d\n", i);
                goto EXIT;
            }

            (void)memset(tmp_buf[i], 0, RX_BUFFER_DEFAULT_SIZE);
            priv->rx_data_ring[i] =
                (u8 *)(((unsigned long)tmp_buf[i] + (CPU_CACHE_LINE_SIZE - 1)) & (~(CPU_CACHE_LINE_SIZE - 1)));
        }
    }

    return 0;

EXIT:
    for (i = 0; i < CHIP_RX_FIFO_NUM; i++) {
        if (tmp_buf[i]) {
            free(tmp_buf[i]);
            priv->rx_data_ring[i] = NULL;
        }
    }

    return ERROR;
}

static s32 gmac_tx_init_ring(struct hi309x_gmac_priv *priv)
{
    u8 i;
    u8 *tmp_buf[CHIP_TX_FIFO_NUM] = { NULL };

    for (i = 0; i < CHIP_TX_FIFO_NUM; i++) {
        if (priv->tx_data_ring[i] == NULL) {
            tmp_buf[i] = malloc(RX_BUFFER_DEFAULT_SIZE);
            if (tmp_buf[i] == NULL) {
                printf("error: malloc tx ring buffer failed %d\n", i);
                goto EXIT;
            }
            (void)memset(tmp_buf[i], 0, RX_BUFFER_DEFAULT_SIZE);
            priv->tx_data_ring[i] =
                (u8 *)(((unsigned long)tmp_buf[i] + (CPU_CACHE_LINE_SIZE - 1)) & (~(CPU_CACHE_LINE_SIZE - 1)));
        }
    }

    return 0;

EXIT:
    for (i = 0; i < CHIP_TX_FIFO_NUM; i++) {
        if (tmp_buf[i]) {
            free(tmp_buf[i]);
            priv->tx_data_ring[i] = NULL;
        }
    }

    return ERROR;
}

static void gmac_rx_fill_all_buff(u32 port, struct hi309x_gmac_priv *priv)
{
    u32 i;
    u32 relative_addr;

    MAC_REG_WRITE(port, PCU_RX_BUF_SIZE_REG, RX_BUFFER_DEFAULT_SIZE);

    /* 不需要进行延时操作 */
    for (i = 0; i < CHIP_RX_FIFO_NUM; i++) {
        relative_addr = gmac_get_rx_ddr_relative_addr(port, (dma_addr_t)priv->rx_data_ring[i]);
        MAC_REG_WRITE(port, PCU_RX_CFF_ADDR_REG, relative_addr);
    }

    return;
}

s32 gmac_hw_set_pkt_store_format(u32 ulPort, const FEMAC_PKT_STORE_CTRL_CFG_S *pstRxCtrl)
{
    u_rx_ctrl unRxCtrl;
    u32 ulCmp;

    if (!pstRxCtrl) {
        printf("input rx ctrl parameter NULL!\n");
        return -1;
    }

    MAC_REG_READ(ulPort, PCU_RX_CTRL_REG, unRxCtrl.u32);

    /* 设置RX控制信息 */
    unRxCtrl.bits.cf_rxbuf_1st_skip_size = pstRxCtrl->cf_buf_1st_skip1;
    unRxCtrl.bits.cf_rxbuf_no_1st_skip_size = pstRxCtrl->cf_buf_n1st_skip;

    /* 时间戳上报去使能 */
    unRxCtrl.bits.cf_time_inf_en = 0;
    unRxCtrl.bits.cf_rx_align_num = pstRxCtrl->cf_rx_align_num;

    /* 地址生成模式配为0,表示group高位由寄存器配置 */
    unRxCtrl.bits.cf_addr_mode = 0;
    unRxCtrl.bits.cf_cache_line_h = 0;
    unRxCtrl.bits.cf_cache_line = pstRxCtrl->cf_cache_line;
    unRxCtrl.bits.cf_rxbuf_1st_skip_size2 = pstRxCtrl->cf_rxbuf_1st_skip_size2;

    MAC_REG_WRITE(ulPort, PCU_RX_CTRL_REG, unRxCtrl.u32);

    MAC_REG_READ(ulPort, PCU_RX_CTRL_REG, ulCmp);
    if (ulCmp != unRxCtrl.u32) {
        return ERROR;
    }

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_pkt_input_parse_mode
 功能描述  : 报文的输入及解析模式设置和查询
 输入参数  : u32 ulPort
             const FEMAC_RX_PKT_MODE_CFG_S *pstRxPktMode
 输出参数  : 无
 返 回 值  : s32

**************************************************************************** */
s32 gmac_hw_set_pkt_input_parse_mode(u32 ulPort, const FEMAC_RX_PKT_MODE_CFG_S *pstRxPktMode)
{
    u_rx_pkt_mode unRxMode;
    u_mac_skip_len unMacSkipLen;
    u32 ulCmp;

    if (!pstRxPktMode) {
        printf(" packet mode parameter NULL!\n");
        return -1;
    }

    /* 设置报文输入模式 */
    MAC_REG_READ(ulPort, PCU_RX_PKT_MODE_REG, unRxMode.u32);
    unRxMode.bits.cf_instr_head_mode = pstRxPktMode->cf_instr_head_mode;
    unRxMode.bits.cf_skip_len = pstRxPktMode->cf_skip_len;
    unRxMode.bits.cf_parse_mode = pstRxPktMode->cf_parse_mode;
    unRxMode.bits.cf_gen_id = 0;
    MAC_REG_WRITE(ulPort, PCU_RX_PKT_MODE_REG, unRxMode.u32);

    MAC_REG_READ(ulPort, PCU_RX_PKT_MODE_REG, ulCmp);
    if (ulCmp != unRxMode.u32) {
        printf("ulCmp != unRxMode.u32\n");
        return -1;
    }

    /* 报文头不解析字段长度,不解析字段做CRC */
    MAC_REG_READ(ulPort, MAC_CORE_MAC_SKIP_LEN_REG, unMacSkipLen.u32);
    unMacSkipLen.bits.mac_skip_len = pstRxPktMode->mac_skip_len;
    unMacSkipLen.bits.mac_skip_crc = 1;
    MAC_REG_WRITE(ulPort, MAC_CORE_MAC_SKIP_LEN_REG, unMacSkipLen.u32);

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_max_frame_len
 功能描述  : 端口接收最大帧长设置和获取
 输入参数  : u32 ulPort 端口号
             u32 ulFramSize 帧长
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
s32 gmac_hw_set_max_frame_len(u32 ulPort, u32 ulFramSize)
{
    u_max_frm_size unMaxFrmSize;

    if (ulFramSize >= 0x3FFF) {
        return -1;
    }

    MAC_REG_READ(ulPort, MAC_CORE_MAX_FRM_SIZE_REG, unMaxFrmSize.u32);
    unMaxFrmSize.bits.max_frm_size = ulFramSize;
    MAC_REG_WRITE(ulPort, MAC_CORE_MAX_FRM_SIZE_REG, unMaxFrmSize.u32);

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_short_frame_len
 功能描述  : 端口接收最短帧长设置和获取
 输入参数  : u32 ulPort       端口号
             u32 ulFramSize   帧长
 输出参数  : 无
 返 回 值  : s32

**************************************************************************** */
s32 gmac_hw_set_short_frame_len(u32 ulPort, u32 ulFramSize)
{
    u_short_runts_thr unShortFrmThr;

    if (ulFramSize > 0x1f) {
        printf("the value of ulFramSize is invalid\n");
        return -1;
    }

    MAC_REG_READ(ulPort, MAC_CORE_SHORT_RUNTS_THR_REG, unShortFrmThr.u32);
    unShortFrmThr.bits.short_runts_thr = ulFramSize;
    MAC_REG_WRITE(ulPort, MAC_CORE_SHORT_RUNTS_THR_REG, unShortFrmThr.u32);

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_sgmii_an_enable
 功能描述  : 设置SGMII自协商使能，参见用户指南
 输入参数  : u32 mac_num
 输出参数  : 无
 返 回 值  : u32
**************************************************************************** */
u32 gmac_hw_set_sgmii_an_enable(u32 mac_num)
{
    u_transmit_control unTxCtrl;
    u32 tmp;

    MAC_REG_WRITE(mac_num, MAC_CORE_TX_LOCAL_PAGE, 0x1);

    MAC_REG_WRITE(mac_num, MAC_CORE_SIXTEEN_BIT_CNTR, 0x3F);

    MAC_REG_WRITE(mac_num, MAC_CORE_LD_LINK_COUNTER, 0x1);

    MAC_REG_READ(mac_num, MAC_CORE_TRANSMIT_CONTROL_REG, unTxCtrl.u32);
    unTxCtrl.bits.an_enable = 1;
    MAC_REG_WRITE(mac_num, MAC_CORE_TRANSMIT_CONTROL_REG, unTxCtrl.u32);

    mdelay(20); /* delay 20ms */

    MAC_REG_READ(mac_num, MAC_CORE_AN_NEG_STATE, tmp);

    /* 判断 Bit21 SDS协商完成状态 */
    if ((tmp & (1 << 21)) == 0) {
        return ERROR;
    }

    return OK;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_rx_enable
 功能描述  : 使能gmac的接收通道
 输入参数  : u32  0 使能
                     1 禁止
 输出参数  : 无
 返 回 值  : INT32   ERROR
                     OK
**************************************************************************** */
void gmac_hw_rx_enable(u32 ulPort, u32 ulEnValue)
{
    u_port_en unPortEn;
    u32 ulCmp;

    /* read the RX channel configuration register to get current status */
    MAC_REG_READ(ulPort, MAC_CORE_PORT_EN_REG, unPortEn.u32);
    unPortEn.bits.rx_en = ulEnValue;
    MAC_REG_WRITE(ulPort, MAC_CORE_PORT_EN_REG, unPortEn.u32);

    MAC_REG_READ(ulPort, MAC_CORE_PORT_EN_REG, ulCmp);
    if (ulCmp != unPortEn.u32) {
        printf(" ulCmp != unPortEn.u32\n");
    }
}

/* ****************************************************************************
 函 数 名  : gmac_hw_tx_enable
 功能描述  : 使能gmac的发送通道
 输入参数  : u32     0 使能
                     1 禁止
 输出参数  : 无
 返 回 值  : s32   ERROR
                     OK
**************************************************************************** */
void gmac_hw_tx_enable(u32 ulPort, u32 ulEnValue)
{
    u_port_en unPortEn;
    u32 ulCmp;

    /* read the RX channel configuration register to get current status */
    MAC_REG_READ(ulPort, MAC_CORE_PORT_EN_REG, unPortEn.u32);
    unPortEn.bits.tx_en = ulEnValue;
    MAC_REG_WRITE(ulPort, MAC_CORE_PORT_EN_REG, unPortEn.u32);

    MAC_REG_READ(ulPort, MAC_CORE_PORT_EN_REG, ulCmp);
    if (ulCmp != unPortEn.u32) {
        printf("ulCmp != unPortEn.u32\n");
    }
}

s32 gmac_hw_set_port_mode(u32 ulPort, u32 ulMode, u32 ulDuplex)
{
    u_duplex_type unDuplexType;
    u_mode_change_en unModeChangeEn;
    u_port_mode unPortMode;
    u32 ulCmp;

    /* 关闭发送，接收通道 */
    gmac_hw_tx_enable(ulPort, DISABLE);
    gmac_hw_rx_enable(ulPort, DISABLE);

    MAC_REG_READ(ulPort, MAC_CORE_DUPLEX_TYPE, unDuplexType.u32);
    unDuplexType.bits.duplex_sel = ulDuplex;
    MAC_REG_WRITE(ulPort, MAC_CORE_DUPLEX_TYPE, unDuplexType.u32);

    MAC_REG_READ(ulPort, MAC_CORE_DUPLEX_TYPE, ulCmp);
    if (ulCmp != unDuplexType.u32) {
        return ERROR;
    }

    /* 设置端口模式,即设置Speed */
    MAC_REG_READ(ulPort, MAC_CORE_PORT_MODE_REG, unPortMode.u32);
    unPortMode.bits.port_mode = ulMode;
    MAC_REG_WRITE(ulPort, MAC_CORE_PORT_MODE_REG, unPortMode.u32);
    MAC_REG_READ(ulPort, MAC_CORE_PORT_MODE_REG, ulCmp);
    if (ulCmp != unPortMode.u32) {
        printf(" ulCmp != unPortMode.u32\n");
        return -1;
    }

    MAC_REG_READ(ulPort, MAC_CORE_MODE_CHANGE_EN_REG, unModeChangeEn.u32);
    unModeChangeEn.bits.mode_change_en = 1;
    MAC_REG_WRITE(ulPort, MAC_CORE_MODE_CHANGE_EN_REG, unModeChangeEn.u32);

    MAC_REG_READ(ulPort, MAC_CORE_MODE_CHANGE_EN_REG, ulCmp);
    if (ulCmp != unModeChangeEn.u32) {
        return ERROR;
    }

    return OK;
}

static void gmac_config_mode(u32 port)
{
    u32 times = 0;
    u32 val = 0;

    if ((port == ETH0) || (port == ETH1)) {
        gmac_hw_set_port_mode(port, MAC_100M_RMII, GMAC_DUPLEX_FULL);
    } else {
        gmac_hw_set_port_mode(port, MAC_1000M_SGMII, GMAC_DUPLEX_FULL);
        gmac_hw_set_sgmii_an_enable(port);
        do {
            MAC_REG_READ(port, MAC_CORE_AN_NEG_STATE, val);

            if (times >= 200) { /* 循环最大次数200次 */
                break;
            }

            mdelay(5); /* delay 5ms */
            times++;
        } while ((val & (1 << 21)) == 0); /* 查看Bit 21 是否协商完成 */
    }
}

static void gmac_hw_set_pcu_ctrl(u32 port)
{
    u_bus_ctrl unBusCtrl;

    /* 设置系统为大端，控制字为大端，数据为大端 */
    MAC_REG_READ(port, PCU_BUS_CTRL_REG, unBusCtrl.u32);
    unBusCtrl.bits.cf_pd_buf = 0;
    unBusCtrl.bits.huser = 1;       /* 指示是否需要硬件保证cache一致性 1：保证     */
    unBusCtrl.bits.noncoh_en = 0;   /* 对于需要释放的cache line，是否需要写
                                       清除在L1 cache中的内容，0：不需要写         */
    unBusCtrl.bits.cf_buf = 1;      /* 总线写操作是否为bufferable，1：可bufferable */
    unBusCtrl.bits.cf_endian = 0x3;   /* 配置未0x3:控制字为小端，数据为大端   */
    unBusCtrl.bits.cf_rx_alloc = 1; /* MAC的cache写操作是否为写分配，1：写分配     */
    MAC_REG_WRITE(port, PCU_BUS_CTRL_REG, unBusCtrl.u32);
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_port_mode_all
 功能描述  : 端口模式相关参数的设置及查询
 输入参数  : u32 ulPort
             const FEMAC_PORT_MODE_CFG_S *pstPortMode
 输出参数  : 无
 返 回 值  : s32

**************************************************************************** */
s32 gmac_hw_set_port_mode_all(u32 ulPort, const FEMAC_PORT_MODE_CFG_S *pstPortMode)
{
    u_rx_ctrl unRxCtrl;
    u_max_frm_size unMaxFrmSize;
    u_short_runts_thr unShortFrmThr;
    u_transmit_control unTxCtrl;
    u_recv_control unRecvCtrl;
    u_mode_change_en unModeChangeEn;
    u32 ulCmp = 0;

    gmac_config_mode(ulPort);

    MAC_REG_READ(ulPort, PCU_RX_CTRL_REG, unRxCtrl.u32);
    unRxCtrl.bits.cf_port_num = pstPortMode->port_num;
    MAC_REG_WRITE(ulPort, PCU_RX_CTRL_REG, unRxCtrl.u32);
    MAC_REG_READ(ulPort, PCU_RX_CTRL_REG, ulCmp);
    if (ulCmp != unRxCtrl.u32) {
        printf(" ulCmp != unRxCtrl.u32\n");
        return -1;
    }

    MAC_REG_READ(ulPort, MAC_CORE_MODE_CHANGE_EN_REG, unModeChangeEn.u32);
    unModeChangeEn.bits.mode_change_en = 1;
    MAC_REG_WRITE(ulPort, MAC_CORE_MODE_CHANGE_EN_REG, unModeChangeEn.u32);

    gmac_hw_set_pcu_ctrl(ulPort);

    /* 设置最大帧长 */
    unMaxFrmSize.bits.max_frm_size = pstPortMode->max_frm_size;

    /* 短帧／超短帧界限 */
    unShortFrmThr.bits.short_runts_thr = pstPortMode->short_runts_thr;

    /* 设置最大帧长 */
    if (gmac_hw_set_max_frame_len(ulPort, unMaxFrmSize.u32)) {
        printf(" gmac_hw_set_max_frame_len fail\n");
        return -1;
    }

    /* 短帧／超短帧界限 */
    if (gmac_hw_set_short_frame_len(ulPort, unShortFrmThr.u32)) {
        printf("gmac_hw_set_short_frame_len fail\n");
        return -1;
    }

    /* 发送添加pad/FCS/自协商使能 */
    MAC_REG_READ(ulPort, MAC_CORE_TRANSMIT_CONTROL_REG, unTxCtrl.u32);
    unTxCtrl.bits.pad_enable = pstPortMode->pad_enable;
    unTxCtrl.bits.crc_add = pstPortMode->crc_add;
    unTxCtrl.bits.an_enable = pstPortMode->an_enable;

    /* 发送添加pad/FCS/自协商使能 */
    MAC_REG_WRITE(ulPort, MAC_CORE_TRANSMIT_CONTROL_REG, unTxCtrl.u32);

    /* 超短帧透传／接收PAD剥离使能 */
    MAC_REG_READ(ulPort, MAC_CORE_RECV_CONTROL_REG, unRecvCtrl.u32);
    unRecvCtrl.bits.runt_pkt_en = pstPortMode->runt_pkt_en;
    unRecvCtrl.bits.strip_pad_en = pstPortMode->strip_pad_en;
    /* 超短帧透传／接收PAD剥离使能 */
    MAC_REG_WRITE(ulPort, MAC_CORE_RECV_CONTROL_REG, unRecvCtrl.u32);

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_mac_addr_filter
 功能描述  : 网口过滤规则设置
 输入参数  : u32 ulPort
             const FEMAC_MAC_FILTER_CFG_S *pstMacFilter
 输出参数  : 无
 返 回 值  : INT32

**************************************************************************** */
s32 gmac_hw_set_mac_addr_filter(u32 ulPort, const FEMAC_MAC_FILTER_CFG_S *pstMacFilter)
{
    u_rec_filt_control unFiltCtrl;
    u32 ulCmp;

    if (!pstMacFilter) {
        printf("input mac filter parameter NULL!\n");
        return -1;
    }

    /* 设置过滤规则，非本地单播帧,非本地组播帧,流控帧和CRC错误帧丢弃 */
    MAC_REG_READ(ulPort, MAC_CORE_REC_FILT_CONTROL_REG, unFiltCtrl.u32);
    unFiltCtrl.bits.crc_err_pass = pstMacFilter->crc_err_pass;
    unFiltCtrl.bits.pause_frm_pass = pstMacFilter->pause_frm_pass;
    unFiltCtrl.bits.bc_drop_en = pstMacFilter->bc_drop_en;
    unFiltCtrl.bits.mc_match_en = pstMacFilter->mc_match_en;
    unFiltCtrl.bits.uc_match_en = pstMacFilter->uc_match_en;
    MAC_REG_WRITE(ulPort, MAC_CORE_REC_FILT_CONTROL_REG, unFiltCtrl.u32);

    MAC_REG_READ(ulPort, MAC_CORE_REC_FILT_CONTROL_REG, ulCmp);
    if (ulCmp != unFiltCtrl.u32) {
        printf("ulCmp != unFiltCtrl.u32\n");
    }

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_rx_max_frame_len
 功能描述  : 设置接收方向允许最长的报文，如果报文超过此值，超过部分将被截断
 输入参数  : u32 ulPort
             u32 ulFrmLen
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
s32 gmac_hw_set_rx_max_frame_len(u32 ulPort, u32 ulFrmLen)
{
    u_rx_max_frame_len unRxMaxFrmLen;
    u32 ulCmp;

    if (ulFrmLen >= 0xFFFF) {
        printf("the value of ulFrmLen is invalid\n");
        return -1;
    }

    MAC_REG_READ(ulPort, PCU_MAX_FRAME_LEN_REG, unRxMaxFrmLen.u32);
    unRxMaxFrmLen.bits.max_frame_len = ulFrmLen;
    MAC_REG_WRITE(ulPort, PCU_MAX_FRAME_LEN_REG, unRxMaxFrmLen.u32);

    MAC_REG_READ(ulPort, PCU_MAX_FRAME_LEN_REG, ulCmp);
    if (ulCmp != unRxMaxFrmLen.u32) {
        printf(" ulCmp != unRxMaxFrmLen.u32\n");
        return -1;
    }

    return 0;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_bmu_close
 功能描述  : 关闭MAC芯片从BMU获取内存的功能
 输入参数  : u32 ulPort
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
s32 gmac_hw_set_bmu_close(u32 ulPort)
{
    u_rx_ctrl unRxCtrl;
    u32 ulCmp;

    MAC_REG_READ(ulPort, PCU_RX_CTRL_REG, unRxCtrl.u32);
    unRxCtrl.bits.cf_rx_get_addr_mode = 1; /* 调试网口必须配置成1，被动接收总线写入地址 */
    MAC_REG_WRITE(ulPort, PCU_RX_CTRL_REG, unRxCtrl.u32);

    MAC_REG_READ(ulPort, PCU_RX_CTRL_REG, ulCmp);
    if (ulCmp != unRxCtrl.u32) {
        printf(" ulCmp != unRxCtrl.u32\n");
        return ERROR;
    }

    return OK;
}

/* ****************************************************************************
 函 数 名  : gmac_hw_set_poe_close
 功能描述  : 关闭MAC芯片与POE相关的功能
 输入参数  : u32 ulPort
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
s32 gmac_hw_set_poe_close(u32 ulPort)
{
    u_we_addr unWorkEntry;
    u32 ulCmp;

    MAC_REG_READ(ulPort, PCU_WE_ADDR_REG, unWorkEntry.u32);
    unWorkEntry.bits.cf_we_enable = 0;
    MAC_REG_WRITE(ulPort, PCU_WE_ADDR_REG, unWorkEntry.u32);

    MAC_REG_READ(ulPort, PCU_WE_ADDR_REG, ulCmp);
    if (ulCmp != unWorkEntry.u32) {
        return ERROR;
    }

    return OK;
}

/* ****************************************************************************
 函 数 名  : gmac_set_rx_fifo_threshold
 功能描述  : 设置发送、接收FIFO水线
 输入参数  : u32 ulPort
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
s32 gmac_set_rx_fifo_threshold(u32 ulPort)
{
    u_cf_cfg_fifo_thrsld unFifoThrsld;
    u32 ulCmp;

    MAC_REG_READ(ulPort, PCU_CF_CFG_FIFO_THRSLD_REG, unFifoThrsld.u32);
    unFifoThrsld.bits.cfg_rx_p_full_th = FE_RX_FIFO_FULL_THRSHLD;
    unFifoThrsld.bits.cfg_rx_p_empty_th = FE_RX_FIFO_EMPTY_THRSHLD;
    MAC_REG_WRITE(ulPort, PCU_CF_CFG_FIFO_THRSLD_REG, unFifoThrsld.u32);

    MAC_REG_READ(ulPort, PCU_CF_CFG_FIFO_THRSLD_REG, ulCmp);
    if (ulCmp != unFifoThrsld.u32) {
        return ERROR;
    }

    return OK;
}

s32 gmac_set_tx_fifo_threshold(u32 ulPort)
{
    u_cf_tx_fifo_thrsld unTxFifo;
    u32 ulCmp;

    MAC_REG_READ(ulPort, PCU_CF_TX_FIFO_THRSLD_REG, unTxFifo.u32);

    unTxFifo.bits.cf_tx_p_full_th = 0x300;
    unTxFifo.bits.cf_tx_p_empty_th = 0x80;
    MAC_REG_WRITE(ulPort, PCU_CF_TX_FIFO_THRSLD_REG, unTxFifo.u32);

    MAC_REG_READ(ulPort, PCU_CF_TX_FIFO_THRSLD_REG, ulCmp);
    if (ulCmp != unTxFifo.u32) {
        return ERROR;
    }

    return OK;
}

s32 gmac_hw_int_int(u32 ulPort)
{
    u_cf_intrpt_msk unIntMsk;
    u_cf_intrpt_clr unIntClr;
    u32 ulCmp;

    unIntMsk.bits.cf_sram_parity_err_int_msk = DISABLE;
    unIntMsk.bits.cf_we_err_int_msk = DISABLE;
    unIntMsk.bits.cf_rbreq_err_int_msk = DISABLE;
    unIntMsk.bits.cf_mac_ff_err_int_msk = DISABLE;
    unIntMsk.bits.cf_ahb_err_int_msk = DISABLE;
    unIntMsk.bits.cf_pkg_intrpt_msk = DISABLE;

    /* 在实际使用过程中，应该关闭接收丢包中断，作测试时，最好放开 */
    unIntMsk.bits.cf_rx_drop_int_msk = DISABLE;

    /* 在实际使用过程中，应该关闭发送丢包中断，作测试时，最好放开 */
    unIntMsk.bits.cf_tx_drop_int_msk = DISABLE;
    unIntMsk.bits.cf_txcfg_avl_msk = DISABLE;
    unIntMsk.bits.cf_rel_buf_msk = DISABLE;
    unIntMsk.bits.cf_buf_avl_msk = DISABLE;
    unIntMsk.bits.cf_sram_parity_err_int_msk = DISABLE;

    /* 更新接收中断聚合值,0x1表示设置的中断数 */
    unIntMsk.bits.cf_intrpt_pkt = 0x1;

    /* 0xF表示设置的时间 */
    unIntMsk.bits.cf_intrpt_time = 0xF;
    MAC_REG_WRITE(ulPort, PCU_CF_INTRPT_MSK_REG, unIntMsk.u32);

    /* 回读校验 */
    MAC_REG_READ(ulPort, PCU_CF_INTRPT_MSK_REG, ulCmp);
    if (ulCmp != unIntMsk.u32) {
        printf("PCU_CF_INTRPT_MSK_REG error!\n");
    }

    /* 清除Rx中断 */
    unIntClr.u32 = 0;
    unIntClr.bits.cf_rx_frm_clr = 1;
    MAC_REG_WRITE(ulPort, PCU_CF_INTRPT_CLR_REG, unIntClr.u32);

    return 0;
}

void gmac_hw_ptp_reset(void)
{
    u32 rst_mask;

    rst_mask = (0x1U << 29); /* 配置bit 29为1 做ptp reset */
    writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);

    mdelay(20); /* delay 20ms */

    rst_mask = (0x1U << 29); /* 配置bit 29为1 做ptp reset */
    writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);

    return;
}

void gmac_hw_core_reset(u32 mac_num)
{
    u32 rst_mask;

    /* mac core软复位 */
    rst_mask = (0x1U << (GMAC_MAC_CORE_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);

    /* 总线接口软复位 */
    rst_mask = (0x1U << (GMAC_BUS_SYS_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);

    /* 工作时钟软复位 */
    rst_mask = (0x1U << (0 + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);

    /* mac_core配置寄存器软复位 */
    rst_mask = (0x1U << (GMAC_REG_CFG_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);

    if ((ETH2 == mac_num) || (ETH3 == mac_num)) {
        rst_mask = (0x1U << (GMAC_TX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
        writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);
    }

    /* rx serdes时钟软复位 */
    rst_mask = (0x1U << (GMAC_RX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_REQ1_ADDR);

    mdelay(20); /* delay 20ms */

    /* mac core解复位 */
    rst_mask = (0x1U << (GMAC_MAC_CORE_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);

    /* 总线接口软复位 */
    rst_mask = (0x1U << (GMAC_BUS_SYS_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);

    /* 工作时钟软复位 */
    rst_mask = (0x1U << (0 + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);

    /* mac_core配置寄存器软复位 */
    rst_mask = (0x1U << (GMAC_REG_CFG_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);

    /* tx serdes时钟解复位 */
    if ((ETH2 == mac_num) || (ETH3 == mac_num)) {
        rst_mask = (0x1U << (GMAC_TX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
        writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);
    }

    /* rx serdes时钟解复位 */
    rst_mask = (0x1U << (GMAC_RX_SERDES_SRST_BIT + mac_num * GMAC_SRST_BIT));
    writel(rst_mask, HP_SUB_CSR_HP_SRST_DIS1_ADDR);
}

s32 gmac_reset_chip(u32 mac_num)
{
    /* 去使能接收通道 */
    gmac_hw_rx_enable(mac_num, 0);

    /* 去使能发送通道 */
    gmac_hw_tx_enable(mac_num, 0);

    gmac_hw_ptp_reset();

    gmac_hw_core_reset(mac_num);

    /* 关闭bmu */
    return gmac_hw_set_bmu_close(mac_num);
}

/* ****************************************************************************
 功能描述  : 为特定平台开启PHY1512的Downshift功能
**************************************************************************** */
void enable_downshift_1512(void)
{
    u32 ulValue = 0;
    u16 ulTmp;

    /* reg num 0x16 写0 */
    hi309x_mdio_write(0, 0, 0x16, 0);
    mdelay(1);
    /* 读取reg num 0x10 */
    hi309x_mdio_read(0, 0, 0x10, &ulValue);
    ulTmp = ~((1 << 11) | (0x7 << 12)); /* bit 11, 12, 13 14的值清零 */
    ulValue &= ulTmp;
    ulValue |= (1 << 11); /* bit 11置 1 */
    mdelay(1);
    /* 写reg num 0x10 */
    hi309x_mdio_write(0, 0, 0x10, ulValue);
    printf("page0, reg 16: 0x%x\n", ulValue);

    mdelay(1);
    hi309x_mdio_read(0, 0, 0, &ulValue);
    ulValue |= (0x1 << 13); /* bit 13， bit 6组合配置速率模式 */
    ulValue &= ~(0x1 << 6); /* bit 13， bit 6组合配置速率模式 */
    mdelay(1);
    hi309x_mdio_write(0, 0, 0, ulValue);
    printf("page0, reg 0: 0x%x\n", ulValue);

    mdelay(1);
    /* reg num 0x16 写0 */
    hi309x_mdio_write(0, 0, 0x16, 0);
    mdelay(1);
}


s32 gmac_1512phy_init(u32 phy_addr)
{
    u32 value;

    /* 为特定平台开启downshift 功能 */
    enable_downshift_1512();

    /* reg num 0x16 写0x12 */
    hi309x_mdio_write(0, phy_addr, 0x16, 0x12);

    mdelay(1);

    /* 读reg num 0x14 设置端口模式为SGMII */
    hi309x_mdio_read(0, phy_addr, 0x14, &value);
    value &= 0xfff8;
    value |= 0x1;
    /* 写reg num 0x14 设置端口模式为SGMII */
    hi309x_mdio_write(0, phy_addr, 0x14, value);

    mdelay(1);

    /* 读reg num 0x14 reset phy */
    hi309x_mdio_read(0, phy_addr, 0x14, &value);
    value |= 0x8000;
    /* 写reg num 0x14 reset phy */
    hi309x_mdio_write(0, phy_addr, 0x14, value);
    mdelay(1);
    /* 写reg num 0x16 为0x0 */
    hi309x_mdio_write(0, phy_addr, 0x16, 0);

    /*  解决网口灯不正常的问题  */
    /*  写page寄存器设置页数为3    */
    /* 写reg num 0x16为0x3 */
    hi309x_mdio_write(0, phy_addr, 0x16, 0x3);
    mdelay(1);
    /* 读reg num 0x10 */
    hi309x_mdio_read(0, phy_addr, 0x10, &value);
    value &= 0xff00;
    value |= 0x0040;
    /* 写reg num 0x10为0x3 */
    hi309x_mdio_write(0, phy_addr, 0x10, value);
    mdelay(1);
    /* 写reg num 0x16 为0x0 */
    hi309x_mdio_write(0, phy_addr, 0x16, 0);

    return OK;
}

s32 gmac_8201phy_init(u32 phy_addr)
{
    u32 value;

    /* 写page寄存器设置页数为0, reset phy */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0x0);
    hi309x_mdio_read(0, phy_addr, 0x0, &value);
    value |= PHY_RESET;
    hi309x_mdio_write(0, phy_addr, 0x0, value);

    /* 写page寄存器设置页数为7 */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0x7);

    /* 读reg num 0x10 选择 RMII 模式 */
    hi309x_mdio_read(0, phy_addr, 0x10, &value);
    value |= SELECT_8201_RMII_MODE;
    /* 写reg num 0x10 */
    hi309x_mdio_write(0, phy_addr, 0x10, value);

    /* 读reg num 0x14 去使能广播地址功能，当phy id = 0x0,将不会响应这个地址发的命令 */
    hi309x_mdio_read(0, phy_addr, 0x14, &value);
    value &= (~ENABLE_BROADCAST_FUN);
    /* 写reg num 0x14 */
    hi309x_mdio_write(0, phy_addr, 0x14, value);

    /* 写page寄存器设置页数为0, phy设置为100M */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0x0);
    hi309x_mdio_read(0, phy_addr, 0x0, &value);
    value |= (0x1 << 13); /* bit 13，速率模式 100M */
    value |= (0x1 << 12); /* bit 12，配置自协商模式 */
    hi309x_mdio_write(0, phy_addr, 0x0, value);

    return OK;
}

/* ****************************************************************************
 函 数 名  : phy_8211_init
 功能描述  : Realtek 8211 PHY初始化
 输入参数  : u32 phy_addr
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
s32 gmac_8211phy_init(u32 phy_addr)
{
    u32 value;

    /* 写page寄存器设置页数为0, reset phy */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0x0);
    hi309x_mdio_read(0, phy_addr, 0x0, &value);
    value |= PHY_RESET;
    hi309x_mdio_write(0, phy_addr, 0x0, value);

    /* 写page寄存器设置页数为0xa43 */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0xa43);

    /* 去使能广播地址功能，当phy id = 0x0,将不会响应这个地址发的命令 */
    hi309x_mdio_read(0, phy_addr, 0x18, &value);
    value &= (~ENABLE_BROADCAST_FUN);
    hi309x_mdio_write(0, phy_addr, 0x18, value);

    /* 写page寄存器设置页数为0, phy设置为1000M */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0x0);
    hi309x_mdio_read(0, phy_addr, 0x0, &value);
    value |= (0x1 << 6); /* bit 13， bit 6组合配置速率模式 */
    value &= ~(0x1 << 13); /* bit 13， bit 6组合配置速率模式 */
    hi309x_mdio_write(0, phy_addr, 0x0, value);

    return OK;
}

/* ****************************************************************************
 函 数 名  : phy_8211_get_speed
 功能描述  : RTL 8211 phy的速率
 输入参数  : u32 phy_addr,phy地址
 输出参数  : u32 *speed  : 速度
 返 回 值  : 成功返回0,失败返回其他错误码
**************************************************************************** */
s32 phy_8211_get_speed(u32 phy_addr, u32 *speed)
{
    u32 value = 0;

    /* 写page寄存器设置页数为0xa43 */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0xa43);
    hi309x_mdio_read(0, phy_addr, 0x1a, &value);

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    /* bit5~bit4 indicate the speed */
    value = (value >> 4) & 0x3;

    /* translate the speed to enum value */
    switch (value) {
        case 0:
            *speed = MAC_10M_SGMII;
            break;
        case 1: /* 值为1时表示百兆模式 */
            *speed = MAC_100M_SGMII;
            break;
        case 2: /* 值为2时表示千兆模式 */
            *speed = MAC_1000M_SGMII;
            break;
        default:
            *speed = MAC_1000M_SGMII;
            break;
    }

    return 0;
}

/* ****************************************************************************
 函 数 名  : phy_8201_get_speed
 功能描述  : RTL 8201 phy的速率
 输入参数  : u32 phy_addr, phy地址
 输出参数  : u32 *speed  : 速度
 返 回 值  : 成功返回0,失败返回其他错误码
**************************************************************************** */
s32 phy_8201_get_speed(u32 phy_addr, u32 *speed)
{
    u32 value = 0;

    /* 写page寄存器设置页数为0x0 */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0);
    hi309x_mdio_read(0, phy_addr, 0, &value);

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    /* bit 13查看配置速率模式 */
    *speed = (value & (0x1 << 13)) ? MAC_100M_RMII : MAC_10M_RMII;

    return 0;
}

s32 phy_1512_get_speed(u32 phy_addr, u32 *speed)
{
    u32 value = 0;

    /* 写page寄存器设置页数为0x0 */
    hi309x_mdio_write(0, phy_addr, 0x1f, 0);
    hi309x_mdio_read(0, phy_addr, REG_1512_SPEED_DUPLEX, &value);

    if ((value & 0xffff) == 0xffff) {
        return -EIO;
    }

    if ((value & MASK_1512_SPEED) == 0x8000) {
        *speed = MAC_1000M_SGMII;
    } else if ((value & MASK_1512_SPEED) == 0x4000) {
        *speed = MAC_100M_SGMII;
    } else if ((value & MASK_1512_SPEED) == 0x0000) {
        *speed = MAC_10M_SGMII;
    } else {
        *speed = MAC_1000M_SGMII;
    }

    return 0;
}

s32 gmac_get_phy_id(u32 phy_addr, u32 *identifier)
{
    s32 ret;
    u32 identifier_l32 = 0;
    u32 identifier_h32 = 0;

    ret = hi309x_mdio_read(0, phy_addr, MII_PHY_ID0_REG, &identifier_h32);
    if (ret) {
        printf("gmac get phy0's identifier h32 failed!");
        return ret;
    }

    ret = hi309x_mdio_read(0, phy_addr, MII_PHY_ID1_REG, &identifier_l32);
    if (ret) {
        printf("gmac get phy1's identifier l32 failed!");
        return ret;
    }

    /* phy id = h32 left 16 + l32 */
    *identifier = ((identifier_h32 << 16) | (identifier_l32 & 0xFFFF));

    return ret;
}

s32 gmac_phy_init(struct hi309x_gmac_priv *priv)
{
    u32 i;
    s32 ret;
    u32 count = sizeof(hi_gmac_phy_info_table) / sizeof(gmac_phy_info_s);
    u32 identifier = 0;
    u32 phy_addr = priv->phy_addr;

    ret = gmac_get_phy_id(phy_addr, &identifier);
    if (ret) {
        printf("get phy id failed\n");
        return ret;
    }

    for (i = 0; i < count; i++) {
        if (hi_gmac_phy_info_table[i].phy_id == identifier) {
            hi_gmac_phy_info_table[i].init(phy_addr);
            priv->gmac_phy_info = (gmac_phy_info_s *)&hi_gmac_phy_info_table[i];
            printf("phy init success...\n");
            return 0;
        }
    }

    priv->gmac_phy_info = NULL;
    printf("phy init failed, phy_id = 0x%x\n", identifier);
    return -1;
}

static void gmac_hw_sds_phy_reset(u32 sds_phy_cfg1_offset_addr)
{
    u_gmac_phy_cfg phy_cfg;

    /* 置PHY为复位状态 */
    REG_READ(sds_phy_cfg1_offset_addr, phy_cfg.u32);
    if (phy_cfg.bits.phy_reset_0 != ENABLE) {
        phy_cfg.bits.phy_reset_0 = ENABLE;
        REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);
    }

    udelay(10); /* delay 10us */
}

static void gmac_hw_sds_phy_los_reg_init(u32 sds_phy_los_cfg_offset_addr)
{
    u_gmac_phy_los_cfg phy_los_cfg;

    /* serdes参数配置 */
    REG_READ(sds_phy_los_cfg_offset_addr, phy_los_cfg.u32);
    phy_los_cfg.bits.rx_los_valid_rst_msk_0 = 0x3;
    phy_los_cfg.bits.tx_vboost_lvl_0 = 0x4;
    phy_los_cfg.bits.rx0_los_filt_cnt_0 = 0x5;
    phy_los_cfg.bits.los_bias_0 = 0x2;
    phy_los_cfg.bits.los_level_0 = 0x9;
    REG_WRITE(sds_phy_los_cfg_offset_addr, phy_los_cfg.u32);
}

static void gmac_hw_sds_phy_cfg2_reg_init(u32 sds_phy_cfg2_offset_addr)
{
    u_gmac_phy_cfg2 phy_cfg2;

    REG_READ(sds_phy_cfg2_offset_addr, phy_cfg2.u32);
    phy_cfg2.bits.phy_data_reverse_en0 = 0x0;
    phy_cfg2.bits.tx0_preemph_0 = 0x13;
    phy_cfg2.bits.tx0_detect_rx_req_0 = 0x0;
    phy_cfg2.bits.tx0_amplitude_0 = 0x7f;
    phy_cfg2.bits.phy_tx0_term_offset_0 = 0x0;
    phy_cfg2.bits.phy_rx0_eq_0 = 0x0;
    REG_WRITE(sds_phy_cfg2_offset_addr, phy_cfg2.u32);
}

static void gmac_hw_sds_phy_cfg1_reg_init(u32 sds_phy_cfg1_offset_addr)
{
    u_gmac_phy_cfg phy_cfg;

    REG_READ(sds_phy_cfg1_offset_addr, phy_cfg.u32);
    phy_cfg.bits.phy_rtune_req_0 = 0x0;
    phy_cfg.bits.ref_clkdiv2_0 = 0x0;
    phy_cfg.bits.ref_use_pad_0 = 0x1;
    phy_cfg.bits.vreg_bypass_0 = 0x1;
    phy_cfg.bits.mpll_multiplier_0 = 0x19;
    phy_cfg.bits.ssc_ref_clk_sel_0 = 0x0;
    phy_cfg.bits.lane0_loopbk_en_0 = DISABLE;
    phy_cfg.bits.rx0_align_en_0 = ENABLE;
    phy_cfg.bits.rx0_data_en_0 = DISABLE;
    phy_cfg.bits.rx0_invert_0 = 0x0;
    phy_cfg.bits.rx0_los_en_0 = ENABLE;
    phy_cfg.bits.rx0_pll_en_0 = DISABLE;
    phy_cfg.bits.rx0_reset_0 = 0x0;
    phy_cfg.bits.rx0_term_en_0 = ENABLE;
    phy_cfg.bits.tx0_cm_en_0 = DISABLE;
    phy_cfg.bits.tx0_en_0 = DISABLE;
    phy_cfg.bits.tx0_reset_0 = 0x0;
    phy_cfg.bits.tx0_vboost_en_0 = DISABLE;
    phy_cfg.bits.ref_ssp_en_0 = 0x0;
    phy_cfg.bits.tx0_invert_0 = 0x0;
    phy_cfg.bits.test_powerdown_0 = 0x0;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);
    udelay(10); /* delay 10us */
}

s32 gmac_hw_sds_phy_init(u32 sds_phy_cfg1_offset_addr, u32 sds_phy_cfg2_offset_addr,
                         u32 sds_phy_state_offset_addr)
{
    u32 count = 0;
    u_gmac_phy_cfg phy_cfg;
    u_gmac_phy_cfg2 phy_cfg2;
    u_gmac_phy_sts phy_state;

    REG_READ(sds_phy_cfg1_offset_addr, phy_cfg.u32);
    /* 使能参考时钟 */
    phy_cfg.bits.ref_ssp_en_0 = ENABLE;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);

    /* 解除PHY的复位 */
    phy_cfg.bits.phy_reset_0 = DISABLE;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);

    /* 配置mpll_en, 启动PLL */
    phy_cfg.bits.mpll_en_0 = ENABLE;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);

    /* 判断mpll是否锁定 */
    do {
        if (count++ > MPLL_LOCK_TIMES) {
            printf("%s:%d  TIMEOUT!\n", __func__, __LINE__);
            return -EIO;
        }
        udelay(1);

        REG_READ(sds_phy_state_offset_addr, phy_state.u32);
    } while (!(phy_state.bits.mpll_state_0));

    /* 使能tx0_cm_en */
    phy_cfg.bits.tx0_cm_en_0 = ENABLE;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);

    /* 判断共模模式是否已经建立 */
    count = 0;
    do {
        if (count++ > TX_CM_TIMES) {
            return -EIO;
        }
        udelay(1);
        REG_READ(sds_phy_state_offset_addr, phy_state.u32);
    } while (!(phy_state.bits.tx0_cm_state_0));

    /* 启动TX、RX 通道 */
    phy_cfg.bits.rx0_pll_en_0 = ENABLE;
    phy_cfg.bits.tx0_en_0 = ENABLE;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);

    /* 判断通过是否启动成功 */
    count = 0;
    do {
        if (count++ > TX_RX_ENABLE_TIMES) {
            printf("%s:%d  TIMEOUT!\n", __func__, __LINE__);
            return -EIO;
        }
        udelay(1);
        REG_READ(sds_phy_state_offset_addr, phy_state.u32);
    } while (!((phy_state.bits.tx0_state_0) && (phy_state.bits.rx0_pll_state_0)));

    /* 启动发送方向数据传输 */
    REG_READ(sds_phy_cfg2_offset_addr, phy_cfg2.u32);
    phy_cfg2.bits.tx0_data_en_0 = ENABLE;
    REG_WRITE(sds_phy_cfg2_offset_addr, phy_cfg2.u32);

    /* 启动接收方向数据传输 */
    phy_cfg.bits.rx0_data_en_0 = ENABLE;
    REG_WRITE(sds_phy_cfg1_offset_addr, phy_cfg.u32);

    return 0;
}

s32 gmac_serdes_init(u32 mac_num)
{
    u32 sds_phy_los_cfg_offset_addr;
    u32 sds_phy_cfg1_offset_addr;
    u32 sds_phy_cfg2_offset_addr;
    u32 sds_phy_state_offset_addr;

    if (mac_num == ETH2) {
        sds_phy_los_cfg_offset_addr = HP_SUB_CSR_GMAC_PHY0_LOS_CFG_OFFSET_ADDR;
        sds_phy_cfg1_offset_addr = HP_SUB_CSR_GMAC_PHY0_CFG_OFFSET_ADDR;
        sds_phy_cfg2_offset_addr = HP_SUB_CSR_GMAC_PHY0_CFG2_OFFSET_ADDR;
        sds_phy_state_offset_addr = HP_SUB_CSR_GMAC_PHY0_STS_OFFSET_ADDR;
    } else if (mac_num == ETH3) {
        sds_phy_los_cfg_offset_addr = HP_SUB_CSR_GMAC_PHY1_LOS_CFG_OFFSET_ADDR;
        sds_phy_cfg1_offset_addr = HP_SUB_CSR_GMAC_PHY1_CFG_OFFSET_ADDR;
        sds_phy_cfg2_offset_addr = HP_SUB_CSR_GMAC_PHY1_CFG2_OFFSET_ADDR;
        sds_phy_state_offset_addr = HP_SUB_CSR_GMAC_PHY1_STS_OFFSET_ADDR;
    } else {
        return 0;
    }

    gmac_hw_sds_phy_reset(sds_phy_cfg1_offset_addr);

    gmac_hw_sds_phy_los_reg_init(sds_phy_los_cfg_offset_addr);

    gmac_hw_sds_phy_cfg2_reg_init(sds_phy_cfg2_offset_addr);

    gmac_hw_sds_phy_cfg1_reg_init(sds_phy_cfg1_offset_addr);

    return gmac_hw_sds_phy_init(sds_phy_cfg1_offset_addr, sds_phy_cfg2_offset_addr, sds_phy_state_offset_addr);
}

/* ****************************************************************************
 函 数 名  : gmac_set_io_cfg
 功能描述  : 配置RMII IO复用
 输入参数  : u32 ulPort
 输出参数  : 无
 返 回 值  : s32
**************************************************************************** */
void gmac_set_io_cfg(u32 ulPort)
{
    if (ulPort == ETH0) {
        REG_WRITE(IO_CONFIG_R_IOMG022_ADDR, 0x0);
        REG_WRITE(IO_CONFIG_R_IOMG023_ADDR, 0x0);
        REG_WRITE(IO_CONFIG_R_IOMG024_ADDR, 0x0);
        REG_WRITE(IO_CONFIG_R_IOMG025_ADDR, 0x0);
    } else if (ulPort == ETH1) {
        REG_WRITE(IO_CONFIG_R_IOMG026_ADDR, 0x0);
        REG_WRITE(IO_CONFIG_R_IOMG027_ADDR, 0x0);
    }
}

static void get_phy_speed(struct hi309x_gmac_priv *priv, u32 *speed)
{
    s32 ret;

    if (priv->gmac_phy_info) {
        ret = priv->gmac_phy_info->get_speed(priv->phy_addr, speed);
        if (ret == 0) {
            return;
        } else {
            printf("get phy speed failed\n");
            goto EXIT;
        }
    }

EXIT:
    if (priv->mac_num == ETH0 || priv->mac_num == ETH1) {
        *speed = MAC_100M_RMII;
    } else {
        *speed = MAC_1000M_SGMII;
    }

    return;
}

static s32 gmac_chip_init(struct hi309x_gmac_priv *priv)
{
    FEMAC_PKT_STORE_CTRL_CFG_S stRxCtrl = {
        .cf_buf_1st_skip1 = 0, .cf_buf_n1st_skip = 1,  .cf_cache_line = 0,
        .cf_rxbuf_1st_skip_size2 = 1, .cf_rx_align_num = 0
    }; /* 报文在内存中的存放格式 */
    FEMAC_RX_PKT_MODE_CFG_S stRxPktMode = {
        .cf_instr_head_mode = 0, .cf_skip_len = 0, .cf_parse_mode = 1, .mac_skip_len = 0
    };
    FEMAC_PORT_MODE_CFG_S stPortMode = {
        .max_frm_size = FE_MAX_PKT_LEN, .short_runts_thr = 0x1f, .pad_enable = 1,
        .crc_add = 1, .an_enable = 1, .runt_pkt_en = 1, .strip_pad_en = 1
    };
    FEMAC_MAC_FILTER_CFG_S stMacFilter = {
        .crc_err_pass = 1, .pause_frm_pass = 1, .bc_drop_en = 0, .mc_match_en = 1, .uc_match_en = 1
    };
    u32 mac_num, ret;

    mac_num = priv->mac_num;

    /* 根据端口号初始化报文在buff中的存储格式参数以及缓存获取方式(PCU_RX_CTRL_REG) */

    if (gmac_hw_set_pkt_store_format(mac_num, &stRxCtrl)) {
        printf(" gmac_hw_set_pkt_store_format fail\n");
        return -1;
    }

    /* 设置报文输入模式及解析模式 */
    if (gmac_hw_set_pkt_input_parse_mode(mac_num, &stRxPktMode)) {
        printf("gmac_hw_set_pkt_input_parse_mode fail\n");
        return -1;
    }

    stPortMode.port_num = mac_num;
    gmac_hw_set_port_mode_all(mac_num, &stPortMode);

    /* 初始化报文过滤规则 */
    gmac_hw_set_mac_addr_filter(mac_num, &stMacFilter);

    if (gmac_hw_set_rx_max_frame_len(mac_num, FE_MAX_PKT_LEN)) {
        printf(" gmac_hw_set_rx_max_frame_len failed\n");
        return -1;
    }

    /* 配置地址扩展功能，实现从32bit地址映射到34bit地址 */
    gmac_init_extend_addr(mac_num);

    ret = gmac_hw_set_tx_pad_add(mac_num, 1);
    ret += gmac_hw_set_poe_close(mac_num);
    ret += gmac_set_rx_fifo_threshold(mac_num);
    ret += gmac_set_tx_fifo_threshold(mac_num);
    ret += gmac_set_hw_mac_addr(mac_num);
    ret += gmac_hw_int_int(mac_num);

    return ret;
}

s32 gmac_eth_init(struct hi309x_gmac_priv *priv)
{
    s32 ret;

    if (priv->mac_num == ETH0 || priv->mac_num == ETH1) {
        gmac_set_io_cfg(priv->mac_num);
    }

    ret = gmac_reset_chip(priv->mac_num);
    if (ret) {
        return ret;
    }

    ret = gmac_chip_init(priv);
    if (ret) {
        return ret;
    }

    mdelay(1);

    gmac_rx_fill_all_buff(priv->mac_num, priv);

    return ret;
}

/* Transmit packet
 * return value: 0 - success
 * negative number: error
 */
static s32 gmac_eth_send(struct udevice *dev, void *packet, s32 length)
{
    u32 relative_addr;
    struct hi309x_gmac_priv *priv = dev_get_priv(dev);
    u8 *buffer = priv->tx_data_ring[priv->tx_data_num];
    u_tx_pcu_cmd stPcuCmd;

    if (priv->mac_num > ETH3) {
        printf("gmac_eth_send:mac_num(%d) is error\n", priv->mac_num);
        return -1;
    }

    if ((length >= RX_BUFFER_DEFAULT_SIZE) || (length <= 0)) {
        printf("Packet length is not correct length = %d\n", length);
        return -1;
    }

    (void)memcpy(buffer, (void *)packet, length);
    (void)memset((void *)&stPcuCmd, 0, sizeof(stPcuCmd));

    /* word0 */
    stPcuCmd.word0.bits.ipoff = 0; /* help:checksum         */
    stPcuCmd.word0.bits.sd = 0;    /* 0表示只包含一个数据段 duzhenquan vx1710 为1 */
    stPcuCmd.word0.bits.l3_cs = 0; /* 支持checksum与ipoff配合使用 */
    stPcuCmd.word0.bits.wb = 1;    /* 支持写回测试 */
    stPcuCmd.word0.bits.l4_cs = 0; /* */

    /* word1 */
    stPcuCmd.word1.bits.l2cache = 1;  /* 不需要使能L2 cache */
    stPcuCmd.word1.bits.fm = 0;       /* 发送完成释放缓存空间 */
    stPcuCmd.word1.bits.back = 0;     /* FE不需要，数据所在cache line首地址与缓存首地址之间距离 */
    stPcuCmd.word1.bits.len = length; /* 发送长度 */
    stPcuCmd.word1.bits.pool = 0;     /* 释放的pool ID */

    /* word2 */
    relative_addr = gmac_get_tx_ddr_relative_addr(priv->mac_num, (u64)priv->tx_data_ring[priv->tx_data_num]);
    stPcuCmd.ulPktAddr = relative_addr;

    /* word3 */
    stPcuCmd.ulClearAddr = relative_addr;

    MAC_REG_WRITE(priv->mac_num, PCU_TX_CFF_ADDR_0_REG, stPcuCmd.word0.u_32);
    MAC_REG_WRITE(priv->mac_num, PCU_TX_CFF_ADDR_1_REG, stPcuCmd.word1.u_32);
    MAC_REG_WRITE(priv->mac_num, PCU_TX_CFF_ADDR_2_REG, stPcuCmd.ulPktAddr);
    MAC_REG_WRITE(priv->mac_num, PCU_TX_CFF_ADDR_3_REG, stPcuCmd.ulClearAddr);
    asm volatile("" ::: "memory");

    priv->tx_data_num++;
    priv->tx_data_num = priv->tx_data_num % CHIP_TX_FIFO_NUM;

    return 0;
}

void gmac_print_pktinfo(u8 *addr, s32 len)
{
    s32 i;
    unsigned long addr_print = (unsigned long)addr;

#define MAX_LINE_LEN    16

    for (i = 0; i < len; i++) {
        if (i % MAX_LINE_LEN == 0) {
            addr_print += MAX_LINE_LEN;
            printf("0x%16lx:  ", addr_print);
        }

        printf("%02x ", addr[i]);
        if ((i + 1) % MAX_LINE_LEN == 0) {
            printf("\n");
        }
    }

    printf("\n");
}


static void gmac_rx_debug(u8 *data, u32 len, u8 *desc)
{
    if (g_eth_rx_print == 1) {
        printf("DATA:\n");
        gmac_print_pktinfo(data, len);
        printf("DESC:\n");
        gmac_print_pktinfo(desc, RX_DESC_LEN);
    }
}

/*
 * Receive packet
 * return value: 0 success
 * positive number: actual packet length
 * negative number: error
 */
static s32 gmac_eth_rx(struct udevice *dev, s32 flags, u8 **packetp)
{
    u8 *mData;
    u8 *packet = packet_buf;
    u32 pkt_len = 0;
    u_cf_intrpt_stat unIntRaw;
    u_cf_intrpt_clr unIntClr = {
        .u32 = 0
    };
    u8 *pcMacDesc = NULL; /* MAC报文描述符 */
    u_cf_cff_data_num unCfCffDataNum;
    u32 relative_addr;
    struct hi309x_gmac_priv *priv = dev_get_priv(dev);

    if (priv->mac_num > ETH3) {
        printf("gmac_eth_rx:mac_num(%d) is error\n", priv->mac_num);
        return -EAGAIN;
    }

    MAC_REG_READ(priv->mac_num, PCU_CF_INTRPT_STAT_REG, unIntRaw.u32);
    if (unIntRaw.bits.stat_rx_frm) {
        unIntClr.bits.cf_rx_frm_clr = 1;
        MAC_REG_WRITE(priv->mac_num, PCU_CF_INTRPT_CLR_REG, unIntClr.u32);
        pcMacDesc = priv->rx_data_ring[priv->rx_data_num];
        mData = pcMacDesc + ((FE_RX_SKIP1 + FE_RX_SKIP2) * CACHE_LINE_SIZE + CACHE_LINE_SIZE);
        /* 左移8 后作为len的高字节 */
        pkt_len = (pcMacDesc[GMAC_DESC_PKT_LEN_HIGH] << 8) + pcMacDesc[GMAC_DESC_PKT_LEN_LOW];

        /* 获取报文长度 word3.Len */
        if (pkt_len == 0) {
            return pkt_len;
        }

        if (pkt_len >= (RX_BUFFER_DEFAULT_SIZE - ((FE_RX_SKIP1 + FE_RX_SKIP2) * CACHE_LINE_SIZE + CACHE_LINE_SIZE))) {
            MAC_REG_READ(priv->mac_num, PCU_CF_CFF_DATA_NUM_REG, unCfCffDataNum.u32);
            printf("error:%s:%d: cf_rx_cfg_num = %d, rxNum = %d, pkt_len = %d\n", __FUNCTION__, __LINE__,
                unCfCffDataNum.bits.cf_rx_cfg_num, priv->rx_data_num, pkt_len);
            pkt_len = -EAGAIN;
            goto EXIT_RECV;
        }

        (void)memcpy(packet, mData, pkt_len);

        gmac_rx_debug(mData, pkt_len, pcMacDesc);

        *packetp = packet;
    EXIT_RECV:
        (void)memset(priv->rx_data_ring[priv->rx_data_num], 0, CPU_CACHE_LINE_SIZE);
        relative_addr = gmac_get_rx_ddr_relative_addr(priv->mac_num, (dma_addr_t)priv->rx_data_ring[priv->rx_data_num]);
        MAC_REG_WRITE(priv->mac_num, PCU_RX_CFF_ADDR_REG, relative_addr);

        asm volatile("" ::: "memory");
        priv->rx_data_num++;
        priv->rx_data_num = priv->rx_data_num % CHIP_RX_FIFO_NUM;
    }

    return pkt_len;
}

static u32 get_mac_port(void)
{
    char *eth = NULL;
    u32 mac_port;

    eth = env_get("ethdev");
    if (eth != NULL) {
        mac_port = (u32)simple_strtoul(eth, NULL, 10); /* 10进制获取参数port */
        if (mac_port > ETH3) {
            printf("Config mac_num(0x%x) so big, Please check\n", mac_port);
            mac_port = 0xFF;
        }
    } else {
        mac_port = ETH2;
    }

    return mac_port;
}

static u32 get_phy_addr(void)
{
    char *phy = NULL;
    u32 phy_addr;

    phy = env_get("phy_addr");
    if (phy != NULL) {
        phy_addr = (u32)simple_strtoul(phy, NULL, 10); /* 10进制获取参数phy addr */
        if (phy_addr > 0x1f) {
            printf("Config phy_addr(0x%x) error\n", phy_addr);
            phy_addr = 0xFF;
        }
    } else {
        phy_addr = 0x3;
    }

    return phy_addr;
}

/* stop the data stream on mac. */
void gmac_eth_stop(struct udevice *dev)
{
    struct hi309x_gmac_priv *priv = dev_get_priv(dev);
    if (priv->mac_num > ETH3) {
        return;
    }

    /* 使能接收通道 */
    gmac_hw_rx_enable(priv->mac_num, DISABLE);
    /* 使能发送通道 */
    gmac_hw_tx_enable(priv->mac_num, DISABLE);

    return;
}

/* Open ethernet device for operation, start the data stream on mac. */
static s32 gmac_eth_start(struct udevice *dev)
{
    u32 ret = 0;
    u32 mac_port, phy_addr;
    struct hi309x_gmac_priv *priv = dev_get_priv(dev);
    u32 speed;

    mac_port = get_mac_port();
    phy_addr = get_phy_addr();
    if (mac_port == 0xFF || phy_addr == 0xFF) {
        return -1;
    }

    if (priv->mac_num == mac_port) {
        if (priv->phy_addr != phy_addr) {
            ret = gmac_phy_init(priv);
            priv->phy_addr = phy_addr;
        }

        get_phy_speed(priv, &speed);
        gmac_hw_set_port_mode(priv->mac_num, speed, GMAC_DUPLEX_FULL);
        /* 使能接收通道 */
        gmac_hw_rx_enable(priv->mac_num, ENABLE);
        gmac_hw_tx_enable(priv->mac_num, ENABLE);
        return ret;
    } else {
        priv->mac_num = mac_port;
        priv->phy_addr = phy_addr;
    }

    gmac_phy_init(priv);

    printf("Use eth%x ,phy addr is 0x%x\n", priv->mac_num, priv->phy_addr);

    ret = gmac_serdes_init(priv->mac_num);
    if (ret) {
        printf("serdes init failede\n");
        return -1;
    }

    ret = gmac_eth_init(priv);
    if (ret) {
        printf("gmac eth%d init failed\n", priv->mac_num);
        return ret;
    }

    printf("Boot gmac[%d] init ok!\n", priv->mac_num);

    /* 使能接收通道 */
    gmac_hw_rx_enable(priv->mac_num, ENABLE);
    /* 使能发送通道 */
    gmac_hw_tx_enable(priv->mac_num, ENABLE);

    return 0;
}

static s32 gmac_write_hwaddr(struct udevice *dev)
{
    u32 addr_low, addr_hi;
    struct eth_pdata *pdata = dev_get_plat(dev);
    struct hi309x_gmac_priv *priv = dev_get_priv(dev);

    if (priv->mac_num > ETH3) {
        return 0;
    }

    addr_hi = (pdata->enetaddr[0] << 8) |   /* enetaddr[0] corresponding 0-8 bit in high 32 bit */
              (pdata->enetaddr[1]);

    addr_low = (pdata->enetaddr[2] << 24) | /* enetaddr[2] corresponding 24-31 bit in low 32 bit */
               (pdata->enetaddr[3] << 16) | /* enetaddr[3] corresponding 16-23 bit in low 32 bit */
               (pdata->enetaddr[4] << 8) |  /* enetaddr[4] corresponding 8-15 bit in low 32 bit */
               (pdata->enetaddr[5]);        /* enetaddr[5] corresponding 0-7 bit in low 32 bit */

    /* 设置端口MAC地址 */
    MAC_REG_WRITE(priv->mac_num, MAC_CORE_STATION_ADDR_HIGH_2_REG, addr_hi);
    MAC_REG_WRITE(priv->mac_num, MAC_CORE_STATION_ADDR_LOW_2_REG, addr_low);

    return 0;
}

static s32 gmac_eth_probe(struct udevice *dev)
{
    u32 platform_id, board_id;
    u32 value;
    s32 ret;
    struct hi309x_gmac_priv *priv = dev_get_priv(dev);

    if (!priv) {
        printf("priv is null\n");
        return -1;
    }
    (void)memset((void *)(priv), 0x0, sizeof(struct hi309x_gmac_priv));

    priv->mac_num = 0xFF;
    priv->phy_addr = 0xFF;

    ret = hi309x_mdio_init(0);
    if (ret) {
        printf("mdio init failed\n");
        return ret;
    }

    if (gmac_rx_init_ring(priv)) {
        printf("error: init rx buffer fail!\n");
        return -1;
    }

    if (gmac_tx_init_ring(priv)) {
        printf("error: init tx buffer fail!\n");
        return -1;
    }

    /* 获取单板硬件信息 */
    platform_id = bdinfo_get_platform_id();
    board_id = bdinfo_get_board_id();
    if ((platform_id == BOARD_PLATM_BUCKLE) && (board_id == EVB_BOARD_ID)) {
        /* 禁用GMAC3、GMAC1、GMAC0对接PHY响应广播地址功能，
         * 规避GMAC2 PHY地址为0引起其他phy 响应引起异常的问题 */
        hi309x_mdio_write(0, 0x7, 0x1f, 0xa43);
        hi309x_mdio_read(0, 0x7, 0x18, &value);
        value &= (~ENABLE_BROADCAST_FUN);
        hi309x_mdio_write(0, 0x7, 0x18, value);

        hi309x_mdio_write(0, 0x3, 0x1f, 0x7);
        hi309x_mdio_read(0, 0x3, 0x14, &value);
        value &= (~ENABLE_BROADCAST_FUN);
        hi309x_mdio_write(0, 0x3, 0x14, value);

        hi309x_mdio_write(0, 0x2, 0x1f, 0x7);
        hi309x_mdio_read(0, 0x2, 0x14, &value);
        value &= (~ENABLE_BROADCAST_FUN);
        hi309x_mdio_write(0, 0x2, 0x14, value);
    }

    return 0;
}

static const struct eth_ops hi309x_eth_ops = {
    .start = gmac_eth_start,
    .send = gmac_eth_send,
    .recv = gmac_eth_rx,
    .stop = gmac_eth_stop,
    .write_hwaddr = gmac_write_hwaddr,
};

U_BOOT_DRIVER(hi309x_gmac_drv) = {
    .name = "hi309x_gmac",
    .id = UCLASS_ETH,
    .of_match = of_match_ptr(hi309x_gmac_id),
    .of_to_plat = of_match_ptr(hi309x_gmac_ofdata_to_platdata),
    .plat_auto = sizeof(struct hi309x_gmac_platdata),
    .probe = gmac_eth_probe,
    .ops = &hi309x_eth_ops,
    .priv_auto = sizeof(struct hi309x_gmac_priv),
};

struct eth_pdata hi309x_eth_pdata = {
    .enetaddr = { 0x08, 0x3, 0x08, 0x08, 0x08, 0x08 },
};

U_BOOT_DRVINFO(hi309x_gmac_dev) = {
    .name = "hi309x_gmac",
    .plat = &hi309x_eth_pdata,
};
