/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Simple U-Boot driver for the Hi309x UARTs
 * Create: 2018-05-10
 */
#include <common.h>
#include <dm.h>
#include <serial.h>
#include <linux/compiler.h>
#include <fdtdec.h>
#include <hi309x_common.h>
#include <asm/io.h>

#ifndef CONFIG_DM_SERIAL
#error "Hi309x serial driver only support DM_SERIAL, please define CONFIG_DM_SERIAL"
#endif

struct hi309x_serial_platdata {
    unsigned char rsvd[4]; /* revserse size 4 bit */
};

struct hi309x_priv {
    unsigned char rsvd[4]; /* revserse size 4 bit */
};

static int hi309x_serial_setbrg(struct udevice *dev, int baudrate)
{
    uart_set_baudrate(CONFIG_SYS_UART_NUM, baudrate, CONFIG_SYS_APB_CLOCK_FREQ);
    return 0;
}

static int hi309x_serial_probe(struct udevice *dev)
{
    uart_early_puts(CONFIG_SYS_UART_NUM, "Serial Probe!\r\n");
    return 0;
}

static int hi309x_serial_getc(struct udevice *dev)
{
    return (int)uart_rx(CONFIG_SYS_UART_NUM);
}

static int hi309x_serial_putc(struct udevice *dev, const char ch)
{
    return uart_tx(CONFIG_SYS_UART_NUM, ch);
}

static int hi309x_serial_pending(struct udevice *dev, bool input)
{
    if (input) {
        return uart_rx_ready(CONFIG_SYS_UART_NUM);
    } else {
        return !uart_tx_ready(CONFIG_SYS_UART_NUM);
    }
}

static const struct dm_serial_ops g_hi309x_serial_ops = {
    .putc = hi309x_serial_putc,
    .pending = hi309x_serial_pending,
    .getc = hi309x_serial_getc,
    .setbrg = hi309x_serial_setbrg,
};

#if CONFIG_IS_ENABLED(OF_CONTROL)

static const struct udevice_id g_hi309x_serial_id[] = {
    {
        .compatible = "arm,hi309x",
        .data = 0x43
    },
    {}
};

static int hi309x_serial_ofdata_to_platdata(struct udevice *dev)
{
    return 0;
}
#endif

U_BOOT_DRIVER(serial_hi309x) = {
    .name = "serial",
    .id = UCLASS_SERIAL,
    .of_match = of_match_ptr(g_hi309x_serial_id),
    .of_to_plat = of_match_ptr(hi309x_serial_ofdata_to_platdata),
    .plat_auto = sizeof(struct hi309x_serial_platdata),
    .probe = hi309x_serial_probe,
    .ops = &g_hi309x_serial_ops,
    .flags = DM_FLAG_PRE_RELOC,
    .priv_auto = sizeof(struct hi309x_priv),
};

/*lint -e729*/
struct hi309x_serial_platdata g_platdata_non_fdt;

U_BOOT_DRVINFO(serial_hi309x_non_fdt) = {
    .name = "serial",
    .plat = &g_platdata_non_fdt,
};
