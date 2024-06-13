/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_spi.h"
#include "bm_dmac.h"
#include "bm_cache.h"

/* ************ spi_test start ************ */

#define HISPI_BAUD_MIN (200000000 / 65534 + 1) /* 波特率最小值 */
#define HISPI_BAUD_MAX (200000000 / 6)             /* 波特率最大值 */

#define BUFFER_SIZE 32
#define SPI_TRANSFER_TIMEOUT 1000 /* 1000 ms */
#define SAMPLE_SPI_TRANSMIT_LEN 2

#define DEVICE_ID 0x00
#define INT_ENABLE 0x2E
#define DATA_FORMAT 0x31
#define BW_RATE 0x2c
#define POWER_CRL 0x2d

static bm_spi_handle g_handle;
static bm_spi_data g_data;

static void spi_test_init(void)
{
    bm_spi_cfg cfg = {
        .spi_id = 1,
        .cs_map = HISPI_CS1_SELECT,
        .baud = HISPI_BAUD_MAX / 2,
        .frame_len = HISPI_FRAME_LENGTH_8_BIT,
        .clk_edge = HISPI_CLOCK_EDGEDOWN,
        .clk_pol = HISPI_IDLE_CLOCK_POLARITYHIGH,
        .bm_spi_rx_threshold = 1,
    };
    bm_spi_init(&cfg);
    g_handle.spi_id = 1;
    g_handle.cs_id = 1;
}

XFER_DATA unsigned char g_write_buffer[BUFFER_SIZE] = {0};
XFER_DATA unsigned char g_read_buffer[BUFFER_SIZE] = {0};

void mcs_peripherals_shutdown(void)
{
#if defined(__BAREMETAL__)
    bm_disable_irq(DMAC_INTID);
    bm_disable_irq(CORE_SYS_SPI1_INTID);
#elif defined(__LIBCK__)
    SRE_HwiDisable(DMAC_INTID);
    SRE_HwiDisable(CORE_SYS_SPI1_INTID);
#endif
    return;
}

static void spi_transmit_async_callback(int spi_id, bm_spi_transmit_async_t type)
{
    bm_log("bm_spi_ids = %d\n", spi_id);
    bm_log("bm_spi_transmit_async_t = %d\n", type);
}

static void spi_recv_async_callback(int spi_id, bm_spi_transmit_async_t type)
{
    bm_log("bm_spi_ids = %d\n", spi_id);
    bm_log("bm_spi_transmit_async_t = %d\n", type);
}

static void build_spi_transmit_buff(void)
{
    g_write_buffer[0] = 0x80;
    g_write_buffer[1] = 0x5a;
    g_read_buffer[0] = 0;
    g_read_buffer[1] = 0;
    g_data.w_buf = g_write_buffer;
    g_data.w_len = SAMPLE_SPI_TRANSMIT_LEN;
    g_data.r_buf = g_read_buffer;
    g_data.r_len = SAMPLE_SPI_TRANSMIT_LEN;
}

static int sample_adxl345_read_dma(void)
{
    int ret;

    build_spi_transmit_buff();
    ret = bm_spi_write_dma(&g_handle, &g_data, spi_transmit_async_callback);
    if (ret) {
        bm_log("spi write dma func err\n");
        return ret;
    }
    udelay(SPI_TRANSFER_TIMEOUT);

    ret = bm_spi_read_dma(&g_handle, &g_data, spi_recv_async_callback); // ok
    if (ret) {
        bm_log("spi read dma func err\n");
        return ret;
    }
    udelay(SPI_TRANSFER_TIMEOUT);
    bm_log("read: %x %x\n", g_read_buffer[0], g_read_buffer[1]);
    return ret;
}
static int sample_adxl345_read_sync(void)
{
    int ret = 0;
    int len = 2;
    g_write_buffer[0] = 0x80;
    g_write_buffer[1] = 0x5a;
    g_data.w_buf = g_write_buffer;
    g_data.w_len = len;
    g_data.r_buf = g_read_buffer;
    g_data.r_len = len;
    g_data.time_out = SPI_TRANSFER_TIMEOUT;

    ret = bm_spi_write(&g_handle, &g_data);
    if (ret) {
        bm_log("spi write func err\n");
        return ret;
    }
    g_read_buffer[0] = 0;
    g_read_buffer[1] = 0;
    ret = bm_spi_read(&g_handle, &g_data);
    if (ret) {
        bm_log("spi write func err\n");
        return ret;
    }
    bm_log("read: %x %x\n", g_read_buffer[0], g_read_buffer[1]);
    return ret;
}

static int sample_adxl345_read_async1(void)
{
    int ret;

    build_spi_transmit_buff();
    ret = bm_spi_write_async(&g_handle, &g_data, spi_transmit_async_callback);
    if (ret) {
        bm_log("spi write async func err\n");
        return ret;
    }
    udelay(SPI_TRANSFER_TIMEOUT);
    ret = bm_spi_read_async(&g_handle, &g_data, spi_recv_async_callback);
    if (ret) {
        bm_log("spi read async func err\n");
        return ret;
    }
    udelay(SPI_TRANSFER_TIMEOUT);
    bm_log("read: %x %x\n", g_read_buffer[0], g_read_buffer[1]);
    return ret;
}

static int sample_adxl345_read_async2(void)
{
    int ret;

    build_spi_transmit_buff();
    ret = bm_spi_write_read_async(&g_handle, &g_data, spi_recv_async_callback);
    if (ret) {
        bm_log("spi write async func err\n");
        return ret;
    }
    udelay(SPI_TRANSFER_TIMEOUT);
    bm_log("read: %x %x\n", g_read_buffer[0], g_read_buffer[1]);
    return ret;
}

static void sample_spi(unsigned int core)
{
    bm_log("core%d spi sample111\n", core);
    spi_test_init();
    bm_dmac_init();
    sample_adxl345_read_dma();
    sample_adxl345_read_sync();
    sample_adxl345_read_async1();
    sample_adxl345_read_async2();
}

/* ************ sample_spi_interrupt end ************ */

/* ************ sample_spi start ************ */
#if defined(__BAREMETAL__)
int main(void)
#elif defined(__LIBCK__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int spi_core = bm_get_coreid();
    switch (spi_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_spi(spi_core);
            break;
        default:
            bm_log("sample_spi: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_spi end ************ */