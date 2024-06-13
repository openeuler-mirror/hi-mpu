// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/io.h>
#include <linux/init.h>
#include <linux/module.h>

#define WDG0_DEVMEM 0x08768C00
#define WDG1_DEVMEM 0x08768008
#define USB_DEVMEM 0x40000081
#define WDG0_DEVMEM_VALUE 0x1acce551

int devmem_init(void)
{
    unsigned char *wdg0;
    unsigned char *wdg1;
    unsigned char *usb_identify;
    int size = 4;
    wdg0 = (unsigned char *)ioremap(WDG0_DEVMEM, size);
    wdg1 = (unsigned char *)ioremap(WDG1_DEVMEM, size);
    usb_identify = (unsigned char *)ioremap(WDG1_DEVMEM, size);

    printk("devmem start\n");
    if (NULL == wdg0) {
        printk("wdg unlock fail\n");
    } else {
        writel(WDG0_DEVMEM_VALUE, wdg0);
        iounmap(wdg0);
    }

    if (NULL == wdg1) {
        printk("wdg close fail\n");
    } else {
        writel(0, wdg1);
        iounmap(wdg1);
    }

    if (NULL == usb_identify) {
        printk("usb_identify open fail\n");
    } else {
        writeb(0, usb_identify);
        iounmap(usb_identify);
    }
    return 0;
}

void devmem_exit(void)
{
    printk("devmem exit\n");
}

module_init(devmem_init);
module_exit(devmem_exit);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("devmem driver");