// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Gpio linux driver, which can be used with the Linux GPIO general-purpose input/output framework.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/io.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/gpio/driver.h>

#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "io_config_t_addr_define.h"
#include "io_config_r_addr_define.h"
#include "io_config.h"
#include "board_info.h"
#include "gpio_ker.h"

struct hi309x_gpio_chip {
    void __iomem *gpio_base;
    struct gpio_chip gc;
};

static struct hi309x_gpio_chip g_hi309x_chip = { 0 };
static gpio_pri_mgr_s g_gpio_pri_mgr;
static gpio_int_mgr_s g_gpio_int_mgr = {
    .init_flag = 0,
    .ext_int_pin_mask = { 0 },
    .ext_int_info = {
        { 0,  64 }, { 1, 102 }, { 2, 95 }, { 3, 96 }, { 4, 97 }, { 5, 98 }, { 6, 99 }, { 7, 100 }, { 8, 79 },
        { 9,  80 }, { 10, 81 }, { 11, 82 }, { 12, 83 }, { 13, 84 }, { 14, 85 }, { 15, 86 }, { 16, 87 }, { 17, 88 },
        { 18, 89 }, { 19, 90 }, { 20, 24 }
    },
};

static u32 gpio_read_reg(u32 group, u32 offset)
{
    return ioread32(g_gpio_pri_mgr.gpio_base + group * 0x1000 + offset);
}

static void gpio_write_reg(u32 group, u32 offset, u32 val)
{
    iowrite32(val, g_gpio_pri_mgr.gpio_base + group * 0x1000 + offset);
}

static u8 g_grade[IO_MAX_ID + 1] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static u8 g_epad_r[IO_R_GPIO_NUM] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 20, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 38, 40, 41, 42, 43, 44, 45, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 64,
    66, 67, 68, 69
};

static u8 g_epad_t[IO_T_GPIO_NUM] = {
    0, 1, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 56, 57, 58, 59, 60, 61, 62, 63, 64, 69, 70,
    71, 72, 73
};

static s32 gpio_set_drive_config(const io_drive_config_info_s *io_drive_config_info)
{
    u32 offset;
    u32 mask;
    u32 type;
    s32 ret;
    u8 id = io_drive_config_info->id;
    u32 val = io_drive_config_info->drive_value;
    u32 gpio_id;

    if (id > IO_MAX_ID) {
        LOG(LOG_ERROR, "gpio id%u out of range(0--126)", id);
        return -EINVAL;
    }

    if (g_grade[id] && (val > GRADE8_MAX_VAL)) {
        LOG(LOG_ERROR, "gpio%u exceeds the configurable(0--7) drive level(%u)", id, val);
        return -EINVAL;
    } else if (!g_grade[id] && (val > GRADE4_MAX_VAL)) {
        LOG(LOG_ERROR, "gpio%u exceeds the configurable(0--3) drive level(%u)", id, val);
        return -EINVAL;
    }

    gpio_id = (id > IO_T_GPIO_NUM) ? (id - IO_R_GPIO_NUM) : id;
    type = (id >= IO_R_GPIO_NUM) ? IO_MULTI_REG_TYPE_IOCFG_T : IO_MULTI_REG_TYPE_IOCFG_R;
    switch (type) {
        case IO_MULTI_REG_TYPE_IOCFG_R:
            offset = BYTE_NUM * g_epad_r[gpio_id] + IO_CONFIG_R_PADMG000_OFFSET_ADDR;
            mask = g_grade[id] ? GRADE8_MASK : GRADE4_MASK;
            val = (val << GRADE_BIT);
            ret = io_config_r_reg_read_write(offset, mask, val);
            break;

        case IO_MULTI_REG_TYPE_IOCFG_T:
            offset = BYTE_NUM * g_epad_t[id - IO_R_GPIO_NUM] + IO_CONFIG_T_PADMG000_OFFSET_ADDR;
            mask = g_grade[id] ? GRADE8_MASK : GRADE4_MASK;
            val = (val << GRADE_BIT);
            ret = io_config_t_reg_read_write(offset, mask, val);
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}

static s32 gpio_get_drive_config(io_drive_config_info_s *io_drive_config_info)
{
    u8 id = io_drive_config_info->id;
    u32 offset;
    u32 type;
    u32 mask;
    u32 value = 0;
    s32 ret;
    u32 gpio_id;

    if (id > IO_MAX_ID) {
        LOG(LOG_ERROR, "gpio id%u out of range(0--126)", id);
        return -EINVAL;
    }

    gpio_id = (id > IO_T_GPIO_NUM) ? (id - IO_R_GPIO_NUM) : id;
    type = (id >= IO_R_GPIO_NUM) ? IO_MULTI_REG_TYPE_IOCFG_T : IO_MULTI_REG_TYPE_IOCFG_R;
    switch (type) {
        case IO_MULTI_REG_TYPE_IOCFG_R:
            offset = BYTE_NUM * g_epad_r[gpio_id] + IO_CONFIG_R_PADMG000_OFFSET_ADDR;
            mask = g_grade[id] ? GRADE8_MASK : GRADE4_MASK;
            ret = io_config_r_reg_read(offset, &value);
            io_drive_config_info->drive_value = (u8)((value & mask) >> GRADE_BIT);
            break;

        case IO_MULTI_REG_TYPE_IOCFG_T:
            offset = BYTE_NUM * g_epad_t[id - IO_R_GPIO_NUM] + IO_CONFIG_T_PADMG000_OFFSET_ADDR;
            mask = g_grade[id] ? GRADE8_MASK : GRADE4_MASK;
            ret = io_config_t_reg_read(offset, &value);
            io_drive_config_info->drive_value = (u8)((value & mask) >> GRADE_BIT);
            break;
        default:
            ret = -ENOTTY;
    }

    return ret;
}

static bool gpio_pin_num_valid(u32 pin)
{
    if ((pin >= MAX_GPIO_CNT) && (pin != SPEC_GPIO_BIT30_PIN) && (pin != SPEC_GPIO_BIT31_PIN)) {
        return false;
    }

    return true;
}

static s32 gpio_get_group_pin_num(u32 pin, u32 *group, u32 *pin_num, u32 *pin_mask)
{
    u32 group_num;
    u32 pin_no;

    if (!gpio_pin_num_valid(pin)) {
        return -EINVAL;
    }

    group_num = pin / GPIO_GROUP_MAX_PIN_NUM;
    pin_no = pin % GPIO_GROUP_MAX_PIN_NUM;

    if (pin_mask != NULL) {
        *pin_mask = (1U << pin_no);
    }

    if (pin_num != NULL) {
        *pin_num = pin_no;
    }

    if (group != NULL) {
        *group = group_num;
    }

    return 0;
}

static u32 gpio_is_spec_pin_skip_process(u32 pin)
{
    if ((pin == SPEC_GPIO_BIT30_PIN) || (pin == SPEC_GPIO_BIT31_PIN)) {
        return 1;
    }

    return 0;
}

static u32 gpio_is_ext_int_pin(u32 group, u32 pin)
{
    if ((group >= GPIO_MAX_GROUP_NUM) || !g_gpio_int_mgr.init_flag) {
        LOG(LOG_ERROR, "gpio group(=%u) out of range or mgr not init", group);
        return 0;
    }

    if (g_gpio_int_mgr.ext_int_pin_mask[group] & (1U << pin)) {
        return 1;
    }

    return 0;
}

static u32 gpio_get_ext_int_pin(u32 ext_index)
{
    return g_gpio_int_mgr.ext_int_info[ext_index].gpio_pin;
}

static u32 gpio_get_ext_index_by_pin(u32 group, u32 pin)
{
    u32 gpio_num;
    u32 i;
    ext_int_s *ext_info = &g_gpio_int_mgr.ext_int_info[0];

    gpio_num = group * GPIO_GROUP_MAX_PIN_NUM + pin;
    for (i = 0; i < TOTAL_INT_NUM; i++) {
        if (ext_info[i].gpio_pin == gpio_num) {
            break;
        }
    }

    return i;
}

s32 gpio_isr_start(u32 pin)
{
    s32 ret;
    u32 group = 0;
    u32 pin_mask = 0;
    unsigned long flag;

    ret = gpio_get_group_pin_num(pin, &group, NULL, &pin_mask);
    if (ret) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    gpio_set_reg_bit_val(group, REG_GPIO_INTMASK, pin_mask, 0);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}
EXPORT_SYMBOL(gpio_isr_start);

s32 gpio_isr_stop(u32 pin)
{
    u32 group = 0;
    u32 pin_mask = 0;
    unsigned long flag;

    if (gpio_get_group_pin_num(pin, &group, NULL, &pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    gpio_set_reg_bit_val(group, REG_GPIO_INTMASK, pin_mask, 1);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}
EXPORT_SYMBOL(gpio_isr_stop);

static s32 io_cfg_reg_read_write_by_type(u32 reg_type, u32 off, u32 mask, u32 val)
{
    s32 ret;
    switch (reg_type) {
        case REG_TYPE_IOR:
            ret = io_config_r_reg_read_write(off, mask, val);
            break;
        case REG_TYPE_IOT:
            ret = io_config_t_reg_read_write(off, mask, val);
            break;
        case REG_TYPE_LP:
            ret = lp_subsys_reg_read_write(off, mask, val);
            break;
        default:
            ret = -EFAULT;
            LOG(LOG_ERROR, "cfg reg type 0x%x err!", reg_type);
            break;
    }

    return ret;
}

static s32 io_cfg_reg_read_by_type(u32 reg_type, u32 off, u32 mask, u32 *val)
{
    s32 ret;
    u32 temp_val = 0;

    switch (reg_type) {
        case REG_TYPE_IOR:
            ret = io_config_r_reg_read(off, &temp_val);
            break;
        case REG_TYPE_IOT:
            ret = io_config_t_reg_read(off, &temp_val);
            break;
        case REG_TYPE_LP:
            ret = lp_subsys_reg_read(off, &temp_val);
            break;
        default:
            ret = -EFAULT;
            LOG(LOG_ERROR, "cfg reg type 0x%x err!", reg_type);
            break;
    }

    *val = mask & temp_val;

    return ret;
}

static s32 io_cfg_get_func_index(gpio_func_set_s *pin_set, u32 func_type, u32 *func_index)
{
    u32 i;

    for (i = 0; i < pin_set->func_cnt; i++) {
        if (func_type == pin_set->func[i]) {
            break;
        }
    }

    if (pin_set->func_cnt == i) {
        LOG(LOG_ERROR, "pin 0x%x func 0x%x not support cfg!", pin_set->pin, func_type);
        return -EFAULT;
    }

    *func_index = i;

    return 0;
}

static s32 gpio_set_reuse_by_pinfunc(u32 pin, u32 func_type)
{
    s32 ret;
    gpio_func_set_s *pin_set = NULL;
    u32 func_index = 0;

    if (!gpio_pin_num_valid(pin)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    if (gpio_is_spec_pin_skip_process(pin)) {
        return 0;
    }

    pin_set = &g_io_func_set[pin];
    if (pin_set->pin != pin) {
        LOG(LOG_ERROR, "set is not order by pin,please check(pin 0x%x)!", pin);
        return -EINVAL;
    }

    ret = io_cfg_get_func_index(pin_set, func_type, &func_index);
    if (ret) {
        LOG(LOG_ERROR, "pin 0x%x func 0x%x not support cfg!", pin, func_type);
        return -EFAULT;
    }

    ret = io_cfg_reg_read_write_by_type(pin_set->reg_type[func_index], pin_set->reg_off[func_index],
                                        pin_set->reg_mask[func_index], pin_set->reg_val[func_index]);
    if (ret) {
        LOG(LOG_ERROR, "pin 0x%x func 0x%x write cfg err!", pin, func_type);
        return ret;
    }

    return ret;
}

static s32 gpio_get_reuse_by_pin(u32 pin)
{
    s32 io_result = -1;
    s32 ret;
    u32 i = 0;
    gpio_func_set_s *pin_set = NULL;
    u32 func_index = 0;
    u32 cfg_val = 0;

    if (!gpio_pin_num_valid(pin)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return io_result;
    }

    if (gpio_is_spec_pin_skip_process(pin)) {
        return 0;
    }

    pin_set = &g_io_func_set[pin];
    if (pin_set->pin != pin) {
        LOG(LOG_ERROR, "set is not order by pin,please check(pin 0x%x)!", pin);
        return io_result;
    }

    ret = io_cfg_get_func_index(pin_set, FUNC_GPIO, &func_index);
    if (ret) {
        LOG(LOG_ERROR, "pin 0x%x gpio not support cfg!", pin);
        return io_result;
    }

    ret = io_cfg_reg_read_by_type(pin_set->reg_type[i], pin_set->reg_off[i], pin_set->reg_mask[i], &cfg_val);
    if (ret) {
        LOG(LOG_ERROR, "pin 0x%x read cfg err!", pin);
        return io_result;
    }

    io_result = (cfg_val == pin_set->reg_val[func_index]) ? 0 : 1;

    return io_result;
}

static s32 gpio_set_reuse_status(u32 group, u32 pin)
{
    s32 ret;

    ret = gpio_set_reuse_by_pinfunc(group * GPIO_GROUP_MAX_PIN_NUM + pin, FUNC_GPIO);

    return ret;
}

static s32 gpio_get_reuse_status(u32 group, u32 pin)
{
    s32 ret;

    ret = gpio_get_reuse_by_pin(group * GPIO_GROUP_MAX_PIN_NUM + pin);

    return ret;
}

static s32 gpio_set_interrupt(u32 pin, u8 flag, gpio_isr handler, void *data)
{
    u32 group_num = 0;
    u32 group_pin = 0;
    u32 pin_mask = 0;
    s32 ret;
    unsigned long lock_flag;

    /* GPIO interrupt type. 0: level-triggered mode; 1: edge-triggered mode */
    s32 gpio_int_type;

    /*
     * GPIO interrupt signal polarity select
     * 0：Active at low level (the corresponding control bit of GPIO_INTTYPE_LEVEL is 0) or falling edge
     * 1：High level (the corresponding control bit of GPIO_INTTYPE_LEVEL is set to 0) or rising edge
     */
    s32 gpio_int_plarity;

    if (flag > GPIO_IRQ_TYPE) {
        LOG(LOG_ERROR, "gpio irq_type(=%u) out of range(0--3)", flag);
        return -EINVAL;
    }

    if (gpio_get_group_pin_num(pin, &group_num, &group_pin, &pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    gpio_int_type = (flag >> 1) & 0x1;
    gpio_int_plarity = flag & 0x1;

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group_num], lock_flag);

    gpio_set_reg_bit_val(group_num, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group_num, REG_GPIO_DIR, pin_mask, 0);
    gpio_set_reg_bit_val(group_num, REG_GPIO_INTTYPE, pin_mask, gpio_int_type);
    gpio_set_reg_bit_val(group_num, REG_GPIO_INTPOLARITY, pin_mask, gpio_int_plarity);
    gpio_set_reg_bit_val(group_num, REG_GPIO_INTCLR, pin_mask, 1);

    gpio_set_reg_bit_val(group_num, REG_GPIO_INTDEBOUNCE, pin_mask, 1);
    udelay(10);

    if (handler) {
        gpio_set_reg_bit_val(group_num, REG_GPIO_INTEN, pin_mask, 1);
    } else {
        gpio_set_reg_bit_val(group_num, REG_GPIO_INTEN, pin_mask, 0);
    }

    g_gpio_int_mgr.gpio_isr_hanler[pin].isr = handler;
    g_gpio_int_mgr.gpio_isr_hanler[pin].data = data;

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group_num], lock_flag);

    ret = gpio_set_reuse_by_pinfunc(group_num * GPIO_GROUP_MAX_PIN_NUM + group_pin, FUNC_GPIO);
    if (ret) {
        LOG(LOG_ERROR, "gpio set reuse error %d", ret);
        return ret;
    }

    return 0;
}

s32 gpio_isr_register(u32 pin, u8 flag, gpio_isr handler, void *data)
{
    return gpio_set_interrupt(pin, flag, handler, data);
}

EXPORT_SYMBOL(gpio_isr_register);

s32 gpio_isr_unregister(u32 pin)
{
    return gpio_set_interrupt(pin, 0, NULL, NULL);
}

EXPORT_SYMBOL(gpio_isr_unregister);

static irqreturn_t irq_handler(s32 irq, void *dev_id)
{
    u32 i;
    u32 group_num;
    u32 count_i;
    u32 ext_index;
    u32 pin_mask;
    u32 raw_int_status;
    u32 gpio_num;

    group_num = *((u32 *)dev_id);
    /* 2 pin in group 4, 31 pin in group 3, 32 pin in each of the remaining 3 groups */
    if (group_num < GPIO_GROUP3) {
        count_i = GPIO_GROUP_MAX_PIN_NUM;
    } else if (group_num == GPIO_GROUP3) {
        count_i = GPIO_GROUP3_MAX_PIN_NUM;
    } else {
        count_i = GPIO_GROUP4_MAX_PIN_NUM;
    }

    for (i = 0; i < count_i; i++) {
        /* Only bits 30 and 31 of GPIO group 5 are valid. */
        pin_mask = (count_i == GPIO_GROUP4_MAX_PIN_NUM) ? (GPIO_BIT30 << i) : (GPIO_BIT0 << i);
        raw_int_status = gpio_get_reg_bit_val(group_num, REG_GPIO_INTRAWSTATUS, pin_mask);
        if (raw_int_status == 1) {
            /* Clear interrupt */
            gpio_set_reg_bit_val(group_num, REG_GPIO_INTCLR, pin_mask, 1);

            if (gpio_is_ext_int_pin(group_num, i)) {
                ext_index = gpio_get_ext_index_by_pin(group_num, i);
                g_gpio_int_mgr.int_event[ext_index] = GPIO_INT_STATE_READY;
                wake_up_interruptible(&g_gpio_int_mgr.gpio_int_private[ext_index].wait_queue);
            }

            gpio_num = group_num * GPIO_GROUP_MAX_PIN_NUM + i;
            if (g_gpio_int_mgr.gpio_isr_hanler[gpio_num].isr) {
                (void)(*(g_gpio_int_mgr.gpio_isr_hanler[gpio_num]).isr)(g_gpio_int_mgr.gpio_isr_hanler[gpio_num].data);
            }
        }
    }

    return IRQ_HANDLED;
}

static s32 gpio_open(struct inode *inodes, struct file *filp)
{
    return 0;
}

static s32 gpio_release(struct inode *inodes, struct file *filp)
{
    return 0;
}

static s32 gpio_check_reused_by_pin(const gpio_info_s *gpio_info, u32 group, u32 pin)
{
    s32 value;

    if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
        value = gpio_get_reuse_status(group, pin);
        if (value != 0) {
            LOG(LOG_ERROR, "the pin no %u is not reused by gpio", group * GPIO_GROUP_MAX_PIN_NUM + pin);
            return -ENODEV;
        }
    }

    return 0;
}

static s32 gpio_is_reused(const gpio_info_s *gpio_info)
{
    u32 group;
    u32 pin;

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (gpio_check_reused_by_pin(gpio_info, group, pin)) {
                return -ENODEV;
            }
        }
    }

    return 0;
}

static s32 gpio_set_reuse(const gpio_info_s *gpio_info)
{
    u32 group;
    u32 pin;
    s32 ret = 0;

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
                ret = gpio_set_reuse_status(group, pin);
            }
        }
    }

    return ret;
}

static void gpio_get_reuse(gpio_info_s * const gpio_info)
{
    u32 value;
    u32 group;
    u32 pin;

    (void)memset_s(gpio_info->gpio_value, sizeof(gpio_info->gpio_value), 0, sizeof(gpio_info->gpio_value));
    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
                value = (u32)gpio_get_reuse_status(group, pin);
                gpio_info->gpio_value[group] |= (value << pin);
            }
        }
    }

    return;
}

static void gpio_save_level(u32 group, u32 pin)
{
    u32 level;
    u32 pin_mask;

    pin_mask = (1U << pin);
    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_DIR, pin_mask, GPIO_DIR_INPUT);

    level = gpio_get_reg_bit_val(group, REG_GPIO_INPUT, pin_mask);
    level = level & 0x1;
    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_OUTPUT, pin_mask, level);

    return;
}

static s32 gpio_set_dir(u32 group, u32 pin, u32 dir)
{
    u32 pin_mask;
    u32 old_dir;
    unsigned long flag;

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    pin_mask = 1U << pin;
    old_dir = gpio_get_reg_bit_val(group, REG_GPIO_DIR, pin_mask);
    if ((dir == GPIO_DIR_OUTPUT) && (old_dir != dir)) {
        gpio_save_level(group, pin);
    }

    gpio_set_reg_bit_val(group, REG_GPIO_DIR, pin_mask, dir);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}

static s32 gpio_get_dir(gpio_info_s *gpio_info)
{
    u32 value;
    u32 group;
    u32 pin_num;
    u32 pin_mask;
    s32 ret;

    ret = gpio_is_reused(gpio_info);
    if (ret) {
        LOG(LOG_ERROR, "the pin group is not used by gpio");
        return -ENODEV;
    }

    (void)memset_s(gpio_info->gpio_value, sizeof(gpio_info->gpio_value), 0, sizeof(gpio_info->gpio_value));

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin_num = 0; pin_num < GPIO_GROUP_MAX_PIN_NUM; pin_num++) {
            if (((gpio_info->gpio_mask[group] >> pin_num) & 0x1) != 0) {
                pin_mask = 1U << pin_num;
                value = gpio_get_reg_bit_val(group, REG_GPIO_DIR, pin_mask);
                gpio_info->gpio_value[group] |= (value << pin_num);
            }
        }
    }

    return 0;
}

static s32 gpio_check_dir_by_pin(const gpio_info_s *gpio_info, u32 group, u32 pin, u32 dir)
{
    u32 value;
    u32 pin_mask;

    if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
        pin_mask = 1U << pin;
        value = gpio_get_reg_bit_val(group, REG_GPIO_DIR, pin_mask);
        if (value != dir) {
            LOG(LOG_ERROR, "the pin no %u dir is't the target value", group * GPIO_GROUP_MAX_PIN_NUM + pin);
            return -ENODEV;
        }
    }

    return 0;
}

static s32 gpio_check_direction(const gpio_info_s *gpio_info, u32 dir)
{
    u32 group;
    u32 pin_num;

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin_num = 0; pin_num < GPIO_GROUP_MAX_PIN_NUM; pin_num++) {
            if (gpio_check_dir_by_pin(gpio_info, group, pin_num, dir)) {
                return -ENODEV;
            }
        }
    }

    return 0;
}

static s32 gpio_get_level(gpio_info_s *gpio_info)
{
    s32 ret;
    u32 group;
    u32 pin;
    u32 pin_mask;
    u32 value;
    unsigned long flag;

    ret = gpio_is_reused(gpio_info);
    if (ret) {
        LOG(LOG_ERROR, "the pin group is not used by gpio");
        return ret;
    }

    ret = gpio_check_direction(gpio_info, GPIO_DIR_INPUT);
    if (ret) {
        LOG(LOG_ERROR, "the pin group dir is not all input");
        return ret;
    }

    (void)memset_s(gpio_info->gpio_value, sizeof(gpio_info->gpio_value), 0, sizeof(gpio_info->gpio_value));
    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
                pin_mask = 1U << pin;
                gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
                value = gpio_get_reg_bit_val(group, REG_GPIO_INPUT, pin_mask);
                gpio_info->gpio_value[group] |= (value << pin);
            }
        }

        spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);
    }

    return 0;
}

static s32 gpio_get_output_level(gpio_info_s *gpio_info)
{
    u32 value;
    u32 group;
    u32 pin;
    u32 pin_mask;

    if (gpio_is_reused(gpio_info)) {
        LOG(LOG_ERROR, "the pin group is not used by gpio");
        return -ENODEV;
    }

    (void)memset_s(gpio_info->gpio_value, sizeof(gpio_info->gpio_value), 0, sizeof(gpio_info->gpio_value));

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
                pin_mask = 1U << pin;
                value = gpio_get_reg_bit_val(group, REG_GPIO_INPUT, pin_mask);
                gpio_info->gpio_value[group] |= (value << pin);
            }
        }
    }

    return 0;
}

static s32 gpio_set_level(const gpio_info_s *gpio_info)
{
    u32 group;
    u32 pin;
    s32 ret;
    unsigned long flag;
    u32 pin_mask;
    u32 value;

    ret = gpio_is_reused(gpio_info);
    if (ret) {
        LOG(LOG_ERROR, "the pin group is not used by gpio");
        return ret;
    }

    ret = gpio_check_direction(gpio_info, GPIO_DIR_OUTPUT);
    if (ret) {
        LOG(LOG_ERROR, "the pin group dir is not all output");
        return ret;
    }

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
                pin_mask = 1U << pin;
                value = (gpio_info->gpio_value[group] >> pin) & 0x1;
                gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
                gpio_set_reg_bit_val(group, REG_GPIO_OUTPUT, pin_mask, value);
            }
        }

        spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);
    }

    return 0;
}

static s32 gpio_check_pin(const gpio_info_s *gpio_info)
{
    if (gpio_info->gpio_mask[GROUP3_INDEX] & GROUP3_INVALID_MASK) {
        return -EINVAL;
    }

    if (gpio_info->gpio_value[GROUP3_INDEX] & GROUP3_INVALID_MASK) {
        return -EINVAL;
    }

    if (gpio_info->gpio_mask[GROUP4_INDEX] & GROUP4_INVALID_MASK) {
        return -EINVAL;
    }

    if (gpio_info->gpio_value[GROUP4_INDEX] & GROUP4_INVALID_MASK) {
        return -EINVAL;
    }

    return 0;
}

static s32 gpio_get_int_event(gpio_int_s *gpio_int)
{
    s32 ret;
    u32 int_num;
    u32 timeout;

    int_num = gpio_int->gpio_int_num;
    timeout = gpio_int->timeout;

    if (int_num >= TOTAL_USER_INT_NUM) {
        LOG(LOG_ERROR, "int_num:%u error", int_num);
        return -EINVAL;
    }

    ret = down_interruptible(&g_gpio_int_mgr.gpio_int_private[int_num].access_sem);
    if (ret) {
        goto EXIT0;
    }

    gpio_int->value = INT_NO_HAPPEN;

    if (timeout == WAIT_ALL_TIME) {
        /*
         * When an interrupt of the corresponding pin occurs, contion is set to ready in sr_handle.
         * Only 20 pins int0-int19 are processed.
         */
        ret = wait_event_interruptible(g_gpio_int_mgr.gpio_int_private[int_num].wait_queue,
                                       (GPIO_INT_STATE_READY == g_gpio_int_mgr.int_event[int_num]));
        if (ret) {
            if (ret == -ERESTARTSYS) {
                ret = -EINTR;
            }
            goto EXIT1;
        }
    } else {
        ret = wait_event_interruptible_timeout(g_gpio_int_mgr.gpio_int_private[int_num].wait_queue,
                                               (GPIO_INT_STATE_READY == g_gpio_int_mgr.int_event[int_num]),
                                               (long)timeout);
        if (ret == -ERESTARTSYS) {
            ret = -EINTR;
            goto EXIT1;
        }

        if (!ret) {
            if (g_gpio_int_mgr.int_event[int_num] != GPIO_INT_STATE_READY) {
                ret = -ETIMEDOUT;
                goto EXIT1;
            }
        }

        ret = 0;
    }

    gpio_int->value = INT_HAPPEN;
    gpio_int->rsv = jiffies;
    g_gpio_int_mgr.int_event[int_num] = GPIO_INT_STATE_IDLE;

EXIT1:
    up(&g_gpio_int_mgr.gpio_int_private[int_num].access_sem);

EXIT0:
    return ret;
}

static s32 gpio_info_copy_and_check(gpio_info_s *gpio_info, unsigned long arg)
{
    s32 ret;

    ret = copy_from_user(gpio_info, (void *)arg, sizeof(gpio_info_s));
    if (ret) {
        LOG(LOG_ERROR, "copy_from_user errs(%d)", ret);
        return -EFAULT;
    }

    ret = gpio_check_pin(gpio_info);
    if (ret) {
        LOG(LOG_ERROR, "input param error(%d)", ret);
        return ret;
    }

    return ret;
}

static s32 gpio_cmd_set_dir(const gpio_info_s *gpio_info)
{
    u32 group;
    u32 pin;
    u32 value;
    s32 ret;

    ret = gpio_is_reused(gpio_info);
    if (ret) {
        LOG(LOG_ERROR, "the pin group is not used by gpio");
        return ret;
    }

    for (group = 0; group < GPIO_MAX_GROUP_NUM; group++) {
        if (!gpio_info->gpio_mask[group]) {
            continue;
        }

        for (pin = 0; pin < GPIO_GROUP_MAX_PIN_NUM; pin++) {
            if (((gpio_info->gpio_mask[group] >> pin) & 0x1) != 0) {
                value = (gpio_info->gpio_value[group] >> pin) & 0x1;
                gpio_set_dir(group, pin, value);
            }
        }
    }

    return 0;
}

static s32 gpio_cmd_reuse(u32 cmd, unsigned long arg)
{
    s32 ret;
    gpio_info_s gpio_info = { 0 };

    ret = gpio_info_copy_and_check(&gpio_info, arg);
    if (ret) {
        LOG(LOG_ERROR, "gpio_info copy or check error(%d)", ret);
        return ret;
    }

    switch (cmd) {
        case GPIO_CMD_SET_REUSE:
            ret = gpio_set_reuse(&gpio_info);
            break;

        case GPIO_CMD_GET_REUSE:
            gpio_get_reuse(&gpio_info);
            if (copy_to_user((void *)arg, &gpio_info, sizeof(gpio_info_s))) {
                LOG(LOG_ERROR, "GPIO_CMD_GET_REUSE copy_to_user errs");
                return -EFAULT;
            }
            break;

        default:
            return -ENOTTY;
    }

    return ret;
}

static s32 gpio_cmd_direction(u32 cmd, unsigned long arg)
{
    gpio_info_s gpio_info = { 0 };
    s32 ret;

    ret = gpio_info_copy_and_check(&gpio_info, arg);
    if (ret) {
        LOG(LOG_ERROR, "gpio_info copy or check error(%d)", ret);
        return ret;
    }

    switch (cmd) {
        case GPIO_CMD_SETDIR:
            ret = gpio_cmd_set_dir(&gpio_info);
            break;

        case GPIO_CMD_GETDIR:
            ret = gpio_get_dir(&gpio_info);
            if (ret) {
                LOG(LOG_ERROR, "gpio_get_dir error(%d)", ret);
                return ret;
            }

            if (copy_to_user((void *)arg, &gpio_info, sizeof(gpio_info_s))) {
                LOG(LOG_ERROR, "GPIO_CMD_GETDIR copy_to_user errs");
                return -EFAULT;
            }
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}

static s32 gpio_cmd_level(u32 cmd, unsigned long arg)
{
    s32 ret_value;
    gpio_info_s gpio_info = { 0 };

    ret_value = gpio_info_copy_and_check(&gpio_info, arg);
    if (ret_value) {
        LOG(LOG_ERROR, "gpio_info copy or check error(%d)", ret_value);
        return ret_value;
    }

    switch (cmd) {
        case GPIO_CMD_SETLEVEL:
            ret_value = gpio_set_level(&gpio_info);
            break;

        case GPIO_CMD_GETLEVEL:
            ret_value = gpio_get_level(&gpio_info);
            if (ret_value) {
                LOG(LOG_ERROR, "gpio_get_level error(%d)", ret_value);
                return ret_value;
            }

            if (copy_to_user((void *)arg, &gpio_info, sizeof(gpio_info_s))) {
                LOG(LOG_ERROR, "GPIO_CMD_GETLEVEL copy_to_user errs");
                return -EFAULT;
            }
            break;

        case GPIO_CMD_OUTPUT_GETLEVEL:
            ret_value = gpio_get_output_level(&gpio_info);
            if (ret_value) {
                LOG(LOG_ERROR, "gpio get output level error(%d)", ret_value);
                return ret_value;
            }

            if (copy_to_user((void *)arg, &gpio_info, sizeof(gpio_info_s))) {
                LOG(LOG_ERROR, "GPIO_CMD_OUTPUT_GETLEVEL copy_to_user errs");
                return -EFAULT;
            }
            break;

        default:
            return -ENOTTY;
    }

    return ret_value;
}

static s32 gpio_cmd_int_config(u32 cmd, unsigned long arg)
{
    u32 group;
    u32 pin;
    s32 ret;
    u32 gpio_int_id;
    u32 pin_mask;
    gpio_int_set_s gpio_int_config; /* Sets the interrupt trigger polarity of the external interrupt pin */
    unsigned long flag;

    if (copy_from_user(&gpio_int_config, (void *)arg, sizeof(gpio_int_config))) {
        LOG(LOG_ERROR, "CMD 0x%x copy_from_user errs", cmd);
        return -EFAULT;
    }

    gpio_int_id = gpio_int_config.gpio_int_num;
    if (gpio_int_id >= TOTAL_USER_INT_NUM) {
        LOG(LOG_ERROR, "gpio_int_id:%u error", gpio_int_id);
        return -EINVAL;
    }

    if (gpio_int_config.int_level > UP_TYPE) {
        LOG(LOG_ERROR, "gpio_int_config.int_level:%u error", gpio_int_config.int_level);
        return -EINVAL;
    }

    group = gpio_get_ext_int_pin(gpio_int_id) / GPIO_GROUP_MAX_PIN_NUM;
    pin = gpio_get_ext_int_pin(gpio_int_id) % GPIO_GROUP_MAX_PIN_NUM;
    pin_mask = 1U << pin;

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_DIR, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_INTTYPE, pin_mask, EDGE_TYPE);
    gpio_set_reg_bit_val(group, REG_GPIO_INTPOLARITY, pin_mask, gpio_int_config.int_level);
    gpio_set_reg_bit_val(group, REG_GPIO_INTCLR, pin_mask, 1);

    gpio_set_reg_bit_val(group, REG_GPIO_INTDEBOUNCE, pin_mask, 1);
    udelay(10);

    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 1);
    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);
    ret = gpio_set_reuse_by_pinfunc(group * GPIO_GROUP_MAX_PIN_NUM + pin, FUNC_GPIO);

    return ret;
}

static s32 gpio_cmd_get_int_event(u32 cmd, unsigned long arg)
{
    s32 ret;
    gpio_int_s gpio_int;

    if (copy_from_user(&gpio_int, (void *)arg, sizeof(gpio_int_s))) {
        LOG(LOG_ERROR, "CMD 0x%x copy_from_user errs", cmd);
        return -EFAULT;
    }

    ret = gpio_get_int_event(&gpio_int);
    if (ret) {
        LOG(LOG_ERROR, "gpio_get_int_event errs(%d)", ret);
        return ret;
    }

    if (copy_to_user((void *)arg, &gpio_int, sizeof(gpio_int_s))) {
        ret = -EFAULT;
    }

    return ret;
}

static s32 gpio_cmd_io_drive_config(u32 cmd, unsigned long arg)
{
    s32 ret;
    io_drive_config_info_s io_drive_config_info;

    if (copy_from_user(&io_drive_config_info, (void *)arg, sizeof(io_drive_config_info_s))) {
        LOG(LOG_ERROR, "CMD 0x%x copy_from_user errs", cmd);
        return -EFAULT;
    }

    switch (cmd) {
        case GPIO_SET_IO_DRIVE_CONFIG:
            ret = gpio_set_drive_config(&io_drive_config_info);
            break;

        case GPIO_GET_IO_DRIVE_CONFIG:
            ret = gpio_get_drive_config(&io_drive_config_info);
            if (ret) {
                LOG(LOG_ERROR, "gpio_get_drive_config error(%d)", ret);
                return ret;
            }

            if (copy_to_user((void *)arg, &io_drive_config_info, sizeof(io_drive_config_info))) {
                LOG(LOG_ERROR, "GPIO_GET_IO_DRIVE_CONFIG copy_to_user errs");
                return -EFAULT;
            }
            break;

        default:
            return -ENOTTY;
    }

    return ret;
}

static long gpio_ioctl(struct file *files, u32 cmd, unsigned long arg)
{
    s32 ret;

    switch (cmd) {
        case GPIO_CMD_SET_REUSE:
        case GPIO_CMD_GET_REUSE:
            ret = gpio_cmd_reuse(cmd, arg);
            break;

        case GPIO_CMD_SETDIR:
        case GPIO_CMD_GETDIR:
            ret = gpio_cmd_direction(cmd, arg);
            break;

        case GPIO_CMD_GETLEVEL:
        case GPIO_CMD_OUTPUT_GETLEVEL:
        case GPIO_CMD_SETLEVEL:
            ret = gpio_cmd_level(cmd, arg);
            break;

        case GPIO_CMD_INT_CONFIG:
            ret = gpio_cmd_int_config(cmd, arg);
            break;

        case GPIO_CMD_GET_INT_EVENT:
            ret = gpio_cmd_get_int_event(cmd, arg);
            break;

        case GPIO_GET_IO_DRIVE_CONFIG:
        case GPIO_SET_IO_DRIVE_CONFIG:
            ret = gpio_cmd_io_drive_config(cmd, arg);
            break;

        default:
            ret = -ENOTTY;
    }

    return ret;
}

s32 gpio_config(u32 pin)
{
    u32 group = 0;
    u32 pin_no = 0;
    s32 ret;

    ret = gpio_get_group_pin_num(pin, &group, &pin_no, NULL);
    if (ret) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    ret = gpio_set_reuse_status(group, pin_no);

    return ret;
}
EXPORT_SYMBOL(gpio_config);

s32 gpio_write_regs(u32 pin, u8 val_arg)
{
    u32 group = 0;
    u32 pin_no = 0;
    u32 pin_mask = 0;
    s32 ret;
    unsigned long flag;

    ret = gpio_get_group_pin_num(pin, &group, &pin_no, &pin_mask);
    if (ret) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    ret = gpio_set_reuse_by_pinfunc(group * GPIO_GROUP_MAX_PIN_NUM + pin_no, FUNC_GPIO);
    if (ret) {
        LOG(LOG_ERROR, "gpio set reuse error %d", ret);
        return ret;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    val_arg = val_arg & 0x1;
    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_OUTPUT, pin_mask, val_arg);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}
EXPORT_SYMBOL(gpio_write_regs);

s32 gpio_write(u32 pin, u8 val_arg)
{
    u32 group = 0;
    s32 ret;
    u32 pin_num = 0;
    u32 pin_mask = 0;
    unsigned long flag;

    if (gpio_get_group_pin_num(pin, &group, &pin_num, &pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    ret = gpio_set_reuse_by_pinfunc(group * GPIO_GROUP_MAX_PIN_NUM + pin_num, FUNC_GPIO);
    if (ret) {
        LOG(LOG_ERROR, "gpio set reuse error %d", ret);
        return ret;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    val_arg = val_arg & 0x1;
    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_DIR, pin_mask, GPIO_DIR_OUTPUT);
    gpio_set_reg_bit_val(group, REG_GPIO_OUTPUT, pin_mask, val_arg);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}
EXPORT_SYMBOL(gpio_write);

s32 gpio_write_short(u32 pin, u8 val_arg)
{
    u32 group = 0;
    u32 pin_mask = 0;
    unsigned long flag;

    if (gpio_get_group_pin_num(pin, &group, NULL, &pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    val_arg = val_arg & 0x1;
    gpio_set_reg_bit_val(group, REG_GPIO_OUTPUT, pin_mask, val_arg);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}
EXPORT_SYMBOL(gpio_write_short);

s32 gpio_read(u32 pin, u8 *val_arg)
{
    u32 group = 0;
    s32 ret;
    u32 pin_no = 0;
    u32 pin_mask = 0;
    unsigned long flag;

    if (!val_arg) {
        LOG(LOG_ERROR, "Error: val_arg is NULL");
        return -EINVAL;
    }

    if (gpio_get_group_pin_num(pin, &group, &pin_no, &pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    ret = gpio_set_reuse_by_pinfunc(group * GPIO_GROUP_MAX_PIN_NUM + pin_no, FUNC_GPIO);
    if (ret) {
        LOG(LOG_ERROR, "gpio set reuse error %d", ret);
        return ret;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[group], flag);

    gpio_set_reg_bit_val(group, REG_GPIO_INTEN, pin_mask, 0);
    gpio_set_reg_bit_val(group, REG_GPIO_DIR, pin_mask, GPIO_DIR_INPUT);
    *val_arg = gpio_get_reg_bit_val(group, REG_GPIO_INPUT, pin_mask);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[group], flag);

    return 0;
}
EXPORT_SYMBOL(gpio_read);

s32 gpio_read_short(u32 pin, u8 *val_arg)
{
    u32 group;
    u32 pin_mask;

    if (!val_arg) {
        LOG(LOG_ERROR, "Error: val_arg is NULL");
        return -EINVAL;
    }

    if (gpio_get_group_pin_num(pin, &group, NULL, &pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", pin);
        return -EINVAL;
    }

    *val_arg = gpio_get_reg_bit_val(group, REG_GPIO_INPUT, pin_mask);

    return 0;
}
EXPORT_SYMBOL(gpio_read_short);

static const struct file_operations g_gpio_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = gpio_ioctl,
    .open = gpio_open,
    .release = gpio_release,
};

static void gpio_free_irq(void)
{
    u32 i;

    for (i = 0; i < GPIO_MAX_GROUP_NUM; i++) {
        free_irq(g_gpio_pri_mgr.virq[i], &g_gpio_pri_mgr.irq_flag[i]);
        g_gpio_pri_mgr.virq[i] = 0;
    }

    return;
}

static s32 gpio_init_irq(struct device_node *node)
{
    s32 ret;
    u32 i;

    for (i = 0; i < GPIO_MAX_GROUP_NUM; i++) {
        g_gpio_pri_mgr.irq_flag[i] = i;
        ret = sprintf_s(g_gpio_pri_mgr.gpio_irq_name[i], MAX_GPIO_IRQNAME_LEN, "gpio_irq%u", i);
        if (ret == -1) {
            LOG(LOG_ERROR, "init gpio name error(%d)", ret);
            goto ERR_EXIT;
        }

        g_gpio_pri_mgr.virq[i] = irq_of_parse_and_map(node, i);
        if (!g_gpio_pri_mgr.virq[i]) {
            LOG(LOG_ERROR, "irq_of_parse_and_map(%u) error", i);
            goto ERR_EXIT;
        }

        ret = request_irq(g_gpio_pri_mgr.virq[i], irq_handler, 0, g_gpio_pri_mgr.gpio_irq_name[i],
                          &g_gpio_pri_mgr.irq_flag[i]);
        if (ret) {
            LOG(LOG_ERROR, "can't get assigned gpio_irq%u, Return code is %d", i, ret);
            goto ERR_EXIT;
        }
    }

    return ret;

ERR_EXIT:
    while (i) {
        i--;
        free_irq(g_gpio_pri_mgr.virq[i], &g_gpio_pri_mgr.irq_flag[i]);
        g_gpio_pri_mgr.virq[i] = 0;
    }

    return ret;
}

static void gpio_int_mgr_init(void)
{
    s32 id;
    u32 group;
    ext_int_s *ext_info = &g_gpio_int_mgr.ext_int_info[0];

    for (id = 0; id < TOTAL_INT_NUM; id++) {
        group = ext_info[id].gpio_pin / GPIO_GROUP_MAX_PIN_NUM;
        g_gpio_int_mgr.ext_int_pin_mask[group] |= (1U << (ext_info[id].gpio_pin % GPIO_GROUP_MAX_PIN_NUM));
    }

    g_gpio_int_mgr.init_flag = 1;

    /* Semaphore for initializing the GPIO interrupt pin */
    for (id = 0; id < TOTAL_INT_NUM; id++) {
        init_mutex(&g_gpio_int_mgr.gpio_int_private[id].access_sem);
        init_waitqueue_head(&g_gpio_int_mgr.gpio_int_private[id].wait_queue);
    }

    return;
}

static void gpio_pri_mgr_exit(void)
{
    comm_cleanup_dev(&g_gpio_pri_mgr.gpio_dev);

    if (g_gpio_pri_mgr.gpio_base) {
        iounmap((void *)g_gpio_pri_mgr.gpio_base);
        g_gpio_pri_mgr.gpio_base = NULL;
    }

    return;
}

static s32 gpio_pri_mgr_init(struct device_node *node)
{
    s32 ret;
    u32 id;

    g_gpio_pri_mgr.gpio_base = of_iomap(node, 0);
    if (!g_gpio_pri_mgr.gpio_base) {
        LOG(LOG_ERROR, "gpio regs remap failed");
        return -EIO;
    }

    if (ret) {
    ret = comm_init_dev(&g_gpio_pri_mgr.gpio_dev, (struct file_operations *)&g_gpio_fops, GPIO_DEV_NAME_KER);
        iounmap(g_gpio_pri_mgr.gpio_base);
        g_gpio_pri_mgr.gpio_base = NULL;
        LOG(LOG_ERROR, "gpio dev init failed");
        return ret;
    }

    for (id = 0; id < GPIO_MAX_GROUP_NUM; id++) {
        spin_lock_init(&g_gpio_pri_mgr.gpio_lock[id]);
    }

    return 0;
}

static int hi309x_gpio_direction_input(struct gpio_chip *gc, unsigned int offset)
{
    u32 input_group_no;
    u32 input_pin_no;
    u32 input_pin_mask;
    unsigned long flag;
    s32 ret;
    (void)gc;

    if (gpio_get_group_pin_num(offset, &input_group_no, &input_pin_no, &input_pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", offset);
        return -EINVAL;
    }

    ret = gpio_set_reuse_by_pinfunc(input_group_no * GPIO_GROUP_MAX_PIN_NUM + input_pin_no, FUNC_GPIO);
    if (ret) {
        LOG(LOG_ERROR, "gpio set reuse error %d", ret);
        return ret;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[input_group_no], flag);

    gpio_set_reg_bit_val(input_group_no, REG_GPIO_INTEN, input_pin_mask, 0);
    gpio_set_reg_bit_val(input_group_no, REG_GPIO_DIR, input_pin_mask, GPIO_DIR_INPUT);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[input_group_no], flag);

    return 0;
}

static int hi309x_gpio_direction_output(struct gpio_chip *gc, unsigned int offset, int value)
{
    u32 output_group_no;
    u32 output_pin_no;
    u32 output_pin_mask;
    u32 reg_val;
    unsigned long flag;
    s32 ret;
    (void)gc;

    if (gpio_get_group_pin_num(offset, &output_group_no, &output_pin_no, &output_pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", offset);
        return -EINVAL;
    }

    ret = gpio_set_reuse_by_pinfunc(output_group_no * GPIO_GROUP_MAX_PIN_NUM + output_pin_no, FUNC_GPIO);
    if (ret) {
        LOG(LOG_ERROR, "gpio set reuse error %d", ret);
        return ret;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[output_group_no], flag);

    reg_val = value ? GPIO_OUT_HIGH : GPIO_OUT_LOW;
    gpio_set_reg_bit_val(output_group_no, REG_GPIO_INTEN, output_pin_mask, 0);
    gpio_set_reg_bit_val(output_group_no, REG_GPIO_DIR, output_pin_mask, GPIO_DIR_OUTPUT);
    gpio_set_reg_bit_val(output_group_no, REG_GPIO_OUTPUT, output_pin_mask, reg_val);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[output_group_no], flag);

    return 0;
}

static void hi309x_gpio_set_value(struct gpio_chip *gc, unsigned int offset, int value)
{
    u32 set_group_no;
    u32 set_pin_mask;
    u32 dir;
    u32 reg_val;
    unsigned long flag;
    (void)gc;

    if (gpio_get_group_pin_num(offset, &set_group_no, NULL, &set_pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", offset);
        return;
    }

    dir = gpio_get_reg_bit_val(set_group_no, REG_GPIO_DIR, set_pin_mask);
    if (dir == 0) {
        LOG(LOG_ERROR, "gpio pin num(=%u) direction is in", offset);
        return;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[set_group_no], flag);

    reg_val = value ? GPIO_OUT_HIGH : GPIO_OUT_LOW;
    gpio_set_reg_bit_val(set_group_no, REG_GPIO_OUTPUT, set_pin_mask, reg_val);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[set_group_no], flag);
}

static int hi309x_gpio_get_value(struct gpio_chip *gc, unsigned int offset)
{
    u32 get_group_no;
    u32 get_pin_mask;
    u32 reg_val;
    u32 dir;
    unsigned long flag;
    (void)gc;

    if (gpio_get_group_pin_num(offset, &get_group_no, NULL, &get_pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", offset);
        return -EINVAL;
    }

    dir = gpio_get_reg_bit_val(get_group_no, REG_GPIO_DIR, get_pin_mask);
    if (dir == 1) {
        LOG(LOG_ERROR, "gpio pin num(=%u) direction is out", offset);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[get_group_no], flag);

    reg_val = gpio_get_reg_bit_val(get_group_no, REG_GPIO_INPUT, get_pin_mask);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[get_group_no], flag);

    return reg_val;
}

static int hi309x_gpio_get_direction(struct gpio_chip *gc, unsigned int offset)
{
    u32 dir_group_no;
    u32 dir_pin_mask;
    u32 dir;
    unsigned long flag;
    (void)gc;

    if (gpio_get_group_pin_num(offset, &dir_group_no, NULL, &dir_pin_mask)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", offset);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_gpio_pri_mgr.gpio_lock[dir_group_no], flag);

    dir = gpio_get_reg_bit_val(dir_group_no, REG_GPIO_DIR, dir_pin_mask);

    spin_unlock_irqrestore(&g_gpio_pri_mgr.gpio_lock[dir_group_no], flag);

    return (~dir);
}

static int hi309x_gpio_to_irq(struct gpio_chip *gc, unsigned int offset)
{
    u32 irq_group_no;
    s32 ret;
    (void)gc;

    if (gpio_get_group_pin_num(offset, &irq_group_no, NULL, NULL)) {
        LOG(LOG_ERROR, "gpio pin num(=%u) out of range", offset);
        return -EINVAL;
    }

    switch (irq_group_no) {
        case GROUP0_INDEX:
            ret = GROUP0_INTID;
            break;
        case GROUP1_INDEX:
            ret = GROUP1_INTID;
            break;
        case GROUP2_INDEX:
            ret = GROUP2_INTID;
            break;
        case GROUP3_INDEX:
            ret = GROUP3_INTID;
            break;
        default:
            ret = -ENOTTY;
    }
    return ret;
}

static void hi309x_gpiochip_add(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *node = dev->of_node;

    g_hi309x_chip.gpio_base = of_iomap(node, 0);

    g_hi309x_chip.gc.base = 0;
    g_hi309x_chip.gc.ngpio = MAX_GPIO_CNT;
    g_hi309x_chip.gc.label = "GPIO_Hi1711";
    g_hi309x_chip.gc.parent = dev;
    g_hi309x_chip.gc.owner = THIS_MODULE;

    g_hi309x_chip.gc.direction_input = hi309x_gpio_direction_input;
    g_hi309x_chip.gc.direction_output = hi309x_gpio_direction_output;
    g_hi309x_chip.gc.set = hi309x_gpio_set_value;
    g_hi309x_chip.gc.get = hi309x_gpio_get_value;
    g_hi309x_chip.gc.get_direction = hi309x_gpio_get_direction;
    g_hi309x_chip.gc.to_irq = hi309x_gpio_to_irq;
    devm_gpiochip_add_data(dev, &g_hi309x_chip.gc, &g_hi309x_chip);
}

static s32 gpio_probe(struct platform_device *pdev)
{
    s32 ret;
    struct device_node *node = pdev->dev.of_node;

    gpio_int_mgr_init();
    ret = gpio_pri_mgr_init(node);
    if (ret) {
        LOG(LOG_ERROR, "gpio int mgr init error(%d)", ret);
        return ret;
    }

    ret = gpio_init_irq(node);
    if (ret) {
        LOG(LOG_ERROR, "gpio init irq failed");
        goto IRQ_EXIT;
    }

    hi309x_gpiochip_add(pdev);

    LOG(LOG_INFO, "gpio init ok!.");
    return 0;

IRQ_EXIT:
    gpio_pri_mgr_exit();
    return ret;
}

static s32 gpio_remove(struct platform_device *pdev)
{
    gpio_free_irq();

    gpio_pri_mgr_exit();

    return 0;
}

static const struct of_device_id g_gpio_dt_ids[] = {
    {.compatible = "hisilicon,hi1711-gpio"},
    {}
};

static struct platform_driver g_gpio_driver = {
    .driver = {
        .name = GPIO_DEV_NAME_KER,
        .of_match_table = g_gpio_dt_ids,
    },

    .probe = gpio_probe,
    .remove = gpio_remove,
};
module_platform_driver(g_gpio_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("gpio driver");