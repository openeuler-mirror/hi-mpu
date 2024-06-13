/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
* Description: none
* Create: 2018-08-01
* Notes: none
*/

#ifndef __HI309x_FEC_H_
#define __HI309x_FEC_H_

#include "hi309x_mdio.h"

#define CHIP_RX_FIFO_NUM 2
#define CHIP_TX_FIFO_NUM 2

#define MAC_ADDR_LEN    6
#define RX_DESC_LEN     64

#define GMAC_DESC_PKT_LEN_HIGH 8
#define GMAC_DESC_PKT_LEN_LOW 9

#define GMAC_SRST_BIT 0x7
#define GMAC_MAC_CORE_SRST_BIT 0x2
#define GMAC_REG_CFG_SRST_BIT 0x3
#define GMAC_BUS_SYS_SRST_BIT 0x4
#define GMAC_RX_SERDES_SRST_BIT 0x5
#define GMAC_TX_SERDES_SRST_BIT 0x6

#define MPLL_LOCK_TIMES         25   /* 25us */
#define TX_CM_TIMES             230  /* 230us */
#define TX_RX_ENABLE_TIMES      4    /* 4us */

typedef struct {
    char *name;
    u32 phy_id;
    s32 (*init)(u32 phy_addr);
    s32 (*get_speed)(u32 phy_addr, u32 *speed);
} gmac_phy_info_s;


struct hi309x_gmac_platdata {
    u8 rsvd[4];
};

struct hi309x_gmac_priv {
    u32 mac_num;
    u32 phy_addr;
    u8 *rx_data_ring[CHIP_RX_FIFO_NUM];
    u8 *tx_data_ring[CHIP_TX_FIFO_NUM];
    u8 rx_data_num;
    u8 tx_data_num;
    gmac_phy_info_s *gmac_phy_info;
};

/* Define the union u_transmit_control */
typedef union {
    /* Define the struct bits */
    struct {
        u32 Reserved_1 : 5;  /* [4..0] */
        u32 an_enable : 1;   /* [5] */
        u32 crc_add : 1;     /* [6] */
        u32 pad_enable : 1;  /* [7] */
        u32 Reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_transmit_control;

/* 接收报文存储格式控制参数 */
typedef struct tagFemacPktStoreCtrlCfg {
    u32 cf_buf_1st_skip1;        /* 第一个接收缓存首部报文描述符之前保留的cache line个数 */
    u32 cf_buf_n1st_skip;        /* 非第一个接收缓存首部保留的cache line个数 */
    u32 cf_rx_align_num;         /* 接收缓存中数据报文头部对齐的空白字节     */
    u32 cf_cache_line;           /* 数据包首部可cache的cache line个数        */
    u32 cf_rxbuf_1st_skip_size2; /* 第一个接收缓存首部报文描述符之后保留的cache line个数 */
} FEMAC_PKT_STORE_CTRL_CFG_S;

/* 输入报文格式参数 */
typedef struct tagFemacRxPktModeCfg {
    u32 cf_instr_head_mode; /* 接收报文为指令头模式         */
    u32 cf_skip_len;        /* 输入报文前skip区域的长度     */
    u32 cf_parse_mode;      /* 非指令头模式下的解析模式,指令头模式下无效 */
    u32 mac_skip_len;       /* 报文头不进行解析字段长度     */
} FEMAC_RX_PKT_MODE_CFG_S;

typedef enum tagFemacPortMode {
    MAC_10M_RMII = 0,
    MAC_100M_RMII,
    MAC_RESERVED_MODE,
    MAC_10M_SGMII = 6,
    MAC_100M_SGMII,
    MAC_1000M_SGMII,
    MAC_UNKNOW_MODE,
} FEMAC_PORT_MODE_E;

/* Duplex, half or full. */
#define GMAC_DUPLEX_HALF 0x00
#define GMAC_DUPLEX_FULL 0x01

#define REG_1512_SPEED_DUPLEX 17
#define MASK_1512_SPEED 0xc000
#define MASK_1512_DUPLEX 0x2000

typedef struct tagFemacPortModeCfg {
    FEMAC_PORT_MODE_E port_mode; /* 端口模式              */
    u32 port_num;                /* 端口号                */
    u32 max_frm_size;            /* 接收报文的最大帧长    */
    u32 short_runts_thr;         /* 接收报文的最小帧长    */
    u32 pad_enable;              /* 发送报文小于最小帧长时自动添加pad使能 */
    u32 crc_add;                 /* 自动添加CRC使能       */
    u32 an_enable;               /* 自协商使能            */
    u32 runt_pkt_en;             /* 接收超短帧透传使能    */
    u32 strip_pad_en;            /* 接收帧自动剥离pad使能 */
} FEMAC_PORT_MODE_CFG_S;


/* MAC过滤设置结构体 */
typedef struct tagFemacMacFilterCfg {
    u32 crc_err_pass : 1;   /* CRC 错误报文接收使能 */
    u32 pause_frm_pass : 1; /* 控制帧接收使能 */
    u32 bc_drop_en : 1;     /* 广播帧丢弃使能 */
    u32 mc_match_en : 1;    /* 非本地组播帧丢弃使能 */
    u32 uc_match_en : 1;    /* 非本地单播帧丢弃使能 */
} FEMAC_MAC_FILTER_CFG_S;

/* Define the union u_rx_ctrl */
typedef union tagRxCtrl {
    /* Define the struct bits */
    struct {
        u32 cf_rxbuf_1st_skip_size2 : 4;   /* [3..0] */
        u32 cf_cache_line : 3;             /* [6..4] */
        u32 cf_rx_cfg_req_en : 1;          /* [7] */
        u32 cf_cache_line_h : 2;           /* [9..8] */
        u32 cf_addr_mode : 2;              /* [11..10] */
        u32 cf_rx_get_addr_mode : 1;       /* [12] */
        u32 cf_port_num : 4;               /* [16..13] */
        u32 cf_rx_align_num : 2;           /* [18..17] */
        u32 cf_pool_num : 4;               /* [22..19] */
        u32 cf_time_inf_en : 1;            /* [23] */
        u32 cf_rxbuf_no_1st_skip_size : 4; /* [27..24] */
        u32 cf_rxbuf_1st_skip_size : 4;    /* [31..28] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_rx_ctrl;

/* Define the union u_mac_skip_len */
typedef union {
    /* Define the struct bits */
    struct {
        u32 mac_skip_len : 6; /* [5..0] */
        u32 mac_skip_crc : 1; /* [6] */
        u32 Reserved_0 : 25;  /* [31..7] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_mac_skip_len;

/* Define the union u_max_frm_size */
typedef union {
    /* Define the struct bits */
    struct {
        u32 max_frm_size : 16; /* [13..0] */
        u32 Reserved_0 : 16;   /* [31..14] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_max_frm_size;

/* Define the union u_short_runts_thr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 short_runts_thr : 5; /* [4..0] */
        u32 Reserved_0 : 27;     /* [31..5] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_short_runts_thr;

/* Define the union u_recv_control */
typedef union {
    /* Define the struct bits */
    struct {
        u32 Reserved_1 : 3;   /* [2..0] */
        u32 strip_pad_en : 1; /* [3] */
        u32 runt_pkt_en : 1;  /* [4] */
        u32 Reserved_0 : 27;  /* [31..5] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_recv_control;

/* Define the union u_port_mode */
typedef union {
    /* Define the struct bits */
    struct {
        u32 port_mode : 4;         /* [3..0]  */
        u32 rgmii_1000m_delay : 1; /* [4]  */
        u32 reserved_0 : 1;        /* [5]  */
        u32 fifo_err_auto_rst : 1; /* [6]  */
        u32 dbg_clk_los_msk : 1;   /* [7]  */
        u32 rmii_clk_edge : 1;     /* [8]  */
        u32 rmii_rst : 1;          /* [9]  */
        u32 reserved_1 : 22;       /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_port_mode;

/* Define the union u_bus_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_rx_alloc : 1; /* [0] */
        u32 cf_endian : 2;   /* [2..1] */
        u32 cf_buf : 1;      /* [3] */
        u32 noncoh_en : 1;   /* [4] */
        u32 huser : 1;       /* [5] */
        u32 cf_pd_buf : 1;   /* [6] */
        u32 Reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_bus_ctrl;

/* Define the union u_mode_change_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 mode_change_en : 1; /* [0] */
        u32 Reserved_0 : 31;    /* [31..1] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_mode_change_en;

/* Define the union u_rec_filt_control */
typedef union tagRecFiltControl {
    /* Define the struct bits */
    struct {
        u32 uc_match_en : 1;    /* [0] */
        u32 mc_match_en : 1;    /* [1] */
        u32 bc_drop_en : 1;     /* [2] */
        u32 reserved_0 : 1;     /* [3] */
        u32 pause_frm_pass : 1; /* [4] */
        u32 crc_err_pass : 1;   /* [5] */
        u32 Reserved_0 : 26;    /* [31..6] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_rec_filt_control;

/* Define the union u_rx_max_frame_len */
typedef union {
    /* Define the struct bits */
    struct {
        u32 max_frame_len : 16; /* [15..0] */
        u32 Reserved_0 : 16;    /* [31..16] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_rx_max_frame_len;

/* Define the union u_cf_intrpt_stat */
typedef union {
    /* Define the struct bits */
    struct {
        u32 Reserved_1 : 20;          /* [19..0] */
        u32 stat_sram_parity_err : 1; /* [20] */
        u32 stat_tx_ahb_err : 1;      /* [21] */
        u32 stat_buf_avl : 1;         /* [22] */
        u32 stat_rel_err : 1;         /* [23] */
        u32 stat_txcfg_avl : 1;       /* [24] */
        u32 stat_tx_drop : 1;         /* [25] */
        u32 stat_rx_drop : 1;         /* [26] */
        u32 stat_rx_frm : 1;          /* [27] */
        u32 stat_ahb_err : 1;         /* [28] */
        u32 stat_mac_fifo_err : 1;    /* [29] */
        u32 stat_rbreq_err : 1;       /* [30] */
        u32 stat_we_err : 1;          /* [31] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_cf_intrpt_stat;

/* Define the union u_cf_intrpt_msk */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_intrpt_time : 6;             /* [5..0] */
        u32 cf_intrpt_pkt : 6;              /* [11..6] */
        u32 Reserved_1 : 8;                 /* [19..12] */
        u32 cf_sram_parity_err_int_msk : 1; /* [20] */
        u32 cf_tx_ahb_err_int_msk : 1;      /* [21] */
        u32 cf_buf_avl_msk : 1;             /* [22] */
        u32 cf_rel_buf_msk : 1;             /* [23] */
        u32 cf_txcfg_avl_msk : 1;           /* [24] */
        u32 cf_tx_drop_int_msk : 1;         /* [25] */
        u32 cf_rx_drop_int_msk : 1;         /* [26] */
        u32 cf_pkg_intrpt_msk : 1;          /* [27] */
        u32 cf_ahb_err_int_msk : 1;         /* [28] */
        u32 cf_mac_ff_err_int_msk : 1;      /* [29] */
        u32 cf_rbreq_err_int_msk : 1;       /* [30] */
        u32 cf_we_err_int_msk : 1;          /* [31] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_cf_intrpt_msk;

/* Define the union DUPLEX TYPE */
typedef union {
    /* Define the struct bits */
    struct {
        u32 duplex_sel : 1;  /* [0] */
        u32 Reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_duplex_type;

/* Define the union u_port_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 Reserved_1 : 1;  /* [0] */
        u32 rx_en : 1;       /* [1] */
        u32 tx_en : 1;       /* [2] */
        u32 Reserved_0 : 29; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_port_en;

typedef struct {
    union {
        struct {
            u32 l4_cs : 1;
            u32 wb : 1;    /* Wb为1时，当报文发送完成，向WORD3地址写0 */
            u32 l3_cs : 1; /* 需要做Checksum */
            u32 reserved2 : 16;
            u32 ds : 6; /* 数据段个数，当Sd==1时有效 */
            u32 sd : 1; /* 为0时，帧只包含一个数据段，直接指向数据段；为1时， */
            /* 帧包含多个数据段，指向数据段列表 */
            u32 ipoff : 5; /* 用来指明IP报文头在数据报中的起始偏移 */
            u32 reserved1 : 1;
        } bits;
        u32 u_32;
    } word0;

    union {
        struct {
            u32 pool : 4;    /* 释放的pool ID */
            u32 len : 16;    /* 发送长度 */
            u32 back : 6;    /* 数据所在cache line首地址与缓存首地址之间距离 */
            u32 fm : 1;      /* 发送完成释放缓存空间 */
            u32 l2cache : 1; /* 读分配L2 cache */
            u32 reserve : 4;
        } bits;
        u32 u_32;
    } word1;

    u32 ulPktAddr;   /* packet地址 */
    u32 ulClearAddr; /* 发包完成后该地址清0 */
} u_tx_pcu_cmd;

/* Define the union u_rx_pkt_mode */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_gen_id : 8;          /* [7..0] */
        u32 Reserved_1 : 4;         /* [11..8] */
        u32 cf_match_offset : 9;    /* [20..12] */
        u32 cf_parse_mode : 2;      /* [22..21] */
        u32 cf_skip_len : 7;        /* [29..23] */
        u32 Reserved_0 : 1;         /* [30] */
        u32 cf_instr_head_mode : 1; /* [31] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_rx_pkt_mode;

/* Define the union u_we_addr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 Reserved_0 : 1;   /* [0] */
        u32 cf_we_enable : 1; /* [1] */
        u32 cf_we_addr : 30;  /* [31..2] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_we_addr;

/* Define the union CF_CFG_FIFO_THRSLD_U */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cfg_rx_p_empty_th : 8; /* [7..0] */
        u32 cfg_rx_p_full_th : 8;  /* [15..8] */
        u32 cfg_tx_p_empty_th : 8; /* [23..16] */
        u32 cfg_tx_p_full_th : 8;  /* [31..24] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_cf_cfg_fifo_thrsld;

typedef union {
    /* Define the struct bits */
    struct {
        u32 Reserved_1 : 20;            /* [19..0] */
        u32 cf_sram_parity_err_clr : 1; /* [20] */
        u32 cf_tx_ahb_err_clr : 1;      /* [21] */
        u32 cf_buf_avl_clr : 1;         /* [22] */
        u32 cf_rel_buf_clr : 1;         /* [23] */
        u32 cf_txcfg_avl_clr : 1;       /* [24] */
        u32 cf_tx_drop_clr : 1;         /* [25] */
        u32 cf_rx_drop_clr : 1;         /* [26] */
        u32 cf_rx_frm_clr : 1;          /* [27] */
        u32 cf_rx_ahb_err_clr : 1;      /* [28] */
        u32 cf_mac_fifo_err_clr : 1;    /* [29] */
        u32 cf_rbreq_err_clr : 1;       /* [30] */
        u32 cf_we_err_clr : 1;          /* [31] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_cf_intrpt_clr;

/* Define the union u_cf_cff_data_num */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_tx_cfg_num : 9; /* [8..0] */
        u32 Reserved_1 : 7;    /* [15..9] */
        u32 cf_rx_cfg_num : 9; /* [24..16] */
        u32 Reserved_0 : 7;    /* [31..25] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_cf_cff_data_num;

/* Define the union u_cf_tx_fifo_thrsld */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_tx_p_empty_th : 10; /* [9..0] */
        u32 Reserved_1 : 6;        /* [15..10] */
        u32 cf_tx_p_full_th : 10;  /* [25..16] */
        u32 Reserved_0 : 6;        /* [31..26] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_cf_tx_fifo_thrsld;

typedef union {
    /* Define the struct bits */
    struct {
        u32 tff_cf_full : 1;       /* [0]  */
        u32 tff_cf_p_full : 1;     /* [1]  */
        u32 tff_cf_empty : 1;      /* [2]  */
        u32 tff_cf_p_empty : 1;    /* [3]  */
        u32 rff_cf_full : 1;       /* [4]  */
        u32 rff_cf_p_full : 1;     /* [5]  */
        u32 rff_cf_empty : 1;      /* [6]  */
        u32 rff_cf_p_empty : 1;    /* [7]  */
        u32 cff_cf_rx_empty : 1;   /* [8]  */
        u32 cff_cf_rx_p_empty : 1; /* [9]  */
        u32 cff_cf_rx_full : 1;    /* [10]  */
        u32 cff_cf_rx_p_full : 1;  /* [11]  */
        u32 cff_cf_tx_empty : 1;   /* [12]  */
        u32 cff_cf_tx_p_empty : 1; /* [13]  */
        u32 cff_cf_tx_full : 1;    /* [14]  */
        u32 cff_cf_tx_p_full : 1;  /* [15]  */
        u32 reserved_0 : 16;       /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_fifo_curr_status;


/* Define the union u_debug_st_mch */
typedef union {
    /* Define the struct bits */
    struct {
        u32 tx_st_mch : 5;   /* [4..0] */
        u32 rx_st_mch : 4;   /* [8..5] */
        u32 Reserved_0 : 23; /* [31..9] */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_debug_st_mch;

/* END:   Added by duzhenquan 00219026, 2012/8/17 */


/* Define the union u_hp_mac_haddr_extend */
typedef union {
    /* Define the struct bits */
    struct {
        u32 m0_haddr_offset : 4;    /* [3..0]  */
        u32 m0_addr_remap_mode : 2; /* [5..4]  */
        u32 m0_haddr_hi : 2;        /* [7..6]  */
        u32 m1_haddr_offset : 4;    /* [11..8]  */
        u32 m1_addr_remap_mode : 2; /* [13..12]  */
        u32 m1_haddr_hi : 2;        /* [15..14]  */
        u32 reserved_0 : 16;        /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_hp_mac_haddr_extend;

/* Define the union u_gmac_phy0_los_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        u32 los_level_0 : 5;            /* [4..0]  */
        u32 los_bias_0 : 3;             /* [7..5]  */
        u32 rx0_los_filt_cnt_0 : 7;     /* [14..8]  */
        u32 tx_vboost_lvl_0 : 3;        /* [17..15]  */
        u32 rx_los_valid_rst_msk_0 : 2; /* [19..18]  */
        u32 reserved_0 : 12;            /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_gmac_phy_los_cfg;

/* Define the union u_gmac_phy0_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        u32 tx0_invert_0 : 1;      /* [0]  */
        u32 lane_10bit_sel_0 : 1;  /* [1]  */
        u32 ref_ssp_en_0 : 1;      /* [2]  */
        u32 tx0_vboost_en_0 : 1;   /* [3]  */
        u32 tx0_reset_0 : 1;       /* [4]  */
        u32 tx0_en_0 : 1;          /* [5]  */
        u32 tx0_cm_en_0 : 1;       /* [6]  */
        u32 rx0_term_en_0 : 1;     /* [7]  */
        u32 rx0_reset_0 : 1;       /* [8]  */
        u32 rx0_pll_en_0 : 1;      /* [9]  */
        u32 rx0_los_en_0 : 1;      /* [10]  */
        u32 rx0_invert_0 : 1;      /* [11]  */
        u32 rx0_data_en_0 : 1;     /* [12]  */
        u32 rx0_align_en_0 : 1;    /* [13]  */
        u32 mpll_en_0 : 1;         /* [14]  */
        u32 lane0_loopbk_en_0 : 1; /* [15]  */
        u32 test_bypass_0 : 1;     /* [16]  */
        u32 test_burnin_0 : 1;     /* [17]  */
        u32 ssc_ref_clk_sel_0 : 1; /* [18]  */
        u32 mpll_multiplier_0 : 7; /* [25..19]  */
        u32 vreg_bypass_0 : 1;     /* [26]  */
        u32 test_powerdown_0 : 1;  /* [27]  */
        u32 ref_use_pad_0 : 1;     /* [28]  */
        u32 ref_clkdiv2_0 : 1;     /* [29]  */
        u32 phy_reset_0 : 1;       /* [30]  */
        u32 phy_rtune_req_0 : 1;   /* [31]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_gmac_phy_cfg;


typedef union {
    /* Define the struct bits */
    struct {
        u32 phy_rx0_eq_0 : 3;          /* [2..0]  */
        u32 phy_tx0_term_offset_0 : 5; /* [7..3]  */
        u32 tx0_amplitude_0 : 7;       /* [14..8]  */
        u32 tx0_detect_rx_req_0 : 1;   /* [15]  */
        u32 tx0_preemph_0 : 7;         /* [22..16]  */
        u32 tx0_data_en_0 : 1;         /* [23]  */
        u32 phy_data_reverse_en0 : 1;  /* [24]  */
        u32 reserved_0 : 7;            /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_gmac_phy_cfg2;

typedef union {
    /* Define the struct bits */
    struct {
        u32 tx0_state_0 : 1;            /* [0]  */
        u32 tx0_detect_rx_result_0 : 1; /* [1]  */
        u32 tx0_detect_rx_ack_0 : 1;    /* [2]  */
        u32 tx0_cm_state_0 : 1;         /* [3]  */
        u32 rx0_pll_state_0 : 1;        /* [4]  */
        u32 mpll_state_0 : 1;           /* [5]  */
        u32 rx_valid_0 : 1;             /* [6]  */
        u32 rx_los_0 : 1;               /* [7]  */
        u32 reserved_0 : 24;            /* [31..8]  */
    } bits;

    /* Define an unsigned member */
    u32 u32;
} u_gmac_phy_sts;


#define ETH0 0
#define ETH1 1
#define ETH2 2
#define ETH3 3

#define ETH0_INITIALIZED (0x1U << ETH0)
#define ETH1_INITIALIZED (0x1U << ETH1)
#define ETH2_INITIALIZED (0x1U << ETH2)
#define ETH3_INITIALIZED (0x1U << ETH3)

#define HI309x_ASIC

#define PHY_RESET (1 << 15)
#define SELECT_8201_RMII_MODE (1 << 3)
#define ENABLE_BROADCAST_FUN (1 << 13)

#define MII_PHY_ID0_REG 0x02 /* MII PHY ID register: r/w */
#define MII_PHY_ID1_REG 0x03 /* MII PHY ID register: r/w */
#define IDENTIFIER_MARVELL1512 0x01410dd1
#define IDENTIFIER_RELTEK8211 0x001cc916
#define IDENTIFIER_RELTEK8201 0x001cc816

#define FE_RX_FIFO_FULL_THRSHLD (100)
#define FE_RX_FIFO_EMPTY_THRSHLD (80)
#define ENABLE (0x1)
#define DISABLE (0x0)

#define CPU_CACHE_LINE_SIZE (64)
#define PCU_CACHE_LINE_SIZE (32)

/* 以CACHE LINE SIZE 为单位 */
#define FE_RX_SKIP1 (0x00)
#define FE_RX_SKIP2 (0x01)

#define CACHE_LINE_SIZE (32)
#define WORD_ALIGN_SIZE (4)
#define RX_BUFFER_DEFAULT_SIZE 0x640 /* >MTU 1518, cacheline align */
#define FE_MAX_PKT_LEN 1536

#define MAC_ADDR_LEN 6
#define FE_MAX_DEVS 4

#define MAC_BASE_ADDR (0xcd00000)
#define MAC_REG_ADDR_LEN 0x1000

#define MAC_REG_ADDR(port, offset) (MAC_BASE_ADDR + (port) * MAC_REG_ADDR_LEN + (offset))

/* general register access macros */
#define REG_WRITE(addr, data) (*(volatile unsigned int *)((unsigned long)(addr)) = (data))
#define REG_READ(addr, data) ((data) = *(volatile unsigned int *)((unsigned long)(addr)))

#define MAC_REG_WRITE(port, offset, data) (REG_WRITE(MAC_REG_ADDR((port), (offset)), (BYTE_SWAP32(data))))
#define MAC_REG_READ(port, offset, data)                    \
    do {                                                    \
        (REG_READ(MAC_REG_ADDR((port), (offset)), (data))); \
        (data) = (BYTE_SWAP32(data));                       \
    } while (0)

#define SYS_CTRL_REG_WRITE(offset, data) (REG_WRITE((offset), (BYTE_SWAP32(data))))

#define SYS_CTRL_REG_READ(offset, data) \
    do {                                \
        (REG_READ((offset), (data)));   \
        (data) = (BYTE_SWAP32(data));   \
    } while (0)

/* hp_subsys寄存器地址 */
#define HP_SUB_CSR_BASE_ADDR 0xcd0c000
#define HP_SUB_CSR_HP_SRST_REQ1_ADDR (HP_SUB_CSR_BASE_ADDR + 0x08)
#define HP_SUB_CSR_HP_SRST_REQ2_ADDR (HP_SUB_CSR_BASE_ADDR + 0x0c)
#define HP_SUB_CSR_HP_SRST_DIS1_ADDR (HP_SUB_CSR_BASE_ADDR + 0x10)
#define HP_SUB_CSR_HP_SRST_DIS2_ADDR (HP_SUB_CSR_BASE_ADDR + 0x14)
#define HP_SUB_CSR_HP_MAC_HADDR_EXTEND_0_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x90)
#define HP_SUB_CSR_GMAC_PHY0_LOS_CFG_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x158)
#define HP_SUB_CSR_GMAC_PHY1_LOS_CFG_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x15c)
#define HP_SUB_CSR_GMAC_PHY0_CFG_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x160)
#define HP_SUB_CSR_GMAC_PHY1_CFG_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x164)
#define HP_SUB_CSR_GMAC_PHY0_CFG2_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x168)
#define HP_SUB_CSR_GMAC_PHY1_CFG2_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x16c)
#define HP_SUB_CSR_GMAC_PHY0_STS_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x170)
#define HP_SUB_CSR_GMAC_PHY1_STS_OFFSET_ADDR (HP_SUB_CSR_BASE_ADDR + 0x174)

/* IO-R RMII复用 */
#define IO_CONFIG_R_BASE_ADDR 0x876e000
#define IO_CONFIG_R_IOMG022_ADDR (IO_CONFIG_R_BASE_ADDR + 0x58)
#define IO_CONFIG_R_IOMG023_ADDR (IO_CONFIG_R_BASE_ADDR + 0x5C)
#define IO_CONFIG_R_IOMG024_ADDR (IO_CONFIG_R_BASE_ADDR + 0x60)
#define IO_CONFIG_R_IOMG025_ADDR (IO_CONFIG_R_BASE_ADDR + 0x64)
#define IO_CONFIG_R_IOMG026_ADDR (IO_CONFIG_R_BASE_ADDR + 0x68)
#define IO_CONFIG_R_IOMG027_ADDR (IO_CONFIG_R_BASE_ADDR + 0x6C)

/* IO-T  MDIO复用 */
#define IO_CONFIG_T_BASE_ADDR 0x8745000
#define IO_CONFIG_T_IOMG033_ADDR (IO_CONFIG_T_BASE_ADDR + 0x84)

s32 gmac_8201phy_init(u32 phy_addr);
s32 gmac_8211phy_init(u32 phy_addr);
s32 gmac_1512phy_init(u32 phy_addr);
s32 phy_8201_get_speed(u32 phy_addr, u32 *speed);
s32 phy_8211_get_speed(u32 phy_addr, u32 *speed);
s32 phy_1512_get_speed(u32 phy_addr, u32 *speed);

#endif
