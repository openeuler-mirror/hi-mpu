// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gpio module header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _GPIO_KER_H_
#define _GPIO_KER_H_
#include "gpio_usr_api.h"

#define GPIO_DEV_NAME_KER "gpio1711"

#define GROUP0_INDEX 0 /* Index of pin group 1 */
#define GROUP1_INDEX 1 /* Index of the second group of pins */
#define GROUP2_INDEX 2 /* Index of the third group of pins */
#define GROUP3_INDEX 3 /* Index of the fourth group of pins */
#define GROUP4_INDEX 4 /* Index of the fifth group of pins */

#define GROUP0_INTID 181
#define GROUP1_INTID 182
#define GROUP2_INTID 183
#define GROUP3_INTID 184
#define GROUP_BASE_INTID GROUP0_INTID

#define MAX_GPIO_IRQNAME_LEN 10

#define GROUP3_INVALID_MASK (0x80000000)
#define GROUP4_INVALID_MASK (0x3FFFFFFF)

/* GPIO register address ,GPIO 32bit register, each bit represents a pin. */
#define REG_GPIO_OUTPUT       0x0
#define REG_GPIO_DIR          0x4
#define REG_GPIO_PORTCTRL     0x8
#define REG_GPIO_INTEN        0x30
#define REG_GPIO_INTMASK      0x34
#define REG_GPIO_INTTYPE      0x38
#define REG_GPIO_INTPOLARITY  0x3c
#define REG_GPIO_INTSTATUS    0x40
#define REG_GPIO_INTRAWSTATUS 0x44
#define REG_GPIO_INTDEBOUNCE  0x48
#define REG_GPIO_INTCLR       0x4c
#define REG_GPIO_INPUT        0x50
#define REG_GPIO_SYNC         0x60

#define MAX_FUNC_CNT  8
#define GPIO_NAME_LEN 20

#define RISING_EDGE 3

#define GPIO_TOTAL_MAX_PIN_NUM 160
#define GPIO_IRQ_TYPE          3

#define GPIO24  24
#define GPIO99  99
#define GPIO100 100
#define GPIO118 118

#define MAX_GPIO_CNT 127
#define FUNC_GPIO    1

#define SPEC_GPIO_BIT30_PIN 158
#define SPEC_GPIO_BIT31_PIN 159

#define GPIO_BIT30 BIT(30)
#define GPIO_BIT0  1U

#define REG_TYPE_IOR IO_CONFIG_R
#define REG_TYPE_IOT IO_CONFIG_T
#define REG_TYPE_LP  (REG_TYPE_IOT + 1)

#define clean_bit(val, mask)                     ((val) & (~(mask)))
#define set_bit(val, mask)                       ((val) | (mask))
#define gpio_clear_reg_bit(group, reg, pin_mask) \
    do {                                         \
        u32 val;                                 \
        val = gpio_read_reg(group, reg);         \
        val = clean_bit(val, pin_mask);          \
        gpio_write_reg(group, reg, val);         \
    } while (0)
#define gpio_set_reg_bit(group, reg, pin_mask) \
    do {                                       \
        u32 val;                               \
        val = gpio_read_reg(group, reg);       \
        val = set_bit(val, pin_mask);          \
        gpio_write_reg(group, reg, val);       \
    } while (0)

#define gpio_set_reg_bit_val(group, reg, pin_mask, bitval) \
    do {                                                   \
        if ((bitval) == 0) {                                 \
            gpio_clear_reg_bit(group, reg, pin_mask);      \
        } else {                                           \
            gpio_set_reg_bit(group, reg, pin_mask);        \
        }                                                  \
    } while (0)
#define gpio_get_reg_bit_val(group, reg, pin_mask) ((gpio_read_reg(group, reg) & (pin_mask)) ? 1 : 0)

#define TOTAL_INT_NUM 21

#define TOTAL_USER_INT_NUM 20

#define GPIO_INT_STATE_IDLE  0 /* No interrupt is generated. */
#define GPIO_INT_STATE_READY 1 /* Interrupt generation */

#define INT_NO_HAPPEN 0
#define INT_HAPPEN    1

#define WAIT_ALL_TIME 0xffffffff

#define EDGE_TYPE 1 /* Edge-triggered */
#define UP_TYPE   1 /* Rising edge or high level trigger */

#define GRADE_BIT   4                  /* Displacement of driving force configuration value */
#define GRADE4_MASK (0x3 << GRADE_BIT)
#define GRADE8_MASK (0x7 << GRADE_BIT)

#define IO_R_GPIO_NUM (64)
#define IO_T_GPIO_NUM (63)

#define IO_MAX_ID (126)

#define BYTE_NUM 4

#define GRADE4_MAX_VAL (0x3)
#define GRADE8_MAX_VAL (0x7)

#define GPIO_DIR_INPUT  0
#define GPIO_DIR_OUTPUT 1
#define GPIO_OUT_LOW    0
#define GPIO_OUT_HIGH   1

typedef struct {
    u32 pin;
    u32 s_pin_start;
    u32 s_pin_end;
    u32 func_cnt;
    u32 func[MAX_FUNC_CNT];
    u32 reg_type[MAX_FUNC_CNT];
    u32 reg_off[MAX_FUNC_CNT];
    u32 reg_mask[MAX_FUNC_CNT];
    u32 reg_val[MAX_FUNC_CNT];
} gpio_func_set_s;

typedef struct {
    /* Mutual exclusion of access to multiple apply processes */
    struct semaphore access_sem;

    /* Synchronization of request processing */
    wait_queue_head_t wait_queue;
} gpio_int_private_s;

typedef struct {
    u32 ext_int_num;
    u32 gpio_pin;
} ext_int_s;

typedef int (*gpio_isr)(void *);
typedef struct gpio_isr_handler_st {
    gpio_isr isr;
    void *data;
} gpio_isr_handler_s;

typedef struct {
    u32 int_event[TOTAL_INT_NUM];
    u32 init_flag;
    u32 ext_int_pin_mask[GPIO_MAX_GROUP_NUM];
    ext_int_s ext_int_info[TOTAL_INT_NUM];
    gpio_isr_handler_s gpio_isr_hanler[GPIO_TOTAL_MAX_PIN_NUM];
    gpio_int_private_s gpio_int_private[TOTAL_INT_NUM];
} gpio_int_mgr_s;

typedef struct {
    void __iomem *gpio_base;
    cdev_st gpio_dev;
    spinlock_t gpio_lock[GPIO_MAX_GROUP_NUM];
    u32 virq[GPIO_MAX_GROUP_NUM];
    s8 gpio_irq_name[GPIO_MAX_GROUP_NUM][MAX_GPIO_IRQNAME_LEN];
    u32 irq_flag[GPIO_MAX_GROUP_NUM];
} gpio_pri_mgr_s;

extern gpio_func_set_s g_io_func_set[MAX_GPIO_CNT];

s32 gpio_isr_register(u32 pin, u8 flag, gpio_isr handler, void *data);
s32 gpio_config(u32 pin);
s32 gpio_write(u32 pin, u8 val);
s32 gpio_read(u32 pin, u8 *val);
s32 gpio_write_regs(u32 pin, u8 val);
s32 gpio_isr_unregister(u32 pin);
s32 gpio_isr_start(u32 pin);
s32 gpio_isr_stop(u32 pin);

#endif
