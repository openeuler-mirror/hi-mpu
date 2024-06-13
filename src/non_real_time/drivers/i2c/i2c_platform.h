// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Platform driver header file
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __I2C_PLATFORM_H__
#define __I2C_PLATFORM_H__

#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/securec.h>
#include <linux/ratelimit.h>
#include "log_api.h"

#define i2c_print(format, args...) \
    do {                           \
        printk(format, ##args);    \
    } while (0)

#define i2c_print_limit(format, args...) \
    do {                           \
        printk_ratelimited(format, ##args);    \
    } while (0)

#define i2c_log(format, args...)        \
    do {                                \
        LOG(LOG_ERROR, format, ##args); \
    } while (0)

#endif /* __I2C_PLATFORM_H__ */
