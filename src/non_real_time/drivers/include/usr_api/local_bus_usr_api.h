// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * local_bus user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
#ifndef _LOCAL_BUS_USR_API__H_
#define _LOCAL_BUS_USR_API__H_

#define DEV_L_BUS_NAME "localbus"

#define L_BUS_IOC_MAGIC            'l'
#define L_BUS_CMD_READ             _IOWR(L_BUS_IOC_MAGIC, 0, int)
#define L_BUS_CMD_WRITE            _IOWR(L_BUS_IOC_MAGIC, 1, int)
#define L_BUS_CMD_SET_TIMING       _IOWR(L_BUS_IOC_MAGIC, 2, int)
#define L_BUS_CMD_SET_WIDTH_OFFSET _IOWR(L_BUS_IOC_MAGIC, 3, int)

#define L_BUS_USER_MODE 0
#define L_BUS_KERNEL_MODE 1

typedef enum {
    L_BUS_CS1 = 0,
    L_BUS_CS2 = 1,
    L_BUS_CS3 = 2
} l_bus_cs_e;

typedef enum {
    L_BUS_LALE = 0,
    L_BUS_ADDR2DATA_NUM = 1,
    L_BUS_TIMING_MAX
} l_bus_timing_e;

/* Read/Write Data Structure */
typedef struct {
    unsigned int cs;
    unsigned int type;
    unsigned int offset;
    unsigned int len;
    unsigned int mask;
    unsigned char *data;
} l_bus_opt_s;

/* Time sequence parameter data structure */
typedef struct {
    unsigned int cs;
    unsigned int type;
    unsigned char value;
} localbus_timing_s;

typedef struct {
    unsigned char cs;
    unsigned char cf_addr_offset;
    unsigned char width;
} localbus_set_bitwide_s;

#endif
