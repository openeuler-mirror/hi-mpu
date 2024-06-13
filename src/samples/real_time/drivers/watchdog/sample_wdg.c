/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_wdg.h"

#define WDG_UDEALY 1000000
#define WDG_3S (WDG_FRQ * 3 - 1) /* 3s */
#define WDG_TEST_TIMERS 10

void mcs_peripherals_shutdown(void)
{
#if defined(__BAREMETAL__)
    bm_disable_irq(WDG_BASE_IRQ + WDG_ID_2);
#elif defined(__LIBCK__)
    SRE_HwiDisable(WDG_BASE_IRQ + WDG_ID_2);
#endif
    return;
}

static void wdg_interrupt_process(bm_wdg_ids id)
{
    /* User add code here */
    bm_log("wdg %d interrupt\r\n", id);
}

static bm_wdg_cfg g_wdg;
int g_count = 0;
static void sample_wdg(unsigned int core)
{
    (void)core;
    g_wdg.id = WDG_ID_2;
    g_wdg.load = WDG_3S;
    g_wdg.callback = wdg_interrupt_process;
    bm_wdg_init(&g_wdg);
    bm_wdg_enable(WDG_ID_2);
    udelay(WDG_UDEALY);
    unsigned int value;
    bm_wdg_get_cnt_value(WDG_ID_2, &value);
    bm_log("wdg count = %x\n", value);
    while (1) {
        bm_wdg_feed(WDG_ID_2); /* if comment out this code. chip will reset */
        udelay(WDG_UDEALY);
        g_count++;
        bm_log("g_count = %d\n", g_count);
        if (g_count >= WDG_TEST_TIMERS) {
            g_count = 0;
            bm_wdg_disable(WDG_ID_2);
            break;
        }
    }
}

/* ************ sample_wdg start ************ */
#if defined(__BAREMETAL__)
int main(void)
#elif defined(__LIBCK__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int wdg_core = bm_get_coreid();
    switch (wdg_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_wdg(wdg_core);
            break;
        default:
            bm_log("sample_wdg: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_wdg end ************ */