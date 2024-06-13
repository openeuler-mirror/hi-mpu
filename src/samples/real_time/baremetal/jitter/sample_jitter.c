/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_hwi.h"

#define PPI_TIMER_INTID_JITTER 27
#define PPI_TIMER_PRINT_TIMES_JITTER 20
#define PPI_TIMER_PERIOD_JITTER 250000
#define PPI_TIMER_HZ_JITTER 1000000000
#define PPI_TIMER_SPACE_JITTER 6250
#define PPI_IRQ_PRIORITY_JITTER 0x5

static unsigned long long g_cnt_ticks[PPI_TIMER_PRINT_TIMES_JITTER + 1];
static unsigned long long g_cur_freq;
static unsigned long long g_data_buffer[PPI_TIMER_PRINT_TIMES_JITTER];
static volatile int g_irq_count = 0;

static void jitter_ppi_timer_init(unsigned int nano_seconds)
{
    unsigned long long cur_cnt = 0;
    asm volatile("mrs %0, cntvct_el0" : "=r"(cur_cnt));
    asm volatile("mrs %0, cntfrq_el0" : "=r"(g_cur_freq));
    g_cnt_ticks[0] = cur_cnt + (nano_seconds * g_cur_freq) / PPI_TIMER_HZ_JITTER;
    asm volatile("msr cntv_cval_el0, %0" ::"r"(g_cnt_ticks[0]));
    unsigned int cnt_ctl = 0x1;
    /* set the control register */
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cnt_ctl));
}

static void jitter_ppi_timer_start(unsigned int prio)
{
    bm_irq_set_priority(PPI_TIMER_INTID_JITTER, prio);
    bm_enable_irq(PPI_TIMER_INTID_JITTER);
}

static void jitter_ppi_reset_timer(unsigned int cnt)
{
    g_cnt_ticks[cnt] =
        g_cnt_ticks[cnt - 1] + (PPI_TIMER_PERIOD_JITTER * g_cur_freq) / PPI_TIMER_HZ_JITTER;
    asm volatile("msr cntv_cval_el0, %0" ::"r"(g_cnt_ticks[cnt]));
    unsigned int cnt_ctl = 0x1;
    /* set the control register */
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cnt_ctl));
}

static void jitter_ppi_int27_isr(void *arg)
{
    (void)arg;
    unsigned long long val;
    asm volatile("mrs %0, cntvct_el0" : "=r"(val));
    unsigned int cnt = g_irq_count;
    if (cnt >= PPI_TIMER_PRINT_TIMES_JITTER) {
        bm_disable_irq(PPI_TIMER_INTID_JITTER);
    } else {
        g_data_buffer[cnt] = val;
        jitter_ppi_reset_timer(cnt + 1);
    }
    g_irq_count++;
    return;
}


static void jitter_ppi_start(void)
{
    g_irq_count = 0;
    int ppi_priority = PPI_IRQ_PRIORITY_JITTER;
    bm_free_irq(PPI_TIMER_INTID_JITTER);
    bm_request_irq(PPI_TIMER_INTID_JITTER, jitter_ppi_int27_isr, NULL);
    int nano_seconds = PPI_TIMER_PERIOD_JITTER;
    jitter_ppi_timer_init(nano_seconds);
    jitter_ppi_timer_start(ppi_priority);
}

static int sample_jitter_ppi(unsigned int core)
{
    jitter_ppi_start();
    while (g_irq_count < PPI_TIMER_PRINT_TIMES_JITTER) {
    }
    bm_uart_enable_print(UART_NUM4);
    bm_printf("\t core%d jitter\t\t\t\t core%d delay\n", core, core);
    for (int i = 0; i < PPI_TIMER_PRINT_TIMES_JITTER - 1; i++) {
        bm_printf("buf[%d](%u) - buf[%d](%u) = %d\t", (i + 1), g_data_buffer[i + 1], i, g_data_buffer[i],
            (g_data_buffer[i + 1] - g_data_buffer[i] - PPI_TIMER_SPACE_JITTER));
        bm_printf("buf[%d] - cnt[%d]: %d \n", i, i, g_data_buffer[i] - g_cnt_ticks[i]);
    }
    bm_uart_enable_print(UART_NUM2);
    return 0;
}

int main(void)
{
    sample_prepare();
    unsigned int jitter_core = bm_get_coreid();
    switch (jitter_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_jitter_ppi(jitter_core);
            break;
        default:
            bm_log("sample_jitter: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}