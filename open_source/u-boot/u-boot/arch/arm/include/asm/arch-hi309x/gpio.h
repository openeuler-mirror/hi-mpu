/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#include <asm/gpio.h>
#include "io_config.h"
#include "common.h"

#define GPIO_DIR_INPUT 0
#define GPIO_DIR_OUTPUT 1

#define MAX_FUNC_CNT 8
#define FUNC_GPIO 1
#define REG_TYPE_IOR IO_CONFIG_R
#define REG_TYPE_IOT IO_CONFIG_T

#define MAX_GPIO_CNT 127
#define GPIO_GROUP_MAX_PIN_NUM  32
#define GPIO_GROUP3_MAX_PIN_NUM 31
#define HI309X_GPIO_PER_BANK		32

/* Hi309x GPIO register */
#define GPIO_BASE_ADDR 0x0874A000
#define GPIO0_BASE_ADDR 0x0874A000
#define GPIO1_BASE_ADDR 0x0874B000
#define GPIO2_BASE_ADDR 0x0874C000
#define GPIO3_BASE_ADDR 0x0874D000
#define GPIO4_BASE_ADDR 0x0874E000

#define GPIO_OUTPUT_OFFSET_ADDR       0x0  /* OUTPUT register */
#define GPIO_DIR_OFFSET_ADDR          0x4  /* Direction Register 0 - Input 1 - Output */
#define GPIO_PORTCTRL_OFFSET_ADDR     0x8  /* Reserved 0 - Software Control 1 - Hardware Control */
#define GPIO_INTEN_OFFSET_ADDR        0x30 /* 0: normal GPIO signal; 1: interrupt signal */
#define GPIO_INTMASK_OFFSET_ADDR      0x34 /* Interrupt mask register 0 - not masked, 1 - masked */
#define GPIO_INTTYPE_OFFSET_ADDR      0x38 /* Interrupt trigger type. 0: level trigger, 1: edge trigger */
#define GPIO_INTPOLARITY_OFFSET_ADDR  0x3c /* 0 - Low/Falling Edge Trigger, 1 - High/Rising Edge Trigger */
#define GPIO_INTSTATUS_OFFSET_ADDR    0x40 /* Masked status register */
#define GPIO_INTRAWSTATUS_OFFSET_ADDR 0x44 /* Raw interrupt status register */
#define GPIO_INTDEBOUNCE_OFFSET_ADDR  0x48 /* Deglitch control register */
#define GPIO_INTCLR_OFFSET_ADDR       0x4c /* Interrupt clear0: not clear1: clear */
#define GPIO_INPUT_OFFSET_ADDR        0x50 /* INPUT register */
#define GPIO_SYNC_OFFSET_ADDR         0x60 /* Interrupt sync enable register 0 - not sync to PCLK, 1 - sync to PCLK */

struct gpio_bank {
    uint index;
    void *base;
};

struct hi309x_gpio_plat {
    uint bank_index;
    ulong base; /* address of registers in physical memory */
};

typedef struct {
    uint pin;
    uint s_pin_start; /* pin配置为gpio时，同步变化的gpio脚的起始pin,做保留使用 */
    uint s_pin_end;   /* pin配置为gpio时，同步变化的gpio脚的终止pin */
    uint func_cnt;    /* 该管脚支持配置功能的个数 */
    uint func[MAX_FUNC_CNT];
    uint reg_type[MAX_FUNC_CNT];
    uint reg_off[MAX_FUNC_CNT];
    uint reg_mask[MAX_FUNC_CNT];
    uint reg_val[MAX_FUNC_CNT];
} gpio_func_set;

gpio_func_set g_gpio_func_tbl[MAX_GPIO_CNT] = {
    {0, 0, 0, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x000}, {0x003}, {0x002}},
    {1, 1, 1, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x004}, {0x003}, {0x002}},
    {2, 2, 2, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x008}, {0x003}, {0x002}},
    {3, 3, 3, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x00c}, {0x003}, {0x002}},
    {4, 4, 4, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x010}, {0x003}, {0x002}},
    {5, 5, 7, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x014}, {0x003}, {0x002}},
    {6, 5, 7, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x014}, {0x003}, {0x002}},
    {7, 5, 7, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x014}, {0x003}, {0x002}},
    {8, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {9, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {10, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {11, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {12, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {13, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {14, 8, 14, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x018}, {0x003}, {0x002}},
    {15, 15, 15, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x01C}, {0x007}, {0x002}},
    {16, 16, 16, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x024}, {0x007}, {0x002}},
    {17, 17, 17, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x028}, {0x007}, {0x002}},
    {18, 18, 18, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x02C}, {0x007}, {0x002}},
    {19, 19, 19, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x030}, {0x007}, {0x002}},
    {20, 20, 20, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x034}, {0x007}, {0x002}},
    {21, 21, 21, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x038}, {0x007}, {0x002}},
    {22, 22, 22, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x03c}, {0x007}, {0x002}},
    {23, 23, 23, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x040}, {0x007}, {0x002}},
    {24, 24, 25, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x044}, {0x007}, {0x002}},
    {25, 24, 25, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x044}, {0x007}, {0x002}},
    {26, 26, 27, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x048}, {0x007}, {0x002}},
    {27, 26, 27, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x048}, {0x007}, {0x002}},
    {28, 28, 29, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x04C}, {0x007}, {0x002}},
    {29, 28, 29, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x04c}, {0x007}, {0x002}},
    {30, 30, 31, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x050}, {0x007}, {0x002}},
    {31, 30, 31, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x050}, {0x007}, {0x002}},
    {32, 32, 33, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x054}, {0x007}, {0x002}},
    {33, 32, 33, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x054}, {0x007}, {0x002}},
    {34, 34, 36, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x058}, {0x003}, {0x002}},
    {35, 34, 36, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x058}, {0x003}, {0x002}},
    {36, 34, 36, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x058}, {0x003}, {0x002}},
    {37, 37, 37, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x060}, {0x007}, {0x002}},
    {38, 38, 39, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x064}, {0x007}, {0x002}},
    {39, 38, 39, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x064}, {0x007}, {0x002}},
    {40, 40, 42, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x068}, {0x003}, {0x002}},
    {41, 40, 42, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x068}, {0x003}, {0x002}},
    {42, 40, 42, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x068}, {0x003}, {0x002}},
    {43, 43, 45, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x06c}, {0x003}, {0x002}},
    {44, 43, 45, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x06c}, {0x003}, {0x002}},
    {45, 43, 45, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x06c}, {0x003}, {0x002}},
    {46, 46, 46, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x070}, {0x007}, {0x002}},
    {47, 47, 47, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x074}, {0x007}, {0x002}},
    {48, 48, 48, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x078}, {0x007}, {0x002}},
    {49, 49, 49, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x07c}, {0x007}, {0x002}},
    {50, 50, 50, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x080}, {0x007}, {0x002}},
    {51, 51, 51, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x084}, {0x007}, {0x002}},
    {52, 52, 52, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x088}, {0x007}, {0x002}},
    {53, 53, 53, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x08c}, {0x007}, {0x002}},
    {54, 54, 54, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x090}, {0x007}, {0x002}},
    {55, 55, 55, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x094}, {0x007}, {0x002}},
    {56, 56, 56, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x098}, {0x003}, {0x002}},
    {57, 57, 58, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x09c}, {0x003}, {0x002}},
    {58, 57, 58, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x09c}, {0x003}, {0x002}},
    {59, 59, 63, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x0a0}, {0x003}, {0x002}},
    {60, 59, 63, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x0a0}, {0x003}, {0x002}},
    {61, 59, 63, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x0a0}, {0x003}, {0x002}},
    {62, 59, 63, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x0a0}, {0x003}, {0x002}},
    {63, 59, 63, 1, {FUNC_GPIO}, {REG_TYPE_IOR}, {0x0a0}, {0x003}, {0x002}},
    {64, 64, 64, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x000}, {0x003}, {0x002}},
    {65, 65, 65, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x004}, {0x003}, {0x002}},
    {66, 66, 67, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x008}, {0x003}, {0x002}},
    {67, 66, 67, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x008}, {0x003}, {0x002}},
    {68, 68, 69, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x00c}, {0x003}, {0x002}},
    {69, 68, 69, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x00c}, {0x003}, {0x002}},
    {70, 70, 70, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x010}, {0x003}, {0x002}},
    {71, 71, 71, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x014}, {0x003}, {0x002}},
    {72, 72, 72, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x01c}, {0x003}, {0x002}},
    {73, 73, 73, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x020}, {0x003}, {0x002}},
    {74, 74, 75, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x024}, {0x003}, {0x002}},
    {75, 74, 75, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x024}, {0x003}, {0x002}},
    {76, 76, 76, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x028}, {0x003}, {0x002}},
    {77, 77, 77, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x02c}, {0x003}, {0x002}},
    {78, 78, 78, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x030}, {0x003}, {0x002}},
    {79, 79, 79, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x034}, {0x007}, {0x002}},
    {80, 80, 80, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x038}, {0x007}, {0x002}},
    {81, 81, 81, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x03C}, {0x007}, {0x002}},
    {82, 82, 82, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x040}, {0x007}, {0x002}},
    {83, 83, 83, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x044}, {0x007}, {0x002}},
    {84, 84, 84, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x048}, {0x007}, {0x002}},
    {85, 85, 85, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x04C}, {0x007}, {0x002}},
    {86, 86, 86, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x050}, {0x007}, {0x002}},
    {87, 87, 87, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x054}, {0x007}, {0x002}},
    {88, 88, 88, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x058}, {0x007}, {0x002}},
    {89, 89, 89, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x05C}, {0x007}, {0x002}},
    {90, 90, 90, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x060}, {0x007}, {0x002}},
    {91, 91, 92, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x064}, {0x007}, {0x002}},
    {92, 91, 92, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x064}, {0x007}, {0x002}},
    {93, 93, 94, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x068}, {0x007}, {0x002}},
    {94, 93, 94, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x068}, {0x007}, {0x002}},
    {95, 95, 95, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x06C}, {0x003}, {0x002}},
    {96, 96, 96, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x070}, {0x003}, {0x002}},
    {97, 97, 97, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x074}, {0x003}, {0x002}},
    {98, 98, 98, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x078}, {0x003}, {0x002}},
    {99, 99, 99, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x07C}, {0x003}, {0x002}},
    {100, 100, 100, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x080}, {0x003}, {0x002}},
    {101, 101, 102, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x084}, {0x003}, {0x002}},
    {102, 101, 102, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x084}, {0x003}, {0x002}},
    {103, 103, 103, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x088}, {0x007}, {0x002}},
    {104, 104, 104, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x08C}, {0x007}, {0x002}},
    {105, 105, 105, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x090}, {0x007}, {0x002}},
    {106, 106, 106, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x094}, {0x007}, {0x002}},
    {107, 107, 107, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x098}, {0x007}, {0x002}},
    {108, 108, 108, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x09C}, {0x007}, {0x002}},
    {109, 109, 109, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0A0}, {0x003}, {0x002}},
    {110, 110, 110, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0A4}, {0x007}, {0x002}},
    {111, 111, 111, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0A8}, {0x007}, {0x002}},
    {112, 112, 112, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0AC}, {0x007}, {0x002}},
    {113, 113, 114, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0B0}, {0x007}, {0x002}},
    {114, 113, 114, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0B0}, {0x007}, {0x002}},
    {115, 115, 116, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0b4}, {0x007}, {0x002}},
    {116, 115, 116, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0b4}, {0x007}, {0x002}},
    {117, 117, 118, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0b8}, {0x007}, {0x002}},
    {118, 117, 118, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0b8}, {0x007}, {0x002}},
    {119, 119, 120, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0bc}, {0x007}, {0x002}},
    {120, 119, 120, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0bc}, {0x007}, {0x002}},
    {121, 121, 121, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0c0}, {0x007}, {0x002}},
    {122, 122, 122, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0c4}, {0x007}, {0x002}},
    {123, 123, 124, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0c8}, {0x007}, {0x002}},
    {124, 123, 124, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0c8}, {0x007}, {0x002}},
    {125, 125, 126, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0cc}, {0x007}, {0x002}},
    {126, 125, 126, 1, {FUNC_GPIO}, {REG_TYPE_IOT}, {0x0cc}, {0x007}, {0x002}},
};