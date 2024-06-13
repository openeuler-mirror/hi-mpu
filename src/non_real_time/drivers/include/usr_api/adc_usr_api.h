// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * adc module user interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _ADC_USR_API_H_
#define _ADC_USR_API_H_

/* Use the standard ioctl command */
#define ADC_IOC_MAGIC 'a'
#define ADC_CMD_ALARM_CONFIG _IOW(ADC_IOC_MAGIC, 0, struct adc_alarm)
#define ADC_CMD_CONFIG _IOW(ADC_IOC_MAGIC, 1, struct adc_config)
#define ADC_CMD_GET_ALARM_MSG _IOWR(ADC_IOC_MAGIC, 2, unsigned int)
#define ADC_CMD_VOLTAGE_GET _IOWR(ADC_IOC_MAGIC, 3, struct adc_voltage)

#define DEV_ADC_NAME             "adc"
#define ADC_IRQ_CLEAN               0x1fff
#define ADC_SCAN_RATE_LIMITIED      0xFFFFFF

/* channel list */
enum adc_chan_id {
    ADC_CHAN_0,
    ADC_CHAN_1,
    ADC_CHAN_2,
    ADC_CHAN_3,
    ADC_CHAN_4,
    ADC_CHAN_5,
    ADC_CHAN_6,
    ADC_CHAN_7,
    ADC_CHAN_8,
    ADC_CHAN_9,
    ADC_CHAN_10,
    ADC_CHAN_11,
    ADC_CHAN_12,
    ADC_CHAN_13,
    ADC_CHAN_14,
    ADC_CHAN_15,
    ADC_CHAN_NONE
};
/* reference voltage */
enum adc_volt_ref {
    EXTERNAL_VOLT,
    INTERNAL_VOLT
};
struct adc_config {
    unsigned int scan_rate;
    enum adc_volt_ref voltage_ref_source;
    unsigned int ref_voltage;
};
struct adc_voltage {
    enum adc_chan_id chan_id;
    unsigned int voltage_val;
};
struct adc_alarm {
    enum adc_chan_id chan_id;
    unsigned int lower_limit;
    unsigned int top_limit;
};
#endif
