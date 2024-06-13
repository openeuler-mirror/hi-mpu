/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 1711 common
 * Author: xxx
 * Create: 2022-11-16
 */
#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <plat/common/platform.h>
#include <platform_def.h>
#include "../hi1711_def.h"
#include "../hi1711_private.h"

unsigned long plat_get_ns_image_entrypoint(void)
{
    return NS_IMAGE_OFFSET;
}

#define IO_SYSCTRL_REG_BASE 0x1100C0000
#define SC_VER              0xFFFC

uint32_t IsAsicBoot()
{
    return 0;
}

unsigned int plat_get_syscnt_freq2(void)
{
    if (IsAsicBoot() == 0x2) {
        INFO("..ESL..\n");
        return SYS_COUNTER_FREQ_ESL;
    } else {
        if (IsAsicBoot() == 0x1) {
            return SYS_COUNTER_FREQ_ASIC;
        }
        INFO("..ASIC..\n");
        return SYS_COUNTER_FREQ;
    }
}
