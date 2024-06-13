// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Implemention of uart module.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/io.h>
#include <asm/irq.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/console.h>
#include <linux/tty_flip.h>
#include <linux/tty.h>
#include "uart_ker.h"

#define DW_UART_SUPPORT_MODEM 0

#define WAIT_MAX_TIME     100
#define DW_ISR_PASS_LIMIT 256
#define UART_DR_ERROR     (UART_DW_DR_OE | UART_DW_DR_BE | UART_DW_DR_PE | UART_DW_DR_FE)
#define UART_DUMMY_DR_RX  BIT(16)
#define UART_DRIVER_NAME  "hisi_dw_uart"
#define HIST_UART_IRQ_NAME "hisi_uart_irq"
#define MAX_DEV_NAME_LEN  32
#define IGNORE_RX_CHAR    (-1)

rs485_flow_register_handler g_rs485_flow_handler_tx = NULL;
rs485_flow_register_handler g_rs485_flow_handler_rx = NULL;

void uart_rs485_register_handle_tx(rs485_flow_register_handler handler)
{
    g_rs485_flow_handler_tx = handler;
}
EXPORT_SYMBOL(uart_rs485_register_handle_tx);

void uart_rs485_register_handle_rx(rs485_flow_register_handler handler)
{
    g_rs485_flow_handler_rx = handler;
}
EXPORT_SYMBOL(uart_rs485_register_handle_rx);

void uart_record_isr(struct uart_dw_port *udp, u32 int_id)
{
    switch (int_id) {
        case DW_TRANSEMP:
            udp->isr_stat.tx_empty++;
            break;
        case DW_RECEIVEAVA:
            udp->isr_stat.rx_valid++;
            break;
        case DW_RECEIVERR:
            udp->isr_stat.rx_err++;
            break;
        case DW_BUSY:
            udp->isr_stat.busy++;
            break;
        case DW_RECTIMEOUT:
            udp->isr_stat.timeout++;
            break;
        default:
            break;
    }
}

void uart_stop_tx(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;

    udp->im &= ~(PTIME | ETBEI);
    writel(udp->im, udp->port.membase + DW_UART_IER);

    if (g_rs485_flow_handler_rx) {
        g_rs485_flow_handler_rx(port);
    }

    return;
}

void uart_start_tx(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;

    if (g_rs485_flow_handler_tx) {
        g_rs485_flow_handler_tx(port);
    }

    udp->im |= ETBEI;
    writel(udp->im, udp->port.membase + DW_UART_IER);

    return;
}

void uart_stop_rx(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;

    udp->im &= ~(ELSI | ERBFI);
    writel(udp->im, udp->port.membase + DW_UART_IER);

    return;
}

void uart_enable_ms(struct uart_port *port)
{
    return;
}

static int uart_proc_rx_err(struct uart_dw_port *udp, u32 *status, u32 *flag)
{
    u32 raw_status;

    raw_status = *status;
    if (raw_status & UART_DW_DR_BE) {
        raw_status &= ~(UART_DW_DR_FE | UART_DW_DR_PE);
        udp->port.icount.brk++;
        if (uart_handle_break(&udp->port)) {
            return IGNORE_RX_CHAR;
        }
    } else if (raw_status & UART_DW_DR_PE) {
        udp->port.icount.parity++;
    } else if (raw_status & UART_DW_DR_FE) {
        udp->port.icount.frame++;
    }

    if (raw_status & UART_DW_DR_OE) {
        udp->port.icount.overrun++;
    }

    raw_status &= udp->port.read_status_mask;

    if (raw_status & UART_DW_DR_BE) {
        *flag = TTY_BREAK;
    } else if (raw_status & UART_DW_DR_PE) {
        *flag = TTY_PARITY;
    } else if (raw_status & UART_DW_DR_FE) {
        *flag = TTY_FRAME;
    }

    *status = raw_status;
    return 0;
}

void uart_rx_chars(struct uart_dw_port *udp)
{
    u32 status;
    u32 ch;
    u32 flag;
    u32 max_count = DW_ISR_PASS_LIMIT;
    struct tty_struct *tty = udp->port.state->port.tty;

    status = readl(udp->port.membase + DW_UART_LSR);
    if (!(status & DW_DR)) {
        /* No data in the RX FIFO */
        (void)readl(udp->port.membase + DW_UART_RBR);
    }

    while ((status & DW_DR) && max_count--) {
        /* There is data in the RX FIFO. */
        ch = readl(udp->port.membase + DW_UART_RBR) | UART_DUMMY_DR_RX;
        flag = TTY_NORMAL;
        udp->port.icount.rx++;

        /*
         * Note that the error handling code is
         * out of the main execution path
         */
        if (unlikely(status & UART_DR_ERROR)) {
            if (uart_proc_rx_err(udp, &status, &flag) == IGNORE_RX_CHAR) {
                goto IGNORE_CHAR;
            }
        }

#ifdef SUPPORT_SYSRQ
        /* Processing sysrq characters */
        if (uart_handle_sysrq_char(&udp->port, ch & 0xff)) {
            goto IGNORE_CHAR;
        }
#endif
        /* This character is added to the flip buffer of the tty device. */
        uart_insert_char(&udp->port, status, UART_DW_DR_OE, ch, flag);

IGNORE_CHAR:
        status = readl(udp->port.membase + DW_UART_LSR);
    }

    tty_flip_buffer_push(tty->port); /* Refresh the flip buffer of the tty device. */

    return;
}

void uart_tx_chars(struct uart_dw_port *udp)
{
    struct circ_buf *xmit = &udp->port.state->xmit;
    s32 count;

    if (udp->port.x_char) {
        while ((readl(udp->port.membase + DW_UART_LSR) & DW_UART_LSR_THRE) != DW_UART_LSR_THRE) {
            ;
        }

        writel(udp->port.x_char, udp->port.membase + DW_UART_THR);
        udp->port.icount.tx++;
        udp->port.x_char = 0;
        return;
    }

    if (uart_circ_empty(xmit) || uart_tx_stopped(&udp->port)) {
        uart_stop_tx(&udp->port);
        return;
    }

    count = (s32)(udp->port.fifosize - 2); /* **2** */

    do {
        while ((readl(udp->port.membase + DW_UART_LSR) & DW_UART_LSR_THRE) != DW_UART_LSR_THRE) {
            ;
        }

        writel((u8)xmit->buf[xmit->tail], udp->port.membase + DW_UART_THR);
        xmit->tail = ((u32)(xmit->tail) + 1) & (UART_XMIT_SIZE - 1);
        udp->port.icount.tx++;
        if (uart_circ_empty(xmit)) {
            break;
        }
    } while (--count > 0);

    if (uart_circ_chars_pending(xmit) < WAKEUP_CHARS) {
        uart_write_wakeup(&udp->port);
    }

    if (uart_circ_empty(xmit)) {
        uart_stop_tx(&udp->port);
    }

    return;
}

void uart_modem_status(struct uart_dw_port *udp)
{
    return;
}

static s32 uart_proc_other_isr(struct uart_dw_port *udp, u32 interrupt_id)
{
    if (interrupt_id == DW_MODEMSTA) {
        uart_modem_status(udp);
    } else if (interrupt_id == DW_TRANSEMP) {
        uart_tx_chars(udp);
    } else if (interrupt_id == DW_BUSY) {
        readl(udp->port.membase + DW_UART_USR);
    } else if (interrupt_id == DW_RECEIVERR) {
        readl(udp->port.membase + DW_UART_LSR);
        return -EIO;
    } else {
        (void)readl(udp->port.membase + DW_UART_USR);
    }

    return 0;
}

irqreturn_t uart_intr_server(s32 irq, void *dev_id)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)dev_id;
    u32 status;
    u32 lsr;
    u32 pass_counter = DW_ISR_PASS_LIMIT;
    u32 interrupt_id;
    s32 handled = IRQ_HANDLED;

    if (dev_id == NULL) {
        /* Use CPBSP_DEBUGOUTPUT with caution in the interrupt service routine. */
        return IRQ_RETVAL(handled);
    }

    spin_lock(&udp->port.lock);
    status = readl(udp->port.membase + DW_UART_IIR);
    interrupt_id = 0x0F & status;

    lsr = readl(udp->port.membase + DW_UART_LSR);

    /* clear busy int */
    readl(udp->port.membase + DW_UART_USR);

    while (interrupt_id != DW_NOINTERRUPT) {
        uart_record_isr(udp, interrupt_id);

        if ((interrupt_id == DW_RECEIVEAVA) || (interrupt_id == DW_RECTIMEOUT)) {
            if ((lsr & 0x01) == 0) {
                readl(udp->port.membase + DW_UART_RBR);
                spin_unlock(&udp->port.lock);
                return IRQ_RETVAL(handled);
            }

            uart_rx_chars(udp);
        } else {
            /* For method complexity, however function name is not a best choice */
            if (uart_proc_other_isr(udp, interrupt_id)) {
                break;
            }
        }

        if (pass_counter-- == 0) {
            break;
        }

        status = readl(udp->port.membase + DW_UART_IIR);
        interrupt_id = 0x0F & status;
        handled = 1;
    }

    spin_unlock(&udp->port.lock);

    return IRQ_RETVAL(handled);
}

u32 uart_tx_empty(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;

    u32 status = readl(udp->port.membase + DW_UART_LSR);
    if (status & DW_TEMT) {
        return TIOCSER_TEMT;
    }

    return 0;
}

u32 uart_get_mctrl(struct uart_port *port)
{
    u32 result = 0;
#if DW_UART_SUPPORT_MODEM
    struct uart_dw_port *udp = (struct uart_dw_port *)port;
    u32 status = readl(udp->port.membase + DW_UART_MSR);

#define uart_bit_cal(uartbit, tiocmbit) \
    if (status & uartbit) {    \
        result |= tiocmbit     \
    }

    uart_bit_cal(DW_DCD, TIOCM_CAR);
    uart_bit_cal(DW_DSR, TIOCM_DSR);
    uart_bit_cal(DW_CTS, TIOCM_CTS);
    uart_bit_cal(DW_RI, TIOCM_RNG);
#undef uart_bit_cal
#endif

    return result;
}

void uart_set_mctrl(struct uart_port *port, u32 mctrl)
{
#if DW_UART_SUPPORT_MODEM
    struct uart_dw_port *udp = (struct uart_dw_port *)port;
    u32 cr;

    cr = readl(udp->port.membase + DW_UART_MCR);

#define uart_bit_cal(tiocmbit, uartbit) \
    if (mctrl & tiocmbit) {    \
        cr |= uartbit;         \
    } else {                   \
        cr &= ~uartbit;        \
    }

    uart_bit_cal(TIOCM_RTS, DW_MC_RTS);
    uart_bit_cal(TIOCM_DTR, DW_MC_DTR);
    uart_bit_cal(TIOCM_OUT1, DW_MC_OUT1);
    uart_bit_cal(TIOCM_OUT2, DW_MC_OUT2);
    uart_bit_cal(TIOCM_LOOP, DW_MC_LOOP);
#undef BIT_CAL

    writel(cr, udp->port.membase + DW_UART_MCR);
#endif
}

void uart_break_ctl(struct uart_port *port, s32 break_state)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;

    unsigned long flags = 0;
    u32 lcr_h;
    s32 count = 0;

    spin_lock_irqsave(&udp->port.lock, flags);

    while (readl(udp->port.membase + DW_UART_USR) & DW_UART_BUSY) {
        if (count++ >= WAIT_MAX_TIME) {
            LOG(LOG_INFO, "port is still busy\n");
            break;
        }
    }

    lcr_h = readl(udp->port.membase + DW_UART_LCR);
    if (break_state) {
        lcr_h |= DW_UART_BREAK;
    } else {
        lcr_h &= ~DW_UART_BREAK;
    }

    writel(lcr_h, udp->port.membase + DW_UART_LCR);

    spin_unlock_irqrestore(&udp->port.lock, flags);
}

static void uart_fifo_reset(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;
    unsigned long old_cr;

    old_cr = readl(port->membase + DW_UART_LCR);
    old_cr |= DW_UART_DLAB;
    writel(old_cr, port->membase + DW_UART_LCR);

    /* disable fifo */
    writel(0, udp->port.membase + DW_UART_FCR);

    /* Enable FIFO Enable RX/TX Interrupt */
    writel(0xd7, udp->port.membase + DW_UART_FCR);

    old_cr = readl(port->membase + DW_UART_LCR);
    old_cr &= ~DW_UART_DLAB;
    writel(old_cr, port->membase + DW_UART_LCR);

    return;
}

static void uart_restart(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;
    /* disable FIFO and interrupt */
    writel(0, udp->port.membase + DW_UART_IER);
    (void)readl(udp->port.membase + DW_UART_USR);

#if DW_UART_SUPPORT_MODEM
    /*
     * initialise the old status of the modem signals
     */
    udp->old_status = readl(udp->port.membase + DW_UART_MSR) & DW_MSR_ANY;
#endif

    uart_fifo_reset(port);
    /* clear interrupt status */
    readl(udp->port.membase + DW_UART_LSR);

    writel(udp->im, udp->port.membase + DW_UART_IER);
}

s32 uart_startup(struct uart_port *port)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)port;
    s32 count = 0;
    unsigned long flag;

    if (hisi_uart_core_get_uartclk(&udp->port.uartclk)) {
        LOG(LOG_ERROR, "uart_startup: uart core is not ready!\n");
        return -ENODEV;
    }

    /* disable FIFO and interrupt */
    writel(0, udp->port.membase + DW_UART_IER);

    (void)readl(udp->port.membase + DW_UART_USR);

#if DW_UART_SUPPORT_MODEM
    /*
     * initialise the old status of the modem signals
     */
    udp->old_status = readl(udp->port.membase + DW_UART_MSR) & DW_MSR_ANY;
#endif

    uart_fifo_reset(port);
    /* clear interrupt status */
    readl(udp->port.membase + DW_UART_LSR);

    spin_lock_irqsave(&udp->port.lock, flag);

    /* Finally, enable interrupts */
    udp->im = (ERBFI | ELSI);
    writel(udp->im, udp->port.membase + DW_UART_IER);

    while (readl(udp->port.membase + DW_UART_USR) & DW_UART_BUSY) {
        if (count++ >= WAIT_MAX_TIME) {
            LOG(LOG_INFO, "port is still busy");
            break;
        }
    }

    spin_unlock_irqrestore(&udp->port.lock, flag);

    return 0;
}

void uart_shutdown(struct uart_port *port)
{
    s32 count = 0;
    unsigned long flag;
    struct uart_dw_port *udp = (struct uart_dw_port *)port;
    unsigned long val;

    spin_lock_irqsave(&udp->port.lock, flag);

    /*
     * disable/clear all interrupts
     */
    udp->im = 0;
    writel(udp->im, udp->port.membase + DW_UART_IER);
    readl(udp->port.membase + DW_UART_LSR);

    spin_unlock_irqrestore(&udp->port.lock, flag);

    /*
     * disable break condition and fifos
     */
    while (readl(udp->port.membase + DW_UART_USR) & DW_UART_BUSY) {
        if (count++ >= WAIT_MAX_TIME) {
            LOG(LOG_INFO, "port is still busy\n");
            break;
        }
    }

    /* When writing to DW_UART_FCR, the DW_UART_LCR DLAB bit must be set first. */
    val = readl(udp->port.membase + DW_UART_LCR);
    val &= ~(DW_UART_BREAK);
    val |= DW_UART_DLAB;
    writel(val, udp->port.membase + DW_UART_LCR);

    /* disable fifo */
    writel(0, udp->port.membase + DW_UART_FCR);

    val = readl(port->membase + DW_UART_LCR);
    val &= ~DW_UART_DLAB;
    writel(val, port->membase + DW_UART_LCR);

    return;
}

static u32 uart_get_termios_lcrh(struct ktermios *termios_arg)
{
    u32 lcr_h;

    switch (termios_arg->c_cflag & CSIZE) {
        case CS5:
            lcr_h = DW_UART_5BIT;
            break;
        case CS6:
            lcr_h = DW_UART_6BIT;
            break;
        case CS7:
            lcr_h = DW_UART_7BIT;
            break;
        case CS8:
            lcr_h = DW_UART_8BIT;
            break;
        default: /* default using 8-bit mode */
            lcr_h = DW_UART_8BIT;
            break;
    }

    if (termios_arg->c_cflag & CSTOPB) {
        lcr_h |= DW_UART_STOP;
    }

    if (termios_arg->c_cflag & PARENB) {
        lcr_h |= DW_UART_PEN;

        if (termios_arg->c_cflag & CMSPAR) {
            lcr_h |= DW_UART_STICK;
        }

        if (!(termios_arg->c_cflag & PARODD)) {
            lcr_h |= DW_UART_EPS;
        }
    }

    return lcr_h;
}

static void uart_update_port_status_mask(struct uart_port *port, struct ktermios *termios_arg)
{
    port->read_status_mask = UART_DW_DR_OE;

    if (termios_arg->c_iflag & INPCK) {
        port->read_status_mask |= UART_DW_DR_FE | UART_DW_DR_PE;
    }

    if (termios_arg->c_iflag & (BRKINT | PARMRK)) {
        port->read_status_mask |= UART_DW_DR_BE;
    }

    /* Characters to ignore */
    port->ignore_status_mask = 0;
    if (termios_arg->c_iflag & IGNPAR) {
        port->ignore_status_mask |= UART_DW_DR_FE | UART_DW_DR_PE;
    }

    if (termios_arg->c_iflag & IGNBRK) {
        port->ignore_status_mask |= UART_DW_DR_BE;

        /*
         * If we're ignoring parity and break indicators,
         * ignore overruns too (for real raw support).
         */
        if (termios_arg->c_iflag & IGNPAR) {
            port->ignore_status_mask |= UART_DW_DR_OE;
        }
    }

    /* Ignore all characters if CREAD is not set. */
    if ((termios_arg->c_cflag & CREAD) == 0) {
        port->ignore_status_mask |= UART_DUMMY_DR_RX;
    }

    return;
}

void uart_set_termios(struct uart_port *port, struct ktermios *termios_arg, struct ktermios *old)
{
    u32 lcr_h;
    u32 old_cr;
    unsigned long flags = 0;
    u32 baud;
    u32 quot;
    struct uart_dw_port *udp = (struct uart_dw_port *)port;

    /* Ask the core to calculate the divisor for us. */
    baud = uart_get_baud_rate(port, termios_arg, old, 0, port->uartclk / 16); // div 16
    quot = DIV_ROUND_CLOSEST(port->uartclk, (16 * baud)); // mul 16

    lcr_h = uart_get_termios_lcrh(termios_arg);

    spin_lock_irqsave(&port->lock, flags);

    /* Update the per-port timeout. */
    uart_update_timeout(port, termios_arg->c_cflag, baud);
    uart_update_port_status_mask(port, termios_arg);
    if (UART_ENABLE_MS(port, termios_arg->c_cflag)) {
        uart_enable_ms(port);
    }

    hisi_uart_core_disconnect(udp->port_no);
    /* Soft reset port */
    hisi_uart_core_reset_port(udp->port_no);
    /* Enable DLL and DLH */
    old_cr = readl(port->membase + DW_UART_LCR);
    old_cr |= DW_UART_DLAB;
    writel(old_cr, port->membase + DW_UART_LCR);

    /* Set baud rate */
    writel(((quot & 0xFF00) >> 8), port->membase + DW_UART_DLH); // >> 8
    writel((quot & 0xFF), port->membase + DW_UART_DLL);

    old_cr &= ~DW_UART_DLAB;
    writel(old_cr, port->membase + DW_UART_LCR);

    writel(lcr_h, port->membase + DW_UART_LCR);

    uart_restart(port);
    hisi_uart_core_connect(udp->port_no);

    spin_unlock_irqrestore(&port->lock, flags);

    return;
}

const char *uart_type(struct uart_port *port)
{
    return (port->type == PORT_AMBA) ? "AMBA/ttySS" : NULL;
}

void uart_release_port(struct uart_port *port)
{
    release_mem_region(port->mapbase, SZ_4K);
}

s32 uart_request_port(struct uart_port *port)
{
    if (request_mem_region(port->mapbase, SZ_4K, UART_DRIVER_NAME) != NULL) {
        return 0;
    } else {
        return -EBUSY;
    }
}

void uart_config_port(struct uart_port *port, s32 flags)
{
    if ((u32)flags & UART_CONFIG_TYPE) {
        port->type = PORT_AMBA;
        (void)uart_request_port(port);
    }
}

s32 uart_verify_port(struct uart_port *port, struct serial_struct *ser)
{
    s32 ret = 0;

    if ((ser->type != PORT_UNKNOWN) && (ser->type != PORT_AMBA)) {
        ret = -EINVAL;
    }

    if ((ser->irq < 0) || (ser->irq >= NR_IRQS)) {
        ret = -EINVAL;
    }

    if (ser->baud_base < 9600) { // the min baudrate is 9600
        ret = -EINVAL;
    }

    return ret;
}

void uart_release(struct device *dev)
{
    return;
}

static const struct uart_ops g_uart_serail_pops = {
    .tx_empty = uart_tx_empty,
    .set_mctrl = uart_set_mctrl,
    .get_mctrl = uart_get_mctrl,
    .stop_tx = uart_stop_tx,
    .start_tx = uart_start_tx,
    .stop_rx = uart_stop_rx,
    .enable_ms = uart_enable_ms,
    .break_ctl = uart_break_ctl,
    .startup = uart_startup,
    .shutdown = uart_shutdown,
    .set_termios = uart_set_termios,
    .type = uart_type,
    .release_port = uart_release_port,
    .request_port = uart_request_port,
    .config_port = uart_config_port,
    .verify_port = uart_verify_port,
};

static s32 uart_parse_irq_and_iobase(struct platform_device *pdev, u32 *irq,
                                     void **base, resource_size_t *phys_base)
{
    struct resource *res = NULL;

    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (!res) {
        LOG(LOG_ERROR, "uart_parse_irq_and_iobase: no irq defined!");
        return -ENODEV;
    }

    *irq = res->start;
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        LOG(LOG_ERROR, "uart_parse_irq_and_iobase: no mmio defined!");
        return -ENODEV;
    }

    *base = ioremap(res->start, resource_size(res));
    if (*base == NULL) {
        LOG(LOG_ERROR, "uart_parse_irq_and_iobase: ioremap err!");
        return -EIO;
    }

    *phys_base = res->start;

    return 0;
}

static void uart_dw_port_init(struct uart_dw_port *udp, struct platform_device *pdev,
                              resource_size_t phys_base, u8 *base, u32 irq)
{
    udp->port.dev = &pdev->dev;
    udp->port.mapbase = phys_base;
    udp->port.membase = base;
    udp->port.iotype = UPIO_MEM32;
    udp->port.irq = irq;
    udp->port.fifosize = PORT_FIFO_SIZE;  // 16*8bits
    udp->port.ops = &g_uart_serail_pops;
    udp->port.flags = UPF_BOOT_AUTOCONF;
    udp->port.line = (u32)udp->port_no;
    platform_set_drvdata(pdev, udp);
}

static s32 uart_of_probe(struct platform_device *pdev)
{
    struct uart_dw_port *udp;
    struct device_node *np = pdev->dev.of_node;
    void __iomem *base;
    s32 ret;
    u32 irq = 0;
    resource_size_t phys_base = 0;
    char *irq_name = NULL;

    udp = (struct uart_dw_port *)kzalloc(sizeof(struct uart_dw_port), GFP_KERNEL);
    if (udp == NULL) {
        LOG(LOG_ERROR, "uart_probe: not enough memory!");

        return -ENOMEM;
    }

    ret = uart_parse_irq_and_iobase(pdev, &irq, &base, &phys_base);
    if (ret) {
        goto ERR_FREE_MEM;
    }

    ret = of_property_read_u32(np, "port-no", &udp->port_no);
    if (ret) {
        LOG(LOG_ERROR, "uart_probe: cannot read port-no from DT (%d)\n", ret);
        goto ERR_UNMAP;
    }

    uart_dw_port_init(udp, pdev, phys_base, (u8 *)base, irq);
    /* disable  interrupts */
    writel(0, udp->port.membase + DW_UART_IER);

    irq_name = devm_kasprintf(&pdev->dev, GFP_KERNEL, "%s%u", HIST_UART_IRQ_NAME, udp->port_no);
    if (irq_name == NULL) {
        LOG(LOG_ERROR, "uart_probe: get irq name err %d", ret);
        goto ERR_UNMAP;
    }

    ret = request_irq(irq, uart_intr_server, 0, irq_name, udp);
    if (ret) {
        LOG(LOG_ERROR, "uart_probe: request_irq err %d", ret);
        goto ERR_UNMAP;
    }

    ret = hisi_uart_core_register_port(udp);
    if (ret) {
        LOG(LOG_ERROR, "uart_probe: register_port err %d", ret);
        goto ERR_FREE_IRQ;
    }

    return 0;
ERR_FREE_IRQ:
    free_irq(irq, udp);
    platform_set_drvdata(pdev, NULL);
ERR_UNMAP:
    iounmap(base);
ERR_FREE_MEM:
    kfree(udp);
    return ret;
}

static s32 uart_of_remove(struct platform_device *pdev)
{
    struct uart_dw_port *udp = (struct uart_dw_port *)platform_get_drvdata(pdev);

    if (udp == NULL) {
        return -EINVAL;
    }

    free_irq(udp->port.irq, udp);
    platform_set_drvdata(pdev, NULL);
    (void)hisi_uart_core_unregister_port(udp);
    iounmap(udp->port.membase);
    kfree(udp);
    return 0;
}

static const struct of_device_id g_hisi_uart_of_match[] = {
    { .compatible = "hisilicon,hisi-uart" }, {}
};

static struct platform_driver g_hisi_uart_driver = {
    .driver = {
        .name = "hisi-uart",
        .of_match_table = g_hisi_uart_of_match,
    },
    .probe = uart_of_probe,
    .remove = uart_of_remove,
};

module_platform_driver(g_hisi_uart_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("uart driver");