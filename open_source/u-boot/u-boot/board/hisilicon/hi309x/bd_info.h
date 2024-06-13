/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2020. All rights reserved.
* Description: none
* Create: 2018-08-01
* Notes: none
*/

#ifndef __BD_INFO_H__
#define __BD_INFO_H__

/* 单板类型 */
#define BOARD_TYPE_HOST 0x0   /* 贴片主板:非插卡形态 */
#define BOARD_TYPE_BUCKLE 0x1 /* 插卡形态 */

/* 单板硬件平台编码 */
#define BOARD_PLATM_ATLAS 0x2      /* ATLAS平台 */
#define BOARD_PLATM_PANGEA_V5 0x5  /* 盘古V3/V5平台 */
#define BOARD_PLATM_TCE 0x6        /* TCE平台 */
#define BOARD_PLATM_TAISHAN 0x7    /* TaiShan机架服务器平台 */
#define BOARD_PLATM_HIGH_SERVE 0x8 /* x86服务器平台 */
#define BOARD_PLATM_CHH      0x9   /* 星光服务器平台 */
#define BOARD_PLATM_PANGEA_V6 0xd  /* 盘古V6平台 */
#define BOARD_PLATM_BUCKLE_NEW 0xe /* 插卡BMC，现行最新方案使用 */
#define BOARD_PLATM_BUCKLE 0xf     /* 插卡BMC */
#define BOARD_PLATM_SKYLAKE_BUCKLE 0x0   /* 天池架构插卡BMC */

/* 插卡子平台编码 */
#define BOARD_PLATM_BUCKLE_X_SERIES 0x3 /* X系列服务器 */
#define BOARD_PLATM_BUCKLE_OSCA 0x4     /* OSCA平台 */
#define BOARD_PLATM_BUCKLE_PANGEA 0x5   /* 盘古平台 */
#define BOARD_PLATM_BUCKLE_TAISHAN 0x7  /* TaiShan机架服务器平台 */
#define BOARD_PLATM_BUCKLE_HIGH 0x8     /* 高性能服务器 */
#define BOARD_PLATM_BUCKLE_RH 0xf       /* RH服务器 */

#define BOARD_SUB_PLATM_ID_OLD          0x3   /* 区分新老方案插卡子平台，子平台ID为3时为老方案 */

/* 星光平台(0x9)下的单板ID */
#define CHH_BOARD_ID_CHH1FPFMBA         0xBA
#define CHH_BOARD_ID_CHH1SPMNTA         0x1B

/* EVB单板ID编码 */
#define EVB_BOARD_ID 0x78 /* EVB单板 */
#define EVB_V100_PCB_VERSION            0x3
#define EVB_V110_PCB_VERSION            0x5

/* TaiShan机架服务器平台(0x7)下的单板ID编码 */
#define TAISHAN_BOARD_ID_2480 0x1  /* TaiShan2480服务器 */
#define TAISHAN_BOARD_ID_5290 0x4  /* TaiShan5290服务器 */
#define TAISHAN_BOARD_ID_2280E 0x5 /* TaiShan2280E服务器 */
#define FUSION_RM210   0x6d        /* FusionRM */
#define RM211_IT31RMUC 0x6c        /* RM211机型管理板 */

/* TCE平台下的单板ID编码 */
#define TCE_BOARD_ID_GPUA               0x30 /* 计算板2280E 5290 */
#define TCE_BOARD_ID_SMMA               0x08 /* 管理板 */
#define TCE_BOARD_ID_CN90BMCBB          0x09  /* 1.1管理板 */
#define TCE_BOARD_ID_CN90BMCBBA         0x0A  /* TCE 4u/8u管理板扣板 */
#define TCE_BOARD_ID_CN90GPUB           0x31  /* 1.1计算板 */
#define TCE_BOARD_ID_CN91SPUA           0x32  /* 2.0计算板 */
#define TCE_BOARD_ID_CN90BMCC           0x51  /* X86_BMC扣卡 */
#define TCE_BOARD_ID_CN90GPUD           0x52  /* TCE 2.0 X86 GPU 底板 */
#define TCE_BOARD_ID_CN90GPUE           0x53  /* TCE 22.0 X86 GPU 底板 */
#define TCE_BOARD_ID_CN90GPUB_VB        0x33  /* TCE TCE 4U/8U ARM主板单板 */

/* ATLAS平台下的单板ID编码 */
#define ATLAS_800_BOARD_ID_9000 		0x92 /* Atlas800-9000服务器 */
#define ATLAS_800_BOARD_ID_9010 		0x52 /* Atlas800-9010服务器 */
#define ATLAS_500_IT21EKMA_BOARD_ID    	0x94 /* Atlas500服务器 */

/* x86服务器平台下的单板ID编码 */
#define HAIYAN_BOARD_ID_5 0x4 /* 海燕5.0服务器 */

/* 盘古V6平台下的单板ID编码 */
#define PANGEA_V6_BOARD_ID_STL6TSPCE_SAS    0x12  /* 盘古C+中端SAS */
#define PANGEA_V6_BOARD_ID_STL6TSPCE_PALM   0x14  /* 盘古C+中端PALM */
#define PANGEA_V6_BOARD_ID_STL6SPCD         0x11  /* 盘古C+高端4P */
#define PANGEA_V6_BOARD_ID_STL6EXCN         0x21  /* 盘古V6大西洋交换板 */
#define PANGEA_V6_BOARD_ID_MANAGEBOARD      0x81  /* 盘古C+高端4P管理板 */
#define PANGEA_V6_BOARD_ID_STL6TSPCM_PACI   0x17  /* 盘古V6太平洋 */
#define PANGEA_V6_BOARD_ID_STL6TSP_PACI_T   0x2B  /* 盘古V6太平洋快板 */
#define PANGEA_V6_BOARD_ID_STL6TSPCM_ARCT   0x18  /* 盘古V6北冰洋 */
#define PANGEA_V6_BOARD_ID_STL6TSP_ARCT_T   0x28  /* 盘古V6北冰洋快板 */
#define PANGEA_V6_BOARD_ID_STL6TSPCM_ATLA   0x1B  /* 盘古V6大西洋 */
#define PANGEA_V6_BOARD_ID_STL6ICBP         0x1E  /* 盘古V6北冰洋共享卡 */
#define PANGEA_V6_BOARD_ID_STL6DESC_SAS     0x24  /* 盘古C+低端SAS */
#define PANGEA_V6_BOARD_ID_STL6DESC_PALM    0x27  /* 盘古C+低端PALM */
#define PANGEA_V6_BOARD_ID_STL6SPCI         0x22  /* 盘古C+低端控制板 */
#define PANGEA_V6_BOARD_ID_STL6SPCK         0x1D  /* 盘古C+低端NVMe控制板 */
#define PANGEA_V6_BOARD_ID_STL6EXCN01       0x30  /* 盘古V6大西洋直通板 */
#define PANGEA_V6_BOARD_ID_STL6SPCQ_T       0x2D  /* 盘古C+ X86 NVMe快板 */
#define PANGEA_V6_BOARD_ID_STL6SPCQ         0x35  /* 盘古C+ X86 NVMe正式板 */
#define PANGEA_V6_BOARD_ID_STL6SPCO         0x90  /* 盘古V6 X86 北冰洋 fusion cube */
#define PANGEA_V6_BOARD_ID_STL6SPCO12       0x93  /* 盘古V6 X86 北冰洋 机器视觉 */
#define PANGEA_V6_BOARD_ID_STL6SPCQ10       0x2E  /* 盘古C+ X86 SAS 1P */
#define PANGEA_V6_BOARD_ID_STL6SPCQ20       0x33  /* 盘古C+ X86 SAS 2P */
#define PANGEA_V6_BOARD_ID_STL6SPCJ         0x71  /* 盘古c+高端2P */
#define PANGEA_V6_BOARD_ID_STL6MGTD10       0x82  /* 盘古C+高端2P管理板 */
#define PANGEA_V6_BOARD_ID_STL6TSPC_TEST    0x9a  /* 大西洋/C+ 高端控制单元测试板 */
#define PANGEA_V6_BOARD_ID_BC13MBSBF2       0x9b  /* 计算型存储全宽计算x86主板2号 */
#define PANGEA_V6_BOARD_ID_BC13MBSBF        0x9c  /* 计算型存储全宽计算x86主板 */
#define PANGEA_V6_BOARD_ID_BC82ADDRF        0x9f  /* 计算型存储全宽计算arm主板 */
#define PANGEA_V6_BOARD_ID_STL6ICBA01       0xa3  /* XPU + 管理板 */
#define PANGEA_V6_BOARD_ID_STL6ICBA02       0xaf  /* 计算存储管理板 */

/* OSCA平台下单板ID编码 */
#define OSCA_E9000_V6_IT11SWCA 0x3D /* E9000_V6 121_V6刀片服务器 */

#define X_SERIES_X6000_v6_BC23SPSA      0XB5
#define X_SERIES_X6000_v6_BC23SPSB      0XB6
/* PCB通用版本号 */
#define PCB_COMMON_VER0 0x0 /* 通用pcb_ver: 0 */
#define PCB_COMMON_VER1 0x1 /* 通用pcb_ver: 1 */
#define PCB_COMMON_VER2 0x2 /* 通用pcb_ver: 2 */
#define PCB_COMMON_VER5 0x5 /* EVB V110单板 pcb_ver: 5 */

/* VCE */
#define UNUSE_VCE_FLAG (0x1)
#define USE_VCE_FLAG (0x0)
#define VCE_RESDDR_ADDR (0x80000000)
#define VCE_RESDDR_SIZE (0x4800000)

/* boot_cfg定义, 对应sysctrl寄存器: INITIAL_INF_REG */
typedef union {
    struct {
        unsigned int single_chip : 1; /* [0] BMC主备标示. */
        unsigned int pcb_ver : 3;     /* [1..3] PCB 版本号 */
        unsigned int board_id : 8;    /* [4..11] 单板ID */
        unsigned int platform_id : 4; /* [12..15] 平台ID */
        unsigned int ddr_size : 2;    /* [16..17] 内存容量 */
        unsigned int ddr_bits : 1;    /* [18] 内存位宽模式 */
        unsigned int debug_gmac : 1;  /* [19] 调试网口指示 */
        unsigned int system_type : 1; /* [20] 系统类型指示/VGA使用指示 */
        unsigned int ddr_ecc : 1;     /* [21] DDR ECC功能配置 */
        unsigned int is_espi : 1;     /* [22] eSPI/LPC功能配置 */
        unsigned int board_type : 1;  /* [23] 插卡指示 */
        unsigned int reserved : 8;    /* [24..31] */
    } bits;

    unsigned int u32;
} boot_cfg_info_s;

/* 获取单板pcb版本 */
extern unsigned int board_info_pcb_ver_get(void);

/* 获取单板类型 */
extern unsigned int bdinfo_get_board_type(void);

/* 获取单板platform_id */
extern unsigned int bdinfo_get_platform_id(void);

/* 获取单板board_id */
extern unsigned int bdinfo_get_board_id(void);

/* 获取单板pcb_ver */
extern unsigned int bdinfo_get_pcb_ver(void);

/* 获取单板pch类型 */
extern unsigned int bdinfo_get_pch_type(void);

/* 获取单板vce 状态 */
extern unsigned int bdinfo_get_vce_state(void);

/* 获取单板DDR空间大小 */
extern unsigned long bdinfo_get_ddr_size(void);

/* 获取单板DDR ecc状态 */
extern unsigned int bdinfo_get_ddr_ecc(void);

/* 获取单板DDR保留空间大小 */
extern unsigned long bdinfo_get_ddr_rsv_size(void);

/* 获取单板DDR预留给rtos的空间大小 */
extern unsigned long bdinfo_get_ddr_rtos_size(void);

/* 获取调试网口信息 */
extern unsigned int bdinfo_get_debug_gmac_type(void);

/* 初始化单板信息 */
extern void bdinfo_init(void);

extern unsigned int  board_info_platform_id_get(void);


#endif /* __BDINFO_H__ */
