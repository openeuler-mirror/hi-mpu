/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#include <dm.h>
#include <fdtdec.h>
#include <asm/arch-hi309x/gpio.h>
#include <asm/io.h>
#include <dm/device-internal.h>
#include <linux/errno.h>
#include <malloc.h>

static int io_cfg_reg_read_write_by_type(uint reg_type, uint off, uint mask, uint val)
{
    int ret;
    switch (reg_type) {
        case IO_CONFIG_R:
            ret = io_config_r_read_write(off, mask, val);
            break;
        case IO_CONFIG_T:
            ret = io_config_t_read_write(off, mask, val);
            break;
        default:
            ret = -EINVAL;
            printf("cfg reg type 0x%x err!\n", reg_type);
            break;
    }

    return ret;
}

static bool gpio_pin_num_valid(uint pin)
{
    if (pin >= MAX_GPIO_CNT) {
        printf("ERROR : gpio pin num(=%u) out of range\n", pin);
        return false;
    }

    return true;
}

static int _get_gpio_direction(struct gpio_bank *bank, uint pin)
{
    int data;

    data = readl(bank->base + GPIO_DIR_OFFSET_ADDR);

    if (data & (1U << pin)) {
        return GPIO_DIR_OUTPUT;
    } else {
        return GPIO_DIR_INPUT;
    }
}

/* read GPIO IN value of 'pin' */
static int hi309x_gpio_get_value(struct udevice *dev, unsigned pin)
{
    struct gpio_bank *bank = dev_get_priv(dev);
    uint data;
    int ret;

    data = readl(bank->base + GPIO_INPUT_OFFSET_ADDR);
    ret = (data & (1U << pin)) ? 1 : 0;

    debug("%s %d\n", __func__, ret);
    return ret;
}

/* write GPIO OUT value to 'pin' */
static int hi309x_gpio_set_value(struct udevice *dev, unsigned pin, int value)
{
    struct gpio_bank *bank = dev_get_priv(dev);
    uint data;

    data = readl(bank->base + GPIO_OUTPUT_OFFSET_ADDR);

    if (value) {
        data |= (1U << pin);    /* "!!value" converts the value to a logical value (0 or 1U) */
    } else {
        data &= ~(1U << pin);
    }

    writel(data, bank->base + GPIO_OUTPUT_OFFSET_ADDR);

    return 0;
}

/* set GPIO 'pin' as an input */
static int hi309x_gpio_direction_input(struct udevice *dev, unsigned pin)
{
    struct gpio_bank *bank = dev_get_priv(dev);
    uint data;

    data = readl(bank->base + GPIO_DIR_OFFSET_ADDR);
    data &= ~(1U << pin);
    writel(data, bank->base + GPIO_DIR_OFFSET_ADDR);

    return 0;
}

/* Set gpio direction as output */
static int hi309x_gpio_direction_output(struct udevice *dev, unsigned pin, int value)
{
    struct gpio_bank *bank = dev_get_priv(dev);
    uint data;

    data = readl(bank->base + GPIO_DIR_OFFSET_ADDR);
    data |= 1U << pin;
    writel(data, bank->base + GPIO_DIR_OFFSET_ADDR);

    hi309x_gpio_set_value(dev, pin, value);

    return 0;
}

/* Obtain gpio direction */
static int hi309x_gpio_get_function(struct udevice *dev, unsigned pin)
{
    struct gpio_bank *bank = dev_get_priv(dev);
    int dir;

    dir =  _get_gpio_direction(bank, pin);
    debug("%s %d\n", __func__, dir);
    return dir;
}

/* Request a gpio before using it. */
static int hi309x_gpio_request(struct udevice *dev, unsigned pin, const char *label)
{
    int ret;
    gpio_func_set *pin_set = NULL;
    uint func_index = 0;
    struct gpio_bank *bank = dev_get_priv(dev);
    uint pin_map = bank->index * GPIO_GROUP_MAX_PIN_NUM + pin;

    if (!gpio_pin_num_valid(pin_map)) {
        return -EINVAL;
    }

    pin_set = &g_gpio_func_tbl[pin_map];
    if (pin_set->pin != pin_map) {
        printf("ERROR: set is not order by pin,please check(pin 0x%x)!", pin_map);
        return -EINVAL;
    }

    ret = io_cfg_reg_read_write_by_type(pin_set->reg_type[func_index], pin_set->reg_off[func_index],
                                        pin_set->reg_mask[func_index], pin_set->reg_val[func_index]);
    if (ret) {
        printf("ERROR: pin 0x%x write cfg err!", pin_map);
    }

    return ret;
}

static int hi309x_gpio_probe(struct udevice *dev)
{
    struct gpio_bank *bank = dev_get_priv(dev);
    struct hi309x_gpio_plat *plat = dev_get_plat(dev);
    struct gpio_dev_priv *uc_priv = dev_get_uclass_priv(dev);
    char name[18], *str;

    sprintf(name, "gpio@%4x_", (uint)plat->base);
    str = strdup(name);
    if (!str) {
        return -ENOMEM;
    }

    uc_priv->bank_name = str;
    dev->name = str;
    uc_priv->gpio_count = HI309X_GPIO_PER_BANK;
    uc_priv->gpio_base = plat->bank_index * HI309X_GPIO_PER_BANK;

    bank->base = (void *)plat->base;
    bank->index = plat->bank_index;

    return 0;
}

static const struct dm_gpio_ops gpio_hi309x_ops = {
	.direction_input	= hi309x_gpio_direction_input,
	.direction_output	= hi309x_gpio_direction_output,
	.get_value		= hi309x_gpio_get_value,
	.set_value		= hi309x_gpio_set_value,
	.get_function		= hi309x_gpio_get_function,
    .request = hi309x_gpio_request,
};

U_BOOT_DRIVER(gpio_hi309x) = {
    .name = "gpio_hi309x",
    .id = UCLASS_GPIO,
    .ops = &gpio_hi309x_ops,
    .probe = hi309x_gpio_probe,
    .plat_auto	= sizeof(struct hi309x_gpio_plat),
    .priv_auto	= sizeof(struct gpio_bank)
};

static const struct hi309x_gpio_plat hi309x_gpio[] = {
	{ 0, 0x874A000},
	{ 1, 0x874B000},
	{ 2, 0x874C000},
	{ 3, 0x874D000},
};

U_BOOT_DRVINFOS(hi309x_gpios) = {
	{ "gpio_hi309x", &hi309x_gpio[0] },
	{ "gpio_hi309x", &hi309x_gpio[1] },
	{ "gpio_hi309x", &hi309x_gpio[2] },
	{ "gpio_hi309x", &hi309x_gpio[3] },
};