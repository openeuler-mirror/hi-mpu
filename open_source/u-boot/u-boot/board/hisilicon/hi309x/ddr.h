/*
* Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
* Description: none
* Create: 2018-08-01
*/
#ifndef __DDR_H__
#define __DDR_H__
#ifndef UINT32
#define  UINT32 unsigned int
#endif

#define DDRC_PRINT_BUF_SIZE 512

#define HI309x_DDRC_REG_BASE    0x08780000
#define SYS_CTRL_CSR_REG_BASE 0x08740000

#define DDR_SFC_MR_MAX      8
#define DDR_SFC_MR3         3
#define DDR_SFC_MR6         6


#define SYSCTL_REG_BASE			(0x08740000UL)
#define SYSCTL_CHIP_ID_BASE		(SYSCTL_REG_BASE)
#define SYSCTL_STRAP_STATUS_BASE	(SYSCTL_REG_BASE + 0x4)
#define SYSCTL_BOOT_DEFINE_BASE		(SYSCTL_REG_BASE + 0xc)
#define SYSCTL_BOOT_STATUS_BASE		(SYSCTL_REG_BASE + 0x10)
#define SYSCTL_CLK_BYPASS_CTRL_BASE	(SYSCTL_REG_BASE + 0x1c)
#define SYSCTL_PLL_LOCK_STATUS_BASE	(SYSCTL_REG_BASE + 0x40)
#define SYSCTL_PPLL0_CTRL_REG1_BASE (SYSCTL_REG_BASE + 0x2c)
#define SYSCTL_DBOOT_STATUS_BASE	(SYSCTL_REG_BASE + 0x60)
#define SYSCTL_RST_CFG_BASE		(SYSCTL_REG_BASE + 0x44)
#define SYSCTL_WD_CTRL_BASE     (SYSCTL_REG_BASE + 0x48)
#define SYSCTL_BMC_RST_RECORD_BASE  (SYSCTL_REG_BASE + 0x160)
#define SYSCTL_DIEID10_BASE         (SYSCTL_REG_BASE + 0x258)
#define SYSCTL_DIEID11_BASE         (SYSCTL_REG_BASE + 0x25c)

/* dram rate */
#define HI309x_DDRC_2400M  0
#define HI309x_DDRC_2133M  1
#define HI309x_DDRC_1866M  2
#define HI309x_DDRC_1600M  3

#define DDRC_CFG_TIMING2_ADDR (HI309x_DDRC_REG_BASE + 0x8108)
#define TAREF_LOWPOS 0x0
#define TAREF_WIDTH  0xB

#define DDRC_CFG_PD_ADDR (HI309x_DDRC_REG_BASE + 0x8028)
#define PD_EN_LOWPOS 0x0
#define PD_EN_WIDTH  0x1

#define DDRC_CFG_SFC_ADDR0 (HI309x_DDRC_REG_BASE + 0x8214)
#define DDRC_CFG_SFC_ADDR1 (HI309x_DDRC_REG_BASE + 0x8218)
#define SFC_RANK_LOWPOS 0x10
#define SFC_RANK_WIDTH  0x10

#define DDRC_CTRL_SFC_ADDR (HI309x_DDRC_REG_BASE + 0x800c)
#define CMD_REQ_LOWPOS 0x0
#define CMD_REQ_WIDTH 0x1

#define MODEREG67_BASE_ADDR (HI309x_DDRC_REG_BASE + 0xc0e4)
#define MR6_LOWPOS 0x0
#define MR6_WIDTH 0x10

#define DDRC_CURR_FUNC_BASE_ADDR (HI309x_DDRC_REG_BASE + 0x8294)
#define IN_SFC_LOWPOS 0xC
#define IN_SFC_WIDTH 0x1

/* ddrphy pll初始化延时次数   */
#define DDRPHY_PLL_INIT_DELAY_CNT          10000

#define MODEREG_BASE_ADDR (HI309x_DDRC_REG_BASE + 0xc068)
#define MR3_LOWPOS 0x10
#define MR3_WIDTH  0x10
#define DDR_BURST_NUM 4

#define DDRC_CFG_SFC_WDATA_ADDR (HI309x_DDRC_REG_BASE + 0x821C)
#define DDRC_CFG_SFC_WCTRL_ADDR (HI309x_DDRC_REG_BASE + 0x8220)

/* exmbist */
#define MDDRC_EXMBIST_CMD_REG (HI309x_DDRC_REG_BASE + 0x8158)
#define MDDRC_EXMBIST_DAT_REG (HI309x_DDRC_REG_BASE + 0x815c)
#define MDDRC_EXMBIST_STS_REG (HI309x_DDRC_REG_BASE + 0x840c)

#define EXMBIST_WALK1S_LOOP        16
#define EXMBIST_DATA_PRESSURE_LOOP 2
#define EXMBIST_1SPER8_LOOP        8
#define EXMBIST_1SPER64_LOOP       4
#define EXMBIST_260PAT1_LOOP       5
#define EXMBIST_MARCHLR_LOOP       10
#define EXMBIST_BITBALANCE_LOOP    48

#define CMD_ASCEND_TEST_DDR  0x00000721 // 升序
#define CMD_DESCEND_TEST_DDR 0x00000021 // 降序
#define CMD_WR_PAT           0x00000120 // wA
#define CMD_RD_PAT           0x00010020 // rA
#define CMD_WR_INV_PAT       0x00004120 // w(!A)
#define CMD_RD_INV_PAT       0x00410020 // r(!A)
#define CMD_IME_WR_PAT       0x40000120 // wA	(立马执行)
#define CMD_IME_RD_PAT       0x40010020 // rA	(立马执行)
#define CMD_IME_WR_INV_PAT   0x40004120 // w(!A) (立马执行)
#define CMD_IME_RD_INV_PAT   0x40410020 // r(!A) (立马执行)
#define CMD_IME_WR_PAT2      0x40001920 // w(B)  (立马执行)
#define CMD_IME_RD_PAT2      0x40190020 // r(B)  (立马执行)

#define DDR_RSV_MEM_START       (0x84804000)
#define DDR_RSV_MEM_BIST_RESULT (DDR_RSV_MEM_START + 0x4)
#define DDR_RSV_MEM_INIT_RESULT (DDR_RSV_MEM_START + 0x8)

#define CSR_DDRC_CTRL_REG   (SYS_CTRL_CSR_REG_BASE + 0x58)
#define CSR_DDRC_STATUS_REG (SYS_CTRL_CSR_REG_BASE + 0x5c)

#define MAX_REG_SAVE_NUM_1 14

#define MAX_REG_SAVE_NUM_2 12
#define MAX_REG_SAVE_NUM_3 9

#define DDR_SIZE_512M (1)
#define DDR_SIZE_1G   (2)
#define DDR_SIZE_2G   (3)
#define DDR_SIZE_4G   (0)

#define DRAM_X16 (0)
#define DRAM_X8  (1)

#define DRAM_ROW_BITS_MIN  (11)
#define DRAM_COL_BITS_MIN  (8)
#define DRAM_BANK_BITS_MIN (2)

#define DRAM_CFG_ITL_BITS    8
#define DRAM_CFG_ITL_SHIFT   8
#define DRAM_CFG_COL_BITS    8
#define DRAM_CFG_COL_SHIFT   8

#define EXMBIST_CLKON_SHIFT     5
#define MBIST_RANK_SHIFT        28


#define SHIFT_2         2
#define SHIFT_8         8
#define SHIFT_16        16
#define SHIFT_19        19
#define SHIFT_24        24

#define DDRPHY_PHASE_VALID_MASK 0x3
#define DDRPHY_PHASE_LEFT_SHIFT  1
#define DDRPHY_PHASE_RIGHT_SHIFT 2

#define DDRPHY_MR6_MODE_SHIFT   8
#define DDRPHY_MR6_MODE_MASK    0x1f
#define DDRC_CFG_SFC_MR6_SHIFT  18

#define DDRPHY_VREF_RANG1   0
#define DDRPHY_VREF_RANG2   (1 << 6)
#define DDRPHY_VREF_MASK    0x7f  /* vref value and vref range */
#define DDRPHY_VREF_SHIFT   10

#define DDRPHY_VREF_TRAINING_ENABLE     (1 << 17)

#define DDRPHY_SWITCH_PHASE_NUM     4
#define DDRPHY_SWITCH_VALID_PHASE   3
#define DDRPHY_PHASE_NUM_MAX        12
#define DDRPHY_BYTELANE_MAX         2
#define DDRPHY_BYTELANE_DQ_MAX      8
#define DDRPHY_WDQ_BDL_MAX          0x7f

#define DDRPHY_WDQ_BDL(bytelane, dq_num) (HI309x_DDRC_REG_BASE + (((dq_num) < 4) ? 0xc210 : 0xc214) + (bytelane)*0x80)
#define DDRPHY_WDQ_BDL_MASK               0x7F
#define DDRPHY_WDQ_BDL_SHIFT(dq_num)      (((dq_num) % 4) * 8)

#define DDRPHY_RDQS_CYCLE(bytelane)       (HI309x_DDRC_REG_BASE + 0xc22C + (bytelane) * 0x80)
#define DDRPHY_RDQS_CYCLE_MASK            0x01FF0000
#define DDRPHY_RDQS_CYCLE_SHIFT           16

#define DDRPHY_WDQ_PHASE(bytelane)        (HI309x_DDRC_REG_BASE + 0xc234 + (bytelane) * 0x80)
#define DDRPHY_WDQ_PHASE_MASK             0x1F00
#define DDRPHY_WDQ_PHASE_SHIFT            8

#define DDRPHY_DXN_BIST_CTRL(bytelane)    (HI309x_DDRC_REG_BASE + 0xc200 + (bytelane) * 0x80)
#define DDRPHY_BIST_STATUS(bytelane)      (HI309x_DDRC_REG_BASE + 0xc204 + (bytelane) * 0x80)
#define DDRPHY_BIST_ERROR_MASK            0xFFFF

#define DDRPHY_DIRECTION_LEFT          0
#define DDRPHY_DIRECTION_RIGHT         1
#define DDRPHY_DIRECTION_MAX           2

#define DDRPHY_WDQSPHASE_OFFSET        8
#define DDRPHY_WDQSPHASE_MASK          0x0f

#define DDRPHY_WDQPHASE_DEFAULT_VAL    0xc
#define DDRPHY_WDQPHASE_OFFSET         8
#define DDRPHY_WDQPHASE_MASK           0x1f


#define MAX_RANG1_WRITE_VREF_SCAN   43
#define MAX_RANG2_WRITE_VREF_SCAN   39
#define MIN_WRITE_VREF_SCAN         16
#define DEFAULT_WRITE_VREF_VAL      25
#define DEFAULT_VREF_CONFIG         (DEFAULT_WRITE_VREF_VAL | DDRPHY_VREF_RANG2)
#define VREF_STEP_JUMP              2
#define CONFIG_DLYUPD               (1 << 19)
extern u32 hw_efuse_read_chip_name(void);
extern u32 hw_efuse_read_chip_version(void);

#define ddrc_is_1711v110() (hw_efuse_read_chip_name() == 0x1711 && hw_efuse_read_chip_version() == 0x110)
#endif /* __DDR_H__ */
