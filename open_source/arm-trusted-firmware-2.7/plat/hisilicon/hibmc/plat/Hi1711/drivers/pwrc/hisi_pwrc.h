/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_ipc
 * Author: xxx
 * Create: 2022-11-16
 */

#ifndef __HISI_PWRC_H__
#define __HISI_PWRC_H__

#include "../hi1711_def.h"

#define CPU_CLUSTER_CFG  0x01000E0000
#define SYSCTRL_CFG_BASE 0x010C010000

#define PMC_FCM_PCTRL   (CPU_CLUSTER_CFG + 0x8008) /* FCM P-Channel控制 */
#define PMC_CORE0_PCTRL (CPU_CLUSTER_CFG + 0x8018) /* Core0 P-Channel控制 */
#define PMC_CORE1_PCTRL (CPU_CLUSTER_CFG + 0x801C)
#define PMC_CORE2_PCTRL (CPU_CLUSTER_CFG + 0x8020)
#define PMC_CORE3_PCTRL (CPU_CLUSTER_CFG + 0x8024)
#define PMC_CORE4_PCTRL (CPU_CLUSTER_CFG + 0x8028)
#define PMC_CORE5_PCTRL (CPU_CLUSTER_CFG + 0x802C)
#define PMC_CORE6_PCTRL (CPU_CLUSTER_CFG + 0x8030)
#define PMC_CORE7_PCTRL (CPU_CLUSTER_CFG + 0x8034)

#define PMC_CORE_PDC_ENABLE     (CPU_CLUSTER_CFG + 0x80C0) /* Core PDC使能 */
#define PMC_CORE_POWER_UP_REQ   (CPU_CLUSTER_CFG + 0x80C4) /* Core power-up请求 */
#define PMC_CORE_POWER_DOWN_REQ (CPU_CLUSTER_CFG + 0x80C8) /* Core power down请求 */

#define SC_CLUS_CRG_RESET_REQ  (CPU_CLUSTER_CFG + 0xA00)
#define SC_CLUS_CRG_RESET_DREQ (CPU_CLUSTER_CFG + 0xA04)
#define SC_CLUS_CRG_RESET_ST   (CPU_CLUSTER_CFG + 0x5A00)

#define SC_CLUS_CRG_CLK_EN (CPU_CLUSTER_CFG + 0x3000)

#define CORE0_PDC_EN    0
#define CORE0_PWRUP_REQ 0
#define CORE0_PWRDN_REQ 0

#define SC_CLUS0_CTRL_RVBA0 (CPU_CLUSTER_CFG + 0x2128)
#define CRG_RVBAR(c, n)     ((0x10 * (c)) + (0x4 * (n)))

#define SC_BAK_DATA0 (SYSCTRL_CFG_BASE + 0x3410)
/* 1711 add start */
#define ACTLR_ELX_PWREN (1ul << 7)

#define MAX_1711_CORE_NUM 4

/* ---------start A55 CSR REG------------- */
#define A55_CSR_BASE_ADDR         0x14000000
#define A55_CORE0_RVBA_OFFSET     0x10
#define A55_SOFT_RESET_REG_OFFSET 0x0

#define COREX_PWUP_RESET(cpu)  (1ul << (cpu))
#define COREX_NPWUP_RESET(cpu) (1ul << ((cpu) + 4))

#define A55_SOFT_DRESET_REG_OFFSET 0x74
#define COREX_PWUP_DRESET(cpu)     (1ul << (cpu))
#define COREX_NPWUP_DRESET(cpu)    (1ul << ((cpu) + 4))
/* ---------end A55 CSR REG--------------- */
void com_core_pwdown_a55_init(uint32_t cpu, uint32_t reset_addr);
void com_core_pwdown_enter_wfi(void);

void set_a55_coren_rvba(uint32_t cpu, uint32_t rvba_addr);
void pdc_switch(uint32_t cpu, uint32_t enable);

/* 1711 add end */
void hi1711_pmc_pctrl_enable(void);

void hi1711_powerup_core(unsigned int cluster, unsigned int core);

void hi1711_powerdn_core(unsigned int cluster, unsigned int core);

void hi1711_disable_core(unsigned int cluster, unsigned int core);

void hi1711_disable_cluster(unsigned int cluster, unsigned int core);

void hi1711_powerdn_cluster(unsigned int cluster, unsigned int core);

#endif /* __HISI_PWRC_H__ */
