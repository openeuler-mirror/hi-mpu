// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kcs user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _KCS_USR_API_H_
#define _KCS_USR_API_H_

#define KCS_IOC_MAGIC  'k'
#define KCS_CMD_INIT   _IOWR(KCS_IOC_MAGIC, 0x00, int)
#define KCS_CMD_CLOSE  _IOWR(KCS_IOC_MAGIC, 0x01, int)
#define KCS_CMD_READ   _IOWR(KCS_IOC_MAGIC, 0x02, int)
#define KCS_CMD_WRITE  _IOWR(KCS_IOC_MAGIC, 0x03, int)
#define KCS_CMD_SETATN _IOWR(KCS_IOC_MAGIC, 0x04, int)
#define KCS_CMD_CHECK  _IOWR(KCS_IOC_MAGIC, 0x05, int)
#define KCS_CMD_MAXNR  0x6

#define MAX_KCS_NUM 4

#pragma pack(1)

typedef struct _kcs_buffer_info {
    unsigned int id;
    unsigned int timeout;
    unsigned char *buffer;
    unsigned int len;
    int reserved;
} kcs_buffer_info_s, *p_kcs_buffer_info;

typedef struct _kcs_check_info {
    unsigned int timeout;
    unsigned char mask_read[MAX_KCS_NUM];
    unsigned char data_ready[MAX_KCS_NUM];
    int reserved;
} kcs_check_info_s, *p_kcs_check_info;

#pragma pack()

#endif  // _KCS_USR_API_H_
