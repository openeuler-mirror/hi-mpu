// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Public module bdinfo header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __BDINFO_H__
#define __BDINFO_H__

enum {
    BDINFO_HOST_PCHPROTO_LPC = 0,
    BDINFO_HOST_PCHPROTO_ESPI,
    BDINFO_HOST_PCHPROTO_UNKNOWN,
};

/* boot_cfg definition, corresponding to sysctrl register: INITIAL_INF_REG */
typedef union {
    struct {
        unsigned int single_chip : 1; /* [0] BMC primary/secondary indication. */
        unsigned int pcb_ver : 3;     /* [1..3] PCB version number. */
        unsigned int board_id : 8;    /* [4..11] board id. */
        unsigned int platform_id : 4; /* [12..15] platform id. */
        unsigned int ddr_size : 2;    /* [16..17] memory capacity. */
        unsigned int ddr_bits : 1;    /* [18] memory bus width mode. */
        unsigned int debug_gmac : 1;  /* [19] debug ethernet port indication. */
        unsigned int system_type : 1; /* [20] system type indication/VGA usage indication. */
        unsigned int ddr_ecc : 1;     /* [21] DDR ECC function configuration. */
        unsigned int is_espi : 1;     /* [22] eSPI/LPC function configuration. */
        unsigned int board_type : 1;  /* [23] card insertion indication. */
        unsigned int reserved : 8;    /* [24..31] */
    } bits;

    unsigned int val;
} boot_cfg_info_s;

/* configuration module io pin multiplexing information. */
extern s32 bdinfo_set_module_iocfg(u16 module_id);

/* get board_id. */
extern u32 bdinfo_get_board_id(void);

/* get platform_id. */
extern u32 bdinfo_get_platform_id(void);

/* get pcb version number. */
extern u32 bdinfo_get_pcb_ver(void);

/* get board type. */
extern u32 bdinfo_get_board_type(void);

extern s32 bdinfo_get_host_pchproto(void);

/* get board system type. */
extern u32 bdinfo_get_system_type(void);

/* get board boot_cfg pin information. */
extern u32 bdinfo_get_boot_cfg_info(void);

extern s32 bdinfo_get_ioreg_val_by_index(u16 module_id, u32 reg_index, u32 *val);

extern s32 bdinfo_iocfg_check(u16 module_id);

extern s32 bdinfo_cfg_init(void);

extern s32 bdinfo_cfg_exit(void);

extern s32 bdinfo_cur_board_moduleid_is_used(u32 module_id);

#endif /* __BDINFO_H__ */

