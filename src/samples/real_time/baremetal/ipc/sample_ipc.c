/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "bm_config.h"
#include "sample_common.h"
#include "bm_mcs.h"
#include "bm_message.h"
#include "bm_cache.h"
#include "securec.h"

#ifndef OS_EMBED_ASM
#define OS_EMBED_ASM    __asm__ __volatile__
#endif

#define COUNTS_SENDS  2500
static long long g_store_ticks[COUNTS_SENDS];
static int g_count_send = 0;
static int flag_uniproton = COUNTS_SENDS;

static int write_message_send(int data_len)
{
    unsigned long long start, end;
    OS_EMBED_ASM("MRS  %0, CNTVCT_EL0" : "=r"(start) : : "memory", "cc");

    int buffer_len;
    void *buffer_addr = bm_mcs_get_zero_cpy_data_addr(ENDPOINT_DATA, &buffer_len);
    if ((int) buffer_len < data_len) {
        bm_log("error\n");
        return BM_FAIL;
    }
    
    if (memset_s(buffer_addr, buffer_len, 'a', data_len) != BM_OK) {
        bm_log("Description Filling failed because the maximum value of the buffer exceeded\r\n");
        return BM_FAIL;
    }
    OS_EMBED_ASM("MRS  %0, CNTVCT_EL0" : "=r"(end) : : "memory", "cc");

    int ret = bm_mcs_send(ENDPOINT_DATA, buffer_addr, data_len);
    if (ret < 0) {
        return BM_FAIL;
    }

    if (g_count_send < flag_uniproton) {
        g_store_ticks[g_count_send] = end - start;
    }

    g_count_send++;
    if (g_count_send == flag_uniproton) {
        long sums_ticks = 0;
        for (int i = 1; i < flag_uniproton; i++) {
            sums_ticks += g_store_ticks[i];
            bm_log("baremetal i = %d, nocopy = %llu\n", i, g_store_ticks[i]);
        }
        bm_log("baremetal tickers average = %llu\n", sums_ticks / (COUNTS_SENDS - 1));
    }

    return BM_OK;
}

static int sample_mcs_cb(struct rpmsg_endpoint *ept, void *data, size_t len, unsigned int src, void *priv)
{
    (void)data;
    (void)len;
    (void)ept;
    (void)src;
    (void)priv;
    int chars_len = 256;
    write_message_send(chars_len);

    return BM_OK;
}

XFER_DATA unsigned char s[] = {0x12, 0x34, 0x12, 0x34};  // buf 0x12, 0x34, 0x12, 0x34
static int sample_mcs_run(void)
{
    int err;
    err = bm_mcs_create_ept(ENDPOINT_DATA, sample_mcs_cb);
    if (err) {
        bm_log("sample_mcs: bm_mcs_create_ept error\n");
        return err;
    }

    while (!get_shutdown_flag()) {
        bm_flush_dcache_all();
    }

    (void)bm_send_msg(0, s, sizeof(s));
    bm_flush_dcache_all();
    bm_log("prepare to shutdown core2\n");
    bm_mcs_shutdown();
    bm_log("shutdown core2 done \n"); // 预期不会打印本行
    return BM_OK;
}

static int sample_mcs_slave_run(void)
{
    int flag = 0;
    while (flag != 0x34123412) {  // 0x34123412 magicword
        unsigned char *str = bm_get_msg(0);
        flag = *(unsigned int *)str;
        bm_flush_dcache_all();
    }
    unsigned char *str = bm_get_msg(0);
    *(unsigned int *)str = 0;
    bm_log("prepare to shutdown core3 \n");
    bm_mcs_shutdown();
    bm_log("shutdown core3 done \n"); // 预期不会打印本行
    return 0;
}


/* ************ sample_mcs start ************ */
int main(void)
{
    sample_prepare();
    unsigned int mcs_core = bm_get_coreid();
    switch (mcs_core) {
        case SAMPLE_CORE0:
            break;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            if (sample_mcs_run()) {
                return BM_FAIL;
            }
            break;
        case SAMPLE_CORE3:
            (void)sample_mcs_slave_run();
            break;
        default:
            bm_log("sample_mcs: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_mcs end ************ */
