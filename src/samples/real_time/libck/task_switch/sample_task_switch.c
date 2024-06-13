/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#include "sre_config.h"
#include "sre_task.h"
#include "sre_common.h"
#include "bm_common.h"

#define TASK_SWITCH_BENCHMARK_TIMES 50000
#define TASK_SWITCH_OS_NANOSECOND_PER_CYCLE 40
#define TASK_SWITCH_TASK_NUMS 5

static U64 g_startCycle = 0;
static U32 g_delayUseTime;
static volatile U32 g_taskEnterCnt = 2;
static TskHandle g_taskIds[TASK_SWITCH_TASK_NUMS];

static void switch_task1(void)
{
    U32 ret;
    U64 cycle;
    int cnt = TASK_SWITCH_BENCHMARK_TIMES;

    ret = SRE_TaskResume(g_taskIds[1]);
    if (ret != SRE_OK) {
        SRE_Printf("resume task2 fail!, ret = 0x%x\n", ret);
        return;
    }

    SRE_TaskDelay(0);
    while (cnt > 0) {
        asm volatile("mrs %0, cntpct_el0" : "=r"(cycle) : : "memory", "cc");
        g_startCycle = cycle;
        SRE_TaskDelay(0);
        cnt--;
    }
}

static void switch_task2(void)
{
    U64 timeUse;
    U64 timeAvg = 0;
    U64 cycle;
    int cnt = TASK_SWITCH_BENCHMARK_TIMES;

    while (cnt > 0) {
        SRE_TaskDelay(0);
        asm volatile("mrs %0, cntpct_el0" : "=r"(cycle) : : "memory", "cc");
        timeUse = (cycle - g_startCycle) * TASK_SWITCH_OS_NANOSECOND_PER_CYCLE - g_delayUseTime;
        timeAvg += timeUse;
        cnt--;
    }

    SRE_Printf("single %lld, avg %lld\n",
        timeUse, timeAvg / TASK_SWITCH_BENCHMARK_TIMES);
}

static void sample_task_switch(void)
{
    U32 ret;
    U64 startCycle;
    U64 endCycle;
    struct TskInitParam taskParam = {0};

    taskParam.taskEntry = (TskEntryFunc)switch_task1;
    taskParam.stackSize = 0x800;
    taskParam.name = "switch_task1";
    taskParam.taskPrio = OS_TSK_PRIORITY_05;
    taskParam.stackAddr = 0;

    ret = SRE_TaskCreateOnly(&g_taskIds[0], &taskParam);
    if (ret != SRE_OK) {
        SRE_Printf("create task2 fail!, ret = 0x%x\n", ret);
        return;
    }

    taskParam.taskEntry = (TskEntryFunc)switch_task2;
    taskParam.stackSize = 0x800;
    taskParam.name = "switch_task2";
    taskParam.taskPrio = OS_TSK_PRIORITY_05;
    taskParam.stackAddr = 0;

    ret = SRE_TaskCreateOnly(&g_taskIds[1], &taskParam);
    if (ret != SRE_OK) {
        SRE_Printf("create task2 fail!, ret = 0x%x\n", ret);
        return;
    }

    asm volatile("mrs %0, cntpct_el0" : "=r"(startCycle) : : "memory", "cc");
    asm volatile("mrs %0, cntpct_el0" : "=r"(endCycle) : : "memory", "cc");
    g_delayUseTime = endCycle - startCycle;

    ret = SRE_TaskResume(g_taskIds[0]);
    if (ret != SRE_OK) {
        SRE_Printf("resume task1 fail!, ret = 0x%x\n", ret);
        return;
    }

    for (int i = 2; i < TASK_SWITCH_TASK_NUMS; i++) {
        ret = SRE_TaskDelete(g_taskIds[i]);
        if (ret != OS_OK) {
            SRE_Printf("delete task[%d] fail!\n", i);
        }
    }
}

static void common_task(void)
{
    int taskId = g_taskEnterCnt;
    g_taskEnterCnt++;

    SRE_TaskSuspend(g_taskIds[taskId]);
}

static void task_switch_build_tasks(void)
{
    U32 ret;
    struct TskInitParam param = {0};

    param.taskEntry = (TskEntryFunc)common_task;
    param.stackSize = 0x200;
    param.name = "common_task";
    param.taskPrio = OS_TSK_PRIORITY_09;
    param.stackAddr = 0;

    for (int i = 2; i < TASK_SWITCH_TASK_NUMS; i++) {
        ret = SRE_TaskCreate(&g_taskIds[i], &param);
        if (ret != SRE_OK) {
            SRE_Printf("create task fail!, ret = 0x%x\n", ret);
            return;
        }
    }
}

int app_main(void)
{
    SRE_Printf("start sample task switch\n");
    task_switch_build_tasks();
    sample_task_switch();

    while (1) {
    }
    return SRE_OK;
}