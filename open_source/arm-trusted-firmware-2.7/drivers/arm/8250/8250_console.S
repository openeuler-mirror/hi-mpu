/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 8250 console
 * Author: xxx
 * Create: 2022-11-16
 */
#include <arch.h>
#include <asm_macros.S>
#include <assert_macros.S>
#include <console_macros.S>
#include "uart8250.h"

	.globl	console_8250_core_init
	.globl	console_8250_core_putc
	.globl	console_8250_core_getc
	.globl	console_8250_core_flush

    .globl	console_8250_putc
	.globl	console_8250_getc
	.globl	console_8250_flush

func console_8250_core_init
	cbz	x0, core_init_fail
	cbz	w1, core_init_fail
	cbz	w2, core_init_fail

	str	wzr, [x0, #UART_IER]

	mov	w3, #(UART_MCR_DTR | UART_MCR_RTS)
	str	w3, [x0, #UART_MCR]

	movz	w3, #:abs_g1:115200
	movk	w3, #:abs_g0_nc:115200
	cmp	w2, w3
	b.hi	1f

	lsl	w2, w2, #4
	mov	w3, wzr
	b	2f

1:	lsl	w2, w2, #2
	mov	w3, #2

2:	str	w3, [x0, #UART_HIGHSPEED]

	udiv	w3, w1, w2
	msub	w1, w3, w2, w1
	lsr	w2, w2, #1
	cmp	w1, w2
	cinc	w3, w3, hs

	mov	w1, #(UART_LCR_DLAB | UART_LCR_WLS_8)
	str	w1, [x0, #UART_LCR]

	and	w1, w3, #0xff
	str	w1, [x0, #UART_DLL]
	lsr	w1, w3, #8
	and	w1, w1, #0xff
	str	w1, [x0, #UART_DLH]
	mov	w1, #UART_LCR_WLS_8
	str	w1, [x0, #UART_LCR]

	mov	w1, #(UART_FCR_FIFO_EN | UART_FCR_CLEAR_RCVR |	\
			UART_FCR_CLEAR_XMIT)
	str	w1, [x0, #UART_FCR]

	mov	w0, #1
	ret
core_init_fail:
	mov	w0, wzr
	ret
endfunc console_8250_core_init

	.globl console_8250_register

	/* -----------------------------------------------
	 * int console_8250_register(uintptr_t baseaddr,
	 *     uint32_t clock, uint32_t baud,
	 *     console_8250_t *console);
	 * Function to initialize and register a new PL011
	 * console. Storage passed in for the console struct
	 * *must* be persistent (i.e. not from the stack).
	 * In: x0 - UART register base address
	 *     w1 - UART clock in Hz
	 *     w2 - Baud rate
	 *     x3 - pointer to empty console_8250_t struct
	 * Out: return 1 on success, 0 on error
	 * Clobber list : x0, x1, x2, x6, x7, x14
	 * -----------------------------------------------
	 */
func console_8250_register
	mov	x7, x30
	mov	x6, x3
	cbz	x6, register_fail
	str	x0, [x6, #CONSOLE_T_8250_BASE]

	bl	console_8250_core_init
	cbz	x0, register_fail

	mov	x0, x6
	mov	x30, x7
	finish_console_register 8250 putc=1, getc=1, flush=1

register_fail:
	ret	x7
endfunc console_8250_register

func console_8250_core_putc
	cbz	x1, putc_error
	cmp	w0, #0xA
	b.ne	2f

1:	ldr	w2, [x1, #UART_LSR]
	and	w2, w2, #UART_LSR_THRE
	cbz	w2, 1b
	mov	w2, #0xD
	str	w2, [x1, #UART_THR]

2:	ldr	w2, [x1, #UART_LSR]
	and	w2, w2, #UART_LSR_THRE
	cbz	w2, 2b
	str	w0, [x1, #UART_THR]
	ret
putc_error:
	mov	w0, #-1
	ret
endfunc console_8250_core_putc

func console_8250_putc
#if ENABLE_ASSERTIONS
	cmp	x1, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */
	ldr	x1, [x1, #CONSOLE_T_8250_BASE]
	b	console_8250_core_putc
endfunc console_8250_putc

func console_8250_core_getc
	cbz	x0, getc_error

1:	ldr	w1, [x0, #UART_LSR]
	tbz	w1, #UART_LSR_DR, 1b
	ldr	w0, [x0, #UART_RBR]
	ret
getc_error:
	mov	w0, #-1
	ret
endfunc console_8250_core_getc

func console_8250_getc
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */
	ldr	x0, [x0, #CONSOLE_T_8250_BASE]
	b	console_8250_core_getc
endfunc console_8250_getc

func console_8250_core_flush
	mov	w0, #0
	ret
endfunc console_8250_core_flush

func console_8250_flush
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */
	ldr	x0, [x0, #CONSOLE_T_8250_BASE]
	b	console_8250_core_flush
endfunc console_8250_flush
