// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gpio user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __GPIO_USR_API_H__
#define __GPIO_USR_API_H__

#define GPIO_DEV_NAME "/dev/gpio1711"

#define GPIO_MAX_GROUP_NUM      5
#define GPIO_GROUP3             3
#define GPIO_GROUP_MAX_PIN_NUM  32
#define GPIO_GROUP3_MAX_PIN_NUM 31
#define GPIO_GROUP4_MAX_PIN_NUM 2

#define GPIO_CMD_MAGIC           'g'
#define GPIO_CMD_SET_REUSE       _IOW(GPIO_CMD_MAGIC, 0, int)
#define GPIO_CMD_GET_REUSE       _IOR(GPIO_CMD_MAGIC, 1, int)
#define GPIO_CMD_SETDIR          _IOW(GPIO_CMD_MAGIC, 2, int)
#define GPIO_CMD_GETDIR          _IOR(GPIO_CMD_MAGIC, 3, int)
#define GPIO_CMD_SETLEVEL        _IOW(GPIO_CMD_MAGIC, 4, int)
#define GPIO_CMD_GETLEVEL        _IOR(GPIO_CMD_MAGIC, 5, int)
#define GPIO_CMD_INT_CONFIG      _IOW(GPIO_CMD_MAGIC, 6, int)
#define GPIO_CMD_GET_INT_EVENT   _IOR(GPIO_CMD_MAGIC, 7, int)
#define GPIO_CMD_OUTPUT_GETLEVEL _IOR(GPIO_CMD_MAGIC, 8, int)
#define GPIO_GET_IO_DRIVE_CONFIG _IOR(GPIO_CMD_MAGIC, 9, int)
#define GPIO_SET_IO_DRIVE_CONFIG _IOW(GPIO_CMD_MAGIC, 10, int)


typedef struct {
    u32 gpio_value[GPIO_MAX_GROUP_NUM];
    u32 gpio_mask[GPIO_MAX_GROUP_NUM];
} gpio_info_s;

typedef struct {
    u32 gpio_int_num; /* Number of external interrupts */
    u32 int_level;    /* Interrupt polarity 0 */
} gpio_int_set_s;

typedef struct {
    u32 gpio_int_num;
    u32 timeout;
    u32 value;
    u32 rsv;
} gpio_int_s;

/* I/O drive capability configuration information */
typedef struct tag_io_drive_config_info_s {
    u8 id;
    u8 drive_value;
} io_drive_config_info_s;

#endif
