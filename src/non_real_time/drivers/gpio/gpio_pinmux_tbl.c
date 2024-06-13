// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm_knl_api.h"
#include "io_config.h"
#include "gpio_ker.h"

/*
 * Gpio iomux table.
 * Currently, the operation set contains only the GPIO configuration function.
 * You can add the supported configuration function set.
 */
gpio_func_set_s g_io_func_set[MAX_GPIO_CNT] = {
    {
        .pin      = 0, /* gpio 0 */
        .s_pin_start = 0, /* gpio 0 */
        .s_pin_end  = 0, /* gpio 0 */
        .func_cnt   = 1,
        .func       = {FUNC_GPIO},
        .reg_type   = {REG_TYPE_IOR},
        .reg_off    = {0x000},
        .reg_mask   = {0x003},
        .reg_val    = {0x002}
    },
    {
        .pin      = 1, /* gpio 1 */
        .s_pin_start = 1, /* gpio 1 */
        .s_pin_end = 1, /* gpio 1 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x004},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 2, /* gpio 2 */
        .s_pin_start = 2, /* gpio 2 */
        .s_pin_end = 2, /* gpio 2 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x008},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 3, /* gpio 3 */
        .s_pin_start = 3, /* gpio 3 */
        .s_pin_end = 3, /* gpio 3 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x00c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 4, /* gpio 4 */
        .s_pin_start = 4, /* gpio 4 */
        .s_pin_end = 4, /* gpio 4 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x010},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 5, /* gpio 5 */
        .s_pin_start = 5, /* gpio 5 */
        .s_pin_end = 7, /* gpio 7 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x014},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 6, /* gpio 6 */
        .s_pin_start = 5, /* gpio 5 */
        .s_pin_end = 7, /* gpio 7 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x014},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 7, /* gpio 7 */
        .s_pin_start = 5, /* gpio 5 */
        .s_pin_end = 7, /* gpio 7 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x014},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 8, /* gpio 8 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 9, /* gpio 9 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 10, /* gpio 10 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 11, /* gpio 11 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 12, /* gpio 12 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 13, /* gpio 13 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 14, /* gpio 14 */
        .s_pin_start = 8, /* gpio 8 */
        .s_pin_end = 14, /* gpio 14 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x018},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 15, /* gpio 15 */
        .s_pin_start = 15, /* gpio 15 */
        .s_pin_end = 15, /* gpio 15 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x01C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 16, /* gpio 16 */
        .s_pin_start = 16, /* gpio 16 */
        .s_pin_end = 16, /* gpio 16 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x024},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 17, /* gpio 17 */
        .s_pin_start = 17, /* gpio 17 */
        .s_pin_end = 17, /* gpio 17 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x028},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 18, /* gpio 18 */
        .s_pin_start = 18, /* gpio 18 */
        .s_pin_end = 18, /* gpio 18 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x02C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 19, /* gpio 19 */
        .s_pin_start = 19, /* gpio 19 */
        .s_pin_end = 19, /* gpio 19 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x030},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 20, /* gpio 20 */
        .s_pin_start = 20, /* gpio 20 */
        .s_pin_end = 20, /* gpio 20 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x034},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 21, /* gpio 21 */
        .s_pin_start = 21, /* gpio 21 */
        .s_pin_end = 21, /* gpio 21 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x038},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 22, /* gpio 22 */
        .s_pin_start = 22, /* gpio 22 */
        .s_pin_end = 22, /* gpio 22 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x03c},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 23, /* gpio 23 */
        .s_pin_start = 23, /* gpio 23 */
        .s_pin_end = 23, /* gpio 23 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x040},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 24, /* gpio 24 */
        .s_pin_start = 24, /* gpio 24 */
        .s_pin_end = 25, /* gpio 25 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x044},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 25, /* gpio 25 */
        .s_pin_start = 24, /* gpio 24 */
        .s_pin_end = 25, /* gpio 25 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x044},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 26, /* gpio 26 */
        .s_pin_start = 26, /* gpio 26 */
        .s_pin_end = 27, /* gpio 27 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x048},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 27, /* gpio 27 */
        .s_pin_start = 26, /* gpio 26 */
        .s_pin_end = 27, /* gpio 27 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x048},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 28, /* gpio 28 */
        .s_pin_start = 28, /* gpio 28 */
        .s_pin_end = 29, /* gpio 29 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x04C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 29, /* gpio 29 */
        .s_pin_start = 28, /* gpio 28 */
        .s_pin_end = 29, /* gpio 29 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x04c},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 30, /* gpio 30 */
        .s_pin_start = 30, /* gpio 30 */
        .s_pin_end = 31, /* gpio 31 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x050},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 31, /* gpio 31 */
        .s_pin_start = 30, /* gpio 30 */
        .s_pin_end = 31, /* gpio 31 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x050},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 32, /* gpio 32 */
        .s_pin_start = 32, /* gpio 32 */
        .s_pin_end = 33, /* gpio 33 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x054},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 33, /* gpio 33 */
        .s_pin_start = 32, /* gpio 32 */
        .s_pin_end = 33, /* gpio 33 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x054},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 34, /* gpio 34 */
        .s_pin_start = 34, /* gpio 34 */
        .s_pin_end = 36, /* gpio 36 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x058},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 35, /* gpio 35 */
        .s_pin_start = 34, /* gpio 34 */
        .s_pin_end = 36, /* gpio 36 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x058},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 36, /* gpio 36 */
        .s_pin_start = 34, /* gpio 34 */
        .s_pin_end = 36, /* gpio 36 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x058},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 37, /* gpio 37 */
        .s_pin_start = 37, /* gpio 37 */
        .s_pin_end = 37, /* gpio 37 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x060},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 38, /* gpio 38 */
        .s_pin_start = 38, /* gpio 38 */
        .s_pin_end = 39, /* gpio 39 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x064},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 39, /* gpio 39 */
        .s_pin_start = 38, /* gpio 38 */
        .s_pin_end = 39, /* gpio 39 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x064},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 40, /* gpio 40 */
        .s_pin_start = 40, /* gpio 40 */
        .s_pin_end = 42, /* gpio 42 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x068},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 41, /* gpio 41 */
        .s_pin_start = 40, /* gpio 40 */
        .s_pin_end = 42, /* gpio 42 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x068},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 42, /* gpio 42 */
        .s_pin_start = 40, /* gpio 40 */
        .s_pin_end = 42, /* gpio 42 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x068},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 43, /* gpio 43 */
        .s_pin_start = 43, /* gpio 43 */
        .s_pin_end = 45, /* gpio 45 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x06c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 44, /* gpio 44 */
        .s_pin_start = 43, /* gpio 43 */
        .s_pin_end = 45, /* gpio 45 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x06c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 45, /* gpio 45 */
        .s_pin_start = 43, /* gpio 43 */
        .s_pin_end = 45, /* gpio 45 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x06c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 46, /* gpio 46 */
        .s_pin_start = 46, /* gpio 46 */
        .s_pin_end = 46, /* gpio 46 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x070},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 47, /* gpio 47 */
        .s_pin_start = 47, /* gpio 47 */
        .s_pin_end = 47, /* gpio 47 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x074},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 48, /* gpio 48 */
        .s_pin_start = 48, /* gpio 48 */
        .s_pin_end = 48, /* gpio 48 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x078},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 49, /* gpio 49 */
        .s_pin_start = 49, /* gpio 49 */
        .s_pin_end = 49, /* gpio 49 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x07c},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 50, /* gpio 50 */
        .s_pin_start = 50, /* gpio 50 */
        .s_pin_end = 50, /* gpio 50 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x080},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 51, /* gpio 51 */
        .s_pin_start = 51, /* gpio 51 */
        .s_pin_end = 51, /* gpio 51 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x084},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 52, /* gpio 52 */
        .s_pin_start = 52, /* gpio 52 */
        .s_pin_end = 52, /* gpio 52 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x088},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 53, /* gpio 53 */
        .s_pin_start = 53, /* gpio 53 */
        .s_pin_end = 53, /* gpio 53 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x08c},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 54, /* gpio 54 */
        .s_pin_start = 54, /* gpio 54 */
        .s_pin_end = 54, /* gpio 54 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x090},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 55, /* gpio 55 */
        .s_pin_start = 55, /* gpio 55 */
        .s_pin_end = 55, /* gpio 55 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x094},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 56, /* gpio 56 */
        .s_pin_start = 56, /* gpio 56 */
        .s_pin_end = 56, /* gpio 56 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x098},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 57, /* gpio 57 */
        .s_pin_start = 57, /* gpio 57 */
        .s_pin_end = 58, /* gpio 58 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x09c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 58, /* gpio 58 */
        .s_pin_start = 57, /* gpio 57 */
        .s_pin_end = 58, /* gpio 58 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x09c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 59, /* gpio 59 */
        .s_pin_start = 59, /* gpio 59 */
        .s_pin_end = 63, /* gpio 63 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x0a0},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 60, /* gpio 60 */
        .s_pin_start = 59, /* gpio 59 */
        .s_pin_end = 63, /* gpio 63 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x0a0},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 61, /* gpio 61 */
        .s_pin_start = 59, /* gpio 59 */
        .s_pin_end = 63, /* gpio 63 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x0a0},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 62, /* gpio 62 */
        .s_pin_start = 59, /* gpio 59 */
        .s_pin_end = 63, /* gpio 63 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x0a0},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 63, /* gpio 63 */
        .s_pin_start = 59, /* gpio 59 */
        .s_pin_end = 63, /* gpio 63 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOR},
        .reg_off  = {0x0a0},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 64, /* gpio 64 */
        .s_pin_start = 64, /* gpio 64 */
        .s_pin_end = 64, /* gpio 64 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x000},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 65, /* gpio 65 */
        .s_pin_start = 65, /* gpio 65 */
        .s_pin_end = 65, /* gpio 65 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x004},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 66, /* gpio 66 */
        .s_pin_start = 66, /* gpio 66 */
        .s_pin_end = 67, /* gpio 67 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x008},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 67, /* gpio 67 */
        .s_pin_start = 66, /* gpio 66 */
        .s_pin_end = 67, /* gpio 67 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x008},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 68, /* gpio 68 */
        .s_pin_start = 68, /* gpio 68 */
        .s_pin_end = 69, /* gpio 69 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x00c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 69, /* gpio 69 */
        .s_pin_start = 68, /* gpio 68 */
        .s_pin_end = 69, /* gpio 69 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x00c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 70, /* gpio 70 */
        .s_pin_start = 70, /* gpio 70 */
        .s_pin_end = 70, /* gpio 70 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x010},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 71, /* gpio 71 */
        .s_pin_start = 71, /* gpio 71 */
        .s_pin_end = 71, /* gpio 71 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x014},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 72, /* gpio 72 */
        .s_pin_start = 72, /* gpio 72 */
        .s_pin_end = 72, /* gpio 72 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x01c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 73, /* gpio 73 */
        .s_pin_start = 73, /* gpio 73 */
        .s_pin_end = 73, /* gpio 73 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x020},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 74, /* gpio 74 */
        .s_pin_start = 74, /* gpio 74 */
        .s_pin_end = 75, /* gpio 75 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x024},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 75, /* gpio 75 */
        .s_pin_start = 74, /* gpio 74 */
        .s_pin_end = 75, /* gpio 75 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x024},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 76, /* gpio 76 */
        .s_pin_start = 76, /* gpio 76 */
        .s_pin_end = 76, /* gpio 76 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x028},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 77, /* gpio 77 */
        .s_pin_start = 77, /* gpio 77 */
        .s_pin_end = 77, /* gpio 77 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x02c},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 78, /* gpio 78 */
        .s_pin_start = 78, /* gpio 78 */
        .s_pin_end = 78, /* gpio 78 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x030},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 79, /* gpio 79 */
        .s_pin_start = 79, /* gpio 79 */
        .s_pin_end = 79, /* gpio 79 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x034},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 80, /* gpio 80 */
        .s_pin_start = 80, /* gpio 80 */
        .s_pin_end = 80, /* gpio 80 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x038},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 81, /* gpio 81 */
        .s_pin_start = 81, /* gpio 81 */
        .s_pin_end = 81, /* gpio 81 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x03C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 82, /* gpio 82 */
        .s_pin_start = 82, /* gpio 82 */
        .s_pin_end = 82, /* gpio 82 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x040},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 83, /* gpio 83 */
        .s_pin_start = 83, /* gpio 83 */
        .s_pin_end = 83, /* gpio 83 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x044},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 84, /* gpio 84 */
        .s_pin_start = 84, /* gpio 84 */
        .s_pin_end = 84, /* gpio 84 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x048},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 85, /* gpio 85 */
        .s_pin_start = 85, /* gpio 85 */
        .s_pin_end = 85, /* gpio 85 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x04C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 86, /* gpio 86 */
        .s_pin_start = 86, /* gpio 86 */
        .s_pin_end = 86, /* gpio 86 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x050},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 87, /* gpio 87 */
        .s_pin_start = 87, /* gpio 87 */
        .s_pin_end = 87, /* gpio 87 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x054},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 88, /* gpio 88 */
        .s_pin_start = 88, /* gpio 88 */
        .s_pin_end = 88, /* gpio 88 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x058},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 89, /* gpio 89 */
        .s_pin_start = 89, /* gpio 89 */
        .s_pin_end = 89, /* gpio 89 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x05C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 90, /* gpio 90 */
        .s_pin_start = 90, /* gpio 90 */
        .s_pin_end = 90, /* gpio 90 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x060},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 91, /* gpio 91 */
        .s_pin_start = 91, /* gpio 91 */
        .s_pin_end = 92, /* gpio 92 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x064},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 92, /* gpio 92 */
        .s_pin_start = 91, /* gpio 91 */
        .s_pin_end = 92, /* gpio 92 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x064},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 93, /* gpio 93 */
        .s_pin_start = 93, /* gpio 93 */
        .s_pin_end = 94, /* gpio 94 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x068},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 94, /* gpio 94 */
        .s_pin_start = 93, /* gpio 93 */
        .s_pin_end = 94, /* gpio 94 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x068},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 95, /* gpio 95 */
        .s_pin_start = 95, /* gpio 95 */
        .s_pin_end = 95, /* gpio 95 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x06C},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 96, /* gpio 96 */
        .s_pin_start = 96, /* gpio 96 */
        .s_pin_end = 96, /* gpio 96 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x070},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 97, /* gpio 97 */
        .s_pin_start = 97, /* gpio 97 */
        .s_pin_end = 97, /* gpio 97 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x074},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 98, /* gpio 98 */
        .s_pin_start = 98, /* gpio 98 */
        .s_pin_end = 98, /* gpio 98 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x078},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 99, /* gpio 99 */
        .s_pin_start = 99, /* gpio 99 */
        .s_pin_end = 99, /* gpio 99 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x07C},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 100, /* gpio 100 */
        .s_pin_start = 100, /* gpio 100 */
        .s_pin_end = 100, /* gpio 100 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x080},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 101, /* gpio 101 */
        .s_pin_start = 101, /* gpio 101 */
        .s_pin_end = 102, /* gpio 102 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x084},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 102, /* gpio 102 */
        .s_pin_start = 101, /* gpio 101 */
        .s_pin_end = 102, /* gpio 102 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x084},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 103, /* gpio 103 */
        .s_pin_start = 103, /* gpio 103 */
        .s_pin_end = 103, /* gpio 103 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x088},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 104, /* gpio 104 */
        .s_pin_start = 104, /* gpio 104 */
        .s_pin_end = 104, /* gpio 104 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x08C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 105, /* gpio 105 */
        .s_pin_start = 105, /* gpio 105 */
        .s_pin_end = 105, /* gpio 105 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x090},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 106, /* gpio 106 */
        .s_pin_start = 106, /* gpio 106 */
        .s_pin_end = 106, /* gpio 106 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x094},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 107, /* gpio 107 */
        .s_pin_start = 107, /* gpio 107 */
        .s_pin_end = 107, /* gpio 107 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x098},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 108, /* gpio 108 */
        .s_pin_start = 108, /* gpio 108 */
        .s_pin_end = 108, /* gpio 108 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x09C},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 109, /* gpio 109 */
        .s_pin_start = 109, /* gpio 109 */
        .s_pin_end = 109, /* gpio 109 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0A0},
        .reg_mask = {0x003},
        .reg_val  = {0x002}
    },
    {
        .pin      = 110, /* gpio 110 */
        .s_pin_start = 110, /* gpio 110 */
        .s_pin_end = 110, /* gpio 110 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0A4},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 111, /* gpio 111 */
        .s_pin_start = 111, /* gpio 111 */
        .s_pin_end = 111, /* gpio 111 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0A8},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 112, /* gpio 112 */
        .s_pin_start = 112, /* gpio 112 */
        .s_pin_end = 112, /* gpio 112 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0AC},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 113, /* gpio 113 */
        .s_pin_start = 113, /* gpio 113 */
        .s_pin_end = 114, /* gpio 114 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0B0},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 114, /* gpio 114 */
        .s_pin_start = 113, /* gpio 113 */
        .s_pin_end = 114, /* gpio 114 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0B0},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 115, /* gpio 115 */
        .s_pin_start = 115, /* gpio 115 */
        .s_pin_end = 116, /* gpio 116 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0b4},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 116, /* gpio 116 */
        .s_pin_start = 115, /* gpio 115 */
        .s_pin_end = 116, /* gpio 116 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0b4},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 117, /* gpio 117 */
        .s_pin_start = 117, /* gpio 117 */
        .s_pin_end = 118, /* gpio 118 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0b8},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 118, /* gpio 118 */
        .s_pin_start = 117, /* gpio 117 */
        .s_pin_end = 118, /* gpio 118 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0b8},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 119, /* gpio 119 */
        .s_pin_start = 119, /* gpio 119 */
        .s_pin_end = 120, /* gpio 120 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0bc},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 120, /* gpio 120 */
        .s_pin_start = 119, /* gpio 119 */
        .s_pin_end = 120, /* gpio 120 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0bc},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 121, /* gpio 121 */
        .s_pin_start = 121, /* gpio 121 */
        .s_pin_end = 121, /* gpio 121 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0c0},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 122, /* gpio 122 */
        .s_pin_start = 122, /* gpio 122 */
        .s_pin_end = 122, /* gpio 122 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0c4},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 123, /* gpio 123 */
        .s_pin_start = 123, /* gpio 123 */
        .s_pin_end = 124, /* gpio 124 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0c8},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 124, /* gpio 124 */
        .s_pin_start = 123, /* gpio 123 */
        .s_pin_end = 124, /* gpio 124 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0c8},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 125, /* gpio 125 */
        .s_pin_start = 125, /* gpio 125 */
        .s_pin_end = 126, /* gpio 126 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0cc},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    },
    {
        .pin      = 126, /* gpio 126 */
        .s_pin_start = 125, /* gpio 125 */
        .s_pin_end = 126, /* gpio 126 */
        .func_cnt = 1,
        .func     = {FUNC_GPIO},
        .reg_type = {REG_TYPE_IOT},
        .reg_off  = {0x0cc},
        .reg_mask = {0x007},
        .reg_val  = {0x002}
    }
};