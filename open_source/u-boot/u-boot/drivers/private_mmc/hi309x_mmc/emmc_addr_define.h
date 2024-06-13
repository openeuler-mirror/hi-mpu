// ******************************************************************************
// Copyright     :  Copyright (C) 2018, Hisilicon Technologies Co. Ltd.
// File name     :  emmc_addr_define.h
// Project line  :  Platform And Key Technologies Development
// Department    :  CAD Development Department
// Author        :  xxx
// Version       :  1
// Date          :  2013/3/10
// Description   :  The description of xxx project
// Others        :  Generated automatically by nManager V4.1
// History       :  xxx 2018/08/20 19:56:56 Create file
// ******************************************************************************

#ifndef __EMMC_ADDR_DEFINE_H__
#define __EMMC_ADDR_DEFINE_H__

// ///////////////////////////////////////////
// //    global parameters, begin         ////
// ///////////////////////////////////////////
#define EMMC_BASE_ADDR 0x8630000

// //             offset address for EMMC registers
#define MCI_CTRL 0x00
#define MCI_PWREN 0x04
#define MCI_CLKDIV 0x08
#define MCI_CLKSRC 0x0C
#define MCI_CLKENA 0x10
#define MCI_TIMEOUT 0x14
#define MCI_CTYPE 0x18
#define MCI_BLKSIZ 0x1c
#define MCI_BYTCNT 0x20
#define MCI_INTMASK 0x24
#define MCI_CMDARG 0x28
#define MCI_CMD 0x2C
#define MCI_RESP0 0x30
#define MCI_RESP1 0x34
#define MCI_RESP2 0x38
#define MCI_RESP3 0x3C
#define MCI_MINTSTS 0x40
#define MCI_RINTSTS 0x44
#define MCI_STATUS 0x48
#define MCI_FIFOTH 0x4C
#define MCI_CDETECT 0x50
#define MCI_WRTPRT 0x54
#define MCI_GPIO 0x58
#define MCI_TCBCNT 0x5C
#define MCI_TBBCNT 0x60
#define MCI_DEBNCE 0x64
#define MCI_USRID 0x68
#define MCI_VERID 0x6C
#define MCI_HCON 0x70
#define MCI_UHS_REG 0x74
#define MCI_BMOD 0x80
#define MCI_DBADDR 0x88
#define MCI_IDSTS 0x8C
#define MCI_IDINTEN 0x90
#define MCI_DSCADDR 0x94
#define MCI_BUFADDR 0x98
#define EMMC_CARDTHRCTL_OFFSET_ADDR 0x100
#define EMMC_EMMC_DDR_REG_OFFSET_ADDR 0x10C
#define EMMC_ENABLE_SHIFT_OFFSET_ADDR 0x110
#define EMMC_TUNING_CONTROL_OFFSET_ADDR 0x118
#define EMMC_DATA_OFFSET_ADDR 0x200

#define BMOD_SWR (1 << 0)
#define BURST_8 (0x2 << 8)

#define CTRL_RESET (1 << 0)
#define FIFO_RESET (1 << 1)
#define DMA_RESET (1 << 2)
#define INTR_EN (1 << 4)
#define USE_INTERNAL_DMA (1 << 25)
#define USE_INTERNAL_SLAVE 0xFDFFFFFF

#define DMA_BUFFER 0x2000
#define MAX_DMA_DES (25600)

#define HIMCI_CARD0 (1 << 0)

#define DATA_TIMEOUT (0xffffffu << 8)
#define RESPONSE_TIMEOUT 0xff

#define CCLK_ENABLE (1 << 0)
#define CCLK_DISABLE (0xffff0000)

#define DMA_DES_OWN (1u << 31)
#define DMA_DES_NEXT_DES (1 << 4)
#define DMA_DES_FIRST_DES (1 << 3)
#define DMA_DES_LAST_DES (1 << 2)

#define BMOD_DMA_EN (1 << 7)

#define CARD_WIDTH_4 (0x1 << 0)
#define CARD_WIDTH_8 (0x1 << 16)

/* MCI_INTMASK(0x24) details:
   bit 16-1: mask MMC host controller each interrupt
*/
#define ALL_INT_MASK 0xffff
#define DTO_INT_MASK (1 << 3)
#define DCRC_INT_MASK (1 << 7)
#define DRTO_INT_MASK (1 << 9)
#define HTO_INT_MASK (1 << 10)
#define FRUN_INT_MASK (1 << 11)
#define SBE_INT_MASK (1 << 13)
#define EBE_INT_MASK (1 << 15)

#define DMA_INT_MASK 0x337
#define RI_INI_MASK (1 << 1)
#define TI_INI_MASK (1 << 0)


/* MCI_CMD(0x2c) details:
   bit 31: cmd execute or load start param of interface clk bit
*/
#define START_CMD (1u << 31)
#define ENABLE_BOOT (1 << 24)
#define EXPECT_BOOT_ACK (1 << 25)
#define DATA_EXPECTED (1 << 9)

/* MCI_BLKSIZ(0x1c) details:
   bit 15-0: block size
 */

#define BLOCK_SIZE 0x200

#define SDIO_INT_STATUS (0x1 << 16)

#define EBE_INT_STATUS (0x1 << 15)

#define ACD_INT_STATUS (0x1 << 14)

#define SBE_INT_STATUS (0x1 << 13)

#define HLE_INT_STATUS (0x1 << 12)

#define FRUN_INT_STATUS (0x1 << 11)

#define HTO_INT_STATUS (0x1 << 10)

#define DRTO_INT_STATUS (0x1 << 9)

#define RTO_INT_STATUS (0x1 << 8)

#define DCRC_INT_STATUS (0x1 << 7)

#define RCRC_INT_STATUS (0x1 << 6)

#define RXDR_INT_STATUS (0x1 << 5)

#define TXDR_INT_STATUS (0x1 << 4)

#define DTO_INT_STATUS (0x1 << 3)

#define CD_INT_STATUS (0x1 << 2)

#define RE_INT_STATUS (0x1 << 1)


/* MCI_RINTSTS(0x44) details:bit 16-1: clear
   MMC host controller each interrupt but
   hardware locked write error interrupt
*/
#define ALL_INT_CLR 0xffff

#define AIS_INT_STATUS (0x1 << 9)
#define NIS_INT_STATUS (0x1 << 8)
#define CES_INT_STATUS (0x1 << 5)
#define DU_INT_STATUS (0x1 << 4)
#define FBE_INT_STATUS (0x1 << 2)
#define RI_INT_STATUS (0x1 << 1)
#define TI_INT_STATUS (0x1 << 0)

#define DMA_INT_CLR 0x337

#define DATA_BUSY (0x1 << 9)

#define BURST_SIZE (0x2 << 28)
#define RX_WMARK (0x7f << 16)
#define TX_WMARK 0x80

#define LP_SUBSYS_CSR_BASE_ADDR 0x0871D000
#define LPSC_EMMC_CRG_CTRL_OFFSET_ADDR 0x00F8

// $Log$
#endif // __EMMC_ADDR_DEFINE_H__
