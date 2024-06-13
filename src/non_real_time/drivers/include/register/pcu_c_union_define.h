// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __PCU_C_UNION_DEFINE_H__
#define __PCU_C_UNION_DEFINE_H__

/* Define the union u_cf_tx_fifo_thrsld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_tx_p_empty_th : 10; /* [9..0]  */
        unsigned int reserved_0 : 6;        /* [15..10]  */
        unsigned int cf_tx_p_full_th : 10;  /* [25..16]  */
        unsigned int reserved_1 : 6;        /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_tx_fifo_thrsld;

/* Define the union u_cf_rx_fifo_thrsld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_rx_p_empty_th : 10; /* [9..0]  */
        unsigned int reserved_0 : 6;        /* [15..10]  */
        unsigned int cf_rx_p_full_th : 10;  /* [25..16]  */
        unsigned int reserved_1 : 6;        /* [31..26]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_rx_fifo_thrsld;

/* Define the union u_cf_cfg_fifo_thrsld */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cfg_rx_p_empty_th : 8; /* [7..0]  */
        unsigned int cfg_rx_p_full_th : 8;  /* [15..8]  */
        unsigned int cfg_tx_p_empty_th : 8; /* [23..16]  */
        unsigned int cfg_tx_p_full_th : 8;  /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_cfg_fifo_thrsld;

/* Define the union u_cf_intrpt_msk */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_intrpt_time : 6;             /* [5..0]  */
        unsigned int cf_intrpt_pkt : 6;              /* [11..6]  */
        unsigned int reserved_0 : 8;                 /* [19..12]  */
        unsigned int cf_sram_parity_err_int_msk : 1; /* [20]  */
        unsigned int cf_tx_ahb_err_int_msk : 1;      /* [21]  */
        unsigned int cf_buf_avl_msk : 1;             /* [22]  */
        unsigned int cf_rel_buf_msk : 1;             /* [23]  */
        unsigned int cf_txcfg_avl_msk : 1;           /* [24]  */
        unsigned int cf_tx_drop_int_msk : 1;         /* [25]  */
        unsigned int cf_rx_drop_int_msk : 1;         /* [26]  */
        unsigned int cf_pkg_intrpt_msk : 1;          /* [27]  */
        unsigned int cf_rx_ahb_err_int_msk : 1;      /* [28]  */
        unsigned int cf_mac_ff_err_int_msk : 1;      /* [29]  */
        unsigned int cf_rbreq_err_int_msk : 1;       /* [30]  */
        unsigned int cf_we_err_int_msk : 1;          /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_intrpt_msk;

/* Define the union u_cf_intrpt_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;          /* [19..0]  */
        unsigned int stat_sram_parity_err : 1; /* [20]  */
        unsigned int stat_tx_ahb_err : 1;      /* [21]  */
        unsigned int stat_buf_avl : 1;         /* [22]  */
        unsigned int stat_rel_err : 1;         /* [23]  */
        unsigned int stat_txcfg_avl : 1;       /* [24]  */
        unsigned int stat_tx_drop : 1;         /* [25]  */
        unsigned int stat_rx_drop : 1;         /* [26]  */
        unsigned int stat_rx_frm : 1;          /* [27]  */
        unsigned int stat_rx_ahb_err : 1;      /* [28]  */
        unsigned int stat_mac_fifo_err : 1;    /* [29]  */
        unsigned int stat_rbreq_err : 1;       /* [30]  */
        unsigned int stat_we_err : 1;          /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_intrpt_stat;

/* Define the union u_cf_intrpt_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;            /* [19..0]  */
        unsigned int cf_sram_parity_err_clr : 1; /* [20]  */
        unsigned int cf_tx_ahb_err_clr : 1;      /* [21]  */
        unsigned int cf_buf_avl_clr : 1;         /* [22]  */
        unsigned int cf_rel_buf_clr : 1;         /* [23]  */
        unsigned int cf_txcfg_avl_clr : 1;       /* [24]  */
        unsigned int cf_tx_drop_clr : 1;         /* [25]  */
        unsigned int cf_rx_drop_clr : 1;         /* [26]  */
        unsigned int cf_rx_frm_clr : 1;          /* [27]  */
        unsigned int cf_rx_ahb_err_clr : 1;      /* [28]  */
        unsigned int cf_mac_fifo_err_clr : 1;    /* [29]  */
        unsigned int cf_rbreq_err_clr : 1;       /* [30]  */
        unsigned int cf_we_err_clr : 1;          /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_intrpt_clr;

/* Define the union u_max_frame_len */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int max_frame_len : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;    /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_max_frame_len;

/* Define the union u_debug_st_mch */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tx_st_mch : 5;   /* [4..0]  */
        unsigned int rx_st_mch : 4;   /* [8..5]  */
        unsigned int reserved_0 : 23; /* [31..9]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_debug_st_mch;

/* Define the union u_fifo_curr_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tff_cf_full : 1;       /* [0]  */
        unsigned int tff_cf_p_full : 1;     /* [1]  */
        unsigned int tff_cf_empty : 1;      /* [2]  */
        unsigned int tff_cf_p_empty : 1;    /* [3]  */
        unsigned int rff_cf_full : 1;       /* [4]  */
        unsigned int rff_cf_p_full : 1;     /* [5]  */
        unsigned int rff_cf_empty : 1;      /* [6]  */
        unsigned int rff_cf_p_empty : 1;    /* [7]  */
        unsigned int cff_cf_rx_empty : 1;   /* [8]  */
        unsigned int cff_cf_rx_p_empty : 1; /* [9]  */
        unsigned int cff_cf_rx_full : 1;    /* [10]  */
        unsigned int cff_cf_rx_p_full : 1;  /* [11]  */
        unsigned int cff_cf_tx_empty : 1;   /* [12]  */
        unsigned int cff_cf_tx_p_empty : 1; /* [13]  */
        unsigned int cff_cf_tx_full : 1;    /* [14]  */
        unsigned int cff_cf_tx_p_full : 1;  /* [15]  */
        unsigned int reserved_0 : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_fifo_curr_status;

/* Define the union u_fifo_his_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tff_cf_full_h : 1;       /* [0]  */
        unsigned int tff_cf_p_full_h : 1;     /* [1]  */
        unsigned int tff_cf_empty_h : 1;      /* [2]  */
        unsigned int tff_cf_p_empty_h : 1;    /* [3]  */
        unsigned int rff_cf_full_h : 1;       /* [4]  */
        unsigned int rff_cf_p_full_h : 1;     /* [5]  */
        unsigned int rff_cf_empty_h : 1;      /* [6]  */
        unsigned int rff_cf_p_empty_h : 1;    /* [7]  */
        unsigned int cff_cf_rx_empty_h : 1;   /* [8]  */
        unsigned int cff_cf_rx_p_empty_h : 1; /* [9]  */
        unsigned int cff_cf_rx_full_h : 1;    /* [10]  */
        unsigned int cff_cf_rx_p_full_h : 1;  /* [11]  */
        unsigned int cff_cf_tx_empty_h : 1;   /* [12]  */
        unsigned int cff_cf_tx_p_empty_h : 1; /* [13]  */
        unsigned int cff_cf_tx_full_h : 1;    /* [14]  */
        unsigned int cff_cf_tx_p_full_h : 1;  /* [15]  */
        unsigned int reserved_0 : 16;         /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_fifo_his_status;

/* Define the union u_cf_cff_data_num */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_tx_cfg_num : 9; /* [8..0]  */
        unsigned int reserved_0 : 7;    /* [15..9]  */
        unsigned int cf_rx_cfg_num : 9; /* [24..16]  */
        unsigned int reserved_1 : 7;    /* [31..25]  */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_cf_cff_data_num;

/* Define the union u_cf_tx_pause */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 1;       /* [0]  */
        unsigned int cf_tx_pause_auto : 1; /* [1]  */
        unsigned int reserved_1 : 30;      /* [31..2]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_cf_tx_pause;

/* Define the union u_tx_cff_addr_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int l4_cs : 1;       /* [0]  */
        unsigned int cl : 1;          /* [1]  */
        unsigned int l3_cs : 1;       /* [2]  */
        unsigned int reserved_0 : 16; /* [18..3]  */
        unsigned int ds : 6;          /* [24..19]  */
        unsigned int sd : 1;          /* [25]  */
        unsigned int ipoff : 5;       /* [30..26]  */
        unsigned int reserved_1 : 1;  /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_tx_cff_addr_0;

/* Define the union u_tx_cff_addr_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int pool : 4;         /* [3..0]  */
        unsigned int send_len : 16;    /* [19..4]  */
        unsigned int back : 6;         /* [25..20]  */
        unsigned int fm : 1;           /* [26]  */
        unsigned int cache : 1;        /* [27]  */
        unsigned int txmacif_1588 : 2; /* [29..28]  */
        unsigned int back_ind : 1;     /* [30]  */
        unsigned int reserved_0 : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_tx_cff_addr_1;

/* Define the union u_we_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 1;   /* [0]  */
        unsigned int cf_we_enable : 1; /* [1]  */
        unsigned int cf_we_addr : 30;  /* [31..2]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_we_addr;

/* Define the union u_rx_buf_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_rx_buf_size : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;     /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rx_buf_size;

/* Define the union u_bus_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_rx_alloc : 1; /* [0]  */
        unsigned int cf_endian : 2;   /* [2..1]  */
        unsigned int cf_buf : 1;      /* [3]  */
        unsigned int noncoh_en : 1;   /* [4]  */
        unsigned int huser : 1;       /* [5]  */
        unsigned int cf_pd_buf : 1;   /* [6]  */
        unsigned int reserved_0 : 25; /* [31..7]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_bus_ctrl;

/* Define the union u_rx_buf_req_addr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 2;          /* [1..0]  */
        unsigned int cf_rx_buf_req_addr : 30; /* [31..2]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rx_buf_req_addr;

/* Define the union u_rx_ctrl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_rxbuf_1st_skip_size2 : 4;   /* [3..0]  */
        unsigned int cf_cache_line_l : 3;           /* [6..4]  */
        unsigned int cf_rx_cfg_req_en : 1;          /* [7]  */
        unsigned int cf_cache_line_h : 2;           /* [9..8]  */
        unsigned int cf_addr_mode : 2;              /* [11..10]  */
        unsigned int cf_rx_get_addr_mode : 1;       /* [12]  */
        unsigned int cf_port_num : 4;               /* [16..13]  */
        unsigned int cf_rx_align_num : 2;           /* [18..17]  */
        unsigned int cf_pool_num : 4;               /* [22..19]  */
        unsigned int cf_time_inf_en : 1;            /* [23]  */
        unsigned int cf_rxbuf_no_1st_skip_size : 4; /* [27..24]  */
        unsigned int cf_rxbuf_1st_skip_size : 4;    /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rx_ctrl;

/* Define the union u_rx_pkt_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int cf_gen_id : 8;          /* [7..0]  */
        unsigned int reserved_0 : 4;         /* [11..8]  */
        unsigned int cf_match_offset : 9;    /* [20..12]  */
        unsigned int cf_parse_mode : 2;      /* [22..21]  */
        unsigned int cf_skip_len : 7;        /* [29..23]  */
        unsigned int reserved_1 : 1;         /* [30]  */
        unsigned int cf_instr_head_mode : 1; /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rx_pkt_mode;

/* Define the union u_qos_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int oth_qos : 3;  /* [2..0]  */
        unsigned int ip_qos : 3;   /* [5..3]  */
        unsigned int udp_qos : 3;  /* [8..6]  */
        unsigned int tcp_qos : 3;  /* [11..9]  */
        unsigned int icmp_qos : 3; /* [14..12]  */
        unsigned int brc_qos : 3;  /* [17..15]  */
        unsigned int mul_qos : 3;  /* [20..18]  */
        unsigned int rarp_qos : 3; /* [23..21]  */
        unsigned int arp_qos : 3;  /* [26..24]  */
        unsigned int port_qos : 3; /* [29..27]  */
        unsigned int qos_mode : 2; /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_qos_mode;

/* Define the union u_vlan_qos */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int vlan_0_qos : 3; /* [2..0]  */
        unsigned int vlan_1_qos : 3; /* [5..3]  */
        unsigned int vlan_2_qos : 3; /* [8..6]  */
        unsigned int vlan_3_qos : 3; /* [11..9]  */
        unsigned int vlan_4_qos : 3; /* [14..12]  */
        unsigned int vlan_5_qos : 3; /* [17..15]  */
        unsigned int vlan_6_qos : 3; /* [20..18]  */
        unsigned int vlan_7_qos : 3; /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_vlan_qos;

/* Define the union u_ip_qos_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_0_qos : 3;   /* [2..0]  */
        unsigned int ip_1_qos : 3;   /* [5..3]  */
        unsigned int ip_2_qos : 3;   /* [8..6]  */
        unsigned int ip_3_qos : 3;   /* [11..9]  */
        unsigned int ip_4_qos : 3;   /* [14..12]  */
        unsigned int ip_5_qos : 3;   /* [17..15]  */
        unsigned int ip_6_qos : 3;   /* [20..18]  */
        unsigned int ip_7_qos : 3;   /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_0;

/* Define the union u_ip_qos_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_8_qos : 3;   /* [2..0]  */
        unsigned int ip_9_qos : 3;   /* [5..3]  */
        unsigned int ip_10_qos : 3;  /* [8..6]  */
        unsigned int ip_11_qos : 3;  /* [11..9]  */
        unsigned int ip_12_qos : 3;  /* [14..12]  */
        unsigned int ip_13_qos : 3;  /* [17..15]  */
        unsigned int ip_14_qos : 3;  /* [20..18]  */
        unsigned int ip_15_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_1;

/* Define the union u_ip_qos_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_16_qos : 3;  /* [2..0]  */
        unsigned int ip_17_qos : 3;  /* [5..3]  */
        unsigned int ip_18_qos : 3;  /* [8..6]  */
        unsigned int ip_19_qos : 3;  /* [11..9]  */
        unsigned int ip_20_qos : 3;  /* [14..12]  */
        unsigned int ip_21_qos : 3;  /* [17..15]  */
        unsigned int ip_22_qos : 3;  /* [20..18]  */
        unsigned int ip_23_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_2;

/* Define the union u_ip_qos_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_24_qos : 3;  /* [2..0]  */
        unsigned int ip_25_qos : 3;  /* [5..3]  */
        unsigned int ip_26_qos : 3;  /* [8..6]  */
        unsigned int ip_27_qos : 3;  /* [11..9]  */
        unsigned int ip_28_qos : 3;  /* [14..12]  */
        unsigned int ip_29_qos : 3;  /* [17..15]  */
        unsigned int ip_30_qos : 3;  /* [20..18]  */
        unsigned int ip_31_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_3;

/* Define the union u_ip_qos_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_32_qos : 3;  /* [2..0]  */
        unsigned int ip_33_qos : 3;  /* [5..3]  */
        unsigned int ip_34_qos : 3;  /* [8..6]  */
        unsigned int ip_35_qos : 3;  /* [11..9]  */
        unsigned int ip_36_qos : 3;  /* [14..12]  */
        unsigned int ip_37_qos : 3;  /* [17..15]  */
        unsigned int ip_38_qos : 3;  /* [20..18]  */
        unsigned int ip_39_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_4;

/* Define the union u_ip_qos_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_40_qos : 3;  /* [2..0]  */
        unsigned int ip_41_qos : 3;  /* [5..3]  */
        unsigned int ip_42_qos : 3;  /* [8..6]  */
        unsigned int ip_43_qos : 3;  /* [11..9]  */
        unsigned int ip_44_qos : 3;  /* [14..12]  */
        unsigned int ip_45_qos : 3;  /* [17..15]  */
        unsigned int ip_46_qos : 3;  /* [20..18]  */
        unsigned int ip_47_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_5;

/* Define the union u_ip_qos_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_48_qos : 3;  /* [2..0]  */
        unsigned int ip_49_qos : 3;  /* [5..3]  */
        unsigned int ip_50_qos : 3;  /* [8..6]  */
        unsigned int ip_51_qos : 3;  /* [11..9]  */
        unsigned int ip_52_qos : 3;  /* [14..12]  */
        unsigned int ip_53_qos : 3;  /* [17..15]  */
        unsigned int ip_54_qos : 3;  /* [20..18]  */
        unsigned int ip_55_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_6;

/* Define the union u_ip_qos_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ip_56_qos : 3;  /* [2..0]  */
        unsigned int ip_57_qos : 3;  /* [5..3]  */
        unsigned int ip_58_qos : 3;  /* [8..6]  */
        unsigned int ip_59_qos : 3;  /* [11..9]  */
        unsigned int ip_60_qos : 3;  /* [14..12]  */
        unsigned int ip_61_qos : 3;  /* [17..15]  */
        unsigned int ip_62_qos : 3;  /* [20..18]  */
        unsigned int ip_63_qos : 3;  /* [23..21]  */
        unsigned int reserved_0 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_ip_qos_7;

/* Define the union u_grp_mode_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int grp_mode : 3;    /* [2..0]  */
        unsigned int reserved_0 : 4;  /* [6..3]  */
        unsigned int grp_hi_bits : 2; /* [8..7]  */
        unsigned int def_grp_res : 4; /* [12..9]  */
        unsigned int grp_offset : 9;  /* [21..13]  */
        unsigned int arp_grp : 4;     /* [25..22]  */
        unsigned int port_grp : 4;    /* [29..26]  */
        unsigned int reserved_1 : 2;  /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_grp_mode_0;

/* Define the union u_grp_mode_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int oth_grp : 4;  /* [3..0]  */
        unsigned int ip_grp : 4;   /* [7..4]  */
        unsigned int udp_grp : 4;  /* [11..8]  */
        unsigned int tcp_grp : 4;  /* [15..12]  */
        unsigned int icmp_grp : 4; /* [19..16]  */
        unsigned int brc_grp : 4;  /* [23..20]  */
        unsigned int mul_grp : 4;  /* [27..24]  */
        unsigned int rarp_grp : 4; /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_grp_mode_1;

/* Define the union u_tt_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 18;    /* [17..0]  */
        unsigned int def_tt : 2;         /* [19..18]  */
        unsigned int tt_ipv6_no_tcp : 2; /* [21..20]  */
        unsigned int tt_ipv6_tcp : 2;    /* [23..22]  */
        unsigned int tt_ipv4_no_tcp : 2; /* [25..24]  */
        unsigned int tt_ipv4_tcp : 2;    /* [27..26]  */
        unsigned int tt_no_ip : 2;       /* [29..28]  */
        unsigned int tt_mode : 2;        /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_tt_mode;

/* Define the union u_tag_mode */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int def_tag : 16;         /* [15..0]  */
        unsigned int tag_offset : 9;       /* [24..16]  */
        unsigned int tag_msk_ip_prot : 1;  /* [25]  */
        unsigned int tag_msk_ip_dst : 1;   /* [26]  */
        unsigned int tag_msk_ip_src : 1;   /* [27]  */
        unsigned int tag_msk_port_dst : 1; /* [28]  */
        unsigned int tag_msk_port_src : 1; /* [29]  */
        unsigned int tag_port_mode : 1;    /* [30]  */
        unsigned int tag_mode : 1;         /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_tag_mode;

/* Define the union u_match_tbl_0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_0 : 8; /* [7..0]  */
        unsigned int match_byte_1 : 8; /* [15..8]  */
        unsigned int match_byte_2 : 8; /* [23..16]  */
        unsigned int match_byte_3 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_0;

/* Define the union u_match_tbl_1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_4 : 8; /* [7..0]  */
        unsigned int match_byte_5 : 8; /* [15..8]  */
        unsigned int match_byte_6 : 8; /* [23..16]  */
        unsigned int match_byte_7 : 8; /* [31..24]  */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_1;

/* Define the union u_match_tbl_2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_8 : 8;  /* [7..0]  */
        unsigned int match_byte_9 : 8;  /* [15..8]  */
        unsigned int match_byte_10 : 8; /* [23..16]  */
        unsigned int match_byte_11 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_2;

/* Define the union u_match_tbl_3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_12 : 8; /* [7..0]  */
        unsigned int match_byte_13 : 8; /* [15..8]  */
        unsigned int match_byte_14 : 8; /* [23..16]  */
        unsigned int match_byte_15 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_3;

/* Define the union u_match_tbl_4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_16 : 8; /* [7..0]  */
        unsigned int match_byte_17 : 8; /* [15..8]  */
        unsigned int match_byte_18 : 8; /* [23..16]  */
        unsigned int match_byte_19 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_4;

/* Define the union u_match_tbl_5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_20 : 8; /* [7..0]  */
        unsigned int match_byte_21 : 8; /* [15..8]  */
        unsigned int match_byte_22 : 8; /* [23..16]  */
        unsigned int match_byte_23 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_5;

/* Define the union u_match_tbl_6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_24 : 8; /* [7..0]  */
        unsigned int match_byte_25 : 8; /* [15..8]  */
        unsigned int match_byte_26 : 8; /* [23..16]  */
        unsigned int match_byte_27 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_6;

/* Define the union u_match_tbl_7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int match_byte_28 : 8; /* [7..0]  */
        unsigned int match_byte_29 : 8; /* [15..8]  */
        unsigned int match_byte_30 : 8; /* [23..16]  */
        unsigned int match_byte_31 : 8; /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_match_tbl_7;

/* Define the union u_rxbuf_req_timer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rxbuf_req_timer : 16;      /* [15..0]  */
        unsigned int rxbuf_req_inter_timer : 4; /* [19..16]  */
        unsigned int rxbuf_req_cnt : 4;         /* [23..20]  */
        unsigned int reserved_0 : 8;            /* [31..24]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rxbuf_req_timer;

/* Define the union u_rx_we_timer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rx_we_timer : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;  /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rx_we_timer;

/* Define the union u_tx_rel_timer */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tx_rel_timer : 16; /* [15..0]  */
        unsigned int reserved_0 : 16;   /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_tx_rel_timer;

/* Define the union u_rx_grp_res */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rx_grp_res_7 : 4; /* [3..0]  */
        unsigned int rx_grp_res_6 : 4; /* [7..4]  */
        unsigned int rx_grp_res_5 : 4; /* [11..8]  */
        unsigned int rx_grp_res_4 : 4; /* [15..12]  */
        unsigned int rx_grp_res_3 : 4; /* [19..16]  */
        unsigned int rx_grp_res_2 : 4; /* [23..20]  */
        unsigned int rx_grp_res_1 : 4; /* [27..24]  */
        unsigned int rx_grp_res_0 : 4; /* [31..28]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_rx_grp_res;

/* Define the union u_addr_filt_s0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_s0 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_s0;

/* Define the union u_addr_filt_e0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_e0 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_e0;

/* Define the union u_addr_filt_s1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_s1 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_s1;

/* Define the union u_addr_filt_e1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_e1 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_e1;

/* Define the union u_addr_filt_s2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_s2 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_s2;

/* Define the union u_addr_filt_e2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_e2 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_e2;

/* Define the union u_addr_filt_s3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_s3 : 12; /* [31..20]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_s3;

/* Define the union u_addr_filt_e3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 20;   /* [19..0]  */
        unsigned int addr_filt_e3 : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} u_addr_filt_e3;

/* Define the union u_idle_cnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int idle_cnt : 16;   /* [15..0]  */
        unsigned int reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_idle_cnt;

/* Define the union u_dbg_st0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tx_bus_incr_num : 5; /* [4..0]  */
        unsigned int tx_bus_size : 3;     /* [7..5]  */
        unsigned int tx_bus_prot : 4;     /* [11..8]  */
        unsigned int tx_bus_burst : 3;    /* [14..12]  */
        unsigned int rx_bus_incr_num : 5; /* [19..15]  */
        unsigned int rx_bus_size : 3;     /* [22..20]  */
        unsigned int rx_bus_prot : 4;     /* [26..23]  */
        unsigned int rx_bus_burst : 3;    /* [29..27]  */
        unsigned int hready_m1_dbg : 1;   /* [30]  */
        unsigned int hready_m0_dbg : 1;   /* [31]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_dbg_st0;

/* Define the union u_dbg_st1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txff_frame_cnt : 10; /* [9..0]  */
        unsigned int tx_fifo_cnt : 10;    /* [19..10]  */
        unsigned int trans_cnt : 3;       /* [22..20]  */
        unsigned int reserved_0 : 9;      /* [31..23]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_dbg_st1;

/* Define the union u_dbg_st2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int frame_cnt : 11;  /* [10..0]  */
        unsigned int fifo_cnt : 11;   /* [21..11]  */
        unsigned int reserved_0 : 10; /* [31..22]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_dbg_st2;

/* Define the union u_bus_rst_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int bus_rst_en : 1;  /* [0]  */
        unsigned int reserved_0 : 31; /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    unsigned int val;
} u_bus_rst_en;

#endif /* __PCU_C_UNION_DEFINE_H__ */