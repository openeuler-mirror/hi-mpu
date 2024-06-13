// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * pwm module sdk interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _PWM_KER__H_
#define _PWM_KER__H_

#define INT_IRQ         0x104
#define COMM_INFO       0
#define EACH_INFO       0x8
#define FAN_INFO_OFFSET 0xA4
#define PWM_IO_ADD      0x0110
#define FAN_IO_ADD      0x0140
#define FAN_STATUS_ADD  0xA0

#define PWM_TYPE 0x0
#define FAN_TYPE 0x1

#define FAN_LOW_MASK  0xFFFF     /* the mask of channel 0-7 fan. */
#define FAN_HIGH_MASK 0xFFFF0000 /* the mask of channel 8-15 */
#define PWM_LOW_MASK  0x3F
#define PWM_HIGH_MASK 0xFC0

#define PWM_FIRST_HALF             5 /* 0-5 */
#define PWM_MAX_CHANNEL            12
#define PWM_FAN_PAD_CONNECT_FULL   (0xFFFFFFF)
#define FAN_FIRST_HALF             7 /* 0-7 */
#define FAN_MAX_CAHNNEL            16
#define FAN_PWM_SECOND_BASE_OFFSET (0x800) /* Base address of the second PWM module */

#define PWM_CLK_FRZ 50000000
#define FAN_NEED_HZ 1000
#define CYCLE_NUM   (PWM_CLK_FRZ / FAN_NEED_HZ) /* 10000 */
#define HIGH_RATIO  1000
#define LOW_RATIO   100

#define FAN_MEASURE_TIME_REG 0x108

#define FAN_MEASURE_TIME_VALUE 1999

#define DEV_PWM_NAME_KER "pwm"

#define PWM3 3

#define PWM_FAN_MODULE_NUM 2
#define PWM_FAN_MODULE0    0
#define PWM_FAN_MODULE1    1

#define PWM_CHEN_MASK    (0x103f) /* bit0-5,bit12 */
#define PWM_HALF_CH_AMSK (0x3f)   /* bit0-5 */
#define PWM_CHEN_BIT_NUM (6)

#define PWM_CH_OFFSET 12
#define PWM_PWH_OFFSET 8
#define PWM_PWL_OFFSET 4

#define FAN_REG_OFFSET 8
#define FAN_SRD_BIT 8

#define PWM_PAD_WIDTH 4

#define pwm_counter_msg(format, args...) KBOX_MSG(KBOX_COUNTER, format, ##args)

typedef struct {
    u32 fan_pad[FAN_MAX_CAHNNEL + 1];
    u32 pwm_pad[PWM_MAX_CHANNEL + 1];
} pwm_fan_pad_info;

typedef struct {
    u32 module_id[PWM_FAN_MODULE_NUM];
    pwm_fan_pad_info pwm_fan_pad;
    u32 virq[PWM_FAN_MODULE_NUM];
    cdev_st infra_pwm_dev;
    void __iomem *infra_pwm_map_addr;
    void __iomem *iocfg_add;
} fan_pwm_mgr;

extern void pwm_dfx_dump(void);

#endif /* _PWM_KER__H_ */
