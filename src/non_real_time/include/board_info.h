/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: none
 */

#ifndef __BOARD_INFO_H__
#define __BOARD_INFO_H__

/* 单板类型 */
#define BOARD_TYPE_HOST                 0x0   /* 贴片主板:非插卡形态 */
#define BOARD_TYPE_BUCKLE               0x1   /* 原有插卡形态 */

/* 单板硬件平台编码 */
#define BOARD_PLATM_X86                 0x1   /* PoD X86 4P平台 */
#define BOARD_PLATM_ATLAS               0x2   /* ATLAS平台 */
#define BOARD_PLATM_X_SERIES            0X3   /* X系列服务器平台 */
#define BOARD_PLATM_PANGEA_V5           0x5   /* 盘古V3/V5平台 */
#define BOARD_PLATM_TCE                 0x6   /* TCE平台 */
#define BOARD_PLATM_TAISHAN             0x7   /* TaiShan机架服务器平台 */
#define BOARD_PLATM_HIGH_SERVE          0x8   /* x86服务器平台 */
#define BOARD_PLATM_CHH                 0x9   /* 星光服务器平台 */
#define BOARD_PLATM_KUNPENG             0xa   /* 鲲鹏开放主板平台 */
#define BOARD_PLATM_CA2200              0xb   /* CJB服务器平台 */
#define BOARD_PLATM_PANGEA_V6           0xd   /* 盘古V6平台 */
#define BOARD_PLATM_BUCKLE_NEW          0xe   /* 插卡BMC，现行最新方案使用 */
#define BOARD_PLATM_BUCKLE              0xf   /* 插卡BMC */
#define BOARD_PLATM_SKYLAKE_BUCKLE      0x0   /* 天池架构插卡BMC */

/* 插卡子平台编码 */
#define BOARD_PLATM_BUCKLE_X            0x3   /* X系列服务器 */
#define BOARD_PLATM_BUCKLE_OSCA         0x4   /* OSCA平台 */
#define BOARD_PLATM_BUCKLE_PANGEA       0x5   /* 盘古平台 */
#define BOARD_PLATM_BUCKLE_TAISHAN      0x7   /* TaiShan机架服务器平台 */
#define BOARD_PLATM_BUCKLE_HIGH         0x8   /* 高性能服务器 */
#define BOARD_PLATM_BUCKLE_RH           0xf   /* RH服务器 */

#define BOARD_SUB_PLATM_ID_OLD          0x3   /* 区分新老方案插卡子平台，子平台ID为3时为老方案 */

/* 星光平台(0x9)下的单板ID */
#define CHH_BOARD_ID_SPGPBA             0xB8
#define CHH_BOARD_ID_SPSWBA             0xB9
#define CHH_BOARD_ID_CHH1FPFMBA         0xBA
#define CHH_BOARD_ID_CHH1SPMNTA         0x1B

/* EVB单板ID编码 */
#define EVB_BOARD_ID                    0x78  /* EVB单板 */
#define EVB_V100_PCB_VERSION            0x3
#define EVB_V110_PCB_VERSION            0x5

/* CJB单板ID */
#define CA2200_BOARD_ID_V7_CM15MBCA     0x01  /* CJB主板 */

/* TaiShan机架服务器平台(0x7)下的单板ID编码 */
#define TAISHAN_BOARD_ID_2480           0x1   /* TaiShan2480服务器 */
#define TAISHAN_BOARD_ID_5290           0x4   /* TaiShan5290服务器 */
#define TAISHAN_BOARD_ID_2280E          0x5   /* TaiShan2280E服务器 */
#define KUNPENG_S920S03                 0x8   /* 鲲鹏基于Atlas  500 增配 */
#define FUSION_RM210                    0x6d  /* FusionRM  platform id 0x7 */
#define FUSION_RM220                    0x62  /* FusionRM220, platform id 0x3 */
#define FUSION_DA140V2                  0x6   /* FusionPod DA140V2 */
#define FUSIONPOD_BC82MMC               0x09  /* FusionPod 武汉超算扩展板 */
#define RM211_IT31RMUC                  0x6c  /* RM211机型管理板 */

/* X86 RH服务器的单板ID编码 */
#define FUSION_DA140V6                  0x3    /* FusionPod DA140V6 */
#define PLATM_X86_2288_V6_BC13MBSC      0xb8   /* 2288 v6 */
#define FUNSION_DH120C_V6               0xB0  /* FusionPod DH120C V6 */

#define RH_2288H_V5_BC11SPSCB_BOARD_ID  0x17  /* 海燕6.0 2288H V5 光口单板 */
#define RH_2288H_V5_BC11SPCCA_BOARD_ID  0x15  /* 海燕6.0 2288H V5 电口单板 */
#define RH_1288H_V5_BC11SPSC_BOARD_ID   0x18  /* 海燕6.0 1288H V5 光口单板 */
#define RH_1288H_V5_BC11SPSCC_BOARD_ID  0x19  /* 海燕6.0 2288H V5 电口单板 */
#define RH_2288X_V5_BC11SPSGA_BOARD_ID  0xbf  /* QingShan 2288X V5-BC11SPSGA-Purley */
#define RH_2288X_V5_BC11SPSG_BOARD_ID   0xb0  /* QingShan 2288X BC11SPSG */
#define RH_2288X_V5_BC11SPSGB_BOARD_ID  0xb4  /* QingShan 2288X BC11SPSGB */

/* TCE平台下的单板ID编码 */
#define TCE_BOARD_ID_GPUA               0x30  /* 计算板2280E 5290 */
#define TCE_BOARD_ID_SMMA               0x08  /* 管理板 */
#define TCE_BOARD_ID_CN90BMCBB          0x09  /* 1.1管理板 */
#define TCE_BOARD_ID_CN90BMCBBA         0x0A  /* TCE 4u/8u管理板扣板 */
#define TCE_BOARD_ID_CN90GPUB           0x31  /* 1.1计算板 */
#define TCE_BOARD_ID_CN91SPUA           0x32  /* 2.0计算板 */
#define TCE_BOARD_ID_CN90BMCC           0x51  /* 2.0 X86_BMC扣卡 */
#define TCE_BOARD_ID_CN90GPUD           0x52  /* TCE 2.0 X86 GPU底板 */
#define TCE_BOARD_ID_CN90GPUE           0x53  /* TCE 22.0 X86 GPU底板 */
#define TCE_BOARD_ID_CN90GPUGA          0x55  /* 22.0 X86_BMC扣卡 */
#define TCE_BOARD_ID_CN90ESWD           0x70  /* X86 Mesh板 */
#define TCE_BOARD_ID_CN91LIUAA          0x56  /* TCE 22.0 X86 板载卡 */
#define TCE_BOARD_ID_CN90GPUB_VB        0x33  /* TCE TCE 4U/8U ARM主板单板 */
#define TCE_BOARD_ID_CM91LIUAA          0x34  /* TCE 23.0 全新开发刀片 */
#define TCE_BOARD_ID_ARM241             0x35  /* TCE 24.1 ARM 计算节点 BMC扣卡 */

/* ATLAS平台下的单板ID编码 */
#define ATLAS_800_BOARD_ID_9000         0x92 /* Atlas800服务器 */
#define ATLAS_500_IT21EKMA_BOARD_ID     0x94 /* Atlas500服务器 */
#define ATLAS_800_BOARD_ID_9010         0x52 /* Atlas800-9010服务器 */
#define ATLAS_800_IT21MNNA_BOARD_ID     0x1  /* Atlas 800I M1 1951推理服务器集群单板 */
#define ATLAS_800_IT21MNCB_BOARD_ID     0xbe /* Atlas 800I M1 1951微服务器汇聚节单板 */
#define ATLAS_CDLS_BOARD_ID             0x9B /* CDLS项目 */
#define ATLAS_2_3_0_BOARD_ID            0x03 /* 2.3.0项目 */
#define ATLAS_T1_5_0_POD_IT21SK4B       0x9c /* Atlas T1.5.0 Pad 3.0G CPU */
#define ATLAS_T1_5_0_POD_IT21SK4C       0x9f /* Atlas T1.5.0 Pad 2.6G CPU */
#define ATLAS_T1_5_0_BOX_IT21SDHA       0xba /* Atlas T1.5.0 200T Box */

/* x86服务器平台下的单板ID编码 */
#define HAIYAN_BOARD_ID_5               0x4  /* 海燕5.0服务器 */

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
#define PANGEA_V6_BOARD_ID_STL6SPCU01       0x95  /* 盘古C+主控板12盘位 */
#define PANGEA_V6_BOARD_ID_STL6SPCU03       0x99  /* 盘古C+主控板 */
#define PANGEA_V6_BOARD_ID_STL6TSPC_TEST    0x9a  /* 大西洋/C+ 高端控制单元测试板 */
#define PANGEA_V6_BOARD_ID_BC13MBSBF2       0x9b  /* 计算型存储全宽计算x86主板2号 */
#define PANGEA_V6_BOARD_ID_BC13MBSBF        0x9c  /* 计算型存储全宽计算x86主板 */
#define PANGEA_V6_BOARD_ID_BC82ADDRF        0x9f  /* 计算型存储全宽计算arm主板 */
#define PANGEA_V6_BOARD_ID_STL6ICBA01       0xa3  /* XPU + 管理板 */
#define PANGEA_V6_BOARD_ID_STL6ICBA02       0xaf  /* 计算存储管理板 */
#define PANGEA_V6_BOARD_ID_STL6SPCY         0xf3  /* 盘古V6R5C60 东海主控板 */
#define PANGEA_V6_BOARD_ID_STL6SPCW_S       0xa9  /* 盘古V6计算型存储半宽节点 存储节点 */
#define PANGEA_V6_BOARD_ID_STL6SPCW_C       0xa6  /* 盘古V6计算型存储半宽节点 计算节点 */
#define PANGEA_V6_BOARD_ID_STL6SPCR         0xf5  /* 盘古存储-wafer框主控板 */

/* 鲲鹏开放主板平台下的单板ID编码 */
#define KUNPENG_BOARD_ID_BC82AMDJ           0xf0  /* 鲲鹏开放主板2.1.0 BC82AMDJ */
#define KUNPENG_BOARD_ID_BC82AMDL           0xf1  /* 鲲鹏开放主板2.1.1 BC82AMDL */
#define KUNPENG_BOARD_ID_BC82AMDR           0xf3  /* 鲲鹏开放主板2.1.1 BC82AMDR */

/* X系列服务器平台下单板ID编码 */
#define X_SERIES_X6000_V6_BC23SPSA      0XB5
#define X_SERIES_X6000_V6_BC23SPSB      0XB6

/* OSCA平台下单板ID编码 */
#define OSCA_E9000_V6_IT11SWCA          0x3D /* E9000_V6 121_V6刀片 */

/* PCB通用版本号 */
#define PCB_COMMON_VER0                 0x0  /* 通用pcb_ver: 0 */
#define PCB_COMMON_VER1                 0x1  /* 通用pcb_ver: 1 */
#define PCB_COMMON_VER2                 0x2  /* 通用pcb_ver: 2 */
#define PCB_COMMON_VER3                 0x3  /* 通用pcb_ver: 3 */

/* 每个IP配置的复用最大寄存器数目 */
#define IO_MULTI_REG_MAX_CNT            28

/* IO复用配置寄存器的类型 */
#define IO_MULTI_REG_TYPE_IOCFG_R       0
#define IO_MULTI_REG_TYPE_IOCFG_T       1
#define IO_MULTI_REG_TYPE_LP            2
#define IO_MULTI_REG_TYPE_SYSCTL        3

/* 需要配置IO复用的IP编码 */
typedef enum {
    IO_MULTI_UART0 = 0,
    IO_MULTI_UART1,
    IO_MULTI_UART2,
    IO_MULTI_UART3,
    IO_MULTI_UART4,
    IO_MULTI_UART5 = 5,
    IO_MULTI_UART6,
    IO_MULTI_UART7,
    IO_MULTI_I2C0,
    IO_MULTI_I2C1,
    IO_MULTI_I2C2 = 10,
    IO_MULTI_I2C3,
    IO_MULTI_I2C4,
    IO_MULTI_I2C5,
    IO_MULTI_I2C6,
    IO_MULTI_I2C7 = 15,
    IO_MULTI_I2C8,
    IO_MULTI_I2C9,
    IO_MULTI_I2C10,
    IO_MULTI_I2C11,
    IO_MULTI_I2C12 = 20,
    IO_MULTI_I2C13,
    IO_MULTI_I2C14,
    IO_MULTI_I2C15,
    IO_MULTI_IPMB0_T_I2C,
    IO_MULTI_IPMB1_T_I2C = 25,
    IO_MULTI_IPMB2_T_I2C,
    IO_MULTI_IPMB3_T_I2C,
    IO_MULTI_IPMB0_R_LOCALBUS,
    IO_MULTI_IPMB1_R_LOCALBUS,
    IO_MULTI_IPMB2_R_LOCALBUS = 30,
    IO_MULTI_IPMB3_R_LOCALBUS,
    IO_MULTI_SMBUS,
    IO_MULTI_SPI0_CS0,
    IO_MULTI_SPI0_CS1,
    IO_MULTI_SPI1_CS0_T_SFC = 35,
    IO_MULTI_SPI1_CS1_T_SFC,
    IO_MULTI_SPI1_CS0_R_EMMC,
    IO_MULTI_SPI1_CS1_R_EMMC,
    IO_MULTI_SGPIO0,
    IO_MULTI_SGPIO1 = 40,
    IO_MULTI_PF_PAD0,   /* ALERT: do not add any macros inside PWM PAD0-28 */
    IO_MULTI_PF_PAD1,
    IO_MULTI_PF_PAD2,
    IO_MULTI_PF_PAD3,
    IO_MULTI_PF_PAD4 = 45,
    IO_MULTI_PF_PAD5,
    IO_MULTI_PF_PAD6,
    IO_MULTI_PF_PAD7,
    IO_MULTI_PF_PAD8,
    IO_MULTI_PF_PAD9 = 50,
    IO_MULTI_PF_PAD10,
    IO_MULTI_PF_PAD11,
    IO_MULTI_PF_PAD12,
    IO_MULTI_PF_PAD13,
    IO_MULTI_PF_PAD14 = 55,
    IO_MULTI_PF_PAD15,
    IO_MULTI_PF_PAD16,
    IO_MULTI_PF_PAD17,
    IO_MULTI_PF_PAD18,
    IO_MULTI_PF_PAD19 = 60,
    IO_MULTI_PF_PAD20,
    IO_MULTI_PF_PAD21,
    IO_MULTI_PF_PAD22,
    IO_MULTI_PF_PAD23,
    IO_MULTI_PF_PAD24 = 65,
    IO_MULTI_PF_PAD25,
    IO_MULTI_PF_PAD26,
    IO_MULTI_PF_PAD27,
    IO_MULTI_SFC0_CS0,
    IO_MULTI_SFC0_CS1 = 70,
    IO_MULTI_SFC1_CS0,
    IO_MULTI_SFC1_CS1,
    IO_MULTI_PECI,
    IO_MULTI_DMAC,
    IO_MULTI_JLC1_T_SDIO = 75,
    IO_MULTI_JLC1_R_RMII,
    IO_MULTI_CANBUS0,
    IO_MULTI_CANBUS1,
    IO_MULTI_EMMC,
    IO_MULTI_SDIO = 80,
    IO_MULTI_USB_VBUS,
    IO_MULTI_RMII0,
    IO_MULTI_RMII1,
    IO_MULTI_MDIO,
    IO_MULTI_ESPI = 85,
    IO_MULTI_LPC,
    IO_MULTI_VGA_SYNC,
    IO_MULTI_VGA_SCL_SDA,
    IO_MULTI_HOSTRTC,
    IO_MULTI_VCE_PIN_CAPTURE = 90,
    IO_MULTI_BMC_UNSE_RST0_N,
    IO_MULTI_INT,
    IO_MULTI_LOCALBUS_CS0,
    IO_MULTI_LOCALBUS_CS1,
    IO_MULTI_LOCALBUS_CS2 = 95,
    IO_MULTI_LOCALBUS_CS3,
    IO_MULTI_LOCALBUS_CPLD,
    IO_MULTI_LOCALBUS_NORFLASH,
    IO_MULTI_CHIP_ACTIVE,
    IO_MULTI_SFC0_CS0_NO_WPHOLD = 100,
    IO_MULTI_SFC0_CS1_NO_WPHOLD,
    IO_MULTI_SFC1_CS0_NO_WPHOLD,
    IO_MULTI_SFC1_CS1_NO_WPHOLD,
    IO_MULTI_UART12,
    IO_MULTI_UART13 = 105,
    IO_MULTI_MODULE_MAX = 106
}IoMultiModuleId;

/* IO复用配置信息 */
typedef struct {
    u16 module_id;                       /* 对应IoMultiModuleId */
    u32 debug_flag;                      /* 是否打印配置信息: 0: 不打印; 1: 打印 */

    u32 reg_cnt;                           /* 需要配置的实际寄存器数目 */
    u32 reg_type[IO_MULTI_REG_MAX_CNT];    /* support:LP,IOR,IOT,要求寄存器的属性只能是RO\RW */
    u32 reg_offset[IO_MULTI_REG_MAX_CNT];
    u32 reg_mask[IO_MULTI_REG_MAX_CNT];
    u32 reg_val[IO_MULTI_REG_MAX_CNT];
}io_multi_cfg_s;

#define VALID_MASK_BIT_NUM     (32)

#define USE_MODULE_MASK_NUM    ((IO_MULTI_MODULE_MAX - 1) / VALID_MASK_BIT_NUM + 1)

/* board cfg info */
typedef struct {
    u32 board_type;
    u32 plat_id;
    u32 board_id;
    u32 pcb_ver;
    u32 use_module_mask[USE_MODULE_MASK_NUM];
}bd_cfg_info;

#endif
