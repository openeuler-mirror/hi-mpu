// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * p80 user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _P80_USR_API_H_
#define _P80_USR_API_H_

#define P80_DEVNAME        "/dev/p80"
#define P80_IOC_MAGIC      'p'
#define P80_CMD_INIT       _IOWR(P80_IOC_MAGIC, 0x00, int)
#define P80_CMD_CLOSE      _IOWR(P80_IOC_MAGIC, 0x01, int)
#define P80_CMD_READ       _IOWR(P80_IOC_MAGIC, 0x02, int)
#define P80_CMD_GETOFFSET  _IOWR(P80_IOC_MAGIC, 0x03, int)
#define P80_CMD_GETCURDATA _IOWR(P80_IOC_MAGIC, 0x04, int)

#define P80_CMD_MAXNR    5
#define MAX_P80_DATA_NUM 256

typedef struct _p80_buffer_info {
    unsigned int timeout;
    char *buffer;
    unsigned int len;
    int reserved;
} p80_buffer_info_s, *p_p80_buffer_info;

#endif
