// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sol user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _SOL_USR_API_H_
#define _SOL_USR_API_H_

#define DEV_SOL_NAME                "/dev/sol"
#define SOL_IOC_MAGIC               's'
#define SOL_CMD_START               _IOW(SOL_IOC_MAGIC, 0, int)
#define SOL_CMD_READ                _IOR(SOL_IOC_MAGIC, 1, int)
#define SOL_CMD_GET_REGS            _IOR(SOL_IOC_MAGIC, 2, int)
#define SOL_CMD_SET_START_SIZE      _IOW(SOL_IOC_MAGIC, 3, int)
#define SOL_CMD_TX_POS              _IOR(SOL_IOC_MAGIC, 7, sol_data_pos_s)
#define SOL_CMD_MAXNR               7

typedef struct {
    u32 enable : 1;
    u32 index : 3; /* Value range: 0~3 */
    u32 reserved : 28;
} sol_enable_s, *psol_enable_s;

typedef struct {
    u32 cur : 1; /* 0: Obtain the data sent by the host. 1: Obtain the data received by the host. */
    u32 index : 3; /* Value range: 0~3 */
    u32 reserved : 28;
    u8 *p_read_buff;
    u32 length; /* <=2MByte, the length must be a multiple of 4 bytes. */
} sol_data_read_s, *psol_data_read_s;

typedef struct tag_sol_data_pos_s {
    u32 index;
    u32 ah;
    u32 pos_start;
    u32 pos_offset;
} sol_data_pos_s, *psol_data_pos_s;

typedef struct {
    u32 index; /* Sol index. The value ranges from 0 to 3. */
    u32 start;
    u32 tx_add;
    u32 tx_size;
    u32 rx_add;
    u32 rx_size;
    u32 tx_now;
    u32 rx_now;
    u32 fifo_state;
    u32 err_times;
    u32 irq;
    u32 re_times;
}sol_reg_val_s, *psol_reg_val_s;

typedef struct {
    u32 index;  /* Value range: 0ex, ~3 */
    u32 reserved;
    u32 ah; /* Upper 32 bits of the start address */
    u32 rx_start_al; /* Lower 32 bits of the RX start address, which must be 32-byte aligned */
    u32 tx_start_al; /* Lower 32 bits of the TX start address, which must be 32-byte aligned */
    u32 rx_size; /* RX size. The value ranges from 0 to 7, corresponding to 0.25M/0.5M/1M/2M/4M/8M/16M/32M. */
    u32 tx_size; /* TX size, ranging from 0 to 7, corresponding to 0.25M/0.5M/1M/2M/4M/8M/16M/32M */
} sol_start_size_s, *psol_start_size_s;

#endif /* _SOL_USR_API_H_ */

