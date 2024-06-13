/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: UART hw adapter APIs
 * Create: 2018-05-10
 */
#include <common.h>
#include <dm.h>
#include <errno.h>
#include <asm/io.h>
#include "uart_core.h"

/* Get UART register base address */
static int uart_core_base_addr(int uart_num, unsigned int *reg_base)
{
    unsigned long reg_bases[] = {
        UART0_REG_BASE,
        UART1_REG_BASE,
        UART2_REG_BASE,
        UART3_REG_BASE,
        UART4_REG_BASE,
        UART5_REG_BASE,
        UART6_REG_BASE,
        UART7_REG_BASE
    };

    if (uart_num >= MAX_UART_NUM || uart_num < 0) {
        return -EINVAL;
    }

    *reg_base = reg_bases[uart_num];

    return 0;
}

int uart_reg_read(int uart_num, unsigned int offset, unsigned int *val)
{
    int ret;
    unsigned int reg_base = 0x0;

    ret = uart_core_base_addr(uart_num, &reg_base);
    if (ret) {
        return ret;
    }

    *val = readl((uintptr_t)(reg_base + offset));
    return 0;
}

void uart_reg_write(int uart_num, unsigned int offset, unsigned int val)
{
    int ret;
    unsigned int reg_base = 0x0;

    ret = uart_core_base_addr(uart_num, &reg_base);
    if (ret) {
        return;
    }

    writel(val, (uintptr_t)(reg_base + offset));
}

void uart_set_lcr_dlab(int uart_num, int dlab_sel)
{
    int ret;
    unsigned int lcr = 0;

    ret = uart_reg_read(uart_num, UART_LCR_REG_OFFSET, &lcr);
    if (ret) {
        return;
    }

    if (dlab_sel) {
        lcr |= UART_LCR_DLAB;
    } else {
        lcr &= ~UART_LCR_DLAB;
    }

    uart_reg_write(uart_num, UART_LCR_REG_OFFSET, lcr);
}

void uart_set_dll_dlh(int uart_num, unsigned int dll, unsigned int dlh)
{
    /* Enable DLL/DLH/FCR access */
    uart_set_lcr_dlab(uart_num, 1);
    uart_reg_write(uart_num, UART_DLL_REG_OFFSET, dll);
    uart_reg_write(uart_num, UART_DLH_REG_OFFSET, dlh);
    uart_set_lcr_dlab(uart_num, 0);
}

void uart_get_dll_dlh(int uart_num, unsigned int *dll, unsigned int *dlh)
{
    /* Enable DLL/DLH/FCR access */
    uart_set_lcr_dlab(uart_num, 1);
    uart_reg_read(uart_num, UART_DLL_REG_OFFSET, dll);
    uart_reg_read(uart_num, UART_DLH_REG_OFFSET, dlh);
    uart_set_lcr_dlab(uart_num, 0);
}

void uart_set_fifo_ctrl(int uart_num, unsigned int fifo_ctrl)
{
    /* Enable DLL/DLH/FCR access */
    uart_set_lcr_dlab(uart_num, 1);
    uart_reg_write(uart_num, UART_FCR_REG_OFFSET, fifo_ctrl);
    uart_set_lcr_dlab(uart_num, 0);
}

void uart_set_lcr(int uart_num, unsigned int lcr)
{
    uart_reg_write(uart_num, UART_LCR_REG_OFFSET, lcr);
}

int uart_is_txfifo_full(int uart_num)
{
    int ret;
    unsigned int usr = 0;

    uart_set_lcr_dlab(uart_num, 0);
    ret = uart_reg_read(uart_num, UART_USR_REG_OFFSET, &usr);
    if (ret) {
        return 0;
    }
    return (usr & UART_USR_TFNF) ? 0 : 1;
}

int uart_is_rx_ready(int uart_num)
{
    unsigned int lsr = 0;

    uart_set_lcr_dlab(uart_num, 0);
    (void)uart_reg_read(uart_num, UART_LSR_REG_OFFSET, &lsr);
    return (lsr & UART_LSR_DR) ? 1 : 0;
}

void uart_tx_char(int uart_num, const char c)
{
    uart_set_lcr_dlab(uart_num, 0);
    /*lint -e571*/
    uart_reg_write(uart_num, UART_THR_REG_OFFSET, (unsigned int)c);
}

void uart_rx_char(int uart_num, char *c)
{
    unsigned int rbr = 0;

    uart_set_lcr_dlab(uart_num, 0);
    (void)uart_reg_read(uart_num, UART_RBR_REG_OFFSET, &rbr);
    *c = (char)(rbr & 0xFF);
}
