// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc linux driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef HISFC350H
#define HISFC350H

#include <linux/platform_device.h>

#ifdef SFC0_COMPILE
#define DEV_SFC_NAME_KER "sfc0"
#else
#define DEV_SFC_NAME_KER "sfc1"
#endif

#define SFC_IOC_MAGIC 's'
#define SFC_GET_ID  _IOR(SFC_IOC_MAGIC, 0x1, int)
#define SFC_SET_CLK _IOR(SFC_IOC_MAGIC, 0x2, int)
#define SFC_SET_FALSH_ADDR_MODE _IOR(SFC_IOC_MAGIC, 0x3, struct hisfc_flash_conf)

#define A55_CAN_ACCESS       1
#define USE_HW_LOCK          1
#define NOT_USE_HW_LOCK      0
#define SFC_DEV_NAME_MAX_LEN 8
#define PART_NAME_LEN        16

#define SFC_DMA_MASK        (~0ULL)

#define SFC_MAX_FLASH_NUM 4
#define SFC_REGION_NAME_SIZE 16
#define SFC_DTB_MATCH_NAME_SIZE 32

struct permission_info {
    u32 use_hw;
    u32 hw_id;
    u32 can_access;
};

struct sfc_id_info {
    u32 ctrl_id;
    u32 cs;
    u8 flash_id[SFC_MAX_ID_LEN];
};

struct sfc_clk_info {
    u32 ctrl_id;
    u32 clk_mhz;
};

struct hisfc_host {
    hisfc_core_host *core_host;
    struct mtd_info mtd[SFC_CS_CNT]; /* mtd[0]: cs0, mtd[1]: cs1 */
    s32 add_partition;
    u32 bus;
    struct permission_info permis_info;
    struct platform_device *pltdev;
    s32 (*suspend)(struct platform_device *pltdev, pm_message_t state);
    s32 (*resume)(struct platform_device *pltdev);
};

#define MAX_MTD_PARTITIONS (32)

struct partition_entry {
    s8 name[PART_NAME_LEN];
    unsigned long long start;
    unsigned long long length;
    u32 flags;
};

struct hisfc_flash_conf {
    u32 ctrl_id;
    u32 cs;
    u32 addrcycle; /* Indicates the address mode, which can be 3-byte or 4-byte. */
};

s32 lp_subsys_reg_read_write(u32 reg_offset, u32 reg_mask, u32 val);
void hisfc_drv_modify_mtd_cb(struct hisfc_host *host_info, u32 bus, u32 cs, u32 is_write, u32 mtd_flag);
u32 hisfc_get_config_clk_frequency(void);

#endif /* HISFC350H */
