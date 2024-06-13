/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x uboot DTB cfg fixing interfaces and macros header file
 * Create: 2018-05-10
 */
#ifndef __FDT_FIX_H__
#define __FDT_FIX_H__

#define FDT_L0FW_VER_NAME "l0fw_ver"
#define FDT_L1FW_VER_NAME "l1fw_ver"
#define FDT_UBOOT_VER_NAME "uboot_ver"
#define FDT_HOST "host"
#define FDT_PCH_PROTO_DESC "pchproto_description"
#define FDT_PCH_PROTO_VAL "pchproto_value"
#define FDT_NVM_PART_NAME "nvm_part"
#define FDT_SECURE_ENABLE "secure_enable"

/* Secure firmware inter-core parameters for uboot */
#define DDR_A55_CONN_PARAM_ADDR (0x87052000) /* 0x87052000, 1KB, M3 boot parameters for A55 */
#define DDR_A55_CONN_PARAM_SIZE (0x400)

#define SECFW_VER_LEN (32)
#define SECFW_BUILD_TIME_LEN (32)
#define FW_VER_LEN (4)        /* Version len(e.g: 5 0 1 1) */
#define FW_BUILD_TIME_LEN (6) /* Build time len(e.g: 19 08 12 16 30 02) */
#define FW_VER_BUILD_TIME_LEN (FW_VER_LEN + FW_BUILD_TIME_LEN)
#define FW_VER_PARAM_CNT (6) /* L0/L1/UBOOT active & gold version/build time info(6) */
#define FW_VER_PARAM_LEN (FW_VER_BUILD_TIME_LEN * FW_VER_PARAM_CNT)

struct fw_ver_build_info {
    unsigned char l0fw_ver_active[FW_VER_LEN];
    unsigned char l0fw_build_time_active[FW_BUILD_TIME_LEN];
    unsigned char l0fw_ver_gold[FW_VER_LEN];
    unsigned char l0fw_build_time_gold[FW_BUILD_TIME_LEN];
    unsigned char l1fw_ver_active[FW_VER_LEN];
    unsigned char l1fw_build_time_active[FW_BUILD_TIME_LEN];
    unsigned char l1fw_ver_gold[FW_VER_LEN];
    unsigned char l1fw_build_time_gold[FW_BUILD_TIME_LEN];
    unsigned char uboot_ver_active[FW_VER_LEN];
    unsigned char uboot_build_time_active[FW_BUILD_TIME_LEN];
    unsigned char uboot_ver_gold[FW_VER_LEN];
    unsigned char uboot_build_time_gold[FW_BUILD_TIME_LEN];
};

/* Parameters for inter-core processing(l1fw to uboot) */
typedef struct secfw_inter_core_param {
    /* Versions & build time */
    union {
        unsigned char raw_param[FW_VER_PARAM_LEN];
        struct fw_ver_build_info ver_param;
    };

    /* Add more param here */
} secfw_inter_core_param_s;

typedef union {
    /* Define the struct bits */
    struct {
        unsigned int gmac0_phy_addr : 8; /* [7..0]   RMII0 PHY Addr */
        unsigned int gmac1_phy_addr : 8; /* [15..8]  RMII1 PHY Addr */
        unsigned int gmac2_phy_addr : 8; /* [23..16] SGMII0 PHY Addr */
        unsigned int gmac3_phy_addr : 8; /* [31..24] SGMII1 PHY Addr */
    } bits;

    /* Define an unsigned member */
    unsigned int u32;
} gmac_phy_addr_info_u;

extern unsigned long get_ramdisk_size(void);
#endif /* __FDT_FIX_H__ */
