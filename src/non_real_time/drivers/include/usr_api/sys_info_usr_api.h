// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sys_info user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _SYS_INFO_USR_APIH_
#define _SYS_INFO_USR_APIH_

#define DEV_SYS_INFO_NAME "/dev/sys_info"

typedef struct tar_mem_info {
    unsigned int start;
    unsigned int size;
}mem_info_s;

#define SYS_INFO_IOC_MAGIC 's'
#define SYS_INFO_GET_CHIP_TEMP              _IOR(SYS_INFO_IOC_MAGIC, 0, int)
#define SYS_INFO_GET_RESET_TYPE             _IOR(SYS_INFO_IOC_MAGIC, 1, int)
#define SYS_INFO_GET_SYSTEM_JIFFIES         _IOR(SYS_INFO_IOC_MAGIC, 2, int)
#define SYS_INFO_GET_STARTUP_TIMES          _IOR(SYS_INFO_IOC_MAGIC, 3, int)
#define SYS_INFO_CLEAN_STARTUP_TIMES        _IOR(SYS_INFO_IOC_MAGIC, 4, int)
#define SYS_INFO_GET_VERSION                _IOR(SYS_INFO_IOC_MAGIC, 5, int)
#define SYS_INFO_GET_BOARD_CFG_INFO         _IOR(SYS_INFO_IOC_MAGIC, 6, int)
#define SYS_INFO_SET_PCIE                   _IOR(SYS_INFO_IOC_MAGIC, 7, int)
#define SYS_INFO_SET_SERDES                 _IOR(SYS_INFO_IOC_MAGIC, 8, int)
#define SYS_INFO_GET_SERDES                 _IOR(SYS_INFO_IOC_MAGIC, 9, int)
#define SYS_INFO_GET_DDR_TEST_RET           _IOR(SYS_INFO_IOC_MAGIC, 10, int)
#define SYS_INFO_GET_SYSTEM_TYPE            _IOR(SYS_INFO_IOC_MAGIC, 15, int)
#define SYS_INFO_GET_APP_RESERVED_DDR       _IOR(SYS_INFO_IOC_MAGIC, 16, mem_info_s)
#define SYS_INFO_GET_PCIE_DDR               _IOR(SYS_INFO_IOC_MAGIC, 17, mem_info_s)
#define SYS_INFO_GET_SERDES_STATUS          _IOR(SYS_INFO_IOC_MAGIC, 18, serdes_status)
#define SYS_INFO_SET_VGA_DAC_CONFIG         _IOW(SYS_INFO_IOC_MAGIC, 19, int)
#define SYS_INFO_GET_RSVMEM_FREE_SUPPORT    _IOR(SYS_INFO_IOC_MAGIC, 20, int)
#define SYS_INFO_GET_PRODUCT_TYPE           _IOR(SYS_INFO_IOC_MAGIC, 21, int)
#define SYS_INFO_GET_BMC_RESET_TYPE         _IOR(SYS_INFO_IOC_MAGIC, 22, int)
#define SYS_INFO_GET_CHIP_RESET_TYPE        _IOR(SYS_INFO_IOC_MAGIC, 23, int)
#define SYS_INFO_GET_HOST_RESET_TYPE        _IOR(SYS_INFO_IOC_MAGIC, 24, int)
#define SYS_INFO_GET_UNSC_BMC_RESET_TYPE    _IOR(SYS_INFO_IOC_MAGIC, 25, int)
#define SYS_INFO_GET_CHIP_VERSION           _IOR(SYS_INFO_IOC_MAGIC, 26, int)
#define SYS_INFO_CMD_MAXNR 26

/* Querying the Reset Reason of the Reset Domain */
#define RST_TYPE_NO_RST             0x0
#define RST_TYPE_PAD_RST            0x1
#define RST_TYPE_WDT_RST            0x2
#define RST_TYPE_SOFTWARE_RST       0x3
#define RST_TYPE_EMPTY_TIMEOUT_RST  0x4

#define BMC_SYSTEM 0
#define SINGLE_CHIP_SYSTEM  1

#define SDK_VER_LEN 64

typedef struct sdk_ver_stru {
    char version[SDK_VER_LEN];
    char build_time[SDK_VER_LEN];
}sdk_ver_s;


typedef struct pcie_set_stru {
    unsigned int flag;
    unsigned char rsv;
}pcie_set_s;

/* board info definition */
typedef union {
    struct {
        unsigned int single_chip            : 1;
        unsigned int pcb_ver                : 3;
        unsigned int board_id               : 8;
        unsigned int platform_id            : 8;
        unsigned int ddr_size               : 2;
        unsigned int ddr_bits               : 1;
        unsigned int debug_gmac             : 1;
        unsigned int system_type            : 1;
        unsigned int ddr_ecc                : 1;
        unsigned int is_espi                : 1;
        unsigned int reserved               : 5; /* [27..31] */
    } bits;
    unsigned int val;
} board_info_s;

/*  serdes config value */
typedef struct {
    unsigned int id;
    unsigned int tx_val;
    unsigned int rx_val;
} serdes_info_s;

/* SerDes status information */
typedef struct tar_serdes_status {
    unsigned char pcie0_serdes_status;
    unsigned char pcie1_serdes_status;
    unsigned char gmac0_serdes_status;
    unsigned char gmac1_serdes_status;
} serdes_status;

/* tsensor 获取芯片温度 */
typedef struct {
    unsigned int high_temp; /* High temperature check value */
    unsigned int normal_temp; /* Normal temperature check value */
    unsigned int temp_value; /* Temperature value in the register */
} tsensor_info_s;

struct chip_version_info {
    unsigned int chip_name;
    unsigned int chip_version;
};

int sys_info_get_system_type(unsigned int *type);
int sys_info_get_edma_ddr_info(mem_info_s *info);

#endif
