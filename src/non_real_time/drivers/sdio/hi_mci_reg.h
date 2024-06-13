// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hi_mci_reg API
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _HI_MCI_REG_H_
#define _HI_MCI_REG_H_

#define CONFIG_HIMCI_IOBASE 0x10030000
#define CONFIG_HIMCI_INTR   0x2e
#define HI_MCI_IO_SIZE      0x1000

#define HISI_MCI_CTRL           0x00
#define HISI_MCI_PWREN          0x04
#define HISI_MCI_CLKDIV         0x08
#define HISI_MCI_CLKSRC         0x0C
#define HISI_MCI_CLKENA         0x10
#define HISI_MCI_TIMEOUT        0x14
#define HISI_MCI_CTYPE          0x18
#define HISI_MCI_BLKSIZ         0x1c
#define HISI_MCI_BYTCNT         0x20
#define HISI_MCI_INTMASK        0x24
#define HISI_MCI_CMDARG         0x28
#define HISI_MCI_CMD            0x2C
#define HISI_MCI_RESP0          0x30
#define HISI_MCI_RESP1          0x34
#define HISI_MCI_RESP2          0x38
#define HISI_MCI_RESP3          0x3C
#define HISI_MCI_MINTSTS        0x40
#define HISI_MCI_RINTSTS        0x44
#define HISI_MCI_STATUS         0x48
#define HISI_MCI_FIFOTH         0x4C
#define HISI_MCI_CDETECT        0x50
#define HISI_MCI_WRTPRT         0x54
#define HISI_MCI_GPIO           0x58
#define HISI_MCI_TCBCNT         0x5C
#define HISI_MCI_TBBCNT         0x60
#define HISI_MCI_DEBNCE         0x64
#define HISI_MCI_USRID          0x68
#define HISI_MCI_VERID          0x6C
#define HISI_MCI_HCON           0x70
#define HISI_MCI_UHS_REG        0x74
#define HISI_MCI_BMOD           0x80
#define HISI_MCI_DBADDR         0x88
#define HISI_MCI_IDSTS          0x8C
#define HISI_MCI_IDINTEN        0x90
#define HISI_MCI_DSCADDR        0x94
#define HISI_MCI_BUFADDR        0x98
#define HISI_MCI_CARDTHRCTL     0x100
#define HISI_MCI_DDR_REG        0x10C
#define HISI_MCI_TUNING_CONTROL 0x118

/* EXT CSD */
#define EXT_CSD_USER_WP 171

/* MCI_BMOD(0x80) details */
#define BMOD_SWR     BIT(0)
#define BURST_LENGTH (2 << 8)
#define BURST_TYPE   BIT(1)

/* MCI_CTRL(0x00) details */
#define CTRL_RESET         BIT(0)
#define FIFO_RESET         BIT(1)
#define DMA_RESET          BIT(2)
#define INTR_EN            BIT(4)
#define USE_INNER_DMA      BIT(25)
#define USE_INNER_SLAVE 0xFDFFFFFF

/* IDMAC DEST1 details */
#define DMA_BUFFER 0x2000

/* MCI_CDETECT(0x50) details */
#define HIMCI_CARD0 BIT(0)

/* MCI_TIMEOUT(0x14) details: */
#define DATA_TIMEOUT     (0xffffffu << 8) /* bit 31-8: data read timeout param */
#define RESPONSE_TIMEOUT 0xff             /* bit 7-0: response timeout param */

/* bit 0: enable of card clk */
#define CCLK_ENABLE  BIT(0)
#define CCLK_DISABLE 0xffff0000

/* IDMAC DEST0 details */
#define DMA_DES_OWN       BIT(31)
#define DMA_DES_NEXT_DES  BIT(4)
#define DMA_DES_FIRST_DES BIT(3)
#define DMA_DES_LAST_DES  BIT(2)

/* MCI_BMOD(0x80) details */
#define BMOD_DMA_EN BIT(7)

/* MCI_CTYPE(0x18) details */
#define CARD_WIDTH_1 BIT(0)
#define CARD_WIDTH_0 BIT(16)

#define DDR_MODE_ENABLE BIT(16)

/* MCI_INTMASK(0x24) details:
   bit 16-1: mask MMC host controller each interrupt
 */
#define ALL_INT_MASK  0x1ffff
#define DTO_INT_MASK  BIT(3)
#define TXRD_INT_MASK BIT(4)
#define RXRD_INT_MASK BIT(5)

/* MCI_CMD(0x2c) details:
   bit 31: cmd execute or load start param of interface clk bit
 */
#define START_CMD BIT(31)

/* MCI_INTSTS(0x44) details */
/***************************************************************/
/* bit 16: sdio interrupt status */
#define SDIO_INT_STATUS BIT(16)

/* bit 15: end-bit error (read)/write no CRC interrupt status */
#define EBE_INT_STATUS BIT(15)

/* bit 14: auto command done interrupt status */
#define ACD_INT_STATUS BIT(14)

/* bit 13: start bit error interrupt status */
#define SBE_INT_STATUS BIT(13)

/* bit 12: hardware locked write error interrupt status */
#define HLE_INT_STATUS BIT(12)

/* bit 11: FIFO underrun/overrun error interrupt status */
#define FRUN_INT_STATUS BIT(11)

/* bit 10: data starvation-by-host timeout interrupt status */
#define HTO_INT_STATUS BIT(10)

/* bit 9: data read timeout interrupt status */
#define DRTO_INT_STATUS BIT(9)

/* bit 8: response timeout interrupt status */
#define RTO_INT_STATUS BIT(8)

/* bit 7: data CRC error interrupt status */
#define DCRC_INT_STATUS BIT(7)

/* bit 6: response CRC error interrupt status */
#define RCRC_INT_STATUS BIT(6)

/* bit 5: receive FIFO data request interrupt status */
#define RXDR_INT_STATUS BIT(5)

/* bit 4: transmit FIFO data request interrupt status */
#define TXDR_INT_STATUS BIT(4)

/* bit 3: data transfer Over interrupt status */
#define DTO_INT_STATUS BIT(3)

/* bit 2: command done interrupt status */
#define CD_INT_STATUS BIT(2)

/* bit 1: response error interrupt status */
#define RE_INT_STATUS BIT(1)

/***************************************************************/
/* MCI_RINTSTS(0x44) details:bit 16-1: clear
   MMC host controller each interrupt but
   hardware locked write error interrupt
 */
#define ALL_INT_CLR 0xffff

/* MCI_STATUS(0x48) details */
#define DATA_BUSY BIT(9)

/* MCI_FIFOTH(0x4c) details */
#define BURST_SIZE (0x2 << 28)
#define RX_WMARK   (0x7f << 16)
#define TX_WMARK   (0x80)

#define CARD_READ_THRESHHOLD  (0x200 << 16)
#define BUSY_CLEAR_INT_ENABLE BIT(1)
#define CARD_READ_THR_EN      BIT(0)
#endif