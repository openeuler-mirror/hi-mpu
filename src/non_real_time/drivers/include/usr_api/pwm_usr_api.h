// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * pwm user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __PWM_USR_API_H__
#define __PWM_USR_API_H__

#define DEV_PWM_NAME                "/dev/pwm"
#define PWM_IOC_MAGIC               'p'
#define PWM_CMD_GET_DUTY_RATIO      _IOR(PWM_IOC_MAGIC, 0, int)
#define PWM_CMD_SET_DUTY_RATIO      _IOW(PWM_IOC_MAGIC, 1, int)
#define PWM_CMD_GET_FAN_SPEED       _IOR(PWM_IOC_MAGIC, 2, int)
#define PWM_CMD_GET_HIGH_DUTY_RATIO _IOR(PWM_IOC_MAGIC, 3, int)
#define PWM_CMD_SET_HIGH_DUTY_RATIO _IOW(PWM_IOC_MAGIC, 4, int)
#define PWM_CMD_ENABLE_RESET_SPEED  _IOW(PWM_IOC_MAGIC, 5, int)

#define PWM_CMD_MAXNR (5)

typedef struct {
    u32 channel_id;
    u32 ratio;
} pwm_info_s;

typedef struct {
    u32 channel_id;
    u32 speed;
} fan_info_s;

typedef struct {
    u32 rst_hold_en;
    u32 ratio;
    u32 rsvd[2];
} pwm_rst_ctrl_s;

#endif
