// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * uart connect user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _UART_CONNECT_USR_API_H_
#define _UART_CONNECT_USR_API_H_

#define DEV_UART_CONNECT_NAME "uart_connect"

#define UART_IOC_MAGIC 'u'
#define SET_UART_CONNECT            _IOWR(UART_IOC_MAGIC, 0, int)
#define GET_UART_CONNECT_INFO       _IOWR(UART_IOC_MAGIC, 1, int)
#define GET_PORT_CONNECT_INFO       _IOWR(UART_IOC_MAGIC, 2, int)
#define GET_HOST_BAUDRATE_INFO       _IOWR(UART_IOC_MAGIC, 3, int)
#define RESET_UART_CONNECT          _IOWR(UART_IOC_MAGIC, 4, int)

enum host_baud_status {
    HOST_UART_IDLE,
    HOST_UART_WORKING
};

typedef enum uart_connect_mode {
    UART_2_UART,
    UART_2_PORT,
    PORT_2_PORT,
    UART_DISCONNECT,
    PORT_DISCONNECT,
    INVAL_CONNECT_MODE
}uart_connect_mode_t;

struct uart_connect_cfg {
    uart_connect_mode_t mode;
    unsigned int src;
    unsigned int dst;
};

struct host_uart_baud_info {
    unsigned int host_uart_chan;
    unsigned int uart_baud;
    unsigned int *baud_constant_list;
};

#endif

