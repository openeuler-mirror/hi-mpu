/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#include "io_config.h"
#include "lp_subsys.h"
#include "common.h"

#define ICR_CONTROL           0x0000
#define ICR_TAR               0x0004
#define ICR_SAR               0x0008
#define ICR_DATA_CMD          0x0010
#define ICR_SS_SCL_HCNT       0x0014
#define ICR_SS_SCL_LCNT       0x0018
#define ICR_FS_SCL_HCNT       0x001C
#define ICR_FS_SCL_LCNT       0x0020
#define ICR_HS_SCL_HCNT       0x0024
#define ICR_HS_SCL_LCNT       0x0028
#define ICR_INTR_STAT         0x002c
#define ICR_INTR_MASK         0x0030
#define ICR_RAW_INTR_STAT     0x0034
#define ICR_RX_TL             0x0038
#define ICR_TX_TL             0x003C
#define ICR_CLR_INTR          0x0040
#define ICR_CLR_RX_UNDER      0x0044
#define ICR_CLR_RX_OVER       0x0048
#define ICR_CLR_TX_OVER       0x004C
#define ICR_CLR_RD_REQ        0x0050
#define ICR_CLR_TX_ABRT       0x0054
#define ICR_CLR_RX_DONE       0x0058
#define ICR_CLR_ACTIVITY      0x005C
#define ICR_CLR_STOP_DET      0x0060
#define ICR_CLR_START_DET     0x0064
#define ICR_CLR_GEN_CALL      0x0068
#define ICR_ENABLE            0x006C
#define ICR_STATUS            0x0070
#define ICR_TXFLR             0x0074
#define ICR_RXFLR             0x0078
#define ICR_SDA_HOLD          0x007C
#define ICR_TX_ABRT_SOURCE    0x0080
#define ICR_DMA_CR            0x0088
#define ICR_DMA_TDLR          0x008C
#define ICR_DMA_RDLR          0x0090
#define ICR_ENABLE_STATUS     0x009C
#define ICR_SCL_LOW_TIMEOUT   0x00AC
#define ICR_SDA_LOW_TIMEOUT   0x00B0
#define ICR_CLR_SCL_STUCK_DET 0x00B4
#define ICR_CLK_LOW_SEXT      0x00BC
#define ICR_CLK_LOW_MEXT      0x00C0
#define ICR_COMP_PARAM_1      0x00F4  // 存疑
#define ICR_COMP_VERSION      0x00F8

#define I2C_KDEPLOY_MASTER          BIT(1) /* The bus is initialized to Master. */
#define I2C_KDEPLOY_SPEED_MODE_STD  BIT(2) /* The bus is initialized to the standard rate mode (100 kbit/s). */
#define I2C_KDEPLOY_SPEED_MODE_FAST BIT(3) /* Bus initialization to fast rate mode (400k) */
/* Forcibly initialize the bus, regardless of whether it has been initialized before. */
#define I2C_KDEPLOY_SPEED_MODE_MASK (I2C_KDEPLOY_SPEED_MODE_STD | I2C_KDEPLOY_SPEED_MODE_FAST)
#define I2C_KDEPLOY_FORCE           BIT(9)

#define IC_CON_MASTER                BIT(0)
#define IC_CON_SPEED_STD             BIT(1)
#define IC_CON_SPEED_FAST            BIT(2)
#define IC_CON_SPEED_MASK            (BIT(1) | BIT(2))
#define IC_CON_10BITADDR_SLAVE       BIT(3)
#define IC_CON_10BITADDR_MASTER      BIT(4)
#define IC_CON_RESTART_EN            BIT(5)
#define IC_CON_SLAVE_DISABLE         BIT(6)
#define IC_CON_STOP_DET_IFADDRESSED  BIT(7)
#define IC_CON_TX_EMPTY_CTRL         BIT(8)
#define IC_CON_RX_FIFO_FULL_HLD_CTRL BIT(9)

#define IC_STATUS_ACTIVITY        BIT(0)
#define IC_STATUS_TFE             BIT(2)
#define IC_STATUS_RFNE            BIT(3)

#define IC_ENA_ENABLE BIT(0)
#define IC_DATA_CMD_WRITE   BIT(8)

#define I2C_INIT_TIMEOUT   10000
#define IC_SDA_LOW_TIMEOUT_DEFAULT 0xFFFFFFFF
#define IC_SCL_LOW_TIMEOUT_DEFAULT 0xFFFFFFFF

#define I2C_MAX_WRITE_LEN 2048
#define I2C_MAX_READ_LEN  2048

#define I2C_LPSC_CLKEN_CTRL0_OFFSET      10

#define I2C_SS_SCL_HCNT_CYCLE 0x188
#define I2C_SS_SCL_LCNT_CYCLE 0x1D5
#define I2C_FS_SCL_HCNT_CYCLE 0x34
#define I2C_FS_SCL_LCNT_CYCLE 0x81

#define I2C_USEC_PER_MSEC  1000
#define I2C_USEC_PER_SEC   1000000

#define I2C_IO_MUX_MASK 0x7
#define I2C_7BIT_MAX_ADDR 0x7F
#define I2C_M_WE 0x0000

typedef enum {
    I2C_ID_0 = 0,
    I2C_ID_1,
    I2C_ID_2,
    I2C_ID_3,
    I2C_ID_4,
    I2C_ID_5,
    I2C_ID_6,
    I2C_ID_7,
    I2C_ID_8,
    I2C_ID_9,
    I2C_ID_10,
    I2C_ID_11,
    I2C_ID_12,
    I2C_ID_13,
    I2C_ID_14,
    I2C_ID_15,
} i2c_ids;

typedef enum {
    I2C_SPEED_100K  = 100,
    I2C_SPEED_400K  = 400,
} i2c_speed;

struct hi309x_i2c_plat {
    i2c_ids i2c_id;
    ulong regbase;
    int irqn;
    i2c_speed speed;
    int init_flag;
};

struct i2c_kmsg {
    ushort addr;    /* Message destination addr */
    ushort flags;   /* Message Properties */
    ulong timeout; /* Timeout interval for sending and receiving messages, in jiffies. */
    uint rcnt;
    uint wcnt;
    uint wlen;   /* Message sending length */
    uchar *wbuf; /* Message sending cache */
    uint rlen;   /* I2C_KMSG_RECV_LEN indicates the length of the message receiving buffer,
                    Otherwise, it indicates the length of the message to be received. */
    uchar *rbuf; /* Message receiving buffer */
};

struct hi309x_i2c_adapter {
    i2c_ids i2c_id;
    ulong regbase;
    int irqn;
    i2c_speed speed;
    int init_flag;
    uint control;
    uint hcnt;
    uint lcnt;
    uint tx_fifo_depth;
    uint rx_fifo_depth;
    ushort msg_flags;
    uint cmd_len;
};

struct i2c_kdepl {
    ulong flags;          /* Initializing Properties */
    uint sda_hold;        /* Number of hold delay cycles of the SDA signal */
    uint sda_hold_ratio;  /* Percentage of hold delay cycles to low-level cycles of the SDA signal */
    uint scl_high;        /* SCL Number of high-level cycles */
    uint scl_low;         /* SCL Number of low-level cycles */
    uint scl_low_timeout; /* SCL Number of low-level cycles */
    uint sda_low_timeout; /* SDL Number of low-level timeout cycles */
};

struct i2c_iomux_info {
    uint flag;
    uint offset;
    uint mask;
    unsigned val;
};

static const struct i2c_iomux_info g_i2c_iomux_data_tbl[] = {
    {IO_CONFIG_T, IO_CONFIG_T_IOMG013_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG015_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG017_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG019_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG021_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG023_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG025_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG026_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG029_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG031_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG033_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG035_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG037_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG050_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x1},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG036_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
};

static const struct i2c_iomux_info g_i2c_iomux_clk_tbl[] = {
    {IO_CONFIG_T, IO_CONFIG_T_IOMG014_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG016_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG018_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG020_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG022_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG024_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG025_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG026_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x0},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG028_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG030_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG032_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG034_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_R, IO_CONFIG_R_IOMG036_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG050_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x1},
    {IO_CONFIG_T, IO_CONFIG_T_IOMG035_OFFSET_ADDR, I2C_IO_MUX_MASK, 0x3},
};

static inline void i2c_writel(ulong regbase, uint val, uint off)
{
    writel(val, regbase + off);
}

static inline int i2c_readl(ulong regbase, uint off)
{
    return readl(regbase + off);
}

static inline void i2c_mask_irq_signal(ulong regbase)
{
    i2c_writel(regbase, 0x0, ICR_INTR_MASK);
}

static inline void i2c_common_disable(ulong regbase)
{
    i2c_writel(regbase, 0x0, ICR_ENABLE);
}

static inline void i2c_common_enable(ulong regbase)
{
    i2c_writel(regbase, IC_ENA_ENABLE, ICR_ENABLE);
}