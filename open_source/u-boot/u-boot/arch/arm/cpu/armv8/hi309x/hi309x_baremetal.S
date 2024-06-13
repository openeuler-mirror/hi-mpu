#include <asm/macro.h>
#include <linux/linkage.h>
#include <asm/gic.h>
#include <configs/hi309x_memmap.h>
#include <configs/hi309x.h>
.global slave_cores_run
.global slave_pwr_down
.globl master_switch_el
.globl master_core_run
.globl hi309x_core_hd_pwdown
.globl smp_kick_all_cpus

slave_cores_run:
    branch_slavex x1, slave_cpu1, slave_cpu2, slave_cpu3
slave_cpu1:
	wfe
	ldr	x1, =(CPU_RELEASE_ADDR)
	ldr	x0, [x1]
	cbz	x0, slave_cpu1

	ldr	x2, =(CPU_RELEASE_GDTBL)
	ldr	x18, [x2]

	ldr	x2, =(CPU_RELEASE_VBAR)
	ldr	x0, [x2]
	msr vbar_el1, x0

	ldr	x0, =(HI_CORE1_STACK_BASE)
	bic	sp, x0, #0xf	/* 16-byte alignment for ABI compliance */
	bl	slave_entry			/* branch to the given address */
	b slave_cpu1

slave_cpu2:
	wfe
	ldr	x1, =(CPU_RELEASE_ADDR)
	ldr	x0, [x1]
	cbz	x0, slave_cpu2

	ldr	x2, =(CPU_RELEASE_GDTBL)
	ldr	x18, [x2]

	ldr	x2, =(CPU_RELEASE_VBAR)
	ldr	x0, [x2]
	msr vbar_el1, x0

	ldr	x0, =(HI_CORE2_STACK_BASE)
	bic	sp, x0, #0xf	/* 16-byte alignment for ABI compliance */
	bl	slave_entry			/* branch to the given address */
	b slave_cpu2

slave_cpu3:
	wfe
	ldr	x1, =(CPU_RELEASE_ADDR)
	ldr	x0, [x1]
	cbz	x0, slave_cpu3

	ldr	x2, =(CPU_RELEASE_GDTBL)
	ldr	x18, [x2]

	ldr	x2, =(CPU_RELEASE_VBAR)
	ldr	x0, [x2]
	msr vbar_el1, x0

	ldr	x0, =(HI_CORE3_STACK_BASE)
	bic	sp, x0, #0xf	/* 16-byte alignment for ABI compliance */
	bl	slave_entry			/* branch to the given address */
    b   slave_cpu3


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
	bl sys_pll_init
	bl board_early_init_f /*Init uart again, using PLL output clk(100Mhz)*/
#ifdef CONFIG_HI3093_SPI_BOOT
	bl dram_init
	bl prepare_uboot
#endif
	ldr x3, =_main
	blr x3


master_switch_el:
	adr	x4, master_el2_entry
	ldr	x5, =ES_TO_AARCH64
	bl	armv8_switch_to_el2
master_el2_entry:
	adr	x4, normal_start_flow
	ldr	x5, =ES_TO_AARCH64
	bl	armv8_switch_to_el1
master_reset:
	b	normal_start_flow
	nop
	nop
normal_start_flow:
//	mov x1, #HI_CORE0_STACK_BASE
//	mov	sp, x1
	br  x0

slave_pwr_down:
	ret

ENTRY(gic_kick_slave_cpus)
#if defined(CONFIG_GICV3)
	mov	x9, #(1 << 40)
  	msr	ICC_SGI1R_EL1, x9
	isb
#elif defined(CONFIG_GICV2)
	mov	w9, #0x8000
	movk	w9, #0x100, lsl #16
	str	w9, [x0, GICD_SGIR]
#endif
	ret
ENDPROC(gic_kick_slave_cpus)

ENTRY(smp_kick_all_cpus)
#if defined(CONFIG_GICV2) || defined(CONFIG_GICV3)
    ldr x0, =GICD_BASE
    b   gic_kick_slave_cpus
#endif
    ret
ENDPROC(smp_kick_all_cpus)