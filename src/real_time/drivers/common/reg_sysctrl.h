/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 * UniProton is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 * 	http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * See the Mulan PSL v2 for more details.
 */
#ifndef __BM_SYSCTRL_H__
#define __BM_SYSCTRL_H__

/**
 * @defgroup bm_sysctrl bm_sysctrl
 */
#define SYSCTRL_CSR_BASE_ADDR 0x8740000

#define SYSCTRL_CSR_CHIP_ID_OFFSET_ADDR 0x0
#define SYSCTRL_CSR_STRAP_STATUS_REG_OFFSET_ADDR 0x4
#define SYSCTRL_CSR_INITIAL_INF_REG_OFFSET_ADDR 0x8
#define SYSCTRL_CSR_BOOT_DEFINE_REG_OFFSET_ADDR 0xC
#define SYSCTRL_CSR_REBOOT_CTRL_OFFSET_ADDR 0x10
#define SYSCTRL_CSR_CLK_DIV_CHG_EN_OFFSET_ADDR 0x14
#define SYSCTRL_CSR_CLK_DIV_CTRL_REG_OFFSET_ADDR 0x18
#define SYSCTRL_CSR_CLK_BYPASS_CTRL_REG_OFFSET_ADDR 0x1C
#define SYSCTRL_CSR_CPLL_CTRL_REG_0_OFFSET_ADDR 0x20
#define SYSCTRL_CSR_CPLL_CTRL_REG_1_OFFSET_ADDR 0x24
#define SYSCTRL_CSR_PPLL0_CTRL_REG_0_OFFSET_ADDR 0x28
#define SYSCTRL_CSR_PPLL0_CTRL_REG_1_OFFSET_ADDR 0x2C
#define SYSCTRL_CSR_PPLL1_CTRL_REG_0_OFFSET_ADDR 0x30
#define SYSCTRL_CSR_PPLL1_CTRL_REG_1_OFFSET_ADDR 0x34
#define SYSCTRL_CSR_PPLL2_CTRL_REG_0_OFFSET_ADDR 0x38
#define SYSCTRL_CSR_PPLL2_CTRL_REG_1_OFFSET_ADDR 0x3C
#define SYSCTRL_CSR_PLL_LOCK_STATUS_OFFSET_ADDR 0x40
#define SYSCTRL_CSR_WD_CTRL_REG_OFFSET_ADDR 0x48
#define SYSCTRL_CSR_WD_DLY_CFG_OFFSET_ADDR 0x4C
#define SYSCTRL_CSR_RESET_SOURCE_ST_OFFSET_ADDR 0x50
#define SYSCTRL_CSR_RESET_SOURCE_CLR_OFFSET_ADDR 0x54
#define SYSCTRL_CSR_DDRC_CTRL_OFFSET_ADDR 0x58
#define SYSCTRL_CSR_DDRC_STATUS_OFFSET_ADDR 0x5C
#define SYSCTRL_CSR_DBOOT_STATUS_REG_OFFSET_ADDR 0x60
#define SYSCTRL_CSR_USB_REF_CLK_CFG_OFFSET_ADDR 0x64
#define SYSCTRL_CSR_AUTO_CLK_CTRL_OFFSET_ADDR 0x6C
#define SYSCTRL_CSR_SYS_CLK_ENABLE_OFFSET_ADDR 0x70
#define SYSCTRL_CSR_SYS_CLK_DISABLE_OFFSET_ADDR 0x74
#define SYSCTRL_CSR_SYS_CLK_STATE_OFFSET_ADDR 0x78
#define SYSCTRL_CSR_INITIAL_INF_REG2_OFFSET_ADDR 0x7C
#define SYSCTRL_CSR_INITIAL_INF_REG3_OFFSET_ADDR 0x80
#define SYSCTRL_CSR_A55_ASSERT_INFO0_OFFSET_ADDR 0x100
#define SYSCTRL_CSR_A55_ASSERT_INFO1_OFFSET_ADDR 0x104
#define SYSCTRL_CSR_A55_ASSERT_INFO2_OFFSET_ADDR 0x108
#define SYSCTRL_CSR_A55_ASSERT_SOFT_INT_OFFSET_ADDR 0x10C
#define SYSCTRL_CSR_M3_ASSERT_INFO0_OFFSET_ADDR 0x110
#define SYSCTRL_CSR_M3_ASSERT_INFO1_OFFSET_ADDR 0x114
#define SYSCTRL_CSR_M3_ASSERT_INFO2_OFFSET_ADDR 0x118
#define SYSCTRL_CSR_M3_ASSERT_SOFT_INT_OFFSET_ADDR 0x11C
#define SYSCTRL_CSR_SC_ASSERT_INFO0_OFFSET_ADDR 0x120
#define SYSCTRL_CSR_SC_ASSERT_INFO1_OFFSET_ADDR 0x124
#define SYSCTRL_CSR_SC_ASSERT_INFO2_OFFSET_ADDR 0x128
#define SYSCTRL_CSR_SC_ASSERT_SOFT_INT_OFFSET_ADDR 0x12C
#define SYSCTRL_CSR_XEMPTY_EN_OFFSET_ADDR 0x130
#define SYSCTRL_CSR_XEMPTY_STATUS_OFFSET_ADDR 0x134
#define SYSCTRL_CSR_SYS_INT_MASK_OFFSET_ADDR 0x138
#define SYSCTRL_CSR_SYS_INT_STAUTS_OFFSET_ADDR 0x13C
#define SYSCTRL_CSR_SYS_INT_SET_OFFSET_ADDR 0x140
#define SYSCTRL_CSR_SYS_RST_REQ_OFFSET_ADDR 0x148
#define SYSCTRL_CSR_SYS_RST_DREQ_OFFSET_ADDR 0x14C
#define SYSCTRL_CSR_SYS_RST_ST_OFFSET_ADDR 0x150
#define SYSCTRL_CSR_RST_OUT_CTRL_REG_OFFSET_ADDR 0x154
#define SYSCTRL_CSR_RST_OUT_DLY_CFG_OFFSET_ADDR 0x158
#define SYSCTRL_CSR_INNER_CHIP_RST_RECORD_OFFSET_ADDR 0x15C
#define SYSCTRL_CSR_BMC_RST_RECORD_OFFSET_ADDR 0x160
#define SYSCTRL_CSR_BMC_UNSE_RST_RECORD_OFFSET_ADDR 0x164
#define SYSCTRL_CSR_HS_RST_RECORD_OFFSET_ADDR 0x168
#define SYSCTRL_CSR_A55_LOCK_REQ_OFFSET_ADDR 0x16C
#define SYSCTRL_CSR_A55_LOC_DREQ_OFFSET_ADDR 0x170
#define SYSCTRL_CSR_A55_LOCK_REQ_ST_OFFSET_ADDR 0x174
#define SYSCTRL_CSR_LOCK_CORE_ST_OFFSET_ADDR 0x178
#define SYSCTRL_CSR_XSEMP_TO_CTRL_OFFSET_ADDR 0x17C
#define SYSCTRL_CSR_TSENSOR_CTRL_OFFSET_ADDR 0x180
#define SYSCTRL_CSR_TSENSOR_OUT_OFFSET_ADDR 0x184
#define SYSCTRL_CSR_XEMPTY_TO_CLR_OFFSET_ADDR 0x188
#define SYSCTRL_CSR_PAD_BMC_CTRL_OFFSET_ADDR 0x18C
#define SYSCTRL_CSR_EFUSE_WAIT_REG_OFFSET_ADDR 0x190
#define SYSCTRL_CSR_WD_TIMER_CLK_EN_OFFSET_ADDR 0x194
#define SYSCTRL_CSR_MISC_ST_OFFSET_ADDR 0x19C
#define SYSCTRL_CSR_MISC_CTRL_OFFSET_ADDR 0x200
#define SYSCTRL_CSR_HEARTBEAT_CNT_CTRL_OFFSET_ADDR 0x204
#define SYSCTRL_CSR_JTAG0_MODE_SEL_OFFSET_ADDR 0x208
#define SYSCTRL_CSR_DAC_CTL_OFFSET_ADDR 0x20C
#define SYSCTRL_CSR_DAC_STATUS_OFFSET_ADDR 0x210
#define SYSCTRL_CSR_CORE_TAP_SEL_OFFSET_ADDR 0x214
#define SYSCTRL_CSR_PROBE_CTRL_OFFSET_ADDR 0x218
#define SYSCTRL_CSR_RW_EXCLUSIVE_OFFSET_ADDR 0x21C
#define SYSCTRL_CSR_PCIE0_RST_RECORD_OFFSET_ADDR 0x220
#define SYSCTRL_CSR_PCIE1_RST_RECORD_OFFSET_ADDR 0x224
#define SYSCTRL_CSR_WARM_RET_LOCK1_OFFSET_ADDR 0x228
#define SYSCTRL_CSR_WARM_RET_LOCK2_OFFSET_ADDR 0x22C
#define SYSCTRL_CSR_DIEID0_OFFSET_ADDR 0x230
#define SYSCTRL_CSR_DIEID1_OFFSET_ADDR 0x234
#define SYSCTRL_CSR_DIEID2_OFFSET_ADDR 0x238
#define SYSCTRL_CSR_DIEID3_OFFSET_ADDR 0x23C
#define SYSCTRL_CSR_DIEID4_OFFSET_ADDR 0x240
#define SYSCTRL_CSR_DIEID5_OFFSET_ADDR 0x244
#define SYSCTRL_CSR_DIEID6_OFFSET_ADDR 0x248
#define SYSCTRL_CSR_DIEID7_OFFSET_ADDR 0x24C
#define SYSCTRL_CSR_DIEID8_OFFSET_ADDR 0x250
#define SYSCTRL_CSR_DIEID9_OFFSET_ADDR 0x254
#define SYSCTRL_CSR_DIEID10_OFFSET_ADDR 0x258
#define SYSCTRL_CSR_DIEID11_OFFSET_ADDR 0x25C
#define SYSCTRL_CSR_FUNC_BIST_CTRL_OFFSET_ADDR 0x260
#define SYSCTRL_CSR_FUNC_CPU_DATAIN_OFFSET_ADDR 0x264
#define SYSCTRL_CSR_FUNC_CPU_DATAOUT_OFFSET_ADDR 0x268

/**
 *
 * @ingroup bm_sysctrl
 * @brief sysctrl register read.
 *
 * @par description:
 * sysctrl register read.
 *
 * @param offset [in]: register offset
 * @param val [out]: read value
 *
 * @return BM_OK/BM_FAIL
 *
 */
int bm_sysctrl_reg_read(unsigned int offset, unsigned int *val);

/**
 *
 * @ingroup bm_sysctrl
 * @brief sysctrl register write.
 *
 * @par description:
 * sysctrl register write.
 *
 * @param offset [in]: register offset
 * @param val [out]: write value
 *
 * @return BM_OK/BM_FAIL
 *
 */
int bm_sysctrl_reg_write(unsigned int offset, unsigned int val);

/**
 *
 * @ingroup bm_sysctrl
 * @brief sysctrl register read write.
 *
 * @par description:
 * sysctrl register read write.
 *
 * @param offset [in]: register offset
 * @param mask [in]: write mask
 * @param val [out]: write value
 *
 * @return BM_OK/BM_FAIL
 *
 */
int bm_sysctrl_reg_read_write(unsigned int offset, unsigned int mask, unsigned int val);

/**
 *
 * @ingroup bm_sysctrl
 * @brief sysctrl register set bits.
 *
 * @par description:
 * sysctrl register set bits.
 *
 * @param offset [in]: register offset
 * @param mask [in]: write mask
 * @param set_bit [in]: 0/1
 *
 * @return BM_OK/BM_FAIL
 *
 */
int bm_sysctrl_reg_set_bits(unsigned int offset, unsigned int mask, unsigned int set_bit);

#endif /* __BM_SYSCTRL_H__ */