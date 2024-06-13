// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * trng driver heade
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _TRNG_TEST_H_
#define _TRNG_TEST_H_

#include "comm_knl_api.h"

#define LPSC_RESET_REQ2   0xc
#define LPSC_RESET_DREQ2  0x204
#define LPSC_RESET_ST2    0x210
#define LPSC_ECC_INT_RAW  0x320
#define LPSC_ECC_INT_MASK 0x324

#define LP_TRNG_OUTPUT_0_REG   0x0
#define LP_TRNG_OUTPUT_1_REG   0x4
#define LP_TRNG_OUTPUT_2_REG   0x8
#define LP_TRNG_OUTPUT_3_REG   0xC
#define LP_TRNG_INPUT_0_REG    0x0
#define LP_TRNG_INPUT_1_REG    0x4
#define LP_TRNG_INPUT_2_REG    0x8
#define LP_TRNG_INPUT_3_REG    0xC
#define LP_TRNG_STATUS_REG     0x10
#define LP_TRNG_INTACK_REG     0x10
#define LP_TRNG_CONTROL_REG    0x14
#define LP_TRNG_CONFIG_REG     0x18
#define LP_TRNG_ALARMCNT_REG   0x1C
#define LP_TRNG_FROENABLE_REG  0x20
#define LP_TRNG_FRODETUNE_REG  0x24
#define LP_TRNG_ALARMMASK_REG  0x28
#define LP_TRNG_ALARMSTOP_REG  0x2C
#define LP_TRNG_RAW_L_REG      0x30
#define LP_TRNG_RAW_H_REG      0x34
#define LP_TRNG_SPB_TESTS_REG  0x38
#define LP_TRNG_COUNT_REG      0x3C
#define LP_TRNG_COND_0_REG     0x40
#define LP_TRNG_COND_1_REG     0x44
#define LP_TRNG_COND_2_REG     0x48
#define LP_TRNG_COND_3_REG     0x4C
#define LP_TRNG_COND_4_REG     0x50
#define LP_TRNG_COND_5_REG     0x54
#define LP_TRNG_COND_6_REG     0x58
#define LP_TRNG_COND_7_REG     0x5C
#define LP_TRNG_KEY_0_REG      0x40
#define LP_TRNG_KEY_1_REG      0x44
#define LP_TRNG_KEY_2_REG      0x48
#define LP_TRNG_KEY_3_REG      0x4C
#define LP_TRNG_KEY_4_REG      0x50
#define LP_TRNG_KEY_5_REG      0x54
#define LP_TRNG_KEY_6_REG      0x58
#define LP_TRNG_KEY_7_REG      0x5C
#define LP_TRNG_PS_AI_0_REG    0x40
#define LP_TRNG_PS_AI_1_REG    0x44
#define LP_TRNG_PS_AI_2_REG    0x48
#define LP_TRNG_PS_AI_3_REG    0x4C
#define LP_TRNG_PS_AI_4_REG    0x50
#define LP_TRNG_PS_AI_5_REG    0x54
#define LP_TRNG_PS_AI_6_REG    0x58
#define LP_TRNG_PS_AI_7_REG    0x5C
#define LP_TRNG_PS_AI_8_REG    0x60
#define LP_TRNG_PS_AI_9_REG    0x64
#define LP_TRNG_PS_AI_10_REG   0x68
#define LP_TRNG_PS_AI_11_REG   0x6C
#define LP_TRNG_RUN_CNT_REG    0x40
#define LP_TRNG_RUN_1_REG      0x44
#define LP_TRNG_RUN_2_REG      0x48
#define LP_TRNG_RUN_3_REG      0x4C
#define LP_TRNG_RUN_4_REG      0x50
#define LP_TRNG_RUN_5_REG      0x54
#define LP_TRNG_RUN_6_UP_REG   0x58
#define LP_TRNG_MONOBITCNT_REG 0x5C
#define LP_TRNG_POKER_3_0_REG  0x60
#define LP_TRNG_POKER_7_4_REG  0x64
#define LP_TRNG_POKER_B_8_REG  0x68
#define LP_TRNG_POKER_F_C_REG  0x6C
#define LP_TRNG_TEST_REG       0x70
#define LP_TRNG_BLOCKCNT_REG   0x74
#define LP_TRNG_OPTIONS_REG    0x78
#define LP_TRNG_EIP_REV_REG    0x7C
#define LP_TRNG_CTRL0_REG      0xC0
#define LP_TRNG_CTRL1_REG      0xC4
#define LP_RNG_CTRL_REG        0xCC
#define LP_RNG_SEED_REG        0xD0
#define LP_TRNG_FSM_ST_REG     0xD4
#define LP_RNG_NUM_REG         0xD8
#define LP_RNG_PHY_SEED_REG    0xDC
#define LP_RNG_ERR_REG         0xE0
#define LP_TRNG_INT_SET_REG    0xE4
#define LP_TRNG_RAN_DATA0_REG  0xF0
#define LP_TRNG_RAN_DATA1_REG  0xF4
#define LP_TRNG_RAN_DATA2_REG  0xF8
#define LP_TRNG_RAN_DATA3_REG  0xFC

#define TRNG_COM_ST_TMOUT 5000 /* 5s, msleep in cycle */
#define TRNG_RANDOM_TMOUT 5000 /* 5s, msleep in cycle */

#define TRNG_ERROR_INT_NUM 7

#define TRNG_WORD_BYTE_CNT 4

typedef enum RANDOM_GROUP_IDX {
    RANDOM_0 = 0,
    RANDOM_1 = 1,
    RANDOM_2 = 2,
    RANDOM_3 = 3,
    RANDOM_GRP_CNT = 4,
}random_group_idx_e;

/* lp subsys trng ram ecc */
#define TRNG_RAM_2BIT_ERR BIT(0)
#define TRNG_RAM_1BIT_ERR BIT(1)

#define TRNG_MODE               BIT(0)
#define TRNG_SEC_INTFS_DISABLE  BIT(1)
#define TRNG_RSA_INTFS_DISABLE  BIT(2)
#define TRNG_JTAG_INTFS_DISABLE BIT(3)
#define TRNG_USR_INTFS_DISABLE  BIT(4)
#define TRNG_AUTO_RESEED_ENABLE BIT(5)

#define TRNG_SAMPLE_CYCLES     (0xFFFFU << 16)
#define TRNG_READ_TIMTOUT      (0xFU << 12)
#define TRNG_SAMPLE_DIV        (0xFU << 8)
#define TRNG_NOISE_BLOCKS      (0xFFU << 0)

#define TRNG_RAM_2BIT_ERR_RST_CNT  5 /* after 5 times 2bit ecc occur, reset trng */
#define TRNG_RAM_RST_MOD_INVAL_CNT 3 /* after 3 times reset trng cause 2bit ecc occur, means module invalid */
#define TRNG_RST_ERR_CNT           100
#define TRNG_RST_ERR_CHK_PERIOD    (5 * 60 * 1000) /* defined as ms.
                                                    * if some error occur over 100-times in 5 minutes, reset trng
                                                    */

#define TRNG_INT_READY         BIT(0)
#define TRNG_INT_SHUTDOWN_OFLO BIT(1)
#define TRNG_INT_STUCK_OUT     BIT(2)
#define TRNG_INT_NOISE_FAIL    BIT(3)
#define TRNG_INT_RUN_FAIL      BIT(4)
#define TRNG_INT_LRUN_FAIL     BIT(5)
#define TRNG_INT_POKER_FAIL    BIT(6)
#define TRNG_INT_MONOBIT_FAIL  BIT(7)
#define TRNG_TEST_READY_MODE   BIT(8)
#define TRNG_INT_STUCK_NRBG    BIT(9)
#define TRNG_OPEN_READ_GATE    (0x7 << 10)
#define TRNG_TRNG_ENABLE       BIT(10)
#define TRNG_DRBG_ENABLE       BIT(12)
#define TRNG_INT_REPCNT_FAIL   BIT(13)
#define TRNG_INT_APROP_FAIL    BIT(14)
#define TRNG_RE_SEED           BIT(15)
#define TRNG_REQUEST_DATA      BIT(16)
#define TRNG_DATA_BLOCKS       (0xFFFU << 20)
#define TRNG_ERROR_INT         (TRNG_INT_SHUTDOWN_OFLO | TRNG_INT_RUN_FAIL | TRNG_INT_LRUN_FAIL \
                                 | TRNG_INT_POKER_FAIL | TRNG_INT_MONOBIT_FAIL | TRNG_INT_STUCK_NRBG \
                                 | TRNG_INT_REPCNT_FAIL | TRNG_INT_APROP_FAIL | TRNG_INT_STUCK_OUT \
                                 | TRNG_INT_NOISE_FAIL)


/* Define the union u_trng_ctrl0 */
typedef union {
    /* Define the struct bits */
    struct {
        u32 trng_mode : 1;                  /* [0] */
        u32 sec_interface_disable : 1;      /* [1] */
        u32 rsa_interface_disable : 1;      /* [2] */
        u32 jtagauth_interface_disable : 1; /* [3] */
        u32 user_interface_disable : 1;     /* [4] */
        u32 auto_reseed_enable : 1;         /* [5] */
        u32 reserved0 : 1;                  /* [6] */
        u32 reserved1 : 1;                  /* [7] */
        u32 mem_ctrl : 8;                   /* [15..8] */
        u32 block_cnt : 12;                 /* [27..16] */
        u32 reserved2 : 4;                  /* [31..28] */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_trng_ctrl0;

/* Define the union u_trng_control */
typedef union {
    /* Define the struct bits */
    struct {
        u32    ready_mask            : 1;  /* [0]  */
        u32    shutdown_oflo_mask    : 1;  /* [1]  */
        u32    stuck_out_mask        : 1;  /* [2]  */
        u32    noise_fail_mask       : 1;  /* [3]  */
        u32    run_fail_mask         : 1;  /* [4]  */
        u32    long_run_fail_mask    : 1;  /* [5]  */
        u32    poker_fail_mask       : 1;  /* [6]  */
        u32    monobit_fail_mask     : 1;  /* [7]  */
        u32    test_mode             : 1;  /* [8]  */
        u32    stuck_nrbg_mask       : 1;  /* [9]  */
        u32    enable_trng           : 1;  /* [10]  */
        u32    reserved0             : 1;  /* [11]  */
        u32    drbg_en               : 1;  /* [12]  */
        u32    repcnt_fail_mask      : 1;  /* [13]  */
        u32    aprop_fail_mask       : 1;  /* [14]  */
        u32    re_seed               : 1;  /* [15]  */
        u32    request_data          : 1;  /* [16]  */
        u32    reserved1             : 3;  /* [19..17]  */
        u32    data_blocks           : 12; /* [31..20]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_trng_control;

/* Define the union u_trng_status */
typedef union {
    /* Define the struct bits */
    struct {
        u32    ready                 : 1; /* [0]  */
        u32    shutdown_oflo         : 1; /* [1]  */
        u32    stuck_out             : 1; /* [2]  */
        u32    noise_fail            : 1; /* [3]  */
        u32    run_fail              : 1; /* [4]  */
        u32    long_run_fail         : 1; /* [5]  */
        u32    poker_fail            : 1; /* [6]  */
        u32    monobit_fail          : 1; /* [7]  */
        u32    test_ready            : 1; /* [8]  */
        u32    stuck_nrbg            : 1; /* [9]  */
        u32    reserved_0            : 3; /* [12..10]  */
        u32    repcnt_fail           : 1; /* [13]  */
        u32    aprop_fail            : 1; /* [14]  */
        u32    test_stuck_out        : 1; /* [15]  */
        u32    blocks_available      : 8; /* [23..16]  */
        u32    reserved_1            : 7; /* [30..24]  */
        u32    need_clock            : 1; /* [31]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_trng_status;

/* Define the union u_trng_intack */
typedef union {
    /* Define the struct bits */
    struct {
        u32    ready_ack             : 1;  /* [0]  */
        u32    shutdown_oflo_ack     : 1;  /* [1]  */
        u32    stuck_out_ack         : 1;  /* [2]  */
        u32    noise_fail_ack        : 1;  /* [3]  */
        u32    run_fail_ack          : 1;  /* [4]  */
        u32    long_run_fail_ack     : 1;  /* [5]  */
        u32    poker_fail_ack        : 1;  /* [6]  */
        u32    monobit_fail_ack      : 1;  /* [7]  */
        u32    test_ready_ack        : 1;  /* [8]  */
        u32    stuck_nrbg_ack        : 1;  /* [9]  */
        u32    open_read_gate        : 3;  /* [12..10]  */
        u32    repcnt_fail_ack       : 1;  /* [13]  */
        u32    aprop_fail_ack        : 1;  /* [14]  */
        u32    test_stuck_out        : 1;  /* [15]  */
        u32    reserved              : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_trng_intack;

/* Define the union u_trng_config */
typedef union {
    /* Define the struct bits */
    struct {
        u32    noise_blocks          : 8;  /* [7..0]  */
        u32    sample_div            : 4;  /* [11..8]  */
        u32    read_timeout          : 4;  /* [15..12]  */
        u32    sample_cycles         : 16; /* [31..16]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_trng_config;

typedef union {
    /* Define the struct bits */
    struct {
        u32    test_en_out           : 1;  /* [0]  */
        u32    test_patt_fro         : 1;  /* [1]  */
        u32    test_patt_det         : 1;  /* [2]  */
        u32    test_shiftreg         : 1;  /* [3]  */
        u32    cont_poker            : 1;  /* [4]  */
        u32    test_known_noise      : 1;  /* [5]  */
        u32    test_aes_256          : 1;  /* [6]  */
        u32    test_sp_800_90        : 1;  /* [7]  */
        u32    test_select           : 5;  /* [12..8]  */
        u32    test_noise            : 1;  /* [13]  */
        u32    test_spb              : 1;  /* [14]  */
        u32    test_cond_func        : 1;  /* [15]  */
        u32    test_pattern          : 12; /* [27..16]  */
        u32    fro_testin2_not       : 1;  /* [28]  */
        u32    fro_testin3           : 1;  /* [29]  */
        u32    fro_testin4           : 1;  /* [30]  */
        u32    test_irq              : 1;  /* [31]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} u_trng_test;

#define LPSC_TRNG_BIT BIT(16)

#define DEV_TRNG_NAME "trng"
#define LP_TRNG_IOC_MAGIC 't'
#define LP_TRNG_RANDOM    _IOWR(LP_TRNG_IOC_MAGIC, 3, trng_rand_s)
#define LP_TRNG_RESET     _IOWR(LP_TRNG_IOC_MAGIC, 4, int)

#define SEED_DATA_WORD_LEN (12)

#define RANDOM_GRP_NUM (4)
#define RANDOM_MAX_CNT (256)

typedef union tag_trng_test_args {
    struct {
        u32 user_flag;
        u32 data_bytes;
        u8 *buffer;
    } get_random;

    struct {
        u32 noise_data_num;
        u64 *buffer;
    } get_noise;

    struct {
        u32 init_flag;
        u32 arg1;
        u32 arg2;
    } test_cmd;

    struct {
        u32 data_num;
        u32 *data;
    } re_seed;

    u8 buffer[0];
} trng_test_args_u;

typedef struct tag_trng_rand_s {
    u32 *rand_data;
    u32 cnt;
    u32 reserved[2];
} trng_rand_s;

extern s32 trng_get_random_user(u32 *data, u32 random_nums);

#endif
