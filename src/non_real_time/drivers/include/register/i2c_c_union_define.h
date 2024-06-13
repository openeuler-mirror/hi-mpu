// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __I2C_C_UNION_DEFINE_H__
#define __I2C_C_UNION_DEFINE_H__

/* Define the union u_i2c_con */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int master : 1;        /* [0] */
        unsigned int spedd : 2;         /* [2..1] */
        unsigned int slave_10bit : 1;   /* [3] */
        unsigned int master_10bit : 1;  /* [4] */
        unsigned int restart_en : 1;    /* [5] */
        unsigned int slave_disable : 1; /* [6] */
        unsigned int reserved_0 : 25;   /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_con;

/* Define the union u_i2c_tar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_tar : 10;             /* [9..0] */
        unsigned int gc_or_start : 1;         /* [10] */
        unsigned int special : 1;             /* [11] */
        unsigned int ic_10bitaddr_master : 1; /* [12] */
        unsigned int reserved_0 : 19;         /* [31..13] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_tar;

/* Define the union u_i2c_sar */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_sar : 10;     /* [9..0] */
        unsigned int reserved_0 : 22; /* [31..10] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_sar;

/* Define the union u_i2c_hs_maddr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_hs_maddr : 3; /* [2..0] */
        unsigned int reserved_0 : 29; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_hs_maddr;

/* Define the union u_i2c_data_cmd */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dat : 8;         /* [7..0] */
        unsigned int cmd : 1;         /* [8] */
        unsigned int reserved_0 : 23; /* [31..9] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_data_cmd;

/* Define the union u_i2c_ss_scl_hcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_ss_scl_hcnt : 16; /* [15..0] */
        unsigned int reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_ss_scl_hcnt;

/* Define the union u_i2c_ss_scl_lcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_ss_scl_lcnt : 16; /* [15..0] */
        unsigned int reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_ss_scl_lcnt;

/* Define the union u_i2c_fs_scl_hcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_fs_scl_hcnt : 16; /* [15..0] */
        unsigned int reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_fs_scl_hcnt;

/* Define the union u_i2c_fs_scl_lcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_fs_scl_lcnt : 16; /* [15..0] */
        unsigned int reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_fs_scl_lcnt;

/* Define the union u_i2c_hs_scl_hcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_hs_scl_hcnt : 16; /* [15..0] */
        unsigned int reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_hs_scl_hcnt;

/* Define the union u_i2c_hs_scl_lcnt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_hs_scl_lcnt : 16; /* [15..0] */
        unsigned int reserved_0 : 16;     /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_hs_scl_lcnt;

/* Define the union u_i2c_intr_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int r_rx_under : 1;  /* [0] */
        unsigned int r_rx_over : 1;   /* [1] */
        unsigned int r_rx_full : 1;   /* [2] */
        unsigned int r_tx_over : 1;   /* [3] */
        unsigned int r_tx_empty : 1;  /* [4] */
        unsigned int r_rd_req : 1;    /* [5] */
        unsigned int r_tx_abrt : 1;   /* [6] */
        unsigned int r_rx_done : 1;   /* [7] */
        unsigned int r_activity : 1;  /* [8] */
        unsigned int r_stop_det : 1;  /* [9] */
        unsigned int r_start_det : 1; /* [10] */
        unsigned int r_gen_call : 1;  /* [11] */
        unsigned int reserved_0 : 20; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_intr_stat;

/* Define the union u_i2c_intr_mask */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int m_rx_under : 1;  /* [0] */
        unsigned int m_rx_over : 1;   /* [1] */
        unsigned int m_rx_full : 1;   /* [2] */
        unsigned int m_tx_over : 1;   /* [3] */
        unsigned int m_tx_empty : 1;  /* [4] */
        unsigned int m_rd_req : 1;    /* [5] */
        unsigned int m_tx_abrt : 1;   /* [6] */
        unsigned int m_rx_done : 1;   /* [7] */
        unsigned int m_activity : 1;  /* [8] */
        unsigned int m_stop_det : 1;  /* [9] */
        unsigned int m_start_det : 1; /* [10] */
        unsigned int m_gen_call : 1;  /* [11] */
        unsigned int reserved_0 : 20; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_intr_mask;

/* Define the union u_i2c_raw_intr_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rx_under : 1;    /* [0] */
        unsigned int rx_over : 1;     /* [1] */
        unsigned int rx_full : 1;     /* [2] */
        unsigned int tx_over : 1;     /* [3] */
        unsigned int tx_empty : 1;    /* [4] */
        unsigned int rd_req : 1;      /* [5] */
        unsigned int tx_abrt : 1;     /* [6] */
        unsigned int rx_done : 1;     /* [7] */
        unsigned int activity : 1;    /* [8] */
        unsigned int stop_det : 1;    /* [9] */
        unsigned int start_det : 1;   /* [10] */
        unsigned int gen_call : 1;    /* [11] */
        unsigned int reserved_0 : 20; /* [31..12] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_raw_intr_stat;

/* Define the union u_i2c_rx_tl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rx_tl : 8;       /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_rx_tl;

/* Define the union u_i2c_tx_tl */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tx_tl : 8;       /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_tx_tl;

/* Define the union u_i2c_clr_intr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_intr : 1;    /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_intr;

/* Define the union u_i2c_clr_rx_under */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_rx_under : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_rx_under;

/* Define the union u_i2c_clr_rx_over */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_rx_over : 1; /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_rx_over;

/* Define the union u_i2c_clr_tx_over */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_tx_over : 1; /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_tx_over;

/* Define the union u_i2c_clr_rd_req */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_rd_req : 1;  /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_rd_req;

/* Define the union u_i2c_clr_tx_abrt */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_tx_abrt : 1; /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_tx_abrt;

/* Define the union u_i2c_clr_rx_done */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_rx_done : 1; /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_rx_done;

/* Define the union u_i2c_clr_activity */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_activity : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_activity;

/* Define the union u_i2c_clr_stop_det */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_stop_det : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_stop_det;

/* Define the union u_i2c_clr_start_det */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_start_det : 1; /* [0] */
        unsigned int reserved_0 : 31;   /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_start_det;

/* Define the union u_i2c_clr_gen_call */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int clr_gen_call : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_clr_gen_call;

/* Define the union u_i2c_enable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int enable : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_enable;

/* Define the union u_i2c_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int i2c_activity : 1; /* [0] */
        unsigned int tfnf : 1;         /* [1] */
        unsigned int tfe : 1;          /* [2] */
        unsigned int rfne : 1;         /* [3] */
        unsigned int rff : 1;          /* [4] */
        unsigned int mst_activity : 1; /* [5] */
        unsigned int slv_activity : 1; /* [6] */
        unsigned int reserved_0 : 25;  /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_status;

/* Define the union u_i2c_txflr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txflr : 7;       /* [6..0] */
        unsigned int reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_txflr;

/* Define the union u_i2c_rxflr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rxflr : 7;       /* [6..0] */
        unsigned int reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_rxflr;

/* Define the union u_i2c_sda_hold */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_sda_hold : 16; /* [15..0] */
        unsigned int reserved_0 : 16;  /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_sda_hold;

/* Define the union u_i2c_tx_abrt_source */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int abrt_7b_addr_noack : 1;   /* [0] */
        unsigned int abrt_10addr1_noack : 1;   /* [1] */
        unsigned int abrt_10addr2_noack : 1;   /* [2] */
        unsigned int abrt_txdata_noack : 1;    /* [3] */
        unsigned int abrt_gcall_noack : 1;     /* [4] */
        unsigned int abrt_gcall_read : 1;      /* [5] */
        unsigned int abrt_hs_ackdet : 1;       /* [6] */
        unsigned int abrt_sbyte_ackdet : 1;    /* [7] */
        unsigned int abrt_hs_norstrt : 1;      /* [8] */
        unsigned int abrt_sbyte_norstrt : 1;   /* [9] */
        unsigned int abrt_10b_rd_norstrt : 1;  /* [10] */
        unsigned int arb_master_dis : 1;       /* [11] */
        unsigned int arb_lost : 1;             /* [12] */
        unsigned int abrt_slvflush_txfifo : 1; /* [13] */
        unsigned int abrt_slv_arblost : 1;     /* [14] */
        unsigned int abrt_slvrd_intx : 1;      /* [15] */
        unsigned int reserved_0 : 16;          /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_tx_abrt_source;

/* Define the union u_i2c_slv_data_only */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int nack : 1;        /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_slv_data_only;

/* Define the union u_i2c_dma_cr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rdmae : 1;       /* [0] */
        unsigned int tdmae : 1;       /* [1] */
        unsigned int reserved_0 : 30; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_dma_cr;

/* Define the union u_i2c_dma_tdlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dmatdl : 4;      /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_dma_tdlr;

/* Define the union u_i2c_dma_rdlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dmardl : 4;      /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_dma_rdlr;

/* Define the union u_i2c_sda_setup */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sda_setup : 8;   /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_sda_setup;

/* Define the union u_i2c_ack_general_call */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ack_gen_call : 1; /* [0] */
        unsigned int reserved_0 : 31;  /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_ack_general_call;

/* Define the union u_i2c_enable_status */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ic_en : 1;                  /* [0] */
        unsigned int slv_disable_while_busy : 1; /* [1] */
        unsigned int slv_rx_data_lost : 1;       /* [2] */
        unsigned int reserved_0 : 29;            /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_i2c_enable_status;

#endif
