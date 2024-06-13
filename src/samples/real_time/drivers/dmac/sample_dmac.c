/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_dmac.h"
#include "securec.h"

#define BUFF_LEN 200
#define DMAC_TRANS_LEN 150
#define DMAC_TIME_OUT 1000
#define DMAC_DEINIT_DELAY 1000

XFER_DATA unsigned char g_sync_src_buff[BUFF_LEN];
XFER_DATA unsigned char g_sync_dest_buff[BUFF_LEN];
XFER_DATA unsigned char g_async_src_buff[BUFF_LEN];
XFER_DATA unsigned char g_async_dest_buff[BUFF_LEN];

void mcs_peripherals_shutdown(void)
{
#if defined(__BAREMETAL__)
    bm_disable_irq(DMAC_INTID);
#elif defined(__LIBCK__)
    SRE_HwiDisable(DMAC_INTID);
#endif
    return;
}

static void build_channel_cfg_sync(bm_dmac_channel_cfg *channel_cfg)
{
    channel_cfg->req = 0;
    channel_cfg->src_addr = (uintptr_t)(&g_sync_src_buff);
    channel_cfg->dest_addr = (uintptr_t)(&g_sync_dest_buff);
    channel_cfg->data_len = DMAC_TRANS_LEN;
    channel_cfg->trans_type = TRASFER_TYPE_M2M;
}

static void build_channel_cfg_async(bm_dmac_channel_cfg *channel_cfg)
{
    channel_cfg->req = 0;
    channel_cfg->src_addr = (uintptr_t)(&g_async_src_buff);
    channel_cfg->dest_addr = (uintptr_t)(&g_async_dest_buff);
    channel_cfg->data_len = DMAC_TRANS_LEN;
    channel_cfg->trans_type = TRASFER_TYPE_M2M;
}

static void sample_damc_init(void)
{
    int ret;
    ret = bm_dmac_init();
    if (ret) {
        bm_log("init fail!\n");
    } else {
        bm_log("init succsess!\n");
    }
}

static void sample_damc_transmit_sync(void)
{
    int ret;
    int i;
    bm_dmac_channel_cfg channel_cfg = { 0 };
    if (BUFF_LEN < DMAC_TRANS_LEN) {
        return;
    }

    if (memset_s(g_sync_src_buff, BUFF_LEN, 'a', BUFF_LEN) != EOK) {
        return;
    }

    if (memset_s(g_sync_dest_buff, BUFF_LEN, 'x', BUFF_LEN) != EOK) {
        return;
    }

    build_channel_cfg_sync(&channel_cfg);
    ret = bm_dmac_transmit_sync(&channel_cfg, DMAC_TIME_OUT);
    if (ret) {
        bm_log("transmit_sync fail!\n");
    } else {
        bm_log("transmit_sync succsess!\n");
    }

    for (i = 0; i < DMAC_TRANS_LEN; i++) {
        if (g_sync_dest_buff[i] != g_sync_src_buff[i]) {
            bm_log("g_sync_dest_buff[%d] != g_sync_src_buff[%d]\n", i, i);
            break;
        }
    }

    for (i = DMAC_TRANS_LEN; i < BUFF_LEN; i++) {
        if (g_sync_dest_buff[i] != 'x') {
            bm_log("g_sync_dest_buff[%d] != x\n", i);
            break;
        }
    }
}

static void dmac_transmit_async_callback(bm_dmac_req req, bm_dmac_transmit_async_t type)
{
    int i;

    bm_log("bm_dmac_req = %d\n", req);
    bm_log("bm_dmac_transmit_async_t = %d\n", type);

    for (i = 0; i < DMAC_TRANS_LEN; i++) {
        if (g_async_dest_buff[i] != g_async_src_buff[i]) {
            bm_log("g_async_dest_buff[%d] != g_async_src_buff[%d]\n", i, i);
            break;
        }
    }

    for (i = DMAC_TRANS_LEN; i < BUFF_LEN; i++) {
        if (g_async_dest_buff[i] != 'x') {
            bm_log("g_async_dest_buff[%d] != x\n", i);
            break;
        }
    }
}

static void sample_damc_transmit_async(void)
{
    int ret;
    bm_dmac_channel_cfg channel_cfg = { 0 };
    if (BUFF_LEN < DMAC_TRANS_LEN) {
        return;
    }

    if (memset_s(g_async_src_buff, BUFF_LEN, 'b', BUFF_LEN) != EOK) {
        return;
    }

    if (memset_s(g_async_dest_buff, BUFF_LEN, 'x', BUFF_LEN) != EOK) {
        return;
    }

    build_channel_cfg_async(&channel_cfg);
    ret = bm_dmac_transmit_async(&channel_cfg, dmac_transmit_async_callback);
    if (ret) {
        bm_log("transmit_async fail!\n");
    } else {
        bm_log("transmit_async succsess!\n");
    }
}

static void sample_dmac_deinit(void)
{
    int ret;
    udelay(DMAC_DEINIT_DELAY);
    ret = bm_dmac_deinit();
    if (ret) {
        bm_log("deinit fail!\n");
    } else {
        bm_log("deinit succsess!\n");
    }
}

#if defined(__BAREMETAL__)
int main(void)
#elif defined (__LIBCK__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int dmac_core = bm_get_coreid();
    switch (dmac_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_damc_init();
            sample_damc_transmit_sync();
            sample_damc_transmit_async();
            sample_dmac_deinit();
            break;
        default:
            bm_log("sample_dmac: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}