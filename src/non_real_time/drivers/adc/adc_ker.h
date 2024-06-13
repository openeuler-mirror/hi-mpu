// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _ADC_H
#define _ADC_H

/* channel list */
#define ADC_GROUP_MAX 2

#define ADC_CTRL_CH_NUM 8

#define SAMPLE_DAT_NUM 4
#define LLB_CH0_NUM    8
struct adc_reg_base {
    u32 adc_ctrl;
    u32 adc_irq_st;
    u32 adc_irq_en;
    u32 adc_test_dat;
    u32 adc_voltage_regs[SAMPLE_DAT_NUM];
    u32 adc_alarm_regs[LLB_CH0_NUM];
    u32 adc_scan_rate;
};

struct adc_host {
    cdev_st adc_dev;
    struct adc_reg_base __iomem *regbase[ADC_GROUP_MAX];
    u32 g_alarm_msg[ADC_GROUP_MAX];
    u32 adc_virq[ADC_GROUP_MAX];
    u32 irq_flag[ADC_GROUP_MAX];
    s8 *adc_irq_name[ADC_GROUP_MAX];
    u32 ref_voltage;
    spinlock_t adc_lock;
    /* reserved */
};

#define ADC_ENABLE               0x1
#define ADC_DISABLE              0x0
#define ADC_INTER_VOLTAGE        0x1
#define ADC_EXTER_VOLTAGE        0x0
#define ADC_12BIT_RES            0x3
#define ADC_INTERAL_REF          1200
#define ADC_EXTERNEL_REF         1800
#define ADC_CHANNEL_NUM          16
#define ADC_ALARM_TOP_MASK       0xfff
#define ADC_ALARM_LOWER_SHIFT    12
#define ADC_IRQ_ENABLE           0x1ff
#define ADC_VOL_REG_DIVISION     2
#define ADC_VOL_REG_SHIFT        12
#define ADC_VOLTAGE_CHANNEL_MASK 0xfff
#define ADC_6BIT_BASE_NUM        64
#define ADC_8BIT_BASE_NUM        256
#define ADC_10BIT_BASE_NUM       1024
#define ADC_12BIT_BASE_NUM       4096
#define ADC_IOC_MAXNR            4
#define ADC_IRQ_DISABLE          0x0

#endif /* _ADC_H */
