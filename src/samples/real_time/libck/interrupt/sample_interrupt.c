/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "sre_config.h"
#include "sre_task.h"
#include "sre_hwi.h"
#include "sre_common.h"
#include "bm_common.h"
#include "os_cpu_armv8.h"

#include "os_cpu_armv8_external.h"

#define INTERRUPT_LATENCY_INT 7
#define INTERRUPT_BENCHMARK_TIMES 100000
#define INTERRUPT_OS_NANOSECOND_PER_CYCLE 40
#define INTERRUPT_TASK_NUMS 6
#define CORE 3

static U64 g_time_use;
static U64 g_start_cycle = 0;
static volatile U32 g_task_enter_cnt = 1;
static TskHandle g_task_ids[INTERRUPT_TASK_NUMS];

static U64 get_timer_cycle(void)
{
    U64 cycle;
    asm volatile("mrs %0, cntpct_el0" : "=r"(cycle) : : "memory", "cc");
    return cycle;
}

static void timer_initialize(void)
{
    g_start_cycle = get_timer_cycle();
}

static U64 timer_read(void)
{
    U64 end_cycle = get_timer_cycle();
    return (end_cycle - g_start_cycle) * INTERRUPT_OS_NANOSECOND_PER_CYCLE;
}

static void isr_handler(U32 uwIntNum)
{
    g_time_use = timer_read();
}

static void interrupt_task(void)
{
    U32 ret;
    U32 priority = 2;
    uintptr_t int_save;
    U64 time_avg = 0;

    ret = SRE_HwiDelete(INTERRUPT_LATENCY_INT);
    if (ret != SRE_OK) {
        SRE_Printf("SRE_HwiDelete error, ret = 0x%x\n", ret);
        return;
    }

    ret = SRE_HwiSetAttr(INTERRUPT_LATENCY_INT, priority, OS_HWI_MODE_ENGROSS);
    if (ret != SRE_OK) {
        SRE_Printf("SRE_HwiSetAttr error, ret = 0x%x\n", ret);
        return;
    }

    ret = SRE_HwiCreate(INTERRUPT_LATENCY_INT, (HwiProcFunc)isr_handler, 0);
    if (ret != SRE_OK) {
        SRE_Printf("create hwi fail!, ret = 0x%x\n", ret);
        return;
    }

    OsHwiAffinitySet(INTERRUPT_LATENCY_INT, 1 << CORE);

    for (U32 i = 0; i < INTERRUPT_BENCHMARK_TIMES; i++) {
        SRE_HwiEnable(INTERRUPT_LATENCY_INT);

        /* goes to isr_handler */
        int_save = SRE_IntLock();
        OsHwiMcTrigger(1 << CORE, INTERRUPT_LATENCY_INT);
        timer_initialize();
        SRE_IntRestore(int_save);
        dsb();
        SRE_HwiDisable(INTERRUPT_LATENCY_INT);

        time_avg += g_time_use;
    }

    SRE_Printf("single %lld, avg %lld\n",
        g_time_use, time_avg / INTERRUPT_BENCHMARK_TIMES);
}

static void sample_interrupt(void)
{
    U32 ret;
    struct TskInitParam param = {0};

    param.taskEntry = (TskEntryFunc)interrupt_task;
    param.stackSize = 0x800;
    param.name = "interrupt_task";
    param.taskPrio = OS_TSK_PRIORITY_08;
    param.stackAddr = 0;

    ret = SRE_TaskCreateOnly(&g_task_ids[0], &param);
    if (ret != SRE_OK) {
        SRE_Printf("create task only fail!, ret = 0x%x\n", ret);
        return;
    }

    ret = SRE_TaskResume(g_task_ids[0]);
    if (ret != SRE_OK) {
        SRE_Printf("resume task fail!, ret = 0x%x\n", ret);
        return;
    }

    for (int i = 1; i < INTERRUPT_TASK_NUMS; i++) {
        ret = SRE_TaskDelete(g_task_ids[i]);
        if (ret != OS_OK) {
            SRE_Printf("delete task[%d] fail!\n", i);
        }
    }
}

static void common_task(void)
{
    int task_id = g_task_enter_cnt;
    g_task_enter_cnt++;

    SRE_TaskSuspend(g_task_ids[task_id]);
}

static void interrupt_build_tasks(void)
{
    U32 ret;
    struct TskInitParam param = {0};

    param.taskEntry = (TskEntryFunc)common_task;
    param.stackSize = 0x200;
    param.name = "common_task";
    param.taskPrio = OS_TSK_PRIORITY_09;
    param.stackAddr = 0;

    for (int i = 1; i < INTERRUPT_TASK_NUMS; i++) {
        ret = SRE_TaskCreate(&g_task_ids[i], &param);
        if (ret != SRE_OK) {
            SRE_Printf("create task fail!, ret = 0x%x\n", ret);
            return;
        }
    }
}

int app_main(void)
{
    SRE_Printf("start sample interrupt latency\n");
    interrupt_build_tasks();
    sample_interrupt();

    while (1) {
    }
    return SRE_OK;
}