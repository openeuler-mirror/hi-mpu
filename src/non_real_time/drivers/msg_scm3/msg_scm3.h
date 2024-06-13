// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * msg secure m3 header file
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _MSG_SCM3_H_
#define _MSG_SCM3_H_

/* IO ctrl error type */
typedef enum {
    MSG_SUCCESS = 0,
    MSG_TX_LEN_OVERFLOW_ERROR,                   /* no more than 4KB */
    MSG_TX_LEN_UNDERFLOW_ERROR,                  /* no less than 6B */
    MSG_RX_MSG_OVERFLOW_ERROR,                   /* MSG len exceed RX len */
    MSG_RX_LEN_UNDERFLOW_ERROR,                  /* no less than 8B */
    MSG_TX_TIME_OUT_ERROR = 5,                   /* TX time out for queue full */
    MSG_RX_TIME_OUT_ERROR,                       /* RX time out for waiting long time */
    MSG_TX_BUFFER_NULL_ERROR,
    MSG_RX_BUFFER_NULL_ERROR,
    MSG_GET_DATA_BUFFER_ERROR,                   /* get data buffer for core communication error */
    MSG_CP_FROM_USER_CMD_ERROR = 10,
    MSG_CP_FROM_USER_DATA_ERROR,
    MSG_CP_TO_USER_CMD_ERROR,
    MSG_CP_TO_USER_DATA_ERROR,
    MSG_IO_CMD_NOT_SUPPORT,
    MSG_COMM_INTERNAL_ERROR,
}sec_msg_error_code_e;

/* Message queue type */
typedef enum {
    MSG_Q_TYPE_A55 = 0,
    MSG_Q_TYPE_SECURE_M3,
    MSG_Q_TYPE_BUTT
}msg_queue_type_e;

typedef struct {
    u64 data_addr;
    u32 data_len;
    u16 msg_index;
    u16 msg_crc;
}__attribute__((packed)) msg_comm_data_s;

/* Message command word definition */
#define MSG_CMD_LEN                 (sizeof(msg_comm_data_s))
#define MSG_CMD_CRC_LEN             2
#define MSG_DATA_MAX_SIZE           4096
#define MSG_DATA_TX_MIN_SIZE        6
#define MSG_DATA_RX_MIN_SIZE        8


#define MSG_Q_DEPTH                 (16)
#define MSG_Q_SIZE                  (MSG_Q_DEPTH * sizeof(msg_comm_data_s))
#define MSG_A55_M3_INT_TRIGGER      (1)

extern void msg_dfx_dump(void);

#endif /* _MSG_H_ */