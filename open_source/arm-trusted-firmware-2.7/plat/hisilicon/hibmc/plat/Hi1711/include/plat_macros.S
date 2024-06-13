/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: plat_macros
 * Author: xxx
 * Create: 2022-11-16
 */
#ifndef __PLAT_MACROS_S__
#define __PLAT_MACROS_S__

#include <drivers/arm/cci.h>
#include <drivers/arm/gicv2.h>
#include <platform_def.h>

.section .rodata.gic_reg_name, "aS"
gicc_regs:
	.asciz "gicc_hppir", "gicc_ahppir", "gicc_ctlr", ""
gicd_pend_reg:
	.asciz "gicd_ispendr regs (Offsets 0x200 - 0x278)\n"    \
                " Offset:\t\t\tvalue\n"
newline:
	.asciz "\n"
spacer:
	.asciz ":\t\t0x"

.section .rodata.cci_reg_name, "aS"
cci_iface_regs:
	.asciz "cci_snoop_ctrl_cluster0", "cci_snoop_ctrl_cluster1" , ""

.macro plat_crash_print_regs
.endm

#endif /* __PLAT_MACROS_S__ */
