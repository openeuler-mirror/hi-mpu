/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "securec.h"
#include "sre_task.h"

#include "sre_config.h"
#include "sre_task.h"
#include "sre_hwi.h"
#include "sre_hook.h"
#include "sre_exc.h"
#include "sre_mem.h"
#include "sre_common.h"
#include "bm_common.h"
#include "string.h"

#include "sre_mcs.h"

#ifndef OS_EMBED_ASM
#define OS_EMBED_ASM    __asm__ __volatile__
#endif

#define COUNTS_SENDS  2500
static unsigned char g_mcs_flag = 0;
static unsigned int g_received_msg;

static U64 g_store_ticks[COUNTS_SENDS];
static unsigned int g_counts_send = 0;
static unsigned int g_flag_libck = COUNTS_SENDS;

static void write_message_send(unsigned int data_len)
{
    U64 start, end;
    OS_EMBED_ASM("MRS  %0, CNTVCT_EL0" : "=r"(start) : : "memory", "cc");

    unsigned int buffer_len;
    void *buffer_addr = sre_mcs_get_zero_cpy_data_addr(ENDPOINT_DATA, &buffer_len);
    if ((int) buffer_len < data_len) {
        SRE_Printf("error\n");
        return;
    }
    
    if (memset_s(buffer_addr, buffer_len, 'a', data_len) != EOK) {
        SRE_Printf("Description Filling failed because the maximum value of the buffer exceeded\r\n");
        return;
    }
    OS_EMBED_ASM("MRS  %0, CNTVCT_EL0" : "=r"(end) : : "memory", "cc");

    int ret = sre_mcs_send(ENDPOINT_DATA, buffer_addr, data_len);
    if (ret < 0) {
        SRE_Printf("Failed to send data\r\n");
        return;
    }

    if (g_counts_send < g_flag_libck) {
        g_store_ticks[g_counts_send] = end - start;
    }

    g_counts_send++;
    if (g_counts_send == g_flag_libck) {
        long sums_ticks = 0;
        for (int i = 1; i < g_flag_libck; i++) {
            sums_ticks += g_store_ticks[i];
            SRE_Printf("Libck i = %d, nocopy = %llu\n", i, g_store_ticks[i]);
        }

        SRE_Printf("Libck %d byte for ticks average = %llu\n", data_len, sums_ticks / (COUNTS_SENDS - 1));
    }

    return;
}

static int sample_mcs_cb(struct rpmsg_endpoint *ept, void *data, size_t len, unsigned int src, void *priv)
{
    g_mcs_flag = 1;
    int chars_send_len = 16;
    write_message_send(chars_send_len);

    return OS_OK;
}

static int sample_mcs_run(void)
{
    int err;
    err = sre_mcs_create_ept(ENDPOINT_DATA, sample_mcs_cb);
    if (err) {
        SRE_Printf("sre_mcs_create_ept error\n");
        return err;
    }

    SRE_Printf("sample mcs wait send success\n");

    while (!g_mcs_flag) {
    }

    return OS_OK;
}

int app_main(void)
{
    static int count = 10;
    sample_mcs_run();
    while (1) {
        if (count >= 0) {
            SRE_Printf("----sample mcs-----\r\n");
            count--;
        }
        udelay(1000000);  // delay 1000000
    }
    return OS_OK;
}