/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "securec.h"
#include "bm_common.h"
#include "sample_common.h"

#define PI 3.14159265
#define ONE_HUNDRED 100

static void sample_math_abs(void)
{
    int abs_t = -1; // -1 is magic number
    bm_log("sample_math_abs: abs(-1) * 100 = %d\n", (unsigned int)(abs(abs_t) * ONE_HUNDRED));
}

static void sample_math_fabs(void)
{
    float fabs_t = -1.5f; // -1.5f is magic number
    bm_log("sample_math_fabs: fabs(-1.5) * 100 = %d\n", (unsigned int)(fabs(fabs_t) * ONE_HUNDRED));
}

static void sample_math_pow(void)
{
    int pow_t = 2; // 2 is magic number
    bm_log("sample_math_pow: pow(2, 2) * 100 = %d\n", (unsigned int)(pow(pow_t, pow_t) * ONE_HUNDRED));
}

static void sample_math_sqrt(void)
{
    int sqrt_t = 25; // 25 is magic number
    bm_log("sample_math_sqrt: sqrt(25) * 100 = %d\n", (unsigned int)(sqrt(sqrt_t) * ONE_HUNDRED));
}

static void sample_math_sin(void)
{
    double sin_t = 45.0; // 45.0 is magic number
    double val = PI / 180; // 180 is magic number
    /* 45.000000 的正弦是 0.707107 度 */
    bm_log("sample_math_sin: sin * 100 = %d\n", (unsigned int)(sin(sin_t * val) * ONE_HUNDRED));
}

static void sample_math_cos(void)
{
    double cos_t = 60.0; // 60.0 is magic number
    double val = PI / 180; // 180 is magic number
    /* 60.000000 的余弦是 0.500000 度 */
    bm_log("sample_math_cos: cos * 100 = %d\n", (unsigned int)(cos(cos_t * val) * ONE_HUNDRED));
}

static void sample_base_strlen(void)
{
    char str[] = "hello";
    bm_log("sample_base_strlen: hello len = %d\n", strlen(str));
}

static void sample_base_sizeof(void)
{
    bm_log("sample_base_sizeof: sizeof(char) = %d\n", sizeof(char));
}

static void sample_base_memory_set(void)
{
    int ret;
    char str[] = "hello";

    bm_log("sample_base_memory_set: before = %s  ", str);
    ret = memset_s(str, strlen(str), 'H', 1);
    if (ret) {
        bm_log("sample_base_memory_set fial!\n");
        return;
    }
    bm_log("sample_base_memory_set: after = %s\n", str);
}

static void sample_base_memory_copy(void)
{
    int ret;
    char src_str[] = "test";
    char dest_str[] = "this";

    bm_log("sample_base_memory_copy: before = %s  ", dest_str);
    ret = memcpy_s(dest_str, strlen(dest_str), src_str, strlen(dest_str));
    if (ret) {
        bm_log("sample_base_memory_copy fial!\n");
        return;
    }
    bm_log("sample_base_memory_copy: after = %s\n", dest_str);
}


int main(void)
{
    sample_prepare();
    unsigned int math_core = bm_get_coreid();
    switch (math_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_math_abs();
            sample_math_fabs();
            sample_math_pow();
            sample_math_sqrt();
            sample_math_sin();
            sample_math_cos();
            sample_base_strlen();
            sample_base_sizeof();
            sample_base_memory_set();
            sample_base_memory_copy();
            break;
        default:
            bm_log("sample_math: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}