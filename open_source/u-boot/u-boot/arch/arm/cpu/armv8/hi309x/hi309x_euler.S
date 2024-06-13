#include <asm/macro.h>
#include <configs/hi309x_memmap.h>
#include <configs/hi309x.h>
.global slave_cores_run
.global slave_pwr_down
.globl master_switch_el
.globl master_core_run
.globl hi309x_core_hd_pwdown

slave_cores_run:
	wfe
	ldr	x1, =CPU_RELEASE_ADDR
	ldr	x0, [x1]
	cbz	x0, slave_cores_run
	br	x0			/* branch to the given address */

master_core_run:
	ldr x1, =0x874021c
	ldr w0, =0xFFFFFFF0
	str w0, [x1]
sram_init:
	/*Ensure SRAM WR_BACK is enabled*/
	ldr x0, =0x0871d3bc
	mov w1, #1
	str w1, [x0]
	ldr x0, =0x0871d264 /* LP_SUBSYS_CSR */
	ldr w1, [x0]
	orr w1, w1, #1		/* BIT0 - LP_SUBSYS_CSR.sram_ini_start */
	str w1, [x0]
sram_init_wait:
	ldr w1, [x0]
	and w1, w1, #2		/* BIT1 - LP_SUBSYS_CSR.sram_ini_done */
	cmp w1, #2
	bne sram_init_wait

	/* resources init done wake up slave */
	/* ARM recommends that software includes a DSB instruction before any SEV instruction.
		A DSB instruction ensures that no instructions, including any SEV instructions,
		that appear in program order after the DSB instruction, can execute until the
		DSB instruction has completed */
	dsb sy
	/* This causes an event to be signaled to all PEs in the multiprocessor system */
	sev
	/* clear master event flag */
	wfe

	ldr	x0, =(CONFIG_SYS_INIT_SP_ADDR)
	bic	sp, x0, #0xf	/* 16-byte alignment for ABI compliance */
	mov	x0, sp
	bl board_early_init_f   /*Init uart, using bypass clk(1.25Mhz)*/
	bl wdg_disable
	bl pmc_wait_all_slave_pwr_done
	bl sys_pll_init
	bl board_early_init_f /*Init uart again, using PLL output clk(100Mhz)*/
#ifdef CONFIG_HI3093_SPI_BOOT
	bl dram_init
	bl prepare_uboot
#endif
	ldr x3, =_main
	blr x3


slave_pwr_down:
	wfe
	branch_slavex x1, cpu1_l, cpu2_l, cpu3_l
cpu1_l:
	ldr x0, =0x8110000
	mov x1, #1
	b slave_com
cpu2_l:
	ldr x0, =0x810f800
	mov x1, #2
	b slave_com
cpu3_l:
	ldr x0, =0x810f000
	mov x1, #3
slave_com:
	bic sp, x0, #0xf	// 16-byte alignment for ABI compliance
	mov	sp, x0
	mov x0, x1
	b hi309x_core_hd_pwdown