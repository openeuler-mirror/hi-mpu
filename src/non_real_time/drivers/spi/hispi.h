// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spi controller driver header file
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SPIDRV_H__
#define __SPIDRV_H__

#define SPI_CHIP_SELCET_TOTAL_NUM 2
#define SPI_MIN_DIV               6
#define SPI_MAX_DIV               65534

typedef enum {
    SPI_SLAVE_0_E = 0,
    SPI_SLAVE_1_E = 1
} spi_slave_sel_e;

#define SPI_CTRLR0_REG_OFFSET 0x0000
#define SPI_CTRLR1_REG_OFFSET 0x0004

#define SPI_SSIENR_REG_OFFSET 0x0008
#define SPI_SER_REG_OFFSET    0x0010
#define SPI_BAUDR_REG_OFFSET  0x0014

#define SPI_TXFTLR 0x0018
#define SPI_RXFTLR 0x001c
#define SPI_TX_NUM 0x0020
#define SPI_RX_NUM 0x0024

#define SPI_SR_REG_OFFSET  0x0028
#define SPI_IMR_REG_OFFSET 0x002C
#define SPI_ISR_REG_OFFSET 0x0030
#define SPI_CONST_ISR_REG  0x0034

#define SPI_CLEAN_TX_ISR_REG 0x0038
#define SPI_CLEAN_RX_ISR_REG 0x003c

#define SPI_ICR_REG_OFFSET 0x0048
#define SPI_DR0_REG_OFFSET 0x0060

/* SPI Register Address Definition */
#define SPI_CTRLR0_REG_ADDR (SPI_CTRLR0_REG_OFFSET)
#define SPI_CTRLR1_REG_ADDR (SPI_CTRLR1_REG_OFFSET)
#define SPI_SSIENR_REG_ADDR (SPI_SSIENR_REG_OFFSET)
#define SPI_SER_REG_ADDR    (SPI_SER_REG_OFFSET)
#define SPI_BAUDR_REG_ADDR  (SPI_BAUDR_REG_OFFSET)

#define SPI_IMR_REG_ADDR (SPI_IMR_REG_OFFSET)
#define SPI_ISR_REG_ADDR (SPI_ISR_REG_OFFSET)
#define SPI_ICR_REG_ADDR (SPI_ICR_REG_OFFSET)
#define SPI_DR0_REG_ADDR (SPI_DR0_REG_OFFSET)

#define SPI_DEFAULT_FRAME_FORMAT 0 /* Motorola SPI Frame Format */

#define SPI_SEND_FIFO_FULL      0
#define SPI_SEND_FIFO_NOT_FULL  1
#define SPI_SEND_FIFO_EMPTY     1
#define SPI_SEND_FIFO_NOT_EMPTY 0

#define SPI_RECV_FIFO_NOT_FULL  0
#define SPI_RECV_FIFO_FULL      1
#define SPI_RECV_FIFO_EMPTY     0
#define SPI_RECV_FIFO_NOT_EMPTY 1

#define SPI_TRAS_MODE_DUPLEX    0
#define SPI_TRAS_MODE_SEND      1
#define SPI_TRAS_MODE_RECEIVE   2
#define SPI_TRAS_MODE_EEPROM_RD 3

#define SPI_LOOP_BACK     1
#define SPI_NOT_LOOP_BACK 0

#define SPI_START_RECEIVE 0x5a5a

#define SPI_IDLE                  0
#define SPI_BUSY                  1
#define SPI_DEFAULT_FREQ_DIVISION 0x18
#define SPI_INT_MASK              (~0x3f)
#define SPI_MIN_UNIT_NUM          1
#define SPI_MAX_UNIT_NUM          1000

#define MAX_CMD_LEN      256
#define READ_CMD_BUF_LEN 32
#define MAX_IRQ_NAME_LEN 20

typedef enum {
    SPI_NON_EEPROM_RD_E = 0,
    SPI_EEPROM_RD_E
} spi_read_mode_e;


#define drv_32bit_mask(n1, n2) (BIT((n2) + 1) - BIT(n1))
#define drv_32bit_set_field(v1, n1, n2, v2) \
    (((u32)(v1) & (~drv_32bit_mask((n1), (n2)))) | (((u32)(v2) << (u32)(n1)) & drv_32bit_mask((n1), (n2))))

#ifndef drv_32bit_reg_set_field
#define drv_32bit_reg_set_field(reg, n1, n2, v)            \
    do {                                                   \
        u32 data;                                          \
        data = readl(reg);                                 \
        data = drv_32bit_set_field(data, (n1), (n2), (v)); \
        writel(data, reg);                                 \
    } while (0)
#endif

#ifndef DRV_32BIT_READ_BIT
#define DRV_32BIT_READ_BIT(v, n) (((u32)(v) >> (n)) & 0x00000001)
#endif

#define ctrlr0_reserved1(value) ((value) << 16)
#define ctrlr0_cfs(value)       ((value) << 12)
#define ctrlr0_srl(value)       ((value) << 11)
#define ctrlr0_reserved2(value) ((value) << 10)
#define ctrlr0_tmod(value)      ((value) << 8)
#define ctrlr0_scpol(value)     ((value) << 7)
#define ctrlr0_scph(value)      ((value) << 6)
#define ctrlr0_frf(value)       ((value) << 4)
#define ctrlr0_dfs(value)       ((value) << 0)

#define CTRLR0_SRL_FIELD_BIT 11
#define CTRLR0_TMOD_FIELD_START_BIT 8
#define CTRLR0_TMOD_FIELD_END_BIT   9
#define CTRLR0_SCPOL_FIELD_BIT 7
#define CTRLR0_SCPH_FIELD_BIT 6
#define CTRLR0_FRF_FIELD_START_BIT 4
#define CTRLR0_FRF_FIELD_END_BIT   5
#define CTRLR0_DFS_FIELD_START_BIT 0
#define CTRLR0_DFS_FIELD_END_BIT   3

#define SPI_EN    0x1
#define SPI_DISEN 0x0

#define SER_REG_VALUE_NON_SLAVE 0
#define SER_REG_VALUE_SLAVE_0   BIT(0)
#define SER_REG_VALUE_SLAVE_1   BIT(1)

#define DEV_SPI_NAME "spi"

#define SPI_LOG_SW_TO_ADISPI
#define SPI_LOG_SW_TO_SSPI

#define TX_FIFO_SIZE 32

#define TX_CMD_SIZE  3
#define TX_DATA_SIZE 24
#define RX_DATA_SIZE 24

#define SEND_LINE_VAL 0

#define SPI_IRQ_NAME_LEN 24

#define INT_MASK_TX_EMPTY 0x01
#define INT_MASK_RX_FULL  0x10
#define INT_MASK_RX_OVER  0x08

#define ISR_TX_EMPTY     0x01
#define ISR_RX_OVERFLOW  0x08
#define ISR_RX_FULL      0x10

struct hisi_spi {
    s32 id;
    struct device *dev;
    u32 v_irq;
    u32 clock_sel;
    phys_addr_t phys_base;
    unsigned long phys_size;
    void __iomem *virt_base;
    s8 irq_name[SPI_IRQ_NAME_LEN];
    wait_queue_head_t w_select_wait;
    wait_queue_head_t r_select_wait;
    volatile s32 w_wait_flags;
    volatile s32 r_wait_flags;
    u8 *rx_text_pos;
    u32 rx_text_len;
    struct semaphore spi_bus_sem;
};

#define HISI_SPI_MAX_CTL 2

struct hisi_spi_mgr {
    s32 init_flag[HISI_SPI_MAX_CTL];
    struct hisi_spi hispi[HISI_SPI_MAX_CTL];
};

#define HISPI_FRAME_LENGTH_4_BIT (0x3)

extern s32 spi_process(u32 cmd, unsigned long arg);
extern s32 kernspi_init(dev_spi_ioctl_std_init *ioctl_init);
extern s32 kernspi_read_data(dev_spi_id *spi_id, u8 *cmd, u32 cmd_len, u8 *data, u32 data_len);
extern s32 kernspi_write_data(dev_spi_id *spi_id, u8 *cmd, u32 cmd_len);

#endif
