// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * scfi opreation
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __SFC_NOR_CFI_H__
#define __SFC_NOR_CFI_H__

#define CFI_MFR_ADDR    0
#define jedec_mfr(info) ((info)->id[CFI_MFR_ADDR])
/* CFI Manufacture ID's */
#define CFI_MFR_MACRONIX 0xC2
#define CFI_MFR_MICRON   0x20
#define CFI_MFR_SPANSION 0x01
#define CFI_MFR_ISSI     0x9D
#define CFI_MFR_SST      0xbf
#define CFI_MFR_WINBOND  0xef
#define CFI_MFR_ATMEL    0x1f

/* PSA:Physical Sector Arch */
#define CFI_PSA_ADDR_SPANSION      0x04
#define CFI_PSA_64KB_SPANSION_VAL  0x01
#define CFI_PSA_256KB_SPANSION_VAL 0x00

#endif
