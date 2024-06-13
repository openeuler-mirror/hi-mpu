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

#define MCS_RECV_TIMES 1

static volatile unsigned char g_mcs_recv_times = 0;
static unsigned int g_received_msg;
static char *g_send_msg = "Hello, Libck! \r\n";

static int sample_mcs_cb(struct rpmsg_endpoint *ept, void *data, size_t len, unsigned int src, void *priv)
{
    (void)ept;
    (void)src;
    (void)priv;
    g_mcs_recv_times++;

    SRE_Printf("data: %s\n", (char *)data);

    int buffer_len;
    void *buffer_addr = sre_mcs_get_zero_cpy_data_addr(ENDPOINT_DATA, &buffer_len);
    if (buffer_len < strlen(g_send_msg)) {
        SRE_Printf("sample_mcs: sre_mcs_get_zero_cpy_data_addr error\r\n");
        return OS_ERROR;
    }

    if (memcpy_s(buffer_addr, buffer_len, g_send_msg, strlen(g_send_msg)) != EOK) {
        return OS_ERROR;
    }

    int ret = sre_mcs_send(ENDPOINT_DATA, buffer_addr, strlen(g_send_msg) * sizeof(char));
    if (ret < 0) {
        SRE_Printf("sre_mcs_send console error, ret = %d\r\n", ret);
        return OS_ERROR;
    }

    return OS_OK;
}

static int sample_mcs_run(void)
{
    int err;
    err = sre_mcs_create_ept(ENDPOINT_DATA, sample_mcs_cb);
    if (err) {
        SRE_Printf("sre_mcs_create_ept error\r\n");
        return err;
    }

    SRE_Printf("sre_mcs_create_ept create first success\r\n");

    while (g_mcs_recv_times != MCS_RECV_TIMES) {
    }

    SRE_Printf("mcs prepare to destroy endpoint, receive times = %d\r\n", g_mcs_recv_times);
    err = sre_mcs_destroy_ept(ENDPOINT_DATA);
    if (err) {
        SRE_Printf("sre_mcs_destroy_ept error\n");
        return err;
    }

    SRE_Printf("mcs destroy endpoint success\r\n");

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