/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x uart registers declarations header file
 * Create: 2018-05-10
 */
#ifndef __UART_REGS_H_
#define __UART_REGS_H_

/* --UART Base Address Definiations-- */
#define UART0_REG_BASE 0x0872A000 /* LPC_UART */
#define UART1_REG_BASE 0x0872B000 /* ESPI_UART */
#define UART2_REG_BASE 0x08710000
#define UART3_REG_BASE 0x08711000
#define UART4_REG_BASE 0x08743000
#define UART5_REG_BASE 0x08744000
#define UART6_REG_BASE 0x0875D000
#define UART7_REG_BASE 0x0875E000

/* --UART Register Offset-- */
#define UART_RBR_REG_OFFSET 0x00
#define UART_THR_REG_OFFSET 0x00
#define UART_DLL_REG_OFFSET 0x00
#define UART_DLH_REG_OFFSET 0x04
#define UART_IEL_REG_OFFSET 0x04
#define UART_IIR_REG_OFFSET 0x08
#define UART_FCR_REG_OFFSET 0x08
#define UART_LCR_REG_OFFSET 0x0C
#define UART_MCR_REG_OFFSET 0x10
#define UART_LSR_REG_OFFSET 0x14
#define UART_USR_REG_OFFSET 0x7C

/* --Register bit definitions-- */
/* FCR: FIFO Control Register */
#define UART_FCR_EN 0x01
#define UART_FCR_RXCLR 0x02
#define UART_FCR_TXCLR 0x04

/* LCR: Line Control Register fields */
#define UART_LCR_DLS5 0x00
#define UART_LCR_DLS6 0x01
#define UART_LCR_DLS7 0x02
#define UART_LCR_DLS8 0x03
#define UART_LCR_STOP 0x04
#define UART_LCR_PEN 0x08
#define UART_LCR_EPS 0x10
#define UART_LCR_DLAB 0x80

/* IER: Interrupt Enable Register */
#define UART_IER_PTIME 0x80
#define UART_IER_ELSI 0x04
#define UART_IER_ETBEI 0x02
#define UART_IER_ERBFI 0x01

/* IIR: Interrupt Identity Register */
#define UART_IIR_FIFOSE 0xC0

/* LSR: Line Status Register */
#define UART_LSR_TEMT 0x40
#define UART_LSR_THRE 0x20
#define UART_LSR_BI 0x10
#define UART_LSR_FE 0x08
#define UART_LSR_PE 0x04
#define UART_LSR_OV 0x02
#define UART_LSR_DR 0x01

/* USR: Uart Status Register */
#define UART_USR_BUSY 0x01
#define UART_USR_TFNF 0x02
#define UART_USR_TFE 0x04
#define UART_USR_RFNE 0x08
#define UART_USR_RFF 0x10

/* Uart port map register */
#define LP_SUBSYS_CSR (0x0871D000)
#define LPSC_PORT_SEL_REG (LP_SUBSYS_CSR + 0x2C) /* physical port to uart port mapping */
#define LPSC_UART_SEL_REG (LP_SUBSYS_CSR + 0x30)
#define LPSC_UART2_SRST_BIT (22U)
#define UART_SRST_TIMEOUT (1000)

#endif /* __UART_REGS_H_ */
