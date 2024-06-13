// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Implemention of uart module.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _UART_KER_H__
#define _UART_KER_H__

#include "comm_knl_api.h"

#define UART0_REG_BASE 0x0872A000 /* LPC_UART */
#define UART1_REG_BASE 0x0872B000 /* ESPI_UART */
#define UART2_REG_BASE 0x08710000
#define UART3_REG_BASE 0x08711000
#define UART4_REG_BASE 0x08743000
#define UART5_REG_BASE 0x08744000
#define UART6_REG_BASE 0x0875D000
#define UART7_REG_BASE 0x0875E000

/* -------------------------------------------------------------------------------
 *  From DW UART Block Specification
 * -------------------------------------------------------------------------------
 *  UART Register Offsets.
 */
#define DW_UART_RBR 0x00 /* Receive buff. */
#define DW_UART_THR 0x00 /* Transmit Holding */
#define DW_UART_DLL 0x00 /* Divisor Latch Low */

#define DW_UART_DLH 0x04 /* Divisor Latch High */
#define DW_UART_IER 0x04 /* int enable */

#define DW_UART_IIR 0x08 /* interrupt indentification REG */
#define DW_UART_FCR 0x08 /* FIFO control */

#define DW_UART_LCR 0x0C /* Line control reg */

#define DW_UART_MCR 0x10 /* Modem Control reg */

#define DW_UART_LSR 0x14 /* Line  statue */
#define DW_UART_MSR 0x18 /* Modem statue */

#define DW_UART_USR 0x7C /* Uart statues */
#define DW_UART_TFL 0x80 /* Tx fifo data count */
#define DW_UART_RFL 0x84 /* Rx fifo data count */

#define DW_UART_HTX 0xA4 /* Halt Tx */

/* LSR Linearity Status Register Area */
#define DW_UART_LSR_TEMT 0x40   /* Transmit Clear Bit */
#define DW_UART_LSR_THRE 0x20   /* Transmit buffer register clear bit */
#define DW_UART_LSR_BI   0x10   /* Break interrupt bit */
#define DW_UART_LSR_FE   0x08   /* Frame error bit */
#define DW_UART_LSR_PE   0x04   /* Check error bit */
#define DW_UART_LSR_R    0x02   /* Overflow error bit */
#define DW_UART_LSR_DDR  (0x01) /* Data ready bit */

/* DW_UART_LCR bit field */
#define DW_UART_DLAB         BIT(7)
#define DW_UART_BREAK        BIT(6)
#define DW_UART_STICK        BIT(5)
#define DW_UART_EPS          BIT(4)
#define DW_UART_PEN          BIT(3)
#define DW_UART_STOP         BIT(2)
#define DW_UART_8BIT         0x3
#define DW_UART_7BIT         0x2
#define DW_UART_6BIT         0x1
#define DW_UART_5BIT         0x0
#define DW_UART_DATALEN_MASK 0x03

/* DW_UART_IER bit field */
#define PTIME BIT(7)
#define EDSSI BIT(3)
#define ELSI  BIT(2)
#define ETBEI BIT(1)
#define ERBFI BIT(0)

/* DW_UART_LSR bit field */
#define DW_RFE BIT(7)
/* Transmit Clear Bit */
#define DW_TEMT BIT(6)
/* Transmit buffer register clear bit */
#define DW_THRE BIT(5)
/* Break interrupt bit */
#define DW_BI BIT(4)
/* Frame error bit */
#define DW_FE BIT(3)
/* Check error bit */
#define DW_PE BIT(2)
/* Overflow error bit */
#define DW_OE BIT(1)
/* Data ready bit */
#define DW_DR BIT(0)

#define UART_DW_DR_PE DW_PE
#define UART_DW_DR_FE DW_FE
#define UART_DW_DR_OE DW_OE
#define UART_DW_DR_BE DW_BI

#define DW_RSR_ANY (DW_OE | DW_PE | DW_FE | DW_BI)
#define DW_DUMMY_RSR_RX
/* DW_UART_MCR bit field */
#define DW_MC_AFCE BIT(5)
#define DW_MC_LOOP BIT(4)
#define DW_MC_OUT2 BIT(3)
#define DW_MC_OUT1 BIT(2)
#define DW_MC_RTS  BIT(1)
#define DW_MC_DTR  BIT(0)

/* DW_UART_MSR bit field */
#define DW_DCD     BIT(7)
#define DW_RI      BIT(6)
#define DW_DSR     BIT(5)
#define DW_CTS     BIT(4)
#define DW_MSR_ANY (DW_DCD | DW_DSR | DW_CTS)

/* DW_UART_IIR bit field */
#define DW_RECEIVERR   (0x06) /* HIGHEST */
#define DW_RECEIVEAVA  (0x04) /* SECOND  RECEIVE date */
#define DW_RECTIMEOUT  (0x0C) /* SECOND */
#define DW_TRANSEMP    (0x02) /* THIRD transmit hold reg empty */
#define DW_NOINTERRUPT (0x01) /* NO interrupt pending */
#define DW_MODEMSTA    (0)    /* FOURTH modem int */
#define DW_BUSY        (0x7)  /* Fifth write while line control busy */

/* DW_UART_FCR bit field */
#define RECFIFO1_2 (0x02 << 6)
#define TXFIFO1_2  (0x03 << 4)
#define FIFOENA    (1)

/* DW_UART_USR bit field */
#define DW_UART_BUSY      0x01
#define DW_XFIFO_NOT_FULL 0x02
#define DW_XFIFO_EMP      0x04
#define DW_RFIFO_NOT_EMP  0x08
#define DW_RFIFO_FULL     0x10

#define REG_BASE_UART1   0x20008000
#define REG_UART1_IOSIZE (0x1000)
#define IRQ_UART1        65

#define REG_BASE_UART2   0x20009000
#define REG_UART2_IOSIZE (0x1000)
#define IRQ_UART2        66

#define SYSCTL_REG_BASE             0x20000000
#define SYSCTL_REG_LEN              0x1000
#define SC_PERCTRL15                0x58
#define SC_PERCTRL15_UART1SEL_MASK  (7UL << 20)
#define SC_PERCTRL15_UART2SEL_MASK  (7UL << 23)
#define SC_PERCTRL15_UART3SEL_MASK  (7UL << 26)
#define SC_PERCTRL15_UART4SEL_MASK  (7UL << 29)
#define SC_PERCTRL15_UART1SEL_SHIFT 20
#define SC_PERCTRL15_UART2SEL_SHIFT 23
#define SC_PERCTRL15_UART3SEL_SHIFT 26
#define SC_PERCTRL15_UART4SEL_SHIFT 29
#define UART_DISCONNECT_STATUS      (7UL)

#define SC_PERCTRL8                0x3c
#define SC_PERCTRL8_UART3_SRST_REQ (1UL << 10)
#define SC_PERCTRL8_UART4_SRST_REQ (1UL << 11)

#define UART_NR         8
#define SERIAL_DW_MINOR 65
#define MAX_UART_NUM    8

#define UART_TX_DMA    0x0
#define UART_RX_DMA    0x1
#define UART_FIFO_SIZE 128

#define UART_MUX_NORMAL (1) /* 3 pins */
#define UART_MUX_FULL   (0) /* 5 pins */

#define PORT_FIFO_SIZE 2

/* Type of the registration function for RS485 TX/RX flow control processing */
typedef void (*rs485_flow_register_handler)(void *);

struct uart_isr_stat {
    int tx_empty; /* DW_TRANSEMP */
    int rx_valid; /* DW_RECEIVEAVA: RX_FIFO FULL or RBR full */
    int rx_err;   /* DW_RECEIVERR:parity , overflow, frame error, rx exception */
    int busy;     /* DW_BUSY:transmiting or receiving, and operating on LCR */
    int timeout;  /* DW_RECTIMEOUT: no transmition occurs in 4 continious tx/rx time */
};

/*
 * We wrap our port structure around the generic uart_port.
 */
struct uart_dw_port {
    struct uart_port port;
    struct clk *clk;
    unsigned int im; /* interrupt mask */
    unsigned int old_status;
    unsigned int port_no; /* port number(parsed from device tree property "port-no") */
    struct uart_isr_stat isr_stat;
};

struct uart_dma_config_info {
    unsigned int src_uart_no;
    unsigned int dest_uart_no;
    unsigned char *src_buff;
    unsigned char *dest_buff;
    unsigned int data_len;
};

struct uart_speed_test_info {
    unsigned int uartcore_tx; /* UART core number for transmiting data */
    unsigned int uartcore_rx; /* UART core number for receiving data */
    unsigned int len;         /* Data length */
    unsigned int ms_consumed; /* Test case time consuming */
};

struct uart_mux_cfg {
    u32 uart_no;
    u32 mux_sel; /* Select whether uart is 3 pins or 5 pins,see UART_MUX_NORMAL/FULL */
};

struct hisi_uart_core {
    int core_init;                /* 0: core not initialized 1: core initialized */
    unsigned int port_registered; /* Uart port registered */
    unsigned int uartclk;         /* uart clock frequency get from DT */
};

extern int hisi_uart_core_register_port(struct uart_dw_port *udp);
extern int hisi_uart_core_unregister_port(struct uart_dw_port *udp);
extern int hisi_uart_core_reset_port(unsigned int uart_no);
extern int hisi_uart_core_get_uartclk(unsigned int *uartclk);
extern int hisi_uart_core_disconnect(unsigned int uart_no);
extern int hisi_uart_core_connect(unsigned int uart_no);
extern void uart_rs485_register_handle_tx(rs485_flow_register_handler handler);
extern void uart_rs485_register_handle_rx(rs485_flow_register_handler handler);

#endif /* _UART_KER_H__ */