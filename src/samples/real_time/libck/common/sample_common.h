/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#ifndef __SAMPLE_COMMON_H__
#define __SAMPLE_COMMON_H__

#include "sre_hwi.h"
#include "os_cpu_armv8_external.h"

void sample_prepare(void);
int app_main(void);

#define SAMPLE_CORE0    0
#define SAMPLE_CORE1    1
#define SAMPLE_CORE2    2
#define SAMPLE_CORE3    3

#endif /* __SAMPLE_COMMON_H__ */