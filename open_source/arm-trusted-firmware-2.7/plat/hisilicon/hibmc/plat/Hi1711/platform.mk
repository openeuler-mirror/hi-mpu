#
# Copyright (c) 2022, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

CRASH_CONSOLE_BASE            := PL011_UART6_BASE
COLD_BOOT_SINGLE_CPU          := 1
PROGRAMMABLE_RESET_ADDRESS    := 1
MULTI_CONSOLE_API             := 1
CRASH_REPORTING               := 1
GICV3_IMPL                    := GIC600
CTX_INCLUDE_AARCH32_REGS := 0

BUILD_MESSAGE_TIMESTAMP       :=  __TIME__", "__TIME__
VERSION_STRING                :=  v0.00:0

$(eval $(call add_define,CRASH_REPORTING))
# Process flags
$(eval $(call add_define,CRASH_CONSOLE_BASE))
$(eval $(call add_define,HIBMC))
#$(eval $(call add_define,PLAT_HELPERS_MK))
$(eval $(call add_define,BUILD_MESSAGE_TIMESTAMP))
$(eval $(call add_define,VERSION_STRING))

ENABLE_PLAT_COMPAT	:=	0

USE_COHERENT_MEM	:=	1

PLAT_INCLUDES := -Iinclude/common/tbbr  \
                 -Iplat/${PLAT}/include \
                 -Iplat/${PLAT}

# Include GICv3 driver files
include drivers/arm/gic/v3/gicv3.mk
						
PLAT_BL_COMMON_SOURCES := drivers/arm/8250/8250_console.S           \
                          drivers/delay_timer/delay_timer.c         \
                          drivers/delay_timer/generic_delay_timer.c \
                          lib/xlat_tables/xlat_tables_common.c      \
                          lib/xlat_tables/aarch64/xlat_tables.c     \
                          plat/${PLAT}/aarch64/hi1711_common.c 

				
GIC_SOURCES := ${GICV3_SOURCES}                     \
               plat/common/plat_gicv3.c

BL31_SOURCES  += drivers/arm/cci/cci.c                         \
                 lib/cpus/aarch64/cortex_a53.S                 \
                 lib/cpus/aarch64/cortex_a72.S                 \
                 lib/cpus/aarch64/cortex_a55.S                 \
                 lib/cpus/aarch64/cortex_a73.S                 \
                 lib/cpus/aarch64/aem_generic.S                \
                 plat/common/plat_psci_common.c                \
                 plat/${PLAT}/aarch64/hi1711_helpers.S         \
                 plat/${PLAT}/drivers/nmi/hi1711_nmi_private.c \
                 plat/${PLAT}/hi1711_bl31_setup.c              \
                 plat/${PLAT}/hi1711_pm.c	                   \
                 plat/${PLAT}/hi1711_topology.c                \
                 plat/${PLAT}/drivers/pwrc/hisi_pwrc.c         \
                 plat/${PLAT}/drivers/ipc/hisi_ipc.c           \
                 plat/${PLAT}/hi1711_gicv3.c                   \
                 ${GIC_SOURCES}
				