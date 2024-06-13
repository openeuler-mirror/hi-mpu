/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x UART core functions declaration header file
 * Create: 2018-05-10
 */
#ifndef __UART_CORE_H__
#define __UART_CORE_H__

#include "uart_regs.h"

#ifdef HOST_DEBUG
#define writel(v, c)
#define readl(c) (1)
#define EINVAL 0
#endif

#define MAX_UART_NUM (8)

/* Uart register read/write: return 0 - success */
int uart_reg_read(int uart_num, unsigned int offset, unsigned int *val);
void uart_reg_write(int uart_num, unsigned int offset, unsigned int val);
/* Uart Divisor register configuration */
void uart_set_dll_dlh(int uart_num, unsigned int dll, unsigned int dlh);
void uart_get_dll_dlh(int uart_num, unsigned int *dll, unsigned int *dlh);
/* Uart FIFO control register configuration */
void uart_set_fifo_ctrl(int uart_num, unsigned int fifo_ctrl);
/* Uart LCR register configuration */
void uart_set_lcr(int uart_num, unsigned int lcr);
/* Uart tx fifo full */
int uart_is_txfifo_full(int uart_num);
/* Uart rx data ready */
int uart_is_rx_ready(int uart_num);
/* Uart tx char */
void uart_tx_char(int uart_num, const char c);
/* Uart rx char */
void uart_rx_char(int uart_num, char *c);

#endif /* __UART_CORE_H__ */
