// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * DesignWare i2c driver (common)
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __I2C_DW_H__
#define __I2C_DW_H__

#include "i2c.h"

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
#define ICR_COMP_PARAM_1      0x00F4
#define ICR_COMP_VERSION      0x00F8

#define IC_M_STATUS_WRITE_IN_PROGRESS 0x1
#define IC_M_STATUS_READ_IN_PROGRESS  0x2
#define IC_M_STATUS_WAIT4_COMPLETION  0x4
#define IC_M_STATUS_INIT              IC_M_STATUS_WAIT4_COMPLETION

#define IC_S_STATUS_IDLE            0x0
#define IC_S_STATUS_WRITE_REQUESTED 0x2
#define IC_S_STATUS_READ_REQUESTED  0x4

#define IC_S_DATA_LEN_MULTI          (2 + IC_MSG_MAX_NUM) /* req_buf + res_buf + req_queue */
#define IC_S_RX_FIFO_TL_COUNT_SMBUS  0x30
#define IC_S_TX_FIFO_TL_COUNT_SMBUS  0x8
#define IC_S_TX_FIFO_DEPTH_SMBUS     0x80

#define IC_SRST_REG_BASE_OFFSET     (9U)
#define IC_MSG_MAX_LEN              (256U)
#define IC_MSG_TRAN_TIME_EACH_BYTE  (300U) /* Maximum transmission time of each byte */

#define IC_SDA_HOLD_MIN_VERS 0x3131312A
#define IC_SDA_HOLD_TX_SHIFT 0
#define IC_SDA_HOLD_TX_MASK  I2C_GENMASK(15, IC_SDA_HOLD_TX_SHIFT)
#define IC_SDA_HOLD_RX_SHIFT 16
#define IC_SDA_HOLD_RX_MASK  I2C_GENMASK(23, IC_SDA_HOLD_RX_SHIFT)
#define IC_SDA_HOLD_DEFAULT  0x00010001

#define IC_SDA_LOW_TIMEOUT_DEFAULT 0xFFFFFFFF
#define IC_SCL_LOW_TIMEOUT_DEFAULT 0xFFFFFFFF

#define IC_CON_MASTER                BIT(0)
#define IC_CON_SPEED_STD             BIT(1)
#define IC_CON_SPEED_FAST            BIT(2)
#define IC_CON_SPEED_HIGH            (BIT(1) | BIT(2))
#define IC_CON_SPEED_MASK            (BIT(1) | BIT(2))
#define IC_CON_10BITADDR_SLAVE       BIT(3)
#define IC_CON_10BITADDR_MASTER      BIT(4)
#define IC_CON_RESTART_EN            BIT(5)
#define IC_CON_SLAVE_DISABLE         BIT(6)
#define IC_CON_STOP_DET_IFADDRESSED  BIT(7)
#define IC_CON_TX_EMPTY_CTRL         BIT(8)
#define IC_CON_RX_FIFO_FULL_HLD_CTRL BIT(9)

#define IC_TAR_SPECIAL          BIT(11)
#define IC_TAR_10BITADDR_MASTER BIT(12)
#define IC_TAR_SMBUS_QUICK_CMD  BIT(16)

#define IC_ENA_ENABLE BIT(0)
#define IC_ENA_ABORT  BIT(1)

#define IC_STATUS_ACTIVITY        BIT(0)
#define IC_STATUS_TFE             BIT(2)
#define IC_STATUS_MASTER_ACTIVITY BIT(5)
#define IC_STATUS_SLAVE_ACTIVITY  BIT(6)

#define IC_INTR_RX_UNDER      BIT(0)
#define IC_INTR_RX_OVER       BIT(1)
#define IC_INTR_RX_FULL       BIT(2)
#define IC_INTR_TX_OVER       BIT(3)
#define IC_INTR_TX_EMPTY      BIT(4)
#define IC_INTR_RD_REQ        BIT(5)
#define IC_INTR_TX_ABRT       BIT(6)
#define IC_INTR_RX_DONE       BIT(7)
#define IC_INTR_ACTIVITY      BIT(8)
#define IC_INTR_STOP_DET      BIT(9)
#define IC_INTR_START_DET     BIT(10)
#define IC_INTR_GEN_CALL      BIT(11)
#define IC_INTR_RESTART_DET   BIT(12)
#define IC_INTR_SCL_STUCK_LOW BIT(14)

#define IC_INTR_DEFAULT_MASK (IC_INTR_RX_FULL | IC_INTR_TX_ABRT | IC_INTR_STOP_DET | IC_INTR_RX_UNDER |  \
                              IC_INTR_RX_OVER | IC_INTR_TX_OVER)
#define IC_INTR_MASTER_MASK     (IC_INTR_DEFAULT_MASK | IC_INTR_TX_EMPTY | IC_INTR_SCL_STUCK_LOW)
#define IC_INTR_MASTER_DMA_MASK (IC_INTR_TX_ABRT | IC_INTR_STOP_DET | IC_INTR_SCL_STUCK_LOW)
#define IC_INTR_SLAVE_MASK      (IC_INTR_DEFAULT_MASK | IC_INTR_RX_DONE | IC_INTR_RX_UNDER | \
                                 IC_INTR_RD_REQ | IC_INTR_START_DET)

#define IC_TX_ABRT_7B_ADDR_NOACK  BIT(0)
#define IC_TX_ABRT_10ADDR1_NOACK  BIT(1)
#define IC_TX_ABRT_10ADDR2_NOACK  BIT(2)
#define IC_TX_ABRT_TXDATA_NOACK   BIT(3)
#define IC_TX_ABRT_GCALL_NOACK    BIT(4)
#define IC_TX_ABRT_GCALL_READ     BIT(5)
#define IC_TX_ABRT_SBYTE_ACKDET   BIT(7)
#define IC_TX_ABRT_SBYTE_NORSTRT  BIT(8)
#define IC_TX_ABRT_10B_RD_NORSTRT BIT(10)
#define IC_TX_ABRT_MASTER_DIS     BIT(11)
#define IC_TX_ABRT_ARB_LOST       BIT(12)
#define IC_TX_ABRT_USR_ABRT       BIT(16)
#define IC_TX_ABRT_SDA_STUCK_LOW  BIT(17)

#define IC_TX_ABRT_NOACK (IC_TX_ABRT_7B_ADDR_NOACK | IC_TX_ABRT_10ADDR1_NOACK | IC_TX_ABRT_10ADDR2_NOACK |  \
                          IC_TX_ABRT_TXDATA_NOACK | IC_TX_ABRT_GCALL_NOACK)

#define IC_DMA_CR_TDMAE BIT(0)
#define IC_DMA_CR_RDMAE BIT(1)

#define IC_DMA_TDLR_CNT 0x3
#define IC_DMA_RDLR_CNT 0x0

#define IC_DATA_CMD_WRITE   BIT(8)
#define IC_DATA_CMD_RECV    BIT(9)
#define IC_DATA_CMD_RESATRT BIT(10)

#define IC_COMP_PARAM_1_SPEED_MODE_HIGH (BIT(2) | BIT(3))
#define IC_COMP_PARAM_1_SPEED_MODE_MASK I2C_GENMASK(3, 2)

enum {
    I2C_DW_MASTER_CONTINUE = 0,
    I2C_DW_MASTER_BREAK,
    I2C_DW_MASTER_NEXT,
};

extern void i2c_dw_common_disable_int(struct i2c_adapter *adap);
extern int i2c_dw_common_idle(struct i2c_adapter *adap);
extern void i2c_dw_common_disable_no_wait(struct i2c_adapter *adap);
extern int i2c_dw_common_disable(struct i2c_adapter *adap);
extern void i2c_dw_common_enable(struct i2c_adapter *adap);
extern void i2c_dw_common_abort(struct i2c_adapter *adap);
extern void i2c_dw_common_configure_sda_hold(struct i2c_adapter *adap, unsigned int sda_hold,
                                             unsigned int sda_hold_ratio, unsigned int is_master);
extern unsigned int i2c_dw_common_rc_intrbits(struct i2c_adapter *adap);
extern unsigned int i2c_dw_common_scl_hcnt(unsigned long long clk_khz, unsigned long long t_high,
                                           unsigned long long tf, int cond, long long offset);
extern unsigned int i2c_dw_common_scl_lcnt(unsigned long long clk_khz, unsigned long long t_low,
                                           unsigned long long tf, long long offset);

#endif /* __I2C_DW_H__ */