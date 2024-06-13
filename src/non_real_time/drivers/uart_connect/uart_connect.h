// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Implemention of uart connect module.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _UART_CONNECT__H_
#define _UART_CONNECT__H_

#include "lp_subsys_csr_addr_define.h"
#include "c80_csr_addr_define.h"
#include "uart_connect_usr_api.h"

#define UART_CONNECT_MAX_UART_NUM   8
#define UART_CONNECT_MAX_PORT_NUM   8
#define UART_SEL_REG_OFFSET         LP_SUBSYS_CSR_LPSC_UART_CTRL1_OFFSET_ADDR
#define PORT_SEL_REG_OFFSET         LP_SUBSYS_CSR_LPSC_UART_CTRL0_OFFSET_ADDR
#define UART_PORT_ENABLE_REG_OFFSET LP_SUBSYS_CSR_LPSC_UART_SEL_OFFSET_ADDR

#define STATUS_ENABLE      1
#define STATUS_DISABLE     0

#define HOST_UART_ESPI_REG  C80_CSR_ESPI_BASE_ADDR
#define HOST_UART_LPC_REG   C80_CSR_LPC_BASE_ADDR

#define HOST_UART_BAUD_REG_OFFSET  0x170

#define HOST_UART_ESPI_BAUD_REG (HOST_UART_ESPI_REG + HOST_UART_BAUD_REG_OFFSET)
#define HOST_UART_LPC_BAUD_REG  (HOST_UART_LPC_REG + HOST_UART_BAUD_REG_OFFSET)

#define HOST_UART_REG_SIZE      0x100
#define HOST_UART_CLK_RATE      1846100
#define HOST_UART_CLK_RATIO     54
#define HOST_UART_MAX_BAUD      115200
#define HOST_UART_MAX_CHAN      1

#define UART_PORT_ID_OFF    8

#define HOST_UART_BAUD_LIST_LEN 11

#endif /* _UART_CONNECT__H_ */