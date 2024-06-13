/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: uart8250
 * Author: xxx
 * Create: 2022-11-16
 */
#ifndef UART8250_H
#define UART8250_H

#include <drivers/console.h>

#define UART_RBR       0x00
#define UART_DLL       0x00
#define UART_THR       0x00
#define UART_DLH       0x04
#define UART_IER       0x04
#define UART_FCR       0x08
#define UART_LCR       0x0c
#define UART_MCR       0x10
#define UART_LSR       0x14
#define UART_HIGHSPEED 0x24

#define UART_FCR_FIFO_EN    0x01
#define UART_FCR_CLEAR_RCVR 0x02
#define UART_FCR_CLEAR_XMIT 0x04

#define UART_LCR_WLS_8 0x03
#define UART_LCR_DLAB  0x80

#define UART_MCR_DTR 0x01
#define UART_MCR_RTS 0x02

#define UART_LSR_DR   0x01
#define UART_LSR_THRE 0x20

#define CONSOLE_T_8250_BASE    CONSOLE_T_DRVDATA

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct {
    console_t console;
    uintptr_t base;
} console_8250_t;

/*
 * Initialize a new PL011 console instance and register it with the console
 * framework. The |console| pointer must point to storage that will be valid
 * for the lifetime of the console, such as a global or static local variable.
 * Its contents will be reinitialized from scratch.
 */
int console_8250_register(uintptr_t baseaddr, uint32_t clock, uint32_t baud,
                          console_8250_t *console);

#endif /* __ASSEMBLER__ */

#endif /* UART8250_H */
