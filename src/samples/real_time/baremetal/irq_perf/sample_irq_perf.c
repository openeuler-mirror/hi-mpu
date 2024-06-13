/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_hwi.h"

#define PERF_PPI_TIMER_INTID 27
#define PERF_PPI_TIMER_PRINT_TIMES 20
#define PERF_PPI_TIMER_PERIOD 250000
#define PERF_PPI_TIMER_HZ 1000000000
#define PERF_PPI_TIMER_SPACE 6250
#define PERF_PPI_IRQ_PRIORITY 0x5
#define PERF_TIMEOUT_FLAG 25

#if defined(BM_INT_PERF) && (BM_INT_PERF == 1)

/* 中断预到达时间 */
static unsigned long long g_cnt_ticks[PERF_PPI_TIMER_PRINT_TIMES + 1];
/* 中断实际到达时间 */
static unsigned long long g_data_buffer[PERF_PPI_TIMER_PRINT_TIMES];

static unsigned long long g_cur_freq;
static volatile int g_irq_count = 0;

static void perf_ppi_timer_init(unsigned int nano_seconds)
{
    unsigned long long cur_cnt = 0;
    unsigned int cnt_ctl = 0x1;

    asm volatile("mrs %0, cntvct_el0" : "=r"(cur_cnt));
    asm volatile("mrs %0, cntfrq_el0" : "=r"(g_cur_freq));
    g_cnt_ticks[0] = cur_cnt + (nano_seconds * g_cur_freq) / PERF_PPI_TIMER_HZ;
    asm volatile("msr cntv_cval_el0, %0" ::"r"(g_cnt_ticks[0]));
    /* set the control register */
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cnt_ctl));

    bm_irq_set_priority(PERF_PPI_TIMER_INTID, PERF_PPI_IRQ_PRIORITY);
    bm_enable_irq(PERF_PPI_TIMER_INTID);
}

static void perf_ppi_reset_timer(unsigned int cnt)
{
    if (cnt >= PERF_PPI_TIMER_PRINT_TIMES) {
        bm_disable_irq(PERF_PPI_TIMER_INTID);
        return;
    }

    g_cnt_ticks[cnt] =
        g_cnt_ticks[cnt - 1] + (PERF_PPI_TIMER_PERIOD * g_cur_freq) / PERF_PPI_TIMER_HZ;
    asm volatile("msr cntv_cval_el0, %0" ::"r"(g_cnt_ticks[cnt]));
    unsigned int cnt_ctl = 0x1;
    /* set the control register */
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cnt_ctl));
}

static void perf_handle(void *arg)
{
    (void)arg;

    perf_ppi_reset_timer(g_irq_count + 1);
    g_irq_count++;
}

static void check_performance(void)
{
    int index = g_irq_count;
    if (index >= PERF_PPI_TIMER_PRINT_TIMES) {
        return;
    }

    int delay_time = g_data_buffer[index] - g_cnt_ticks[index];
    if (delay_time > PERF_TIMEOUT_FLAG) {
        bm_log("warnning: the delay time exceeds 1 us. delay time = %d\n", delay_time);
    }

    if (index <= 0) {
        return;
    }

    int jitter_times = g_data_buffer[index] - g_data_buffer[index - 1] - PERF_PPI_TIMER_SPACE;
    if (jitter_times > PERF_TIMEOUT_FLAG) {
        bm_log("warnning: the jitter time exceeds 1 us. jitter time = %d\n", jitter_times);
    }
}

static void perf_hook(void)
{
    unsigned long long val;
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    g_data_buffer[g_irq_count] = val;
    check_performance();
    return;
}

static int sample_ppi_perf(void)
{
    g_irq_count = 0;
    bm_free_irq(PERF_PPI_TIMER_INTID);
    bm_request_irq(PERF_PPI_TIMER_INTID, perf_handle, NULL);
    bm_irq_perf_register(PERF_PPI_TIMER_INTID, perf_hook);
    perf_ppi_timer_init(PERF_PPI_TIMER_PERIOD);
    return 0;
}
#endif

int main(void)
{
    sample_prepare();
    unsigned int perf_core = bm_get_coreid();
    switch (perf_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            #if defined(BM_INT_PERF) && (BM_INT_PERF == 1)
                sample_ppi_perf();
            #endif
            break;
        default:
            bm_log("sample_irq_perf: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}