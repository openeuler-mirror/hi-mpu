/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_dmac.h"
#include "bm_i2c.h"

#define I2C_ID I2C_ID_3
#define TRANS_ADDR 0X53
#define BUFF_LEN 1000
#define I2C_TIME_OUT 1000
#define I2C_TRANSMIT_SYNC 1

XFER_DATA unsigned char g_buff_write[BUFF_LEN] = { 0 };
XFER_DATA unsigned char g_buff_read[BUFF_LEN] = { 0 };

void mcs_peripherals_shutdown(void)
{
#if defined(__BAREMETAL__)
    bm_disable_irq(DMAC_INTID);
    bm_disable_irq(I2C_BASE_IRQ + I2C_ID);
#elif defined(__LIBCK__)
    SRE_HwiDisable(DMAC_INTID);
    SRE_HwiDisable(I2C_BASE_IRQ + I2C_ID);
#endif
    return;
}

static void sample_i2c_init(void)
{
    int ret;
    struct i2c_init_info init_info;

    init_info.i2c_id = I2C_ID;
    init_info.speed = I2C_SPEED_100K;

    ret = bm_i2c_init(&init_info);
    if (ret) {
        bm_log("init fail!\n");
    } else {
        bm_log("init succsess!\n");
    }
}

static void sample_i2c_deinit(void)
{
    int ret;

    ret = bm_i2c_deinit(I2C_ID);
    if (ret) {
        bm_log("deinit fail!\n");
    } else {
        bm_log("deinit succsess!\n");
    }
}

static void i2c_transmit_async_callback(bm_i2c_ids i2c_id, bm_i2c_transmit_async_t type)
{
    bm_log("bm_i2c_ids = %d\n", i2c_id);
    bm_log("bm_i2c_transmit_async_t = %d\n", type);
}

static void sample_adxl345_read_sync(void)
{
    int ret;
    struct i2c_xfer_info read_info;
    struct i2c_xfer_info write_info;

    g_buff_write[0] = 0x80;
    write_info.i2c_id = I2C_ID;
    write_info.addr = TRANS_ADDR;
    write_info.buffer = g_buff_write;
    write_info.length = 1;
    write_info.timeout = I2C_TIME_OUT;
    ret = bm_i2c_write(&write_info);
    if (ret) {
        bm_log("sync write fail!\n");
        return;
    }
    bm_log("sync write succsess!\n");

    read_info.i2c_id = I2C_ID;
    read_info.addr = TRANS_ADDR;
    read_info.buffer = g_buff_read;
    read_info.length = 1;
    read_info.timeout = I2C_TIME_OUT;

    ret = bm_i2c_read(&read_info);
    if (ret) {
        bm_log("sync read fail!\n");
        return;
    }

    bm_log("g_buff_read[0] = 0x%x\n", g_buff_read[0]);
    if (g_buff_read[0] != 0xe5) { /* 0xe5 is device id */
        bm_log("sync read fail!\n");
    }
    bm_log("sync read succsess!\n");
}

static void sample_adxl345_read_async(void)
{
    int ret;
    struct i2c_xfer_info read_info;
    struct i2c_xfer_info write_info;

    g_buff_write[0] = 0x80;
    write_info.i2c_id = I2C_ID;
    write_info.addr = TRANS_ADDR;
    write_info.buffer = g_buff_write;
    write_info.length = 1;
    ret = bm_i2c_write_async(&write_info, i2c_transmit_async_callback);
    if (ret) {
        bm_log("async write fail!\n");
        return;
    }
    bm_log("async write succsess!\n");

    udelay(I2C_TIME_OUT);

    read_info.i2c_id = I2C_ID;
    read_info.addr = TRANS_ADDR;
    read_info.buffer = g_buff_read;
    read_info.length = 1;
    ret = bm_i2c_read_async(&read_info, i2c_transmit_async_callback);
    if (ret) {
        bm_log("async read fail!\n");
        return;
    }

    udelay(I2C_TIME_OUT);

    bm_log("g_buff_read[0] = 0x%x\n", g_buff_read[0]);
    if (g_buff_read[0] != 0xe5) { /* 0xe5 is device id */
        bm_log("async read fail!\n");
    }
    bm_log("async read succsess!\n");
}

static void sample_adxl345_read_dma(void)
{
    int ret;
    struct i2c_xfer_info read_info;
    struct i2c_xfer_info write_info;

    g_buff_write[0] = 0x80;
    write_info.length = 1;
    write_info.i2c_id = I2C_ID;
    write_info.addr = TRANS_ADDR;
    write_info.buffer = g_buff_write;
    ret = bm_i2c_write_dma(&write_info, i2c_transmit_async_callback);
    if (ret) {
        bm_log("dma write fail!\n");
        return;
    }
    bm_log("dma write succsess!\n");

    read_info.length = 1;
    read_info.i2c_id = I2C_ID;
    read_info.addr = TRANS_ADDR;
    read_info.buffer = g_buff_read;
    ret = bm_i2c_read_dma(&read_info, i2c_transmit_async_callback);
    if (ret) {
        bm_log("dma read fail!\n");
        return;
    }
    udelay(I2C_TIME_OUT);
    bm_log("g_buff_read[0] = 0x%x\n", g_buff_read[0]);
    bm_log("dma read succsess!\n");
}

#if defined(__BAREMETAL__)
int main(void)
#elif defined (__LIBCK__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int i2c_core = bm_get_coreid();
    switch (i2c_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_i2c_init();
            sample_adxl345_read_sync();
            sample_adxl345_read_async();
            bm_dmac_init();
            sample_adxl345_read_dma();
            sample_i2c_deinit();
            break;
        default:
            bm_log("sample_i2c: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}