/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_def.h
 * Author:
 * Create: 2022-11-16
 */
#ifndef __hi1711_DEF_H__
#define __hi1711_DEF_H__

#include <plat/common/common_def.h>
#include <tbbr_img_def.h>

#ifndef UMPTE_BOARD
#define UMPTE_BOARD 1
#endif
/* Special value used to verify platform parameters from BL2 to BL3-1 */
#define hi1711_BL31_PLAT_PARAM_VAL 0x0f1e2d3c4b5a6978ULL

/*******************************************************************************
 * hi1711 memory map related constants
 ******************************************************************************/
#define BOOT_LOG_BASE (MHU_SECURE_BASE - FLASH_LOG_SIZE - BOOT_LOG_SIZE)
#define BOOT_LOG_SIZE 0x2000
#define NMI_ID_BASE   (BOOT_LOG_BASE)
#define NMI_ID_SIZE   0x400
#define PSCI_VAL_BASE (NMI_ID_BASE + NMI_ID_SIZE)
#define PSCI_VAL_SIZE 0x400

#define FLASH_LOG_BASE (MHU_SECURE_BASE - FLASH_LOG_SIZE)
#define FLASH_LOG_SIZE 0x21000

#define FLASH_LOG_SAVE_OFFSET 0x800

/* st: cer work space */
#define CER_WORK_BASE 0xC00000
#define CER_WORK_SIZE 0x1000

#define TF1_LOGMEM_BASE (CER_WORK_BASE + CER_WORK_SIZE) /* 0xC01000 */
#define TF1_LOGMEM_SIZE (0x100000 - CER_WORK_SIZE)      /* Size 1020K--0xFF000 */

#define BIOS_TF1_BASE (TF1_LOGMEM_BASE + TF1_LOGMEM_SIZE) /* TF1 run space */
#define BIOS_TF1_SIZE 0x100000

/*
 * DRAM at 0xF400_0000 is divided in two regions:
 *   - Secure DRAM (default is the top 16MB except for the last 2MB, which are
 *     used by the SCP for DDR retraining)
 *   - Non-Secure DRAM (remaining DRAM starting at DRAM_BASE)
 */
#define DRAM_NS_BASE 0x00001000
#define DRAM_NS_SIZE CER_WORK_BASE /* TF1_LOGMEM_BASE--0xC00000 cer work space function */

#define MHU_SECURE_BASE BIOS_TF1_BASE /* 0xD00000 */
#define MHU_SECURE_SIZE 0x00001000

#define TZRAM_BASE (MHU_SECURE_BASE + MHU_SECURE_SIZE) /* 0xD01000 */
#define TZRAM_SIZE 0x50000

/* ATF会访问一些IO资源，如UART、GIC等 */
#define IO_BASE       0x0
#define IO_END        0x80000000

/* 引脚定义DDR大小 */
#define DDR_SIZE_512M (1)
#define DDR_SIZE_1G   (2)
#define DDR_SIZE_2G   (3)
#define DDR_SIZE_4G   (0)

#define SIZE_512M (0x20000000UL)
#define SIZE_1G   (0x40000000UL)
#define SIZE_2G   (0x80000000UL)
#define SIZE_4G   (0x100000000UL)

/* 临终遗言RTOS会发送一个地址到ATF，这个地址不是固定的，因此需要配置RTOS所有的地址空间 */
#define DDR_BASE  0x80000000
#define RTOS_BASE 0x87480000

/* BIOS copy TF bl1.bin/fip.bin to This address */
#ifndef TZROM_BASE
/* Use the bypass address */
#define TZROM_BASE (TZRAM_BASE + TZRAM_SIZE) /* 0xD51000 */
#endif

/* Actual ROM size on hi1711 is 64 KB, but TBB requires at least 80 KB in debug
 * mode. We can test TBB on hi1711 bypassing the ROM and using 128 KB of flash */
#if TRUSTED_BOARD_BOOT
#define TZROM_SIZE 0x8000
#else
#define TZROM_SIZE 0x8000
#endif

#define FIP_BASE (TZROM_BASE + TZROM_SIZE) /* 0xD59000 */
#define FIP_SIZE 0x98000                 /* 已经把TEE文件大小计算在内 */

#define RESV_MEM_BASE1 (FIP_BASE + FIP_SIZE)                              /* 0xDF1000 */
#define RESV_MEM_SIZE1 ((BIOS_TF1_BASE + BIOS_TF1_SIZE) - RESV_MEM_BASE1) /* 0xE00000 - 0xDF1000 eq 0xF000 */

/* Following covers Columbus Peripherals excluding NSROM and NSRAM */
#define DRAM_TEE_BASE (BIOS_TF1_BASE + BIOS_TF1_SIZE) /* 0xE00000 */

#define DRAM_TEE_SIZE (0x2600000 - DRAM_TEE_BASE)

#define TF_LOG_BASE     0x7750000 /* 0x7740000 - 0x7750000 bbox for tee */
#define TF_LOG_SIZE     0x8000
#define TF_LOG_MAX      (TF_LOG_BASE + TF_LOG_SIZE)
#define TF_BAK_LOG_BASE TF_LOG_MAX
#define TF_BAK_LOG_SIZE 0x8000

#define DRAM_NS_BASE3 (DRAM_TEE_BASE + DRAM_TEE_SIZE) /* 0x2600000 */
#define DRAM_NS_SIZE3 ((0x11200000) - DRAM_NS_BASE3)

#define PERI_SRAM_BASE 0x11200000
#define PERI_SRAM_SIZE 0x400000

#define DEVICE1_BASE (PERI_SRAM_BASE + PERI_SRAM_SIZE)
#define DEVICE1_SIZE ((0x100000000) - (DEVICE1_BASE))

#define DEVICE0_BASE 0x100000000
#define DEVICE0_SIZE 0x100000000

#define FLASH_NS_BASE 0x150000000
#define FLASH_SIZE    0x00800000

#define FLASH_ADDR 0xE0000000

#define PLAT_TRUSTED_SRAM_ID 0
#define PLAT_DRAM_ID         1

#define MHU_PAYLOAD_CACHED 1

/* the mailbox for a cpu */
#define TRUSTED_MAILBOXES_BASE MHU_SECURE_BASE
#define TRUSTED_MAILBOX_SHIFT  4

/* BL32 use the region below */
#define TRUSTED_DRAM_BASE 0xDE000000
#define TRUSTED_DRAM_SIZE 0x00E00000 /* 14 MB */

/* V2M motherboard system registers & offsets */
#define VE_SYSREGS_BASE 0x1c010000
#define V2M_SYS_LED     0x8

/* hha base address, which have been mapped in DEVICE0_BASE region */
#define HHA0_ADDR 0x100070000
#define HHA1_ADDR 0x100080000

/* Debug Switch */
#define DEBUG_TF 1

#define PERISUBCTRLADR         0xF1000000
#define AISUB_APB_SUBCTRL_BASE 0xFC000000
#define SYS_CTRL_BASE          0xF1010000

#define SC_PLL_CLK_BYPASS1 0xA4
#define TIMER1_BASE        0xf1370000
#define TIMER1_INIT_PARA   0x1800000
#define TIMER_CONTROL      0x8
#define TIMER_INTCLR       0x14
#define TIMER_BGLOAD       0xc
#define ARM_CORE_MASK      0xFFFF

#define CPLD_BASE 0x78000000
#define CPLD_SIZE 0x08000000
#define ALG_BASE  0xD0000000

#define LLC_MAINT_CTRL                    0x00020
#define DJTAG_MODULE_SEL_LLC0             0x01
#define DJTAG_MODULE_SEL_LLC1             0x02
#define DJTAG_MODULE_SEL_LLC2             0x03
#define DJTAG_MODULE_SEL_LLC3             0x04
#define DJTAG_MODULE_SEL_PERI_ACESS_AGENT 0x05

#if USE_COHERENT_MEM
#define hi1711_BL_REGIONS 3
#else
#define hi1711_BL_REGIONS 2
#endif

/*
 * The hi1711_MAX_MMAP_REGIONS depend on the number of entries in hi1711_mmap[]
 * defined for each BL stage in hi1711_common.c.
 */
#if IMAGE_BL1
#define hi1711_MMAP_ENTRIES 12
#endif
#if IMAGE_BL2
#define hi1711_MMAP_ENTRIES 12
#endif
#if IMAGE_BL31
#define hi1711_MMAP_ENTRIES 12
#endif
#if IMAGE_BL32
#define hi1711_MMAP_ENTRIES 12
#endif

/*******************************************************************************
 * GIC-400 & interrupt handling related constants
 ******************************************************************************/
#define GICD_BASE 0x24000000
#define GICR_BASE 0x24040000
#define GICC_BASE 0xFE000000

/* wdg 5 interrupt used as fiq */
#define PERI_WDT5_S 141
/* used for trigger interrupt to other cores */
#define FIQ_SGI_S 8

#define PERI_smmu_spi0_s    64
#define PERI_smmu_spi1_s    65
#define PERI_smmu_spi2_s    66
#define PERI_s_wdg0_ns_spi0 76
#define PERI_s_wdg0_ns_spi1 77
#define IO_smmu_spi0_s      235
#define IO_smmu_spi1_s      236
#define IO_smmu_spi2_s      237
#define DVPP_smmu0_spi0_s   383
#define DVPP_smmu0_spi1_s   384
#define DVPP_smmu0_spi2_s   385
#define DVPP_smmu1_spi0_s   392
#define DVPP_smmu1_spi1_s   393
#define DVPP_smmu1_spi2_s   394
#define AICORE0_smmu_spi0_s 490
#define AICORE0_smmu_spi1_s 491
#define AICORE0_smmu_spi2_s 492
#define AICORE1_smmu_spi0_s 502
#define AICORE1_smmu_spi1_s 503
#define AICORE1_smmu_spi2_s 504

#define SECURE_GR0_INT PERI_smmu_spi0_s,    \
                       PERI_smmu_spi1_s,    \
                       PERI_smmu_spi2_s,    \
                       PERI_s_wdg0_ns_spi0, \
                       PERI_s_wdg0_ns_spi1, \
                       IO_smmu_spi0_s,      \
                       IO_smmu_spi1_s,      \
                       IO_smmu_spi2_s,      \
                       DVPP_smmu0_spi0_s,   \
                       DVPP_smmu0_spi1_s,   \
                       DVPP_smmu0_spi2_s,   \
                       DVPP_smmu1_spi0_s,   \
                       DVPP_smmu1_spi1_s,   \
                       DVPP_smmu1_spi2_s,   \
                       AICORE0_smmu_spi0_s, \
                       AICORE0_smmu_spi1_s, \
                       AICORE0_smmu_spi2_s, \
                       AICORE1_smmu_spi0_s, \
                       AICORE1_smmu_spi1_s, \
                       AICORE1_smmu_spi2_s

/* NMI context reg */
#define SC_SPI_INT_EN         0x4094
#define SC_SPI_INT_EN_ENALBE  0x1
#define SC_SPI_INT_EN_DISABLE 0x0

#define SC_N2N_INT_SEL9 0x40C4
/* watchdog 56 */
#define SC_N2N_INT_RESET_WATCHDOG_TYPE 0x38

#define GP_NUM              31
#define GPREG_SP_EL0        0
#define SP_EL1              1
#define SP_EL2              2
#define SCR_EL3             3
#define SPSR_EL3            4
#define ELR_EL3             5
#define EL3_STATE_NUM       6
#define RTOS_NMI_PT_REG_NUM 36

#define TF_ERROR 0xffffffffull
#define TF_OK    0

/*******************************************************************************
 * PL011 related constants
 ******************************************************************************/
/* UART0 */
#define HI1711_UART_BASE 0x08710000
/* UART1 */
#define PL011_UART1_BASE HI1711_UART_BASE
/* SoC UART0 */
#define PL011_UART0_BASE HI1711_UART_BASE
/* SoC UART1 */
#define PL011_UART3_BASE HI1711_UART_BASE

#define Hi1711_BAUDRATE 115200

#define CCI400_REG_BASE             0xE8100000
#define CCI400_SL_IFACE3_CLUSTER_IX 0
#define CCI400_SL_IFACE4_CLUSTER_IX 1

#define Hi1711_UART0_CLK_IN_HZ 100000000
#define Hi1711_UART1_CLK_IN_HZ 100000000
#define HI1711_UART_CLK_IN_HZ  100000000
#define Hi1711_UART3_CLK_IN_HZ 100000000

/*******************************************************************************
 * The flag address(SRAM) of whether primary core cold boot. If equal
 * to the magic number 0x5a5a5a5a, it indicate that it's a warm boot of primary core, else it's
 * a cold boot of primary core.
******************************************************************************/
#define SRAM_BASE            0x1000
#define PSCI_CPUON_FLAG_ADDR (RESV_MEM_BASE1 + 0x0000C0)  /* PSCI_cpuon单核复位标记地址 */

/* The Sram address for save the ELR_EL3 generated by smc through UEFI */
#define ELR_PEI_SAVE_ADDR RESV_MEM_BASE1

#define MAGIC_NUMBER      0x5A5A5A5A

#define SYS_COUNTER_FREQ_ESL  50000000
#define SYS_COUNTER_FREQ      25000000
#define SYS_COUNTER_FREQ_ASIC 25000000

#define CPU_FREQ 0x2000000000UL /* Hi1711 2G Hz freq */
#define G_FREQ   0x1000000000UL

/*******************************************************************************
 * Reset config and status register offset.
******************************************************************************/
#define SC_CLUS0_CRG_RESET_REQ  0xA00
#define SC_CLUS0_CRG_RESET_DREQ 0xA04
#define SC_CLUS1_CRG_RESET_REQ  0xA08
#define SC_CLUS1_CRG_RESET_DREQ 0xA0C
#define SC_CLUS2_CRG_RESET_REQ  0xA10
#define SC_CLUS2_CRG_RESET_DREQ 0xA14
#define SC_CLUS3_CRG_RESET_REQ  0xA18
#define SC_CLUS3_CRG_RESET_DREQ 0xA1C
#define SC_CLUS0_CRG_RESET_ST   0x5A00
#define SC_CLUS1_CRG_RESET_ST   0x5A08
#define SC_CLUS2_CRG_RESET_ST   0x5A10
#define SC_CLUS3_CRG_RESET_ST   0x5A18

#define ALLRESET_BIT   0x1fe
#define ONERESET_BIT   0x22
#define L2_SRST_DREQ   0x1
#define RESET_CONTROL  0
#define DRESET_CONTROL 1
#define SETBIT0        (1 << 0)
#define CLRBIT0        (0 << 0)
#define SETBIT1        (1 << 1)
#define CLRBIT1        (0 << 1)
#define SETBIT2        (1 << 2)
#define CLRBIT2        (0 << 2)
#define SETBIT3        (1 << 3)
#define CLRBIT3        (0 << 3)

/* HHA config */
#define HHA0_CHAIN_ID 0x10
#define HHA1_CHAIN_ID 0x20
#define AA_CHAIN_ID   0x06
/* why MASK alwayls 0x1 ? */
#define MASK             0x1
#define AA_MODE          0x4
#define HHA_INTLV_EN_BIT (1 << 9)
#define HHA_INTLV        0xC
#define HHA_SEC0_SA      0x20
#define HHA_SEC0_EA      0x24
#define HHA_SEC1_SA      0x28
#define HHA_SEC1_EA      0x2C

/*******************************************************************************
 * DIE_ID define in hi1711.
******************************************************************************/
#ifndef TRUE
#define TRUE ((unsigned char)(1 == 1))
#endif
#ifndef FALSE
#define FALSE ((unsigned char)(0 == 1))
#endif

#define IDIE_ID    0
#define CDIEA_ID   1
#define CDIEC_ID   2
#define CDIEB_ID   3
#define DIE_ID_MAX 4

#define S1_CDIEA_ID 5
#define S1_CDIEC_ID 6
#define S1_CDIEB_ID 7
#define S1_IDIE_ID  4

#define PACKAGE_TC_NI       0
#define PACKAGE_TC_TA_NI    1
#define PACKAGE_TC_TB_TA_NI 2
#define PACKAGE_TYPE_NUM    3

#define ADDRESS_MAP_INVALID ((uint64_t)(-1))

/* SOC DJTAG */
#define SC_DJTAG_SEC_ACC_EN    0xD800 /* DJTAG访问各模块内安全世界寄存器的安全访问控制使能。（注意：1，此寄存器空间必须是安全访问才能成功″ */
#define SC_DJTAG_MSTR_ADDR     0xD810 /* SC_DJTAG_MSTR_ADDR为DJTAG Master配置访问模块的偏移地址寄存器 */
#define SC_DJTAG_MSTR_DATA     0xD814 /* SC_DJTAG_MSTR_DATA为DJTAG Master的写数据 */
#define SC_DJTAG_MSTR_CFG      0xD818 /* SC_DJTAG_MSTR_CFG为DJTAG Master的配置寄存器 */
#define SC_DJTAG_MSTR_START_EN 0xD81C /* SC_DJTAG_MSTR_START_EN为DJTAG Master访问chain链的start使能 */
#define SC_DJTAG_RD_DATA0      0xE800 /* DJTAG扫描链上第一个模块读数据寄存器 */
#define SC_DJTAG_OP_ST         0xE828 /* DJTAG访问状态寄存器 */
#define DJTAG_CHAIN_MODULE_MAX 8
#define PERI_APB_SYSCTRL_BASE  SYS_CTRL_BASE
#define DJTAG_MODULE_SEL_MN1   0x21
#define pll3_peri_mode         (0x1 << 3)
#define pll2_peri_mode         (0x1 << 2)
#define pll1_peri_mode         (0x1 << 1)

#endif /* __hi1711_DEF_H__ */
