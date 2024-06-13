// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * uart user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _UART_HI1711_H
#define _UART_HI1711_H

#define DEV_NAME   "uart_connect"

struct uart_connect_allinfo {
    unsigned int uart;
    unsigned int port_or_uart;
};

#endif