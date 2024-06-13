/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#ifndef __HWI_INIT_H__
#define __HWI_INIT_H__

#define TEST_CLK_INT               30

#define OS_GIC_VER                 3
#define SICR_ADDR_OFFSET_PER_CORE  0x20000U
#define GIC_REG_BASE_ADDR          0x24000000ULL

#define GICD_CTLR_S_ADDR           (GIC_REG_BASE_ADDR + 0x0000U)
#define GICD_IGROUPN_ADDR          (GIC_REG_BASE_ADDR + 0x0080U)
#define GICD_ISENABLER0_ADDR       (GIC_REG_BASE_ADDR + 0x0100U)
#define GICD_ICENABLER0_ADDR       (GIC_REG_BASE_ADDR + 0x0180U)
#define GICD_IPRIORITYN_ADDR       (GIC_REG_BASE_ADDR + 0x0400U)
#define GICD_IGRPMODRN_ADDR        (GIC_REG_BASE_ADDR + 0x0D00U)

#define GICR_BASE0                 (GIC_REG_BASE_ADDR + 0x40000U)
#define GICR_BASE1                 (GIC_REG_BASE_ADDR + 0x50000U)

#define GICR_CTRL_ADDR             (GICR_BASE0 + 0x0000U)
#define GICR_WAKER_ADDR            (GICR_BASE0 + 0x0014U)

#define GICR_IGROUPR0_ADDR         (GICR_BASE1 + 0x0080U)
#define GICR_ISENABLER0_ADDR       (GICR_BASE1 + 0x0100U)
#define GICR_ICENABLER0_ADDR       (GICR_BASE1 + 0x0180U)
#define GICR_IGRPMODR0_ADDR        (GICR_BASE1 + 0x0D00U)

#define MAX_INT_NUM                288
#define MIN_GIC_SPI_NUM            32
#define SICD_IGROUP_INT_NUM        32
#define SICD_REG_SIZE              4

#define GROUP_MAX_BPR              0x7U
#define GROUP0_BP                  0
#define GROUP1_BP                  0

#define PRIO_MASK_LEVEL            0xFFU

#define ICC_SRE_EL1                S3_0_C12_C12_5
#define ICC_BPR0_EL1               S3_0_C12_C8_3
#define ICC_BPR1_EL1               S3_0_C12_C12_3
#define ICC_IGRPEN1_EL1            S3_0_C12_C12_7
#define ICC_PMR_EL1                S3_0_C4_C6_0

#define PARAS_TO_STRING(x...)      #x
#define REG_ALIAS(x...)            PARAS_TO_STRING(x)

#define GIC_REG_READ(addr)         (*(volatile U32 *)((uintptr_t)(addr)))
#define GIC_REG_WRITE(addr, data)  (*(volatile U32 *)((uintptr_t)(addr)) = (U32)(data))

#endif /* __HWI_INIT_H__ */
