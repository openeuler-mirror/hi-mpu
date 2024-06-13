/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */
#ifndef __SPI_H__
#define __SPI_H__

#include "io_config.h"
#include "lp_subsys.h"
#include "common.h"

/* spi ctrl regs */
#define SPI0_BASE_ADDR 0x8713000
#define SPI1_BASE_ADDR 0x8715000

#define SPI_CTRLR0_OFFSET_ADDR 0x0
#define SPI_CTRLR1_OFFSET_ADDR 0x4
#define SPI_SSIENR_OFFSET_ADDR 0x8
#define SPI_MWCR_OFFSET_ADDR 0xC
#define SPI_SER_OFFSET_ADDR 0x10
#define SPI_BAUDR_OFFSET_ADDR 0x14
#define SPI_TXFTLR_OFFSET_ADDR 0x18
#define SPI_RXFTLR_OFFSET_ADDR 0x1C
#define SPI_TXFLR_OFFSET_ADDR 0x20
#define SPI_RXFLR_OFFSET_ADDR 0x24
#define SPI_SR_OFFSET_ADDR 0x28
#define SPI_IMR_OFFSET_ADDR 0x2C
#define SPI_ISR_OFFSET_ADDR 0x30
#define SPI_SISR_OFFSET_ADDR 0x34
#define SPI_TXOICR_OFFSET_ADDR 0x38
#define SPI_RXOICR_OFFSET_ADDR 0x3C
#define SPI_RXUICR_OFFSET_ADDR 0x40
#define SPI_MSTICR_OFFSET_ADDR 0x44
#define SPI_ICR_OFFSET_ADDR 0x48
#define SPI_DMACR_OFFSET_ADDR 0x4C
#define SPI_DMATDLR_OFFSET_ADDR 0x50
#define SPI_DMARDLR_OFFSET_ADDR 0x54
#define SPI_IDR_OFFSET_ADDR 0x58
#define SPI_DR0_OFFSET_ADDR 0x60
#define SPI_DR1_OFFSET_ADDR 0x64
#define SPI_DR2_OFFSET_ADDR 0x68
#define SPI_DR3_OFFSET_ADDR 0x6C
#define SPI_DR4_OFFSET_ADDR 0x70
#define SPI_DR5_OFFSET_ADDR 0x74
#define SPI_DR6_OFFSET_ADDR 0x78
#define SPI_DR7_OFFSET_ADDR 0x7C
#define SPI_DR8_OFFSET_ADDR 0x80
#define SPI_DR9_OFFSET_ADDR 0x84
#define SPI_DR10_OFFSET_ADDR 0x88
#define SPI_DR11_OFFSET_ADDR 0x8C
#define SPI_DR12_OFFSET_ADDR 0x90
#define SPI_DR13_OFFSET_ADDR 0x94
#define SPI_DR14_OFFSET_ADDR 0x98
#define SPI_DR15_OFFSET_ADDR 0x9C

/* io regs */
#define SPI_IO_MUX_MASK 0x7
#define IO_CONFIG_SPI0_CLK_ADDR 0x1c
#define IO_CONFIG_SPI0_CS0_ADDR 0x20
#define IO_CONFIG_SPI0_CS1_ADDR 0x24
#define IO_CONFIG_SPI0_DO_ADDR 0x28
#define IO_CONFIG_SPI0_DI_ADDR 0x2C
/* The 3093 chip has two sets of IO ports that can be multiplexed as SPI1:
   one set is multiplexed with SFC1 and the other set is multiplexed with eMMC. 
   The configuration to select which set of multiplexing to use as SPI1 can be set through the macro USE_SPI1_SFC_PIN. */
#ifdef USE_SPI1_SFC_PIN
#define IO_CONFIG_SPI1_CLK_ADDR 0x18
#define IO_CONFIG_SPI1_CS0_ADDR 0x1c
#define IO_CONFIG_SPI1_CS1_ADDR 0x20
#define IO_CONFIG_SPI1_DO_ADDR 0x24
#define IO_CONFIG_SPI1_DI_ADDR 0x24
#else
#define IO_CONFIG_SPI1_CLK_ADDR 0x30
#define IO_CONFIG_SPI1_CS0_ADDR 0x34
#define IO_CONFIG_SPI1_CS1_ADDR 0x38
#define IO_CONFIG_SPI1_DO_ADDR 0x3C
#define IO_CONFIG_SPI1_DI_ADDR 0x40
#endif

/* lp sybsys regs */
#define LP_SUBSYS_CSR_LPSC_SPI_CTRL_OFFSET_ADDR 0x1C
#define LP_SUBSYS_CSR_LPSC_IOMUX_CTRL_OFFSET_ADDR 0x334

/* spi ctrl parms */
#define SPI_EN 0x1
#define SPI_DISEN 0x0

#define SPI_MAX_CTL 2
#define SPI_MAX_CS 2
#define SPI_CTL_PIN_NUM 3
#define SPI_CS_PIN_NUM 2

#define SPI_LOOP_BACK 1
#define SPI_NOT_LOOP_BACK 0

#define SPI_DEFAULT_FRAME_FORMAT 0 /* Motorola SPI Frame Format */
#define SER_REG_VALUE_NON_SLAVE 0    /* When no slave device is selected */

#define SYS_AHB_CLOCK_FREQ (200000000)
#define SPI_MAX_SPEED 20000000
#define SPI_MIN_DIV 6
#define SPI_MAX_DIV 65534

#define SPI_MAX_CTL 2
#define SPI_MAX_CS 2
#define SPI_CTL_PIN_NUM 3
#define SPI_CS_PIN_NUM 2

#define SPI_TX_MODE 0
#define SPI_RX_MODE 1
#define SPI_TX_RX_MODE 2
#define SPI_CHIP_SELCET_TOTAL_NUM 2

#define SPI_TRANSFER_TIMEOUT 1000 /* 1000 ms */

/* CTRLR0 srl */
#define CTRLR0_SRL_FIELD_BIT 11 /* Srl CTRLR0[11] */

/* CTRLR0 tmod */
#define CTRLR0_TMOD_FIELD_START_BIT 8 /* tmod CTRLR0[8:9] */
#define CTRLR0_TMOD_FIELD_END_BIT 9   /* tmod CTRLR0[8:9] */

/* CTRLR0 scpol */
#define CTRLR0_SCPOL_FIELD_BIT 7 /* scpol CTRLR0[7] */

/* CTRLR0 scph */
#define CTRLR0_SCPH_FIELD_BIT 6 /* scph CTRLR0[6] */

/* CTRLR0 frf */
#define CTRLR0_FRF_FIELD_START_BIT 4 /* frf CTRLR0[4:5] */
#define CTRLR0_FRF_FIELD_END_BIT 5   /* frf CTRLR0[4:5] */

/* CTRLR0 dfs */
#define CTRLR0_DFS_FIELD_START_BIT 0 /* dfs CTRLR0[0:3] */
#define CTRLR0_DFS_FIELD_END_BIT 3   /* dfs CTRLR0[0:3] */

#define WRITE_DELAY_TIME 160 // 2560//160//1710的参数 //经过验证，写后等待150微秒不会导致异常，为保险起见，设为160微秒

#define drv_32bit_mask(n1, n2) (BIT((n2) + 1) - BIT(n1))

#define drv_32bit_set_field(v1, n1, n2, v2)                 \
    (((unsigned int)(v1) & (~drv_32bit_mask((n1), (n2)))) | \
        (((unsigned int)(v2) << (unsigned int)(n1)) & drv_32bit_mask((n1), (n2))))

/* Define the union lp_subsys_spi_reg */
typedef union {
    /* Define the struct bits */
    struct {
        unsigned int spi0_in_n : 1;         /* [0]  */
        unsigned int spi0_cs_polarity0 : 1; /* [1]  */
        unsigned int spi0_cs_polarity1 : 1; /* [2]  */
        unsigned int spi0_wire_mode : 1;    /* [3]  */
        unsigned int spi0_rx_only : 1;      /* [4]  */
        unsigned int spi1_in_n : 1;         /* [5]  */
        unsigned int spi1_cs_polarity0 : 1; /* [6]  */
        unsigned int spi1_cs_polarity1 : 1; /* [7]  */
        unsigned int spi1_wire_mode : 1;    /* [8]  */
        unsigned int spi1_rx_only : 1;      /* [9]  */
        unsigned int reserved_0 : 22;       /* [31..10]  */
    } bits;

    /* Define an unsigned member */
    unsigned int val;
} lp_subsys_spi_reg;

typedef struct {
    unsigned int offset;
    unsigned int mask;
    unsigned val;
} spi_iomux_info;

static const spi_iomux_info g_spi_iomux_tbl[] = {
    {IO_CONFIG_SPI0_CLK_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI0_DO_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI0_DI_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI1_CLK_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI1_DO_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI1_DI_ADDR, SPI_IO_MUX_MASK, 0x1},
};

static const spi_iomux_info g_spi_cs_iomux_tbl[] = {
    {IO_CONFIG_SPI0_CS0_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI0_CS1_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI1_CS0_ADDR, SPI_IO_MUX_MASK, 0x1},
    {IO_CONFIG_SPI1_CS1_ADDR, SPI_IO_MUX_MASK, 0x1},
};

typedef enum {
    SPI_ID_0 = 0,
    SPI_ID_1,
} spi_ids;

typedef enum {
    WRITE = 0,
    READ,
} wr_flags;

typedef enum {
    FRM_FORMAT = 0,
    FRM_LEN,
    EDGE,
    POL,
} spi_config_items;

typedef enum {
    HISPI_CLK_EDGE_POL_0 = 0,  /* clk edge is 0, clk pol is 0 */
    HISPI_CLK_EDGE_POL_1 = 1,  /* clk edge is 0, clk pol is 1 */
    HISPI_CLK_EDGE_POL_2 = 2,  /* clk edge is 1, clk pol is 0 */
    HISPI_CLK_EDGE_POL_3 = 3,  /* clk edge is 1, clk pol is 1 */
} spi_mode;

typedef enum {
    HISPI_CS0_SELECT = 1,  /* cs0 select. */
    HISPI_CS1_SELECT = 2,  /* cs1 select. */
    HISPI_CS_ALL_SELECT = 3,  /* all select. */
} spi_cs_select;

typedef enum {
    HISPI_WORDLEN_8_BIT = 0x7, /* The length of the data frame is 8 bits. */
    HISPI_WORDLEN_16_BIT = 0xf  /* The length of the data frame is 16 bits. */
} spi_wordlen;

struct hi309x_spi_plat {
    spi_ids spi_id;
    ulong regbase;
    spi_wordlen wordlen;
};

struct hi309x_spi_adapter {
    spi_ids spi_id;
    ulong regbase;
    spi_cs_select cs;
    uint baud;
    spi_wordlen wordlen;
    spi_mode mode;
};

static inline void spi_writel(ulong regbase, uint val, uint off)
{
    writel(val, regbase + off);
}

static inline int spi_readl(ulong regbase, uint off)
{
    return readl(regbase + off);
}

static inline void spi_mask_irq_signal(ulong regbase)
{
    spi_writel(regbase, 0x0, SPI_IMR_OFFSET_ADDR);
}

static void spi_disable(ulong regbase)
{
    spi_writel(regbase, SPI_DISEN, SPI_SSIENR_OFFSET_ADDR);
}

static void spi_enable(ulong regbase)
{
    spi_writel(regbase, SPI_EN, SPI_SSIENR_OFFSET_ADDR);
}

#endif