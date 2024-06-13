// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * API of BT driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _BT_USR_API_H_
#define _BT_USR_API_H_

#define BT_DEVNAME          "/dev/bt"

#define MAX_BT_MSG_SIZE     (248 + 4)

#define BT_RESP_TIMEOUT     4000

#define IPMI_MAX_MSG_LENGTH 256

#define BT_IOC_MAGIC  'b'
#define BT_CMD_MAXNR  0x5
#define BT_CMD_INIT   _IOWR(BT_IOC_MAGIC, 0x00, int)
#define BT_CMD_CLOSE  _IOWR(BT_IOC_MAGIC, 0x01, int)
#define BT_CMD_READ   _IOWR(BT_IOC_MAGIC, 0x02, int)
#define BT_CMD_WRITE  _IOWR(BT_IOC_MAGIC, 0x03, int)
#define BT_CMD_SETATN _IOWR(BT_IOC_MAGIC, 0x04, int)

#pragma pack(1)

struct bt_buf_info {
    unsigned int timeout;
    char *buffer;
    unsigned int len;
    unsigned int bt_id;
};

#pragma pack()
#endif
