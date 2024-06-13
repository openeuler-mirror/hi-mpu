// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __JLC_C_UNION_DEFINE_H__
#define __JLC_C_UNION_DEFINE_H__

/* Define the union u_jlc_cr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mode : 1;        /* [0] */
        unsigned int buf_en : 1;      /* [1] */
        unsigned int width : 1;       /* [2] */
        unsigned int tms_auto : 1;    /* [3] */
        unsigned int dw_auto : 1;     /* [4] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_cr;

/* Define the union u_jlc_enable */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int enable : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_enable;

/* Define the union u_jlc_tdi2_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tdi2_size : 5;   /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_tdi2_size;

/* Define the union u_jlc_tms_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tms_size : 5;    /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_tms_size;

/* Define the union u_jlc_tms2_size */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tms2_size : 5;   /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_tms2_size;

/* Define the union u_jlc_buf_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int buf_level : 7;   /* [6..0] */
        unsigned int reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_buf_level;

/* Define the union u_jlc_tck_rate */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tck_rate : 7;    /* [6..0] */
        unsigned int reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_tck_rate;

/* Define the union u_jlc_ch_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ch_en : 7;       /* [6..0] */
        unsigned int reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_ch_en;

/* Define the union u_jlc_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int busy : 1;        /* [0] */
        unsigned int tdi_empty : 1;   /* [1] */
        unsigned int tdo_valid : 1;   /* [2] */
        unsigned int tms_empty : 1;   /* [3] */
        unsigned int tms_finish : 1;  /* [4] */
        unsigned int tdi2_empty : 1;  /* [5] */
        unsigned int tdo2_valid : 1;  /* [6] */
        unsigned int tms2_empty : 1;  /* [7] */
        unsigned int tms2_finish : 1; /* [8] */
        unsigned int buf1_valid : 1;  /* [9] */
        unsigned int buf2_valid : 1;  /* [10] */
        unsigned int tck_empty : 1;   /* [11] */
        unsigned int tck_finish : 1;  /* [12] */
        unsigned int tdi2_empty2 : 1; /* [13] */
        unsigned int tdo2_valid2 : 1; /* [14] */
        unsigned int reserved_0 : 17; /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_stat;

/* Define the union u_jlc_int_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int busy_en : 1;        /* [0] */
        unsigned int tdi_empty_en : 1;   /* [1] */
        unsigned int tdi_valid_en : 1;   /* [2] */
        unsigned int tms_empty_en : 1;   /* [3] */
        unsigned int tms_finish_en : 1;  /* [4] */
        unsigned int tdi2_empty_en : 1;  /* [5] */
        unsigned int tdo2_valid_en : 1;  /* [6] */
        unsigned int tms2_empty_en : 1;  /* [7] */
        unsigned int tms2_finish_en : 1; /* [8] */
        unsigned int buf1_valid_en : 1;  /* [9] */
        unsigned int buf2_valid_en : 1;  /* [10] */
        unsigned int tck_empty_en : 1;   /* [11] */
        unsigned int tck_finish_en : 1;  /* [12] */
        unsigned int tdi2_empty2_en : 1; /* [13] */
        unsigned int tdo2_valid2_en : 1; /* [14] */
        unsigned int reserved_0 : 17;    /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_int_en;

/* Define the union u_jlc_int_stat */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int busy_int : 1;        /* [0] */
        unsigned int tdi_empty_int : 1;   /* [1] */
        unsigned int tdi_valid_int : 1;   /* [2] */
        unsigned int tms_empty_int : 1;   /* [3] */
        unsigned int tms_finish_int : 1;  /* [4] */
        unsigned int tdi2_empty_int : 1;  /* [5] */
        unsigned int tdo2_valid_int : 1;  /* [6] */
        unsigned int tms2_empty_int : 1;  /* [7] */
        unsigned int tms2_finish_int : 1; /* [8] */
        unsigned int buf1_valid_int : 1;  /* [9] */
        unsigned int buf2_valid_int : 1;  /* [10] */
        unsigned int tck_empty_int : 1;   /* [11] */
        unsigned int tck_finish_int : 1;  /* [12] */
        unsigned int tdi2_empty2_int : 1; /* [13] */
        unsigned int tdo2_valid2_int : 1; /* [14] */
        unsigned int reserved_0 : 17;     /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_int_stat;

/* Define the union u_jlc_int_clr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int reserved_0 : 1;      /* [0] */
        unsigned int reserved_1 : 1;      /* [1] */
        unsigned int tdi_valid_clr : 1;   /* [2] */
        unsigned int reserved_2 : 1;      /* [3] */
        unsigned int tms_finish_clr : 1;  /* [4] */
        unsigned int reserved_3 : 1;      /* [5] */
        unsigned int tdo2_valid_clr : 1;  /* [6] */
        unsigned int reserved_4 : 1;      /* [7] */
        unsigned int tms2_finish_clr : 1; /* [8] */
        unsigned int buf1_valid_clr : 1;  /* [9] */
        unsigned int buf2_valid_clr : 1;  /* [10] */
        unsigned int reserved_5 : 1;      /* [11] */
        unsigned int tck_finish_clr : 1;  /* [12] */
        unsigned int reserved_6 : 1;      /* [13] */
        unsigned int tdo2_valid2_clr : 1; /* [14] */
        unsigned int reserved_7 : 17;     /* [31..15] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_int_clr;

/* Define the union u_jlc_start */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int start_buf1 : 1;  /* [0] */
        unsigned int start_buf2 : 1;  /* [1] */
        unsigned int start_busy : 1;  /* [2] */
        unsigned int reserved_0 : 29; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_start;

/* Define the union u_jlc_trst */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int trst : 1;        /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_trst;

/* Define the union u_jlc_dw */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dw0 : 1;         /* [0] */
        unsigned int dw1 : 1;         /* [1] */
        unsigned int dw2 : 1;         /* [2] */
        unsigned int dw3 : 1;         /* [3] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_dw;

/* Define the union u_jlc_sen */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sen0 : 1;        /* [0] */
        unsigned int sen1 : 1;        /* [1] */
        unsigned int sen2 : 1;        /* [2] */
        unsigned int sen3 : 1;        /* [3] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_sen;

/* Define the union u_jlc_tdi2_size2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tdi2_size2 : 5;  /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_tdi2_size2;

/* Define the union u_jlc_dw_len */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dw_len : 6;      /* [5..0] */
        unsigned int reserved_0 : 26; /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_dw_len;

/* Define the union u_jlc_dw_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dw_cfg : 4;      /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_dw_cfg;

/* Define the union u_jlc_dw2_cfg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dw2_cfg : 4;     /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_dw2_cfg;

/* Define the union u_jlc_buf2_level */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int buf2_level : 7;  /* [6..0] */
        unsigned int reserved_0 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_buf2_level;

/* Define the union u_jlc_dly_en */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jlc_dly_en : 1;  /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_dly_en;

/* Define the union u_jlc_sam_dly */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int jlc_sam_dly : 8; /* [7..0] */
        unsigned int reserved_0 : 24; /* [31..8] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_jlc_sam_dly;

#endif
