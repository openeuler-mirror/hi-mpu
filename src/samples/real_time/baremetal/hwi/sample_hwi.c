/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_hwi.h"

/* ************ sample_hwi_ipi start ************ */
#define SMPLE_SGI_SEND_CORE 3
#define SMPLE_SGI_SEND_TIMES 3
#define SMPLE_SGI_SEND_INTERVAL 1000
#define SMPLE_MAIN_ROUTE_CORE_MAP 0x9
#define SMPLE_IPI_INT13_NO 13

static void hwi_ipi_int13_isr(void *args)
{
    (void)args;
    bm_log("sample_hwi: ipi interrupt enter\n");
    return;
}

static void sample_hwi_ipi(unsigned int core)
{
    bm_enable_irq(SMPLE_IPI_INT13_NO);
    bm_request_irq(SMPLE_IPI_INT13_NO, hwi_ipi_int13_isr, NULL);
    if (core == SMPLE_SGI_SEND_CORE) {
        udelay(SMPLE_SGI_SEND_INTERVAL);
        bm_hwi_trigger(SMPLE_IPI_INT13_NO, SMPLE_MAIN_ROUTE_CORE_MAP);
        udelay(SMPLE_SGI_SEND_INTERVAL);
        bm_hwi_trigger(SMPLE_IPI_INT13_NO, SMPLE_MAIN_ROUTE_CORE_MAP);
        udelay(SMPLE_SGI_SEND_INTERVAL);
        bm_hwi_trigger(SMPLE_IPI_INT13_NO, SMPLE_MAIN_ROUTE_CORE_MAP);
        udelay(SMPLE_SGI_SEND_INTERVAL);
    }
}
/* ************ sample_hwi_ipi end ************ */

/* ************ sample_hwi_ppi end ************ */
#define SMPLE_PPI_TIMER_INTID 27
#define SMPLE_PPI_TIMER_PERIOD 250000
#define SMPLE_PPI_TIMER_HZ 1000000000
#define SMPLE_PPI_TIMER_PRINT_TIMES 3

static void bm_ppi_timer_init(unsigned int nano_seconds)
{
    unsigned int cnt_ticks = 0;
    unsigned long long cur_freq = 0;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(cur_freq));
    cnt_ticks = (nano_seconds * cur_freq) / SMPLE_PPI_TIMER_HZ;
    unsigned int cnt_ctl = 0;
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cnt_ctl));
    asm volatile("msr cntv_tval_el0, %0" ::"r"(cnt_ticks));
    cnt_ctl = 1;
    /* set the control register */
    asm volatile("msr cntv_ctl_el0, %0" ::"r"(cnt_ctl));
}

static void bm_ppi_timer_start(unsigned int prio)
{
    bm_irq_set_priority(SMPLE_PPI_TIMER_INTID, prio);
    bm_enable_irq(SMPLE_PPI_TIMER_INTID);
}

static void hwi_ppi_int27_isr(void *arg)
{
    (void)arg;
    static int irq_count = 0;
    irq_count++;
    if (irq_count >= SMPLE_PPI_TIMER_PRINT_TIMES) {
        bm_disable_irq(SMPLE_PPI_TIMER_INTID);
    }
    bm_log("sample_hwi: ppi interrupt enter\n");
    return;
}
static void sample_hwi_ppi(unsigned int core)
{
    (void)core;
    int ppi_priority = 0x5;
    bm_request_irq(SMPLE_PPI_TIMER_INTID, hwi_ppi_int27_isr, NULL);
    int nano_seconds = SMPLE_PPI_TIMER_PERIOD;
    bm_ppi_timer_init(nano_seconds);
    bm_ppi_timer_start(ppi_priority);
}
/* ************ sample_hwi_ppi end ************ */

/* sample_hwi_spi_interrupt(Shared Peripheral Interrupt) start */
#define SMPLE_UART_IRQ_CORE 3
#define SAMPLE_UART_NUM 4
#define SAMPLE_UART4_INT 92
#define SAMPLE_UART4_PRIO 6
#define SAMPLE_SEND_LEN 128
#define SAMPLE_UART_RECV_TIMES 6

static void hwi_spi_interrupt_recv_isr(void *arg)
{
    char data;
    int ret;
    static int count = 0;
    ret = bm_uart_rx(SAMPLE_UART_NUM, &data);
    if (ret == BM_OK) {
        bm_log("sample_hwi: [arg:%d] uart4 interrupt rxdata[%c]\n", (int)(uintptr_t)arg, data);
        count++;
    } else {
        bm_log("sample_hwi: uart4 interrupt rx failed\n");
    }
    if (count > SAMPLE_UART_RECV_TIMES) {
        bm_uart_set_irq_enable(SAMPLE_UART_NUM, 0);
        bm_free_irq(SAMPLE_UART_NUM);
    }
    return;
}

static int sample_hwi_spi_interrupt(unsigned int core)
{
    int ret;
    int uart_num = SAMPLE_UART_NUM;
    serial_cfg uart_cfg = {
        .hw_uart_no = uart_num,
        .uart_src_clk = UART_CLK,
        .data_bits = UART_DATA_8BIT, /* default data_bits is 8 */
        .stop = UART_STOP_1BIT,
        .pen = UART_VERIFY_DISABLE,
        .baud_rate = 115200 /* default baud_rate is 115200 */
    };

    ret = bm_uart_init(&uart_cfg);
    if (ret) {
        return BM_FAIL;
    }

    ret = bm_uart_set_irq_enable(uart_num, 1);
    if (ret) {
        return BM_FAIL;
    }

    ret = bm_request_irq(SAMPLE_UART4_INT, hwi_spi_interrupt_recv_isr, NULL);
    if (ret) {
        return BM_FAIL;
    }

    /* route */
    if (core == SMPLE_UART_IRQ_CORE) {
        ret = bm_irq_set_affinity(SAMPLE_UART4_INT, core);
        if (ret) {
            return BM_FAIL;
        }
        ret = bm_irq_set_priority(SAMPLE_UART4_INT, SAMPLE_UART4_PRIO);
        if (ret) {
            return BM_FAIL;
        }
        ret = bm_enable_irq(SAMPLE_UART4_INT);
        if (ret) {
            return BM_FAIL;
        }
    }
    return BM_OK;
}
/* sample_hwi_spi_interrupt(Shared Peripheral Interrupt) end */

/* ************ sample_hwi start ************ */
int main(void)
{
    sample_prepare();
    unsigned int hwi_core = bm_get_coreid();
    switch (hwi_core) {
        case SAMPLE_CORE0:
            sample_hwi_ipi(hwi_core);
            sample_hwi_spi_interrupt(hwi_core);
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_hwi_ipi(hwi_core);
            sample_hwi_ppi(hwi_core);
            sample_hwi_spi_interrupt(hwi_core);
            break;
        default:
            bm_log("sample_hwi: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_hwi end ************ */