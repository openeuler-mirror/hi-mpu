/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
* Description: none
* Create: 2018-08-01
* Notes: none
*/

#ifndef _FE_MAC_REG_H
#define _FE_MAC_REG_H

#define MAC01_ALL_PORT    0

/******************************************************************************/
/*                      PhosphorV600 MAC_CORE 寄存器定义                      */
/******************************************************************************/
#define MAC_CORE_DUPLEX_TYPE            0x8   /* 全双工半双工模式寄存器 */
#define MAC_CORE_FD_FC_TYPE_REG         0xC   /* FD_FC_TYPE为流控帧类型域寄存器 */
#define MAC_CORE_FIFO_THRSLD_REG        0x10  /* FIFO_THRSLD为MAC应用侧FIFO参数设置寄存器 */
#define MAC_CORE_FC_TX_TIMER_REG        0x1C  /* FC_TX_TIMER为流控时间参数寄存器 */
#define MAC_CORE_FD_FC_ADDR_LOW_REG     0x20  /* FD_FC_ADDR_LOW为流控帧目的地址寄存器1 */
#define MAC_CORE_FD_FC_ADDR_HIGH_REG    0x24  /* FD_FC_ADDR_HIGH为流控帧目的地址寄存器2 */
#define MAC_CORE_IPG_TX_TIMER_REG       0x30  /* IPG_TX_TIMER为发送帧间隙寄存器 */
#define MAC_CORE_PAUSE_THR_REG          0x38  /* PAUSE_THR为发送流控帧间隙寄存器 */
#define MAC_CORE_MAX_FRM_SIZE_REG       0x3C  /* MAX_FRM_SIZE为最大帧长寄存器 */
#define MAC_CORE_PORT_MODE_REG          0x40  /* PORT_MODE为端口状态寄存器 */
#define MAC_CORE_PORT_EN_REG            0x44  /* PORT_EN为通道使能寄存器 */
#define MAC_CORE_PAUSE_EN_REG           0x48  /* PAUSE_EN为流控使能寄存器 */
#define MAC_CORE_SHORT_RUNTS_THR_REG    0x50  /* SHORT_RUNTS_THR为超短帧界限寄存器 */
#define MAC_CORE_DROP_UNK_CTL_FRM_REG   0x54  /* DROP_UNK_CTL_FRM为未知控制帧丢弃使能寄存器 */
#define MAC_CORE_AN_NEG_STATE           0x58  /* 自协商状态寄存器 */
#define MAC_CORE_TX_LOCAL_PAGE          0x5C  /* 自协商本地配置寄存器 */
#define MAC_CORE_TRANSMIT_CONTROL_REG   0x60  /* TRANSMIT_CONTROL为常用配置寄存器 */
#define MAC_CORE_REC_FILT_CONTROL_REG   0x64  /* REC_FILT_CONTROL为接收帧过滤控制寄存器 */
#define MAC_CORE_PORT_MC_ADDR_LOW_REG   0x68  /* PORT_MC_ADDR_LOW为多播地址寄存器1 */
#define MAC_CORE_PORT_MC_ADDR_HIGH_REG  0x6C  /* PORT_MC_ADDR_HIGH为多播地址寄存器2 */
#define MAC_CORE_MAC_CLR_REG            0x70  /* MAC_CLR为MAC清零寄存器 */
#define RX_OCTETS_TOTAL_OK_REG          0x80  /* RX_OCTETS_TOTAL_OK为接收有效帧的字节统计寄存器 */
#define RX_OCTETS_BAD_REG               0x84  /* RX_OCTETS_BAD为接收错误帧字节统计寄存器 */
#define RX_UC_PKTS_REG                  0x88  /* RX_UC_PKTS为MAC接收单播帧数统计寄存器 */
#define RX_MC_PKTS_REG                  0x8C  /* RX_MC_PKTS为接收多播帧数统计寄存器 */
#define RX_BC_PKTS_REG                  0x90  /* RX_BC_PKTS为接收的广播帧数统计寄存器 */
#define RX_PKTS_64OCTETS_REG            0x94  /* RX_PKTS_64OCTETS为接收到帧长为64byte的帧数统计寄存器 */
#define RX_PKTS_65TO127OCTETS_REG       0x98  /* RX_PKTS_65TO127OCTETS为接收到帧长为65～127byte的帧数统计寄存器 */
#define RX_PKTS_128TO255OCTETS_REG      0x9C  /* RX_PKTS_128TO255OCTETS为接收到帧长为128～255byte的帧数统计寄存器 */
#define RX_PKTS_255TO511OCTETS_REG      0xA0  /* RX_PKTS_255TO511OCTETS为接收到帧长为256～511byte的帧数统计寄存器 */
#define RX_PKTS_512TO1023OCTETS_REG     0xA4  /* RX_PKTS_512TO1023OCTETS为接收到帧长为512～1023byte的帧数统计寄存器 */
#define RX_PKTS_1024TO1518OCTETS_REG    0xA8  /* RX_PKTS_1024TO1518OCTETS为接收帧长为1024～1518byte的帧数统计寄存器 */
#define RX_PKTS_1519TOMAXOCTETS_REG     0xAC  /* RX_PKTS_1519TOMAXOCTETS为接收到帧长为1519～最大byte的帧数统计寄存器 */
#define RX_FCS_ERRORS_REG               0xB0  /* RX_FCS_ERRORS为接收CRC检验错误的帧数统计寄存器 */
#define RX_TAGGED_REG                   0xB4  /* RX_TAGGED为接收带TAG的帧统计寄存器 */
#define RX_DATA_ERR_REG                 0xB8  /* RX_DATA_ERR为接收数据错误帧统计寄存器 */
#define RX_ALIGN_ERRORS_REG             0xBC  /* RX_ALIGN_ERRORS为接收数据非字节对齐错误帧统计寄存器 */
#define RX_LONG_ERRORS_REG              0xC0  /* RX_LONG_ERRORS为接收帧长大于允许的最大长度的帧数统计寄存器1 */
#define RX_JABBER_ERRORS_REG            0xC4  /* RX_JABBER_ERRORS为接收帧长大于允许的最大长度的帧数统计寄存器2 */
#define RX_PAUSE_FRAME_REG              0xC8  /* RX_PAUSE_MACCONTROL_FRAMCOUNTER为接收到的流控帧数统计寄存器 */
#define RX_UNKNOWN_MACCONTROL_FRAMCOUNTER_REG 0xCC /* MAC接收到的非流控控制帧数统计寄存器 */
#define RX_VERY_LONG_ERR_CNT_REG        0xD0  /* RX_VERY_LONG_ERR_CNT为接收到超长帧统计寄存器。 */
#define RX_RUNT_ERR_CNT_REG             0xD4  /* RX_RUNT_ERR_CNT为接收到帧长小于64byte大于等于12byte的帧数统计寄存器 */
#define RX_SHORT_ERR_CNT_REG            0xD8  /* RX_SHORT_ERR_CNT为接收帧长小于96bit的帧数统计寄存器。 */
#define RX_FILT_PKT_CNT_REG             0xE8  /* 接收方向过滤掉的包统计 */
#define RX_OCTETS_TOTAL_FILT_CNT_REG    0xEC  /* 接收方向过滤掉的字节统计 */
#define TX_OCTETS_TOTAL_OK_REG          0x100  /* OCTETS_TRANSMITTED_OK为发送成功的好包字节数统计寄存器 */
#define TX_OCTETS_TRANSMITTED_BAD_REG   0x104  /* OCTETS_TRANSMITTED_BAD为发送成功的坏包字节数统计寄存器 */
#define TX_UC_PKTS_REG                  0x108  /* TX_UC_PKTS为发送的单播帧数统计寄存器 */
#define TX_MC_PKTS_REG                  0x10C  /* TX_MC_PKTS为发送的多播帧数统计寄存器 */
#define TX_BC_PKTS_REG                  0x110  /* TX_BC_PKTS为发送的广播帧数统计寄存器 */
#define TX_PKTS_64OCTETS_REG            0x114  /* TX_PKTS_64OCTETS为发送帧长为64byte的帧数统计寄存器 */
#define TX_PKTS_65TO127OCTETS_REG       0x118  /* TX_PKTS_65TO127OCTETS为发送帧长为65～127byte的帧数统计寄存器 */
#define TX_PKTS_128TO255OCTETS_REG      0x11C  /* TX_PKTS_128TO255OCTETS为发送帧长为128～255byte的帧数统计寄存器 */
#define TX_PKTS_255TO511OCTETS_REG      0x120  /* TX_PKTS_255TO511OCTETS为发送帧长为256～511byte的帧数统计寄存器 */
#define TX_PKTS_512TO1023OCTETS_REG     0x124  /* TX_PKTS_512TO1023OCTETS为发送帧长为512～1023byte节的帧数统计寄存器 */
#define TX_PKTS_1024TO1518OCTETS_REG    0x128  /* TX_PKTS_1024TO1518OCTETS为发送帧长为1024～1518byte的帧数统计寄存器 */
#define TX_PKTS_1519TOMAXOCTETS_REG     0x12C  /* TX_PKTS_1519TOMAXOCTETS为发送帧长大于1519byte的帧数统计寄存器 */
#define TX_EXCESSIVE_LENGTH_DROP_REG    0x14C  /* 超过设定的最大帧长导致发送失败次数统计寄存器 */
#define TX_UNDERRUN_REG                 0x150  /* TX_UNDERRUN为帧发送过程中发生内部错误而导致发送失败的次数统计 */
#define TX_TAGGED_REG                   0x154  /* TX_TAGGED为发送的VLAN帧数统计寄存器 */
#define TX_CRC_ERROR_REG                0x158  /* TX_CRC_ERROR为发送帧长正确CRC错误的帧数统计寄存器 */
#define TX_PAUSE_FRAMES_REG             0x15C  /* TX_PAUSE_FRAMES为发送PAUSE帧数统计寄存器 */
#define MAC_CORE_LED_MOD_REG            0x16C  /* LED_MOD为点灯模式控制寄存器 */
#define MAC_CORE_LINE_LOOP_BACK_REG     0x1A8  /* LINE_LOOP_BACK为MAC线路侧环回寄存器 */
#define MAC_CORE_CF_CRC_STRIP_REG       0x1B0  /* CF_CRC_STRIP为CRC剥离使能寄存器 */
#define MAC_CORE_MODE_CHANGE_EN_REG     0x1B4  /* MODE_CHANGE_EN为端口模式改变使能寄存器 */
#define MAC_CORE_SIXTEEN_BIT_CNTR       0x1CC  /* 自协商链接时间寄存器 */
#define MAC_CORE_LD_LINK_COUNTER        0x1D0  /* 自协商链接时间更新寄存器 */
#define MAC_CORE_LOOP_REG               0x1DC  /* LOOP_REG为环回补充寄存器 */
#define MAC_CORE_RECV_CONTROL_REG       0x1E0  /* RECV_CONTROL为接收控制寄存器 */
#define MAC_CORE_VLAN_CODE_REG          0x1E8  /* VLAN_CODE为VLAN Code寄存器 */
#define RX_OVERRUN_CNT_REG              0x1EC  /* RX_OVERRUN_CNT为FIFO溢出统计寄存器 */
#define RX_LENGTHFIELD_ERR_CNT_REG      0x1F4  /* RX_LENGTHFIELD_ERR_CNT为含PAD帧统计寄存器 */
#define RX_FAIL_COMMA_CNT_REG           0x1F8  /* RX_FAIL_COMMA_CNT为字节定界COMMA统计寄存器 */
#define MAC_CORE_STATION_ADDR_LOW_0_REG         0x200  /* STATION_ADDR_LOW为本机MAC地址0寄存器1 */
#define MAC_CORE_STATION_ADDR_HIGH_0_REG        0x204  /* STATION_ADDR_HIGH为本机MAC地址0寄存器2 */
#define MAC_CORE_STATION_ADDR_LOW_1_REG         0x208  /* STATION_ADDR_LOW为本机MAC地址1寄存器1 */
#define MAC_CORE_STATION_ADDR_HIGH_1_REG        0x20C  /* STATION_ADDR_HIGH为本机MAC地址1寄存器2 */
#define MAC_CORE_STATION_ADDR_LOW_2_REG         0x210  /* STATION_ADDR_LOW为本机MAC地址2寄存器1 */
#define MAC_CORE_STATION_ADDR_HIGH_2_REG        0x214  /* STATION_ADDR_HIGH为本机MAC地址2寄存器2 */
#define MAC_CORE_STATION_ADDR_LOW_3_REG         0x218  /* STATION_ADDR_LOW为本机MAC地址3寄存器1 */
#define MAC_CORE_STATION_ADDR_HIGH_3_REG        0x21C  /* STATION_ADDR_HIGH为本机MAC地址3寄存器2 */
#define MAC_CORE_STATION_ADDR_LOW_4_REG         0x220  /* STATION_ADDR_LOW为本机MAC地址4寄存器1 */
#define MAC_CORE_STATION_ADDR_HIGH_4_REG        0x224  /* STATION_ADDR_HIGH为本机MAC地址4寄存器2 */
#define MAC_CORE_STATION_ADDR_LOW_5_REG         0x228  /* STATION_ADDR_LOW为本机MAC地址5寄存器1 */
#define MAC_CORE_STATION_ADDR_HIGH_5_REG        0x22C  /* STATION_ADDR_HIGH为本机MAC地址5寄存器2 */
#define MAC_CORE_STATION_ADDR_LOW_MSK_0_REG     0x230  /* STATION_ADDR_LOW为本机MAC地址0寄存器1的掩码 */
#define MAC_CORE_STATION_ADDR_HIGH_MSK_0_REG    0x234  /* STATION_ADDR_HIGH为本机MAC地址0寄存器2的掩码 */
#define MAC_CORE_STATION_ADDR_LOW_MSK_1_REG     0x238  /* STATION_ADDR_LOW为本机MAC地址1寄存器1的掩码 */
#define MAC_CORE_STATION_ADDR_HIGH_MSK_1_REG    0x23C  /* STATION_ADDR_HIGH为本机MAC地址1寄存器2的掩码 */
#define MAC_CORE_MAC_SKIP_LEN_REG               0x240  /* SKIP_LEN为报文头不解析字段长度 */


/******************************************************************************/
/*                      PhosphorV600 PCU 寄存器定义                           */
/******************************************************************************/
#define PCU_CF_TX_FIFO_THRSLD_REG    0x420  /* CF_TX_FIFO_THRSLD为发送FIFO水线寄存器。 */
#define PCU_CF_RX_FIFO_THRSLD_REG    0x424  /* CF_RX_FIFO_THRSLD为接收FIFO水线寄存器。 */
#define PCU_CF_CFG_FIFO_THRSLD_REG   0x428  /* CF_CFG_FIFO_THRSLD为配置FIFO水线寄存器。 */
#define PCU_CF_INTRPT_MSK_REG        0x42C  /* CF_INTRPT_MSK为中断屏蔽寄存器。 */
#define PCU_CF_FIFO_CLR_REG          0x430  /* CF_FIFO_CLR为FIFO清零寄存器。 */
#define PCU_CF_INTRPT_STAT_REG       0x434  /* CF_INTRPT_STAT为中断状态寄存器。 */
#define PCU_CF_INTRPT_CLR_REG        0x438  /* CF_INTRPT_CLR为中断清零寄存器。 */
#define PCU_BUS_ERR_ADDR_REG         0x43C  /* BUS_ERR_ADDR为出错总线地址。 */
#define PCU_MAX_FRAME_LEN_REG        0x444  /* MAX_FRAME_LEN为接收超长帧界限寄存器 */
#define PCU_TX_DROP_CNT_REG          0x448  /* TX_DROP_CNT为发送方向因FIFO空而丢弃的包的统计计数器。 */
#define PCU_RX_OVER_FLOW_CNT_REG     0x44C  /* OVER_FLOW_CNT为因接收FIFO满而丢弃的包的统计计数器。 */
#define PCU_DEBUG_ST_MCH_REG         0x450  /* DEBUG_ST_MCH状态机上报寄存器。 */
#define PCU_FIFO_CURR_STATUS_REG     0x454  /* FIFO_CURR_STATUS为FIFO当前状态寄存器。 */
#define PCU_FIFO_HIS_STATUS_REG      0x458  /* FIFO_HIS_STATUS为FIFO历史状态寄存器。 */
#define PCU_CF_CFF_DATA_NUM_REG      0x45C  /* CF_CFF_DATA_NUM为配置FIFO数据个数寄存器。 */
#define PCU_TX_CS_FAIL_CNT_REG       0x460  /* TX_CS_FAIL_CNT为发送添加校验和失败计数器。 */
#define PCU_RX_TRANS_PKG_CNT_REG     0x464  /* RX_TRANS_PKG_CNT为下行搬运成功的包计数器。 */
#define PCU_TX_TRANS_PKG_CNT_REG     0x468  /* TX_TRANS_PKG_CNT为上行搬运成功的包计数器。 */
#define PCU_RX_ADDR_OVERFLOW_REG     0x46C  /* RX_ADDR_OVERFLOW为写下行配置FIFO溢出计数器。 */
#define PCU_CF_TX_PAUSE_REG          0x470  /* CF_TX_PAUSE为发送流控控制寄存器。 */
#define PCU_TX_CFF_ADDR_0_REG        0x488  /* TX_CFF_ADDR_0为发送描述符FIFO地址寄存器0。 */
#define PCU_TX_CFF_ADDR_1_REG        0x48C  /* TX_CFF_ADDR_1为发送描述符FIFO地址寄存器1。 */
#define PCU_TX_CFF_ADDR_2_REG        0x490  /* TX_CFF_ADDR_2为发送描述符FIFO地址寄存器2。 */
#define PCU_TX_CFF_ADDR_3_REG        0x494  /* TX_CFF_ADDR_3为发送描述符FIFO地址寄存器3。 */
#define PCU_RX_CFF_ADDR_REG          0x4A0  /* RX_CFF_ADDR为接收配置FIFO地址寄存器。 */
#define PCU_WE_ADDR_REG              0x4E0  /* WE_ADDR为报文描述符的写地址。 */
#define PCU_RX_BUF_SIZE_REG          0x4E4  /* RX_BUF_SIZE为接收缓存大小。 */
#define PCU_BUS_CTRL_REG             0x4E8  /* BUS_CTRL为总线控制寄存器。 */
#define PCU_RX_BUF_REQ_ADDR_REG      0x4EC  /* RX_BUF_REQ_ADDR为接收缓存的申请地址。 */
#define PCU_RX_CTRL_REG              0x4F0  /* RX_CTRL为接收控制寄存器。 */
#define PCU_RX_PKT_MODE_REG          0x4F4  /* RX_PKT_MODE为接收报文输入模式。 */
#define PCU_QOS_MODE_REG             0x4F8  /* QOS_MODE为QOS信息的生成模式。 */
#define PCU_VLAN_QOS_REG             0x4FC  /* VLAN_QOS为vlan帧中优先级到qos的查找表。 */
#define PCU_IP_QOS_0_REG             0x500  /* IP_QOS_0为IP帧中优先级到qos的查找表0。 */
#define PCU_IP_QOS_1_REG             0x504  /* IP_QOS_1为IP帧中优先级到qos的查找表11。 */
#define PCU_IP_QOS_2_REG             0x508  /* IP_QOS_2为IP帧中优先级到qos的查找表22。 */
#define PCU_IP_QOS_3_REG             0x50C  /* IP_QOS_3为IP帧中优先级到qos的查找表3。 */
#define PCU_IP_QOS_4_REG             0x510  /* IP_QOS_4为IP帧中优先级到qos的查找表4。 */
#define PCU_IP_QOS_5_REG             0x514  /* IP_QOS_5为IP帧中优先级到qos的查找表55。 */
#define PCU_IP_QOS_6_REG             0x518  /* IP_QOS_6为IP帧中优先级到qos的查找表66。 */
#define PCU_IP_QOS_7_REG             0x51C  /* IP_QOS_6为IP帧中优先级到qos的查找表7。 */
#define PCU_GRP_MODE_0_REG           0x520  /* GRP_MODE为group信息的生成模式寄存器0。 */
#define PCU_GRP_MODE_1_REG           0x524  /* GRP_MODE为group信息的生成模式寄存器1。 */
#define PCU_TT_MODE_REG              0x528  /* TT_MODE为type信息的生成模式。 */
#define PCU_TAG_MODE_REG             0x52C  /* TAG_MODE为tag信息的生成模式。 */
#define PCU_TAG_MASK_0_REG           0x530  /* TAG_MASK_0为参与tag计算的报文头部字节掩码表0。 */
#define PCU_TAG_MASK_1_REG           0x534  /* TAG_MASK_0为参与tag计算的报文头部字节掩码表1。 */
#define PCU_MATCH_MASK_0_REG         0x538  /* MATCH_MASK_0为参与匹配的报文头部字节掩码表0。 */
#define PCU_MATCH_TBL_0_REG          0x540  /* MATCH_TBL_0为进行报头匹配的字节表0。 */
#define PCU_MATCH_TBL_1_REG          0x544  /* MATCH_TBL_1为进行报头匹配的字节表1。 */
#define PCU_MATCH_TBL_2_REG          0x548  /* MATCH_TBL_2为进行报头匹配的字节表2。 */
#define PCU_MATCH_TBL_3_REG          0x54C  /* MATCH_TBL_3为进行报头匹配的字节表3。 */
#define PCU_MATCH_TBL_4_REG          0x550  /* MATCH_TBL_4为进行报头匹配的字节表4。 */
#define PCU_MATCH_TBL_5_REG          0x554  /* MATCH_TBL_5为进行报头匹配的字节表5。 */
#define PCU_MATCH_TBL_6_REG          0x558  /* MATCH_TBL_6为进行报头匹配的字节表6。 */
#define PCU_MATCH_TBL_7_REG          0x55C  /* MATCH_TBL_7为进行报头匹配的字节表7。 */
#define PCU_RXBUF_REQ_TIMER_REG      0x580  /* RXBUF_REQ_TIMER为接收缓存申请超时寄存器。 */
#define PCU_RX_WE_TIMER_REG          0x584  /* RX_PD_TIMER为接收写work entry失败后的超时寄存器。 */
#define PCU_TX_REL_TIMER_REG         0x588  /* TX_REL_TIMER为发送完成释放缓存失败后的超时寄存器。 */
#define PCU_RX_BUFRQ_ERR_CNT_REG     0x58C  /* RX_BUFRQ_ERR_CNT为接收方向申请缓存错误计数器。 */
#define PCU_TX_BUFRL_ERR_CNT_REG     0x590  /* TX_BUFRQ_ERR_CNT为发送方向释放缓存错误计数器。 */
#define PCU_RX_WE_ERR_CNT_REG        0x594  /* RX_WE_ERR_CNT为接收方向写工作描述符错误计数器。 */
#define PCU_RX_GRP_TBL_0_REG         0x598  /* RX_GRP_TBL_0为group匹配表项0 */
#define PCU_RX_GRP_TBL_1_REG         0x59C  /* RX_GRP_TBL_1为group匹配表项1 */
#define PCU_RX_GRP_TBL_2_REG         0x5A0  /* RX_GRP_TBL_2为group匹配表项2 */
#define PCU_RX_GRP_TBL_3_REG         0x5A4  /* RX_GRP_TBL_3为group匹配表项3 */
#define PCU_RX_GRP_TBL_4_REG         0x5A8  /* RX_GRP_TBL_4为group匹配表项4 */
#define PCU_RX_GRP_TBL_5_REG         0x5AC  /* RX_GRP_TBL_5为group匹配表项5 */
#define PCU_RX_GRP_TBL_6_REG         0x5B0  /* RX_GRP_TBL_6为group匹配表项6 */
#define PCU_RX_GRP_TBL_7_REG         0x5B4  /* RX_GRP_TBL_7为group匹配表项7 */
#define PCU_RX_GRP_RES_REG           0x5B8  /* RX_GRP_RES为匹配结果配置寄存器 */
#define PCU_RX_GRP_MSK_REG           0x5BC  /* RX_GRP_MSK为字节匹配模式下group匹配掩码寄存器 */
#define PCU_ADDR_FILT_S0_REG         0x5C0  /* ADDR_FILT_S0为地址过滤范围0的起始地址 */
#define PCU_ADDR_FILT_E0_REG         0x5C4  /* ADDR_FILT_S0为地址过滤范围0的结束地址 */
#define PCU_ADDR_FILT_S1_REG         0x5C8  /* ADDR_FILT_S1为地址过滤范围0的起始地址 */
#define PCU_ADDR_FILT_E1_REG         0x5CC  /* ADDR_FILT_S1为地址过滤范围0的结束地址 */
#define PCU_ADDR_FILT_S2_REG         0x5D0  /* ADDR_FILT_S0为地址过滤范围0的起始地址 */
#define PCU_ADDR_FILT_E2_REG         0x5D4  /* ADDR_FILT_S0为地址过滤范围0的结束地址 */
#define PCU_ADDR_FILT_S3_REG         0x5D8  /* ADDR_FILT_S1为地址过滤范围0的起始地址 */
#define PCU_ADDR_FILT_E3_REG         0x5DC  /* ADDR_FILT_S1为地址过滤范围0的结束地址 */

#endif
