// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __GMAC_C_UNION_DEFINE_H__
#define __GMAC_C_UNION_DEFINE_H__

/* Define the union u_duplex_type */
typedef union {
    /* Define the struct bits */
    struct {
        u32 duplex_sel : 1;  /* [0]  */
        u32 reserved_0 : 31; /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_duplex_type;

/* Define the union u_fd_fc_type */
typedef union {
    /* Define the struct bits */
    struct {
        u32 fd_fc_type : 16; /* [15..0]  */
        u32 reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_fd_fc_type;

/* Define the union u_fc_tx_timer */
typedef union {
    /* Define the struct bits */
    struct {
        u32 fc_tx_timer : 16; /* [15..0]  */
        u32 reserved_0 : 16;  /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_fc_tx_timer;

/* Define the union u_fd_fc_addr_high */
typedef union {
    /* Define the struct bits */
    struct {
        u32 fd_fc_addr_high : 16; /* [15..0]  */
        u32 reserved_0 : 16;      /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_fd_fc_addr_high;

/* Define the union u_ipg_tx_timer */
typedef union {
    /* Define the struct bits */
    struct {
        u32 ipg_tx_timer : 8; /* [7..0]  */
        u32 reserved_0 : 24;  /* [31..8]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_ipg_tx_timer;

/* Define the union u_pause_thr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 pause_thr : 16;  /* [15..0]  */
        u32 reserved_0 : 16; /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_pause_thr;

/* Define the union u_max_frm_size */
typedef union {
    /* Define the struct bits */
    struct {
        u32 max_frm_size : 16; /* [15..0]  */
        u32 reserved_0 : 16;   /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_max_frm_size;

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
    u32 val;
} u_port_mode;

/* Define the union u_port_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 1;  /* [0]  */
        u32 rx_en : 1;       /* [1]  */
        u32 tx_en : 1;       /* [2]  */
        u32 reserved_1 : 29; /* [31..3]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_port_en;

/* Define the union u_pause_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 rx_fdfc : 1;     /* [0]  */
        u32 tx_fdfc : 1;     /* [1]  */
        u32 tx_hdfc : 1;     /* [2]  */
        u32 reserved_0 : 29; /* [31..3]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_pause_en;

/* Define the union u_short_runts_thr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 short_runts_thr : 5; /* [4..0]  */
        u32 reserved_0 : 27;     /* [31..5]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_short_runts_thr;

/* Define the union u_an_neg_state */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 5;  /* [4..0]  */
        u32 fd : 1;          /* [5]  */
        u32 hd : 1;          /* [6]  */
        u32 ps : 2;          /* [8..7]  */
        u32 reserved_1 : 1;  /* [9]  */
        u32 speed : 2;       /* [11..10]  */
        u32 rf1_duplex : 1;  /* [12]  */
        u32 rf2 : 1;         /* [13]  */
        u32 reserved_2 : 1;  /* [14]  */
        u32 np_link_ok : 1;  /* [15]  */
        u32 reserved_3 : 4;  /* [19..16]  */
        u32 rx_sync_ok : 1;  /* [20]  */
        u32 an_done : 1;     /* [21]  */
        u32 reserved_4 : 10; /* [31..22]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_an_neg_state;

/* Define the union u_tx_local_page */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cfg_0bit : 1;     /* [0]  */
        u32 reserved_0 : 4;   /* [4..1]  */
        u32 cfg_8_5bit : 4;   /* [8..5]  */
        u32 reserved_1 : 3;   /* [11..9]  */
        u32 cfg_13_12bit : 2; /* [13..12]  */
        u32 reserved_2 : 1;   /* [14]  */
        u32 cfg_15bit : 1;    /* [15]  */
        u32 reserved_3 : 16;  /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_tx_local_page;

/* Define the union u_transmit_control */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 5;  /* [4..0]  */
        u32 an_enable : 1;   /* [5]  */
        u32 crc_add : 1;     /* [6]  */
        u32 pad_enable : 1;  /* [7]  */
        u32 reserved_1 : 24; /* [31..8]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_transmit_control;

/* Define the union u_rec_filt_control */
typedef union {
    /* Define the struct bits */
    struct {
        u32 uc_match_en : 1;    /* [0]  */
        u32 mc_match_en : 1;    /* [1]  */
        u32 bc_drop_en : 1;     /* [2]  */
        u32 reserved_0 : 1;     /* [3]  */
        u32 pause_frm_pass : 1; /* [4]  */
        u32 crc_err_pass : 1;   /* [5]  */
        u32 reserved_1 : 26;    /* [31..6]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_rec_filt_control;

/* Define the union u_ptp_config */
typedef union {
    /* Define the struct bits */
    struct {
        u32 corfield_msb_en : 1; /* [0]  */
        u32 chksum_calc_en : 1;  /* [1]  */
        u32 reserved_0 : 30;     /* [31..2]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_ptp_config;

/* Define the union u_led_mod */
typedef union {
    /* Define the struct bits */
    struct {
        u32 led_mod : 1;     /* [0]  */
        u32 reserved_0 : 31; /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_led_mod;

/* Define the union u_line_loop_back */
typedef union {
    /* Define the struct bits */
    struct {
        u32 line_loop_back : 1; /* [0]  */
        u32 reserved_0 : 31;    /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_line_loop_back;

/* Define the union u_cf_crc_strip */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_crc_strip : 1; /* [0]  */
        u32 reserved_0 : 31;  /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_cf_crc_strip;

/* Define the union u_mode_change_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 mode_change_en : 1; /* [0]  */
        u32 reserved_0 : 31;    /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_mode_change_en;

/* Define the union u_sixteen_bit_cntr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 sixteen_bit_cntr : 16; /* [15..0]  */
        u32 reserved_0 : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_sixteen_bit_cntr;

/* Define the union u_ld_link_counter */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 1;      /* [0]  */
        u32 ld_link_counter : 1; /* [1]  */
        u32 reserved_1 : 30;     /* [31..2]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_ld_link_counter;

/* Define the union u_loop_reg */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 1;      /* [0]  */
        u32 cf_ext_drive_lp : 1; /* [1]  */
        u32 cf2mi_lp_en : 1;     /* [2]  */
        u32 reserved_1 : 29;     /* [31..3]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_loop_reg;

/* Define the union u_recv_control */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 3;   /* [2..0]  */
        u32 strip_pad_en : 1; /* [3]  */
        u32 runt_pkt_en : 1;  /* [4]  */
        u32 reserved_1 : 27;  /* [31..5]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_recv_control;

/* Define the union u_vlan_code */
typedef union {
    /* Define the struct bits */
    struct {
        u32 cf_vlan_code : 16; /* [15..0]  */
        u32 reserved_0 : 16;   /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_vlan_code;

/* Define the union u_station_addr_high_0 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_0 : 16; /* [15..0]  */
        u32 reserved_0 : 16;          /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_0;

/* Define the union u_station_addr_high_1 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_1 : 16; /* [15..0]  */
        u32 reserved_0 : 16;          /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_1;

/* Define the union u_station_addr_high_2 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_2 : 16; /* [15..0]  */
        u32 reserved_0 : 16;          /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_2;

/* Define the union u_station_addr_high_3 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_3 : 16; /* [15..0]  */
        u32 reserved_0 : 16;          /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_3;

/* Define the union u_station_addr_high_4 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_4 : 16; /* [15..0]  */
        u32 reserved_0 : 16;          /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_4;

/* Define the union u_station_addr_high_5 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_5 : 16; /* [15..0]  */
        u32 reserved_0 : 16;          /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_5;

/* Define the union u_station_addr_high_msk_0 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_msk_0 : 16; /* [15..0]  */
        u32 reserved_0 : 16;              /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_msk_0;

/* Define the union u_station_addr_high_msk_1 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 station_addr_high_msk_1 : 16; /* [15..0]  */
        u32 reserved_0 : 16;              /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_station_addr_high_msk_1;

/* Define the union u_mac_skip_len */
typedef union {
    /* Define the struct bits */
    struct {
        u32 mac_skip_len : 6; /* [5..0]  */
        u32 mac_skip_crc : 1; /* [6]  */
        u32 reserved_0 : 25;  /* [31..7]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_mac_skip_len;

/* Define the union u_debug_pkt_ptr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 debug_pkt_ptr : 3; /* [2..0]  */
        u32 reserved_0 : 29;   /* [31..3]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_debug_pkt_ptr;

/* Define the union u_debug_en */
typedef union {
    /* Define the struct bits */
    struct {
        u32 debug_en : 1;    /* [0]  */
        u32 reserved_0 : 31; /* [31..1]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_debug_en;

/* Define the union u_intr_state */
typedef union {
    /* Define the struct bits */
    struct {
        u32 dbg_grp1_num : 2;   /* [1..0]  */
        u32 dbg_grp0_num : 2;   /* [3..2]  */
        u32 dbg_grp1_flag : 1;  /* [4]  */
        u32 dbg_grp0_flag : 1;  /* [5]  */
        u32 back_1588_flag : 1; /* [6]  */
        u32 reserved_0 : 25;    /* [31..7]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_intr_state;

/* Define the union u_intr_clr */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 4;    /* [3..0]  */
        u32 dbg_grp1_clr : 1;  /* [4]  */
        u32 dbg_grp0_clr : 1;  /* [5]  */
        u32 back_1588_clr : 1; /* [6]  */
        u32 reserved_1 : 25;   /* [31..7]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_intr_clr;

/* Define the union u_intr_msk */
typedef union {
    /* Define the struct bits */
    struct {
        u32 reserved_0 : 4;    /* [3..0]  */
        u32 dbg_grp1_msk : 1;  /* [4]  */
        u32 dbg_grp0_msk : 1;  /* [5]  */
        u32 back_1588_msk : 1; /* [6]  */
        u32 reserved_1 : 25;   /* [31..7]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_intr_msk;

/* Define the union u_seq_id */
typedef union {
    /* Define the struct bits */
    struct {
        u32 stp_79_to_64 : 16; /* [15..0]  */
        u32 seq_id : 16;       /* [31..16]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_seq_id;

/* Define the union u_dbg_grp0_vld_words */
typedef union {
    /* Define the struct bits */
    struct {
        u32 ram3_vld_word : 6;       /* [5..0]  */
        u32 ram2_vld_word : 6;       /* [11..6]  */
        u32 ram1_vld_word : 6;       /* [17..12]  */
        u32 ram0_vld_word : 6;       /* [23..18]  */
        u32 ram3_drop_crc_bytes : 2; /* [25..24]  */
        u32 ram2_drop_crc_bytes : 2; /* [27..26]  */
        u32 ram1_drop_crc_bytes : 2; /* [29..28]  */
        u32 ram0_drop_crc_bytes : 2; /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_dbg_grp0_vld_words;

/* Define the union u_dbg_grp1_vld_words */
typedef union {
    /* Define the struct bits */
    struct {
        u32 ram7_vld_word : 6;       /* [5..0]  */
        u32 ram6_vld_word : 6;       /* [11..6]  */
        u32 ram5_vld_word : 6;       /* [17..12]  */
        u32 ram4_vld_word : 6;       /* [23..18]  */
        u32 ram7_drop_crc_bytes : 2; /* [25..24]  */
        u32 ram6_drop_crc_bytes : 2; /* [27..26]  */
        u32 ram5_drop_crc_bytes : 2; /* [29..28]  */
        u32 ram4_drop_crc_bytes : 2; /* [31..30]  */
    } bits;
    /* Define an unsigned member */
    u32 val;
} u_dbg_grp1_vld_words;

#endif /* __GMAC_C_UNION_DEFINE_H__ */
