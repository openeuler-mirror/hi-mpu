// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SPI_C_UNION_DEFINE_H__
#define __SPI_C_UNION_DEFINE_H__

/* Define the union u_spi_ctrlr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dfs : 4;         /* [3..0] */
        unsigned int frf : 2;         /* [5..4] */
        unsigned int scph : 1;        /* [6] */
        unsigned int scpol : 1;       /* [7] */
        unsigned int tmod : 2;        /* [9..8] */
        unsigned int reserved_0 : 1;  /* [10] */
        unsigned int srl : 1;         /* [11] */
        unsigned int cfs : 4;         /* [15..12] */
        unsigned int reserved_1 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_ctrlr0;

/* Define the union u_spi_ctrlr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ndf : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_ctrlr1;

/* Define the union u_spi_ssienr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int spi_en : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_ssienr;

/* Define the union u_spi_mwcr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int mwmod : 1;       /* [0] */
        unsigned int mdd : 1;         /* [1] */
        unsigned int mhs : 1;         /* [2] */
        unsigned int reserved_0 : 29; /* [31..3] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_mwcr;

/* Define the union u_spi_ser */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int ser : 4;         /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_ser;

/* Define the union u_spi_baudr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int sckdv : 16;      /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_baudr;

/* Define the union u_spi_txftlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int tft : 5;         /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_txftlr;

/* Define the union u_spi_rxftlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rft : 5;         /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rxftlr;

/* Define the union u_spi_txflr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txtfl : 5;       /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_txflr;

/* Define the union u_spi_rxflr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rxtfl : 5;       /* [4..0] */
        unsigned int reserved_0 : 27; /* [31..5] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rxflr;

/* Define the union u_spi_sr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int busy : 1;        /* [0] */
        unsigned int tfnf : 1;        /* [1] */
        unsigned int tfe : 1;         /* [2] */
        unsigned int rfne : 1;        /* [3] */
        unsigned int eff : 1;         /* [4] */
        unsigned int reserved_0 : 1;  /* [5] */
        unsigned int dcol : 1;        /* [6] */
        unsigned int reserved_1 : 25; /* [31..7] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_sr;

/* Define the union u_spi_imr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txeim : 1;       /* [0] */
        unsigned int txoim : 1;       /* [1] */
        unsigned int rxuim : 1;       /* [2] */
        unsigned int rxoim : 1;       /* [3] */
        unsigned int rxfim : 1;       /* [4] */
        unsigned int mstim : 1;       /* [5] */
        unsigned int reserved_0 : 26; /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_imr;

/* Define the union u_spi_isr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txeis : 1;       /* [0] */
        unsigned int txois : 1;       /* [1] */
        unsigned int rxuis : 1;       /* [2] */
        unsigned int rxois : 1;       /* [3] */
        unsigned int rxfis : 1;       /* [4] */
        unsigned int mstis : 1;       /* [5] */
        unsigned int reserved_0 : 26; /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_isr;

/* Define the union u_spi_sisr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txeir : 1;       /* [0] */
        unsigned int txoir : 1;       /* [1] */
        unsigned int rxuir : 1;       /* [2] */
        unsigned int rxoir : 1;       /* [3] */
        unsigned int rxfir : 1;       /* [4] */
        unsigned int mstir : 1;       /* [5] */
        unsigned int reserved_0 : 26; /* [31..6] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_sisr;

/* Define the union u_spi_txoicr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int txoicr : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_txoicr;

/* Define the union u_spi_rxoicr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rxoicr : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rxoicr;

/* Define the union u_spi_rxuicr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rxuicr : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_rxuicr;

/* Define the union u_spi_msticr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int msticr : 1;      /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_msticr;

/* Define the union u_spi_icr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int icr : 1;         /* [0] */
        unsigned int reserved_0 : 31; /* [31..1] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_icr;

/* Define the union u_spi_dmacr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int rdmae : 1;       /* [0] */
        unsigned int tdmae : 1;       /* [1] */
        unsigned int reserved_0 : 30; /* [31..2] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dmacr;

/* Define the union u_spi_dmatdlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dmatdl : 4;      /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dmatdlr;

/* Define the union u_spi_dmardlr */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dmardl : 4;      /* [3..0] */
        unsigned int reserved_0 : 28; /* [31..4] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dmardlr;

/* Define the union u_spi_dr0 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr0 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr0;

/* Define the union u_spi_dr1 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr1 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr1;

/* Define the union u_spi_dr2 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr2 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr2;

/* Define the union u_spi_dr3 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr3 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr3;

/* Define the union u_spi_dr4 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr4 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr4;

/* Define the union u_spi_dr5 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr5 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr5;

/* Define the union u_spi_dr6 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr6 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr6;

/* Define the union u_spi_dr7 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr7 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr7;

/* Define the union u_spi_dr8 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr8 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr8;

/* Define the union u_spi_dr9 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr9 : 16;        /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr9;

/* Define the union u_spi_dr10 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr10 : 16;       /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr10;

/* Define the union u_spi_dr11 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr11 : 16;       /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr11;

/* Define the union u_spi_dr12 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr12 : 16;       /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr12;

/* Define the union u_spi_dr13 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr13 : 16;       /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr13;

/* Define the union u_spi_dr14 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr14 : 16;       /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr14;

/* Define the union u_spi_dr15 */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int dr15 : 16;       /* [15..0] */
        unsigned int reserved_0 : 16; /* [31..16] */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} u_spi_dr15;

#endif
