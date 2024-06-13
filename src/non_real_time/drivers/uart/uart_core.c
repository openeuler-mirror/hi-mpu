// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * uart module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/io.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/console.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/serial_core.h>
#include <linux/serial.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/slab.h>
#include <linux/of.h>
#include "uart_ker.h"
#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "lp_subsys_csr_addr_define.h"

static struct hisi_uart_core g_uart_core;

static struct uart_driver g_uart_driver = {
    .owner       = THIS_MODULE,
    .driver_name = "ttySS",
    .dev_name    = "ttySS",
    .major       = 0, /* Let the kernel automatically allocate */
    .minor       = SERIAL_DW_MINOR,
    .nr          = UART_NR,
    .cons        = NULL,
};

static int hisi_uart_get_srst_req_ctrl(unsigned int uart_no, unsigned int *reg_offset, unsigned int *mask)
{
    *reg_offset = LP_SUBSYS_CSR_LPSC_SRST_REQ1_OFFSET_ADDR;

    switch (uart_no) {
        case 0: /* uart0, for codestyle */
        case 1: /* uart1, for codestyle */
            *mask = (1UL << (uart_no + 12UL)); /* BIT12,13 is used to reset uart0,1 */
            break;
        case 2: /* uart2, for codestyle */
        case 3: /* uart3, for codestyle */
        case 4: /* uart4, for codestyle */
        case 5: /* uart5, for codestyle */
        case 6: /* uart6, for codestyle */
        case 7: /* uart7, for codestyle */
            *mask = (1UL << ((uart_no - 2) + 22)); /* BIT22-27 is used to reset uart2-7 */
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

int hisi_uart_core_connect(unsigned int uart_no)
{
    u32 reg_mask;

    if (uart_no >= MAX_UART_NUM) {
        return -EINVAL;
    }

    /* Bit0-7 represents the uart_en */
    reg_mask = (1UL << uart_no);
    lp_subsys_reg_set_bits(LP_SUBSYS_CSR_LPSC_UART_SEL_OFFSET_ADDR, reg_mask, 1);

    return 0;
}
EXPORT_SYMBOL(hisi_uart_core_connect);

int hisi_uart_core_disconnect(unsigned int uart_no)
{
    u32 reg_mask;

    if (uart_no >= MAX_UART_NUM) {
        return -EINVAL;
    }

    /* Bit0-7 represents the uart_en */
    reg_mask = (1UL << uart_no);
    lp_subsys_reg_set_bits(LP_SUBSYS_CSR_LPSC_UART_SEL_OFFSET_ADDR, reg_mask, 0);

    return 0;
}
EXPORT_SYMBOL(hisi_uart_core_disconnect);

int hisi_uart_core_reset_port(unsigned int uart_no)
{
    unsigned int srst_reg_off = 0;
    unsigned int srst_reg_mask = 0;

    if (uart_no >= MAX_UART_NUM && g_uart_core.core_init == 0) {
        LOG(LOG_ERROR, "inval param port(%u) core_init(%d)\n", uart_no, g_uart_core.core_init);
        return -EINVAL;
    }

    if (hisi_uart_get_srst_req_ctrl(uart_no, &srst_reg_off, &srst_reg_mask)) {
        return -EINVAL;
    }

    if (m_reset_handle_atomic(srst_reg_off, srst_reg_mask)) {
        return -EIO;
    }

    return 0;
}
EXPORT_SYMBOL(hisi_uart_core_reset_port);

int hisi_uart_core_get_uartclk(unsigned int *uartclk)
{
    if ((g_uart_core.core_init == 0) || (uartclk == NULL)) {
        return -EINVAL;
    }

    *uartclk = g_uart_core.uartclk;
    return 0;
}
EXPORT_SYMBOL(hisi_uart_core_get_uartclk);

int hisi_uart_core_unregister_port(struct uart_dw_port *udp)
{
    if (udp == NULL || g_uart_core.core_init == 0) {
        LOG(LOG_ERROR, "inval param port(%p) core_init(%d)\n", udp, g_uart_core.core_init);
        return -EINVAL;
    }

    (void)uart_remove_one_port(&g_uart_driver, &udp->port);

    g_uart_core.port_registered &= ~(1UL << udp->port_no);

    return 0;
}
EXPORT_SYMBOL(hisi_uart_core_unregister_port);

int hisi_uart_core_register_port(struct uart_dw_port *udp)
{
    int ret;

    if (udp == NULL || g_uart_core.core_init == 0) {
        LOG(LOG_ERROR, "inval param port(%p) core_init(%d)\n", udp, g_uart_core.core_init);
        return -EINVAL;
    }

    ret = uart_add_one_port(&g_uart_driver, &udp->port);
    if (ret) {
        LOG(LOG_ERROR, "uart_add_one_port error %d\n", ret);
        return ret;
    }

    g_uart_core.port_registered |= (1UL << udp->port_no);

    return 0;
}
EXPORT_SYMBOL(hisi_uart_core_register_port);

static int uart_core_of_probe(struct platform_device *pdev)
{
    int ret;
    unsigned int val = 0;
    struct resource *regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);

    if (regs == NULL) {
        return -EINVAL;
    }

    if (!g_uart_core.core_init) {
        if (of_property_read_u32(pdev->dev.of_node, "clock-frequency", &val)) {
            LOG(LOG_ERROR, "cannot get clock-frequency for uart core\n");
            return -ENODEV;
        }

        g_uart_core.uartclk = val;

        ret = uart_register_driver(&g_uart_driver);
        if (ret) {
            LOG(LOG_ERROR, "uart_register_driver failed (%d)\n", ret);
            return ret;
        }

        g_uart_core.core_init = 1;
    }

    return 0;
}

static int uart_core_of_remove(struct platform_device *pdev)
{
    if (g_uart_core.core_init) {
        if (g_uart_core.port_registered) {
            /* One or more uart port exist, cannot remove g_uart_driver */
            return -EBUSY;
        }

        uart_unregister_driver(&g_uart_driver);
        g_uart_core.core_init = 0;
    }

    return 0;
}

static const struct of_device_id g_hisi_uart_core_of_match[] = {
    { .compatible = "hisilicon,hisi-uart-core" }, {}
};

static struct platform_driver g_hisi_uart_core_driver = {
    .driver = {
        .name = "hisi-uart-core",
        .of_match_table = g_hisi_uart_core_of_match,
    },
    .probe = uart_core_of_probe,
    .remove = uart_core_of_remove,
};

module_platform_driver(g_hisi_uart_core_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("uart core driver");