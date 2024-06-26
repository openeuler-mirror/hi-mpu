/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 1711 helpers
 * Author: xxx
 * Create: 2022-11-16
 */
#include <arch.h>
#include <asm_macros.S>
#include <cortex_a53.h>
#include <cortex_a73.h>
#include "../hi1711_def.h"

	.globl	plat_my_core_pos
	.globl	plat_arm_calc_core_pos
	.globl	platform_mem_init
	.globl	plat_crash_console_init
	.globl	plat_crash_console_putc
	.globl	plat_crash_console_flush
	.globl	plat_report_exception
	.globl	plat_reset_handler
	.globl	set_retention_ticks
	.globl	clr_retention_ticks
	.globl	clr_ex
	.globl	nop

func plat_my_core_pos
    mrs x0, mpidr_el1       /* Read Multiprocessor Affinity Register */
    lsr x1, x0,  #8         /* Shift CORE ID to LSB */
    ands x1, x1, #0xff      /* Mask off, leaving the CPU ID field*/
    lsr x2, x0,  #16        /* Shift cluster ID to LSB */
    ands x2, x2, #0x03      /* Mask off, leaving the cluster ID field */
    add x0, x1, x2, lsl #2  /* the last cpuid = (cluster ID) * 4 + CPU ID */
    ret
endfunc plat_my_core_pos

    /* -----------------------------------------------------
     *  unsigned int plat_arm_calc_core_pos(u_register_t mpidr)
     *  Helper function to calculate the core position.
     *  With this function: CorePos = (ClusterId * 4) +
     *                    CoreId
     * -----------------------------------------------------
     */
func plat_arm_calc_core_pos
    lsr x1, x0,  #8         /* Shift CORE ID to LSB */
    ands x1, x1, #0xff      /* Mask off, leaving the CPU ID field*/
    lsr x2, x0,  #16        /* Shift cluster ID to LSB */
    ands x2, x2, #0x03      /* Mask off, leaving the cluster ID field */
    add x0, x1, x2, lsl #3  /* the last cpuid = (cluster ID) * 8 + CPU ID */
    ret
endfunc plat_arm_calc_core_pos

	/* -----------------------------------------------------
	 * void platform_mem_init(void);
	 *
	 * We don't need to carry out any memory initialization
	 * on HIKEY. The Secure RAM is accessible straight away.
	 * -----------------------------------------------------
	 */
func platform_mem_init
    /* Save UEFI address.
    Description: Coz the ELR_EL3 would be moderated in TF,
    so we need to save the ELR_EL3 to SRAM,
    So that we could return to the UEFI when run off the TF
    */
    mrs x0, elr_el3
    ldr x1, =ELR_PEI_SAVE_ADDR
    str x0, [x1]
    ret
endfunc platform_mem_init

	/* ---------------------------------------------
	 * int plat_crash_console_init(void)
	 * Function to initialize the crash console
	 * without a C Runtime to print crash report.
	 * Clobber list : x0, x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_init
	mov_imm	x0, HI1711_UART_BASE
	mov_imm	x1, HI1711_UART_CLK_IN_HZ
	mov_imm	x2, Hi1711_BAUDRATE
	//b	console_core_init
endfunc plat_crash_console_init

	/* ---------------------------------------------
	 * int plat_crash_console_putc(int c)
	 * Function to print a character on the crash
	 * console without a C Runtime.
	 * Clobber list : x1, x2
	 * ---------------------------------------------
	 */
func plat_crash_console_putc
	mov_imm	x1, HI1711_UART_BASE
	b	console_8250_putc
endfunc plat_crash_console_putc

	/* ---------------------------------------------
	 * int plat_crash_console_flush()
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * Out : return -1 on error else return 0.
	 * Clobber list : r0
	 * ---------------------------------------------
	 */
func plat_crash_console_flush
	mov_imm	x0, HI1711_UART_BASE
	b	console_8250_core_flush
endfunc plat_crash_console_flush

func plat_report_exception
	mov	x8, x30
    b .
	ldr	x2, =0xf7020000
	and	x1, x0, #1
	str	w1, [x2, #4]
	and	x1, x0, #2
	str	w1, [x2, #8]
	and	x1, x0, #4
	str	w1, [x2, #16]
	and	x1, x0, #8
	str	w1, [x2, #32]

	mrs	x2, currentel
	and	x2, x2, #0x0c
	cmp	x2, #0x04
	beq	plat_report_el1

	adr	x4, plat_err_str
	bl	asm_print_str

	adr	x4, esr_el3_str
	bl	asm_print_str

	mrs	x4, esr_el3
	bl	asm_print_hex

	adr	x4, elr_el3_str
	bl	asm_print_str

	mrs	x4, elr_el3
	bl	asm_print_hex
	b	plat_report_end

plat_report_el1:
	adr	x4, plat_err_str
	bl	asm_print_str

	adr	x4, esr_el1_str
	bl	asm_print_str

	mrs	x4, esr_el1
	bl	asm_print_hex

	adr	x4, elr_el1_str
	bl	asm_print_str

	mrs	x4, elr_el1
	bl	asm_print_hex
plat_report_end:
	mov	x30, x8
	ret
endfunc plat_report_exception

func plat_reset_handler
	ret
endfunc plat_reset_handler

func set_retention_ticks
	mrs	x0, CORTEX_A53_ECTLR_EL1
	bic	x0, x0, #CORTEX_A53_ECTLR_CPU_RET_CTRL_MASK
	orr	x0, x0, #RETENTION_ENTRY_TICKS_8
	msr	CORTEX_A53_ECTLR_EL1, x0
	isb
	dsb	sy
	ret
endfunc set_retention_ticks

func clr_retention_ticks
	mrs	x0, CORTEX_A53_ECTLR_EL1
	bic	x0, x0, #CORTEX_A53_ECTLR_CPU_RET_CTRL_MASK
	msr	CORTEX_A53_ECTLR_EL1, x0
	isb
	dsb	sy
	ret
endfunc clr_retention_ticks

func clr_ex
	clrex
	ret
endfunc clr_ex

func nop
	nop
	ret
endfunc nop

.section .rodata.rev_err_str, "aS"
plat_err_str:
	.asciz "\nPlatform exception reporting:"
esr_el3_str:
	.asciz "\nESR_EL3: "
elr_el3_str:
	.asciz "\nELR_EL3: "
esr_el1_str:
	.asciz "\nESR_EL1: "
elr_el1_str:
	.asciz "\nELR_EL1: "
