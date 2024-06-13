// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * dual boot startup information interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __DBOOT_USR_API_H__
#define __DBOOT_USR_API_H__

/* DBOOT command word */
#define DBOOT_CMD_MAGIC        'd'
#define DBOOT_CMD_GET_INFO     _IOR(DBOOT_CMD_MAGIC, 0, int)
#define DBOOT_CMD_VER_INFO     _IOR(DBOOT_CMD_MAGIC, 1, int)
#define DBOOT_GET_STRAP_STATUS _IOR(DBOOT_CMD_MAGIC, 3, int)
#define DBOOT_GET_SEC_FW_VER   _IOR(DBOOT_CMD_MAGIC, 4, int)
#define DBOOT_CMD_MAX_NR       4

#define VER_DATA_LEN         32
#define SECFW_VER_LEN        32
#define SECFW_BUILD_TIME_LEN 32

typedef struct {
    unsigned int cur_no;     // Indicates the ID of the current boot region. Value range: 0~1）
    unsigned int boot0_cnt;  // Number of boot times in boot0
    unsigned int boot1_cnt;  // Number of boot times in boot1
} dboot_info_s;

typedef struct {
    char uboot0_ver[VER_DATA_LEN];
    char uboot0_build_time[VER_DATA_LEN];
    char uboot1_ver[VER_DATA_LEN];
    char uboot1_build_time[VER_DATA_LEN];
} uboot_ver;

typedef struct {
    unsigned char jtag_mode;      /* 0: normal 1: JTAG mode */
    unsigned char lb_boot_width;  /* Localbus boot width, 0: 8bit 1:16bit */
    unsigned char lb_boot_offset; /* Localbus boot offset, 0:no offset 1: use offset */
    unsigned char sfc_addr_mode;  /* 0:3bytes 1:4bytes */
    unsigned char boot_size;      /* 0: 1MB 1: 2MB */
    unsigned char boot_dbios;     /* 0: diable dbios 1: enable dbios */
    unsigned char us_boot_core;   /* 0: CA55 non-secure, 1:M3 secure */
    unsigned char boot_mode;      /* 00: SPI FLASH, 01: local bus 10: ROM(eMMC) 11: reserved */
} dboot_strap_info;

typedef struct tag_sec_fw_ver_s {
    /* Version information about the active and standby areas of the L0FW */
    unsigned char l0fw_active_ver[SECFW_VER_LEN];
    unsigned char l0fw_active_build_time[SECFW_BUILD_TIME_LEN];
    unsigned char l0fw_gold_ver[SECFW_VER_LEN];
    unsigned char l0fw_gold_build_time[SECFW_BUILD_TIME_LEN];
    /* Version information about the active and standby areas of the L1FW */
    unsigned char l1fw_active_ver[SECFW_VER_LEN];
    unsigned char l1fw_active_build_time[SECFW_BUILD_TIME_LEN];
    unsigned char l1fw_gold_ver[SECFW_VER_LEN];
    unsigned char l1fw_gold_build_time[SECFW_BUILD_TIME_LEN];
} sec_fw_ver;

#endif

