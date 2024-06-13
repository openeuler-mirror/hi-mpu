// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hisilicon MMC Host driver API
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _HI_MCI_H_
#define _HI_MCI_H_
#include <linux/types.h>
#include <linux/spinlock_types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/of.h>

#include <linux/mmc/host.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>

#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/freezer.h>
#include <asm/dma.h>
#include <asm/irq.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <linux/../../drivers/mmc/core/core.h>
#include <linux/../../drivers/mmc/core/mmc_ops.h>
#endif

#include "hi_mci_reg.h"

#define himci_readl(addr)           readl(addr)
#define himci_writel(v, addr)       writel((v), (addr))

#define EMMC_GET_HW_LOCK_RETRY_COUNT    10000
#define MMC_RETRY_WAIT_CARD_IDLE        400000

#define HI_MCI_DETECT_TIMEOUT  (HZ)
#define HI_MCI_REQUEST_TIMEOUT (5 * HZ)
#define MAX_RETRY_COUNT        1000
#define MMC_BLK_SIZE           512
#define EMMC_FIFO_DEPTH        1024

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
#define MMC_RSP_R1_NO_CRC      (MMC_RSP_R1 & (~MMC_RSP_CRC))
#endif

#define FIFO_RX_MASK 0x1ff0000
#define FIFO_TX_MASK 0x1ff

#define MMC_MAX_PAGE_COUNT  16
#define MMC_MAX_BLOCK_COUNT 32768
#define MMC_MAX_BLOCK_SIZE  512
#define MMC_MAX_SEGMENTS    128

#define MMC_MAX_HW_SEGMENTS        1024
#define MMC_MAX_PHYS_SEGMENTS      1024

#define POWER_ON  1
#define POWER_OFF 0

#define CARD_UNPLUGED 1
#define CARD_PLUGED   0

#define ENABLE  1
#define DISABLE 0

#define LP_EMMC_MASK ((0x1) << 1)

#define HALF_BLOCK_SIZE 256
#define KEY_MAC_SIZE    32
#define RANDOM_SIZE     16
#define PACKET_SIZE     512
#define RELIABLE_WR     0
#define NORMAL_WR       1

/* Related to eMMC registers */
#define EXT_CSD                 0
#define CID                     1
#define CSD                     2
#define MAX_EXT_CSD_OFFSET      511
#define EXT_CSD_COUNT           512
#define CSD_COUNT               16
#define CSD_OFFSET              0
#define CID_COUNT               16
#define CID_OFFSET              0
#define MMC_STATUS_MASK         (~0x0206BF7F)
#define MMC_STATUS_RDY_FOR_DATA BIT(8)
#define MMC_STATUS_CURR_STATE   (0xf << 9)
#define MMC_STATE_PRG           (7 << 9)

/* Address extension */
#define SDIO_ADDR_REMAP_MODE 0x1
#define SDIO_HADDR_OFFSET    0x8
#define EMMC_ADDR_REMAP_MODE 0x1
#define EMMC_HADDR_OFFSET    0x8

/* Sets the internal DMA support bus address bit 34 */
#define MMC_DMA_BIT_MAST	 34

#define DMA_DONE 0x80000000

#define EMMC_CLK_BASE 100000000
#define SDIO_CLK_BASE 25000000

#define EMMC_CCLK_MAX 100000000
#define SDIO_CCLK_MAX 25000000
#define MMC_CCLK_MIN  60000

#define HYNIX_CMD60_ARG1 0x534D4900
#define HYNIX_CMD60_ARG2 0x48525054
#define CMD60            60
#define MMC_CLEAR        1

#define MAX_CMD 63

/* Base address of SD card register */
#define HI_MCI_INTR (48 + 32)

#define MMC_HOST_BUS_WIDTH_1 1
#define MMC_HOST_BUS_WIDTH_4 4
#define MMC_HOST_BUS_WIDTH_8 8

#define MMC_COMPATIBILITY_MODE 0 /* Compatible mode */
#define MMC_HS_SDR_MODE        1
#define MMC_HS_DDR_MODE        2
#define MMC_HS200_SDR_MODE     3
#define MMC_HS400_DDR_MODE     4

/* Macro Definitions Related to GPP Partitions */
#define GPP_MAX_SIZE       (1024 * 1024) /* The maximum GPP size is 1 GB.*/
#define GPP_MAX_NUM        4
#define GPP0_AREA_INDEX    4
#define GP_MULT_GP_NUM     3
#define GPP_COMPLETED      0x1
#define ERASE_GROUP_ENABLE 0x1
#define SYSTEM_CODE_ATTR   0x11

#define EMMC_AREA_MAX_NUM         7
#define EMMC_SEND_CMD_RETRY_COUNT 10000

#define PARTITIONING_EN  BIT(0)
#define EXT_ATTRIBUTE_EN BIT(2)

/* Extended Register Configuration Partition Attributes */
#define EXT_PARTITIONS_ATTRIBUTE0 52
#define EXT_PARTITIONS_ATTRIBUTE1 53

/* TUNING CONTROL bits */
#define HISI_MMC_FIND_EDGE         BIT(5)
#define HISI_MMC_TUNING_EDGE_CTR   BIT(1)
#define HISI_MMC_ENABLE_TUNING     BIT(0)

/* Command register defines */
#define HISI_MMC_CMD_START         BIT(31)
#define HISI_MMC_CMD_USE_HOLD_REG  BIT(29)
#define HISI_MMC_CMD_VOLT_SWITCH   BIT(28)
#define HISI_MMC_CMD_CCS_EXP       BIT(23)
#define HISI_MMC_CMD_CEATA_RD      BIT(22)
#define HISI_MMC_CMD_UPD_CLK       BIT(21)
#define HISI_MMC_CMD_INIT          BIT(15)
#define HISI_MMC_CMD_STOP          BIT(14)
#define HISI_MMC_CMD_PRV_DAT_WAIT  BIT(13)
#define HISI_MMC_CMD_SEND_STOP     BIT(12)
#define HISI_MMC_CMD_STRM_MODE     BIT(11)
#define HISI_MMC_CMD_DAT_WR        BIT(10)
#define HISI_MMC_CMD_DAT_EXP       BIT(9)
#define HISI_MMC_CMD_RESP_CRC      BIT(8)
#define HISI_MMC_CMD_RESP_LONG     BIT(7)
#define HISI_MMC_CMD_RESP_EXP      BIT(6)

#define SDIO_SRST_MASK          BIT(3)

#define EMMC_EXTEND_ADDR_BIT_MASK   0x3f000
#define SDIO_EXTEND_ADDR_BIT_MASK   0xfc0000

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#define MMC_CAPS_MASK (MMC_CAP_4_BIT_DATA | MMC_CAP_SD_HIGHSPEED)
#else
#define MMC_CAPS_MASK (MMC_CAP_4_BIT_DATA | MMC_CAP_ERASE | MMC_CAP_SD_HIGHSPEED)
#endif

/* Internal DMA Descriptor */
struct himci_des {
    u32 idmac_des_ctrl;
    u32 idmac_des_buf_size;
    u32 idmac_des_buf_addr;
    u32 idmac_des_next_addr;
};

struct himci_host {
    struct mmc_host *mmc;
    spinlock_t lock;
    struct mmc_request *mrq;
    struct mmc_command *cmd;
    struct mmc_data *data;
    void __iomem *base;
    u32 card_status;
    struct scatterlist *dma_sg;
    u32 dma_sg_num;
    u32 dma_alloc_size;
    u32 dma_dir;
    dma_addr_t dma_paddr;
    u32 *dma_vaddr;
    struct timer_list timer;
    s32 irq;
    wait_queue_head_t intr_wait;
    ulong pending_events;
    u32 block_count;
    u32 card_type;
    struct mutex m_lock;

#define HIMCI_PEND_DTO_B 0
#define HIMCI_PEND_DTO_M (1 << HIMCI_PEND_DTO_B)
};

#endif