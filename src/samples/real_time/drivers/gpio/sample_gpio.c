/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */

#include "bm_common.h"
#include "sample_common.h"
#include "bm_gpio.h"

/* ************ sample_gpio_level start ************ */
#define SAMPLE_GPIO_LEVEL_GROUP 2
#define SAMPLE_GPIO_LEVEL_PIN 12 /* 2*32+12 = 76 beep  */

void mcs_peripherals_shutdown(void)
{
#if defined(__BAREMETAL__)
    bm_disable_irq(CORE_SYS_GPIO_INTID_BASE + SAMPLE_GPIO_LEVEL_GROUP);
#elif defined(__LIBCK__)
    SRE_HwiDisable(CORE_SYS_GPIO_INTID_BASE + SAMPLE_GPIO_LEVEL_GROUP);
#endif
    return;
}

static void gpio_set_level_sample(unsigned int group, unsigned int pin)
{
    bm_gpio_set_level(group, pin, 1);
    udelay(2000000); /* delay 2000000us to wait */
    bm_gpio_set_level(group, pin, 0);
}

static void gpio_get_level_sample(unsigned int group, unsigned int pin)
{
    unsigned int val;
    bm_gpio_get_level(group, pin, &val);
    bm_log("sample_gpio: gpio read val = %d\n", val);
}

static void sample_gpio_level(unsigned int core)
{
    (void)core;
    bm_gpio_init(SAMPLE_GPIO_LEVEL_GROUP, SAMPLE_GPIO_LEVEL_PIN);
    gpio_set_level_sample(SAMPLE_GPIO_LEVEL_GROUP, SAMPLE_GPIO_LEVEL_PIN);
    gpio_get_level_sample(SAMPLE_GPIO_LEVEL_GROUP, SAMPLE_GPIO_LEVEL_PIN);
    bm_gpio_deinit(SAMPLE_GPIO_LEVEL_GROUP, SAMPLE_GPIO_LEVEL_PIN);
}
/* ************ sample_gpio_level end ************ */

/* ************ sample_gpio_interrupt start ************ */
#define SAMPLE_GPIO_INT_GROUP 2
#define SAMPLE_GPIO_INT_PIN 8 /* 2*32+8 = 72 spi1_cs0  */

static int gpio_int_isr(void *args)
{
    bm_log("sample_gpio: pin = %d\n", *(int *)args);
    return 0;
}

unsigned int g_gpio_num = 0;
static void gpio_irq_sample(unsigned int group, unsigned int pin, unsigned int core)
{
    g_gpio_num = group * GPIO_PIN_BUTT + pin;
    bm_gpio_enable_int(group, pin, GPIO_INT_LEVEL_TRIGGER, GPIO_INT_ACTIVE_HIGH);
    bm_gpio_isr_register(group, pin, gpio_int_isr, (void *)&g_gpio_num);
#if defined(__BAREMETAL__)
    bm_irq_set_affinity(CORE_SYS_GPIO_INTID_BASE + group, core);
    bm_enable_irq(CORE_SYS_GPIO_INTID_BASE + group);
#elif defined(__LIBCK__)
    OsHwiAffinitySet(CORE_SYS_GPIO_INTID_BASE + group, 1 << core);
    SRE_HwiEnable(CORE_SYS_GPIO_INTID_BASE + group);
#elif defined(__UNIPROTON__)
    sample_hwi_affinity_set(CORE_SYS_GPIO_INTID_BASE + group, 1 << core);
    PRT_HwiEnable(CORE_SYS_GPIO_INTID_BASE + group);
#endif
}

static void sample_gpio_interrupt(unsigned int core)
{
    bm_gpio_init(SAMPLE_GPIO_INT_GROUP, SAMPLE_GPIO_INT_PIN);
    gpio_irq_sample(SAMPLE_GPIO_INT_GROUP, SAMPLE_GPIO_INT_PIN, core);
}
/* ************ sample_gpio_interrupt end ************ */

/* ************ sample_gpio start ************ */
#if defined(__BAREMETAL__)
int main(void)
#elif defined(__LIBCK__)
int app_main(void)
#elif defined(__UNIPROTON__)
int app_main(void)
#endif
{
    sample_prepare();
    unsigned int gpio_core = bm_get_coreid();
    switch (gpio_core) {
        case SAMPLE_CORE0:
            return BM_OK;
        case SAMPLE_CORE1:
            break;
        case SAMPLE_CORE2:
            break;
        case SAMPLE_CORE3:
            sample_gpio_level(gpio_core);
            sample_gpio_interrupt(gpio_core);
            break;
        default:
            bm_log("sample_gpio: invalid core_num\n");
            break;
    }
    while (1) {
    }
    return BM_OK;
}
/* ************ sample_gpio end ************ */