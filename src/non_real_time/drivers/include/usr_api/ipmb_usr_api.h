// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * ipmb user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _IPMB_USR_API_H_
#define _IPMB_USR_API_H_

#define DEV_IPMB_NAME "/dev/ipmb"

#define IPMB_MIN_CHANNEL 0
#define IPMB_MAX_CHANNEL 3
#define MAX_MSG_LEN      255
#define MAX_IPMB_NUM     4

#define IPMB_IOC_MAGIC      'i'
#define IPMB_CMD_INIT       _IOW(IPMB_IOC_MAGIC, 0, int)
#define IPMB_CMD_READ       _IOR(IPMB_IOC_MAGIC, 1, int)
#define IPMB_CMD_WRITE      _IOWR(IPMB_IOC_MAGIC, 2, int)
#define IPMB_CMD_GET_ADDR   _IOR(IPMB_IOC_MAGIC, 3, int)
#define IPMB_CMD_SET_ADDR   _IOW(IPMB_IOC_MAGIC, 4, int)
#define IPMB_CMD_GET_STATUS _IOR(IPMB_IOC_MAGIC, 5, int)
#define IPMB_CMD_CHECK      _IOR(IPMB_IOC_MAGIC, 6, int)
#define IPMB_CMD_RESET      _IOW(IPMB_IOC_MAGIC, 7, int)
#define IPMB_CMD_SET_ENABLE _IOW(IPMB_IOC_MAGIC, 8, int)
#define IPMB_CMD_GET_ENABLE _IOR(IPMB_IOC_MAGIC, 9, int)
#define IPMB_IOC_MAXNR      (9)

typedef struct {
    u32 bus;
    u32 len;
    u32 timeout_count;
    u8 *buf;
} ipmb_msg_s, *pipmb_msg;

typedef struct {
    u32 bus;
    u32 speed;
    u32 slave_addr;
} ipmb_init_s, *pipmb_init_s;

typedef struct {
    u32 bus;
    u32 slave_addr;
} ipmb_addr_s, *pipmb_addr_s;

typedef struct {
    u32 bus;
    u8 status;
} ipmb_status_s, *pipmb_status_s;

typedef struct {
    u32 bus;
    u32 enable;
} ipmb_enable_s, *pipmb_enable_s;

typedef struct {
    u32 timeout;
    u8 mask_read[MAX_IPMB_NUM];
    u8 data_ready[MAX_IPMB_NUM];
    u32 reserved;
} ipmb_check_info_s, *pipmb_check_info_s;

#endif
