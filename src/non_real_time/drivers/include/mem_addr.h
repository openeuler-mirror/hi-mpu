// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kbox external interfacer
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _MEM_ADDR_H_
#define _MEM_ADDR_H_

#define DDR_BASE_ADDR                           (0x80000000)

/*************************SINGLE_TYPE Board's DDR address define ***************************/
#define DDR_SINGLE_CHIP_RSVMEM_SIZE             (0x1000000)
#define DDR_SINGLE_CHIP_RSVMEM_BASE             (DDR_BASE_ADDR)

#define DDR_SINGLE_CHIP_PCIE_SIZE               (0x800000)
#define DDR_SINGLE_CHIP_PCIE_BASE               (DDR_SINGLE_CHIP_RSVMEM_BASE + DDR_SINGLE_CHIP_RSVMEM_SIZE)

/*************************BMC_TYPE Board's DDR address define ******************************/
#define DDR_GPU_SIZE                            (0x2000000)
#define DDR_GPU_BASE                            (DDR_BASE_ADDR)

#define DDR_VCE_SIZE                            (0x2800000)
#define DDR_VCE_BASE                            (DDR_GPU_BASE + DDR_GPU_SIZE)

#define DDR_RSVMEM_SIZE                         (0x1000000)
#define DDR_RSVMEM_BASE                         (DDR_VCE_BASE + DDR_VCE_SIZE)

#define DDR_PCIE_SIZE                           (0x800000)
#define DDR_PCIE_BASE                           (DDR_RSVMEM_BASE + DDR_RSVMEM_SIZE)

#define DDR_SOL_SIZE                            (0xe00000)
#define DDR_SOL_BASE                            (DDR_PCIE_BASE + DDR_PCIE_SIZE)

#define DDR_SECURE_SIZE                         (0x300000)
#define DDR_SECURE_BASE                         (DDR_SOL_BASE + DDR_SOL_SIZE)

#define DDR_ATF_SIZE                            (0x100000)
#define DDR_ATF_BASE                            (DDR_SECURE_BASE + DDR_SECURE_SIZE)

#define DDR_OS_SIZE                             (0x20000000)
#define DDR_OS_BASE                             (DDR_ATF_BASE + DDR_ATF_SIZE)

/************************* RSVMEM address details define ******************************/
#define DDR_RSVMEM_GMAC_OFFSET                  (0x0)
#define DDR_RSVMEM_GMAC_SIZE                    (0x4000)

#define DDR_RSVMEM_SDK_OFFSET                   (DDR_RSVMEM_GMAC_OFFSET + DDR_RSVMEM_GMAC_SIZE)
#define DDR_RSVMEM_SDK_SIZE                     (0xc000)

#define DDR_RSVMEM_EDMA_OFFSET                  (DDR_RSVMEM_SDK_OFFSET + DDR_RSVMEM_SDK_SIZE)
#define DDR_RSVMEM_EDMA_SIZE                    (0x10000)

#define DDR_RSVMEM_VNET_OFFSET                  (DDR_RSVMEM_EDMA_OFFSET + DDR_RSVMEM_EDMA_SIZE)
#define DDR_RSVMEM_VNET_SIZE                    (0xe0000)

#define DDR_RSVMEM_KBOX_OFFSET                  (DDR_RSVMEM_VNET_OFFSET + DDR_RSVMEM_VNET_SIZE)
#define DDR_RSVMEM_KBOX_SIZE                    (0x2e0000)

#define DDR_RSVMEM_EKBOX_OFFSET                 (DDR_RSVMEM_KBOX_OFFSET + DDR_RSVMEM_KBOX_SIZE)
#define DDR_RSVMEM_EKBOX_SIZE                   (0x20000)

#define DDR_RSVMEM_PME_OFFSET                   (DDR_RSVMEM_EKBOX_OFFSET + DDR_RSVMEM_EKBOX_SIZE)
#define DDR_RSVMEM_PME_SIZE                     (0xc00000)

/************************* SRAM address define ******************************/
#define SRAM_BASE_ADDR                          (0x8100000)

#endif

