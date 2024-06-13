/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_mcs.h"
#include "securec.h"
#include "bm_message.h"
#include "bm_cache.h"

#define MCS_RECV_TIMES 1

static volatile unsigned char g_mcs_recv_times = 0;
static const char *g_send_msg = "Hello, Baremetal!\n";
#define SHUTDOWN_CMD "stop"

static int sample_mcs_cb(struct rpmsg_endpoint *ept, void *data, size_t len, unsigned int src, void *priv)
{
    (void)data;
    (void)len;
    (void)ept;
    (void)src;
    (void)priv;
    g_mcs_recv_times++;

    bm_log("data: %s\n", (char *)data);

    int buffer_len;

    void *buffer_addr = bm_mcs_get_zero_cpy_data_addr(ENDPOINT_DATA, &buffer_len);
    if (buffer_len < (int)strlen(g_send_msg)) {
        bm_log("sample_mcs: bm_mcs_create_ept error\n");
        return BM_FAIL;
    }

    if (memcpy_s(buffer_addr, buffer_len, g_send_msg, strlen(g_send_msg)) != EOK) {
        return BM_FAIL;
    }

    bm_mcs_send(ENDPOINT_DATA, buffer_addr, strlen(g_send_msg) * sizeof(char));
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

    bm_log("bm_mcs_create_ept create first success\n");

    while (g_mcs_recv_times != MCS_RECV_TIMES) {
        bm_flush_dcache_all();
    }

    bm_log("mcs prepare to destroy endpoint, receive times = %d\n", g_mcs_recv_times);
    err = bm_mcs_destroy_ept(ENDPOINT_DATA);
    if (err) {
        bm_log("bm_mcs_destroy_ept error\n");
        return err;
    }

    bm_log("mcs destroy endpoint success\n");

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