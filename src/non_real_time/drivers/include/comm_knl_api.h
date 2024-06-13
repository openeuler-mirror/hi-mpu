// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module comm external interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _COMM_KNL_API_H_
#define _COMM_KNL_API_H_

/* Kernel Common Header File Definition */
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/semaphore.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/poll.h>
#include <linux/io.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/completion.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/securec.h>

#include "mem_addr.h"
#include "log_api.h"

/* Hi1711 clock frequency */
#define SYS_REF_CLOCK_FREQ	(25000000)
#define SYS_APB_CLOCK_FREQ	(100000000)
#define SYS_AHB_CLOCK_FREQ	(200000000)

#define HI1711_TEST

#ifndef init_mutex
#define init_mutex(sem)             sema_init(sem, 1)
#endif

#ifndef DECLARE_MUTEX
#define DECLARE_MUTEX               DEFINE_SEMAPHORE
#endif

#define bit_value(val, offset)      (((val) >> (offset)) & 0x1)

#define OFFSET_BYTE_NUM             0x4

/* Definition of the reset domain ID */
#define RST_ZONE_CHIP               0
#define RST_ZONE_HOST               1
#define RST_ZONE_BMC                2
#define RST_ZONE_BMC_UNSE           3
#define RST_ZONE_PCIE0              4
#define RST_ZONE_PCIE1              5

/* Sub-Reset Domain Type */
#define SUBSYS_RST_MAX_COUNT        3

#define SUBSYS_RST_HOST             0
#define SUBSYS_RST_PCIE0            1
#define SUBSYS_RST_PCIE1            2

/* Each sub-reset domain callback module */
#define SUBSYS_RST_MAX_CB_MODULES   10

#define CB_ID_BT                    0
#define CB_ID_KCS                   1
#define CB_ID_P80                   2
#define CB_ID_MCTP                  3
#define CB_ID_EDMA                  4
#define CB_ID_ESPI                  5
#define CB_ID_VCE                   6

/* Table for allocating six non-secure watchdogs */
#define WDG_NO_A55_RESET            5

/* Allocation table of six hardware timers */
#define HISITIMER_NO_FEED_A55_WDG   0
#define HISITIMER_NO_CPLD           5

/* Reset mask of each module */
/* Reset register with offset 0x4 */
#define MODULE_RST_MASK_PECI        0x80000000

/* Reset register with offset 0x8 */
#define MODULE_RST_MASK_JLC1        0x80000000
#define MODULE_RST_MASK_JLC0        0x40000000
#define MODULE_RST_MASK_SGPI1       0x20000000
#define MODULE_RST_MASK_SGPI0       0x10000000
#define MODULE_RST_MASK_MDIO        0x00200000
#define MODULE_RST_MASK_WDI         0x00008000
#define MODULE_RST_MASK_SOL         0x00004000
#define MODULE_RST_MASK_MAILBOX     0x00000800
#define MODULE_RST_MASK_C80         0x00000400
#define MODULE_RST_MASK_BT1         0x00000200
#define MODULE_RST_MASK_BT0         0x00000100
#define MODULE_RST_MASK_KCS3        0x00000080
#define MODULE_RST_MASK_KCS2        0x00000040
#define MODULE_RST_MASK_KCS1        0x00000020
#define MODULE_RST_MASK_KCS0        0x00000010
#define MODULE_RST_MASK_CAN1        0x00000008
#define MODULE_RST_MASK_CAN0        0x00000004
#define MODULE_RST_MASK_ADC0        0x00000002

/* Reset register with offset 0xc */
#define MODULE_RST_MASK_DMAC        0x00040000
#define MODULE_RST_MASK_ADC1        0x00004000
#define MODULE_RST_MASK_ESPI        0x00002000
#define MODULE_RST_MASK_UART4_SOL   0x00001000
#define MODULE_RST_MASK_UART3_SOL   0x00000800
#define MODULE_RST_MASK_UART2_SOL   0x00000400
#define MODULE_RST_MASK_WDG6        0x00000040
#define MODULE_RST_MASK_WDG5        0x00000020
#define MODULE_RST_MASK_WDG4        0x00000010
#define MODULE_RST_MASK_WDG3        0x00000008
#define MODULE_RST_MASK_WDG2        0x00000004
#define MODULE_RST_MASK_WDG1        0x00000002
#define MODULE_RST_MASK_WDG0        0x00000001

typedef int (*subsys_rst_callback)(unsigned long);

typedef struct subsys_rst_callbacks {
    subsys_rst_callback raise_handle;
    subsys_rst_callback fall_handle;
    subsys_rst_callback delay_handle;
} subsys_rst_callbacks_s;

typedef struct st_devtype {
    u32 dev_major;
    u32 dev_minor;
    struct cdev cdev;
    struct class *dev_class;
} cdev_st;

/* Creating a Character Device */
extern s32 comm_init_dev(cdev_st *pDev, const struct file_operations *pfoprs, const s8 *pdevname);

/* Deleting a Character Device */
extern void comm_cleanup_dev(cdev_st *pDev);

/* Reset domain soft reset */
extern s32 subsys_sft_rst(u8 rst_zone);

/* Module reset */
extern s32 m_reset_handle(u32 reg_offset, u32 mask);

/* Module Reset Mutually Exclusive Interface */
extern s32 m_reset_handle_atomic(u32 reg_offset, u32 mask);

/* The eMMC module is reset. */
extern s32 m_emmc_reset(u32 reg_offset, u32 mask);

/* Configuring the callback parameters of subsys rst */
extern s32 subsys_rst_callback_register(u32 subsys, u32 module_id, subsys_rst_callbacks_s *handle, u64 arg);

/* Common parameters of the ioctrl interface */
extern s32 comm_ioc_parm_check(u32 cmd, void __user *arg, u32 ioc_magic, u32 ioc_max_nr);

extern void cancel_rearming_delayed_workqueue(const struct workqueue_struct *wq, struct delayed_work *work);

#endif
