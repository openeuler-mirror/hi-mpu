/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_timer.h"

#define SPI_TIMER_PRINT_TIMES 10
#define SPI_TIMER_MODIFY_TIMERS 5
#define SPI_TIMER_UDEALY 500000
#define SPI_TIMER_1S (TIMER_FRQ - 1)
#define SPI_TIMER_2S (TIMER_FRQ * 2 - 1)
#define SPI_TIMER_5S (TIMER_FRQ * 5 - 1)

static int g_spi_timer_count = 0;

void mcs_peripherals_shutdown(void)
{
#if defined(__BAREMETAL__)
    bm_disable_irq(TIMER_BASE_IRQ + TIMER_ID_3);
#elif defined(__LIBCK__)
    SRE_HwiDisable(TIMER_BASE_IRQ + TIMER_ID_3);
#endif
    return;
}

static void timer_interrupt_process(int irq, uintptr_t param)
{
    int id = (int)param;
    bm_timer_irq_clear(id);
    g_spi_timer_count++;
    if (g_spi_timer_count == SPI_TIMER_MODIFY_TIMERS) {
        bm_timer_modify_bgload_value(id, SPI_TIMER_2S, 0);
    }
    if (g_spi_timer_count >= SPI_TIMER_PRINT_TIMES) {
        bm_timer_stop(id);
    }
    /* User add code here */
    bm_log("timer interrupt %d %d\r\n", irq, g_spi_timer_count);
}

static bm_timer_cfg g_htim;
static void sample_timer(unsigned int core)
{
    (void)core;
    g_htim.id = TIMER_ID_3;
    g_htim.load_l = SPI_TIMER_1S;
    g_htim.load_h = 0;
    g_htim.bgload_l = SPI_TIMER_5S;
    g_htim.bgload_h = 0;
    g_htim.mode = TIMER_MODE_RUN_PERIODIC; /* Run in period mode */
    g_htim.prescaler = 0;                  /* Don't frequency division */
    g_htim.interrupten = 1;                /* Trigger interrupt enable */
    g_htim.size = 0;                       /* 1 for 64bit, 0 for 32bit */
    g_htim.callback = timer_interrupt_process;
    g_htim.param = TIMER_ID_3;
    bm_timer_init(&g_htim);
    bm_timer_start(TIMER_ID_3);
    udelay(SPI_TIMER_UDEALY);
    unsigned int value_l;
    unsigned int value_h;
    bm_timer_get_cnt_value(TIMER_ID_3, &value_l, &value_h);
    bm_log("timer count = %x %x\n", value_l, value_h);
}

/* ************ sample_timer start ************ */
#if defined(__BAREMETAL__)
int main(void)
#elif defined(__LIBCK__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int timer_core = bm_get_coreid();
    switch (timer_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_timer(timer_core);
            break;
        default:
            bm_log("sample_timer: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_timer end ************ */