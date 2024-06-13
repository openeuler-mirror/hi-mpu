/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: UART driver APIs
 * Create: 2018-05-10
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <hi309x_common.h>
#include <asm/io.h>
#include "uart_core.h"


#define UART_BUSY_TIMEOUT (CONFIG_SYS_CPU_FREQ >> 1)

void uart_reset(int uart_num)
{
    unsigned int timeout = UART_SRST_TIMEOUT;
    unsigned int uart_srst_bit = (1U << (LPSC_UART2_SRST_BIT + uart_num - 2)); /* uart_num 从2开始，对应bit为22 */

    writel(uart_srst_bit, LP_SUBSYS_SRST_REQ1_BASE);
    while (timeout) {
        if (readl(LP_SUBSYS_SRST_ST1_BASE) & uart_srst_bit) {
            break;
        }

        timeout--;
    }

    writel(uart_srst_bit, LP_SUBSYS_SRST_DREQ1_BASE);
    timeout = UART_SRST_TIMEOUT;
    while (timeout) {
        if ((readl(LP_SUBSYS_SRST_ST1_BASE) & uart_srst_bit) == 0) {
            break;
        }

        timeout--;
    }
}

void uart_port_map_init(void)
{
    unsigned int i;
    unsigned char phy_port;
    unsigned char uart_port;
    unsigned int port_uart_sel = 0;
    unsigned int uart_port_sel = 0;
    unsigned char uart2port[MAX_UART_NUM] = {
        0, /* uart0 -> physical port0 */
        1,
        2,
        3,
        4,
        5,
        6,
        7
    };

    for (i = 0; i < MAX_UART_NUM; i++) {
        uart_port = i & 0xf;
        phy_port = uart2port[i];
        /* port_uart_sel: Every 4 bits represent a phyical port to uart port mapping */
        port_uart_sel |= ((unsigned int)uart_port << (unsigned int)(phy_port * 4));
        /* uart_port_sel: Every 4 bits represent a uart port to physical port mapping */
        uart_port_sel |= ((unsigned int)phy_port << (unsigned int)(uart_port * 4));
    }

    writel(port_uart_sel, LPSC_PORT_SEL_REG);
    writel(uart_port_sel, LPSC_UART_SEL_REG);
}

/* Caculate UART Divisor Factor */
void calc_uart_dll_dlh(unsigned int baudrate, unsigned int *dll, unsigned int *dlh, unsigned int uart_src_clk)
{
    unsigned int divisor;

    /*
     * baudrate计算公式： bus_clock / (16 * divisor)
     * ==> divisor = bus_clock/(baudrate * 16)
     *
     */
    /*lint -e506*/
    divisor = DIV_ROUND_CLOSEST(uart_src_clk, (16 * baudrate)); /* 分频因子=内部总线时钟/(16*波特率) */
    *dll = divisor & 0xFF;
    *dlh = (divisor >> 8) & 0xFF;  /* 高位分频因子寄存器配置8-15位 */
}

static unsigned int calc_lcr_reg_val(struct uart_cfg *cfg)
{
    unsigned int lcr = 0;
     /* data_bits数据范围长度为5-8 */
    switch (cfg->data_bits) {
        case 8: /* 长度为8 */
            lcr |= UART_LCR_DLS8;
            break;
        case 7: /* 长度为7 */
            lcr |= UART_LCR_DLS7;
            break;
        case 6: /* 长度为6 */
            lcr |= UART_LCR_DLS6;
            break;
        case 5: /* 长度为5 */
            lcr |= UART_LCR_DLS5;
            break;
        default:
            lcr |= UART_LCR_DLS8;
            break;
    }

    if (cfg->stop) {
        lcr |= UART_LCR_STOP;
    }

    if (cfg->pen) {
        lcr |= UART_LCR_PEN;
        if (cfg->eps) {
            lcr |= UART_LCR_EPS;
        }
    }

    return lcr;
}

void uart_set_baudrate(int uart_num, unsigned int baudrate, unsigned int uart_src_clk)
{
    unsigned int dll = 0;
    unsigned int dlh = 0;

    calc_uart_dll_dlh(baudrate, &dll, &dlh, uart_src_clk);
    uart_set_dll_dlh(uart_num, dll, dlh);
}

int uart_init(int uart_num, struct uart_cfg *cfg, unsigned int uart_src_clk)
{
    unsigned int fifo_ctrl;
    /* data_bits数据范围为5-8 */
    if (uart_num >= MAX_UART_NUM || uart_num < 0 || cfg == NULL || cfg->data_bits > 8) {
        return -EINVAL;
    }

    /* UART Reset */
    uart_reset(uart_num);
    /* Config FIFO,DLL,DLH at first */
    uart_set_baudrate(uart_num, cfg->baudrate, uart_src_clk);
    fifo_ctrl = UART_FCR_EN | UART_FCR_RXCLR | UART_FCR_TXCLR;
    uart_set_fifo_ctrl(uart_num, fifo_ctrl);
    /* Set data bits, stop bit, parity check */
    uart_set_lcr(uart_num, calc_lcr_reg_val(cfg));

    return 0;
}

int uart_tx(int uart_num, const char c)
{
    int timeout = 0;

    while (uart_is_txfifo_full(uart_num)) {
        timeout++;
        if (timeout >= UART_BUSY_TIMEOUT) {
            return -EAGAIN;
        }
    }

    uart_tx_char(uart_num, c);
    return 0;
}

char uart_rx(int uart_num)
{
    int timeout = 0;
    char c;

    while (!uart_is_rx_ready(uart_num)) {
        timeout++;
        if (timeout >= UART_BUSY_TIMEOUT) {
            return -EAGAIN;
        }
    }

    uart_rx_char(uart_num, &c);
    return c;
}

int uart_tx_ready(int uart_num)
{
    if (uart_is_txfifo_full(uart_num)) {
        return 0;
    } else {
        return 1;
    }
}

int uart_rx_ready(int uart_num)
{
    return uart_is_rx_ready(uart_num);
}

/* Can only be used after uart_init() */
void uart_early_puts(int uart_num, const char *str)
{
    int ret;

    while (*str != '\0') {
        ret = uart_tx(uart_num, *str);
        if (ret) {
            asm volatile("nop"); /* early put, do nothing */
        }
        str++;
    }
}
