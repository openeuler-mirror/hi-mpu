/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "securec.h"
#include "sre_config.h"
#include "sre_common.h"
#include "bm_common.h"


#define PI 3.14159265
#define ONE_HUNDRED 100

static void sample_math_abs(void)
{
    int abs_t = -1; // -1 is magic number
    SRE_Printf("sample_math_abs: abs(-1) * 100 = %d\n", (unsigned int)(abs(abs_t) * ONE_HUNDRED));
}

static void sample_math_fabs(void)
{
    float fabs_t = -1.5f; // -1.5f is magic number
    SRE_Printf("sample_math_fabs: fabs(-1.5) * 100 = %d\n", (unsigned int)(fabs(fabs_t) * ONE_HUNDRED));
}

static void sample_math_pow(void)
{
    int pow_t = 2; // 2 is magic number
    SRE_Printf("sample_math_pow: pow(2, 2) * 100 = %d\n", (unsigned int)(pow(pow_t, pow_t) * ONE_HUNDRED));
}

static void sample_math_sqrt(void)
{
    int sqrt_t = 25; // 25 is magic number
    SRE_Printf("sample_math_sqrt: sqrt(25) * 100 = %d\n", (unsigned int)(sqrt(sqrt_t) * ONE_HUNDRED));
}

static void sample_math_sin(void)
{
    double sin_t = 45.0; // 45.0 is magic number
    double val = PI / 180; // 180 is magic number
    /* 45.000000 的正弦是 0.707107 度 */
    SRE_Printf("sample_math_sin: sin * 100 = %d\n", (unsigned int)(sin(sin_t * val) * ONE_HUNDRED));
}

static void sample_math_cos(void)
{
    double cos_t = 60.0; // 60.0 is magic number
    double val = PI / 180; // 180 is magic number
    /* 60.000000 的余弦是 0.500000 度 */
    SRE_Printf("sample_math_cos: cos * 100 = %d\n", (unsigned int)(cos(cos_t * val) * ONE_HUNDRED));
}

static void sample_base_strlen(void)
{
    char str[] = "hello";
    SRE_Printf("sample_base_strlen: hello len = %d\n", strlen(str));
}

static void sample_base_sizeof(void)
{
    SRE_Printf("sample_base_sizeof: sizeof(char) = %d\n", sizeof(char));
}

static void sample_base_memory_set(void)
{
    int ret;
    char str[] = "hello";

    SRE_Printf("sample_base_memory_set: before = %s  ", str);
    ret = memset_s(str, strlen(str), 'H', 1);
    if (ret) {
        SRE_Printf("sample_base_memory_set fial!\n");
        return;
    }
    SRE_Printf("sample_base_memory_set: after = %s\n", str);
}

static void sample_base_memory_copy(void)
{
    int ret;
    char src_str[] = "test";
    char dest_str[] = "this";

    SRE_Printf("sample_base_memory_copy: before = %s  ", dest_str);
    ret = memcpy_s(dest_str, strlen(dest_str), src_str, strlen(dest_str));
    if (ret) {
        SRE_Printf("sample_base_memory_copy fial!\n");
        return;
    }
    SRE_Printf("sample_base_memory_copy: after = %s\n", dest_str);
}

int app_main(void)
{
    SRE_Printf("start sample math\n");
    sample_math_fabs();
    sample_math_abs();
    sample_math_sqrt();
    sample_math_pow();
    sample_math_cos();
    sample_math_sin();
    sample_base_sizeof();
    sample_base_strlen();
    sample_base_memory_copy();
    sample_base_memory_set();
    while (1) {
    }
    return SRE_OK;
}