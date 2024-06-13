// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * msg_scm3 user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __MSG_SCM3_USER_API_H__
#define __MSG_SCM3_USER_API_H__

#define SEC_MODULE_DEVNAME              "sec_module"

#define SEC_MODULE_IOC_MAGIC            's'
#define SEC_MODULE_CMD_TX_RX            _IOWR(SEC_MODULE_IOC_MAGIC, 0x01, int)

/* user add cmd type here */
typedef enum {
    MSG_CMD_A55_DICE = 1,
    MSG_CMD_A55_SEC_FW,

    MSG_CMD_A55_MAX_NUM
}msg_type_a55_e;

typedef struct {
    unsigned char cmd;
    unsigned char sub_cmd;
    unsigned char param1;
    unsigned char param2;
}msg_user_head_s;

#define MSG_USER_HEAD_LENGTH            (sizeof(msg_user_head_s))

typedef struct {
    msg_user_head_s hd;
    unsigned short resp_code;
    unsigned short crc_16;
}__attribute__((packed)) sec_common_msg_resp_s;

typedef struct tag_sec_module_cmd_s {
    unsigned char *s_buff;
    unsigned int  s_len;
    unsigned char *r_buff;
    unsigned int  r_len;
} sec_module_cmd_s;

#endif