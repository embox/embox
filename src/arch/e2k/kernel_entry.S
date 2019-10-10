/**
 * @file
 * @brief
 *
 * @date Mar 13, 2018
 * @author Anton Bondarev
 */

/*
 * Trap table entries implemented on assembler
 */

#include <asm/ptrace.h>

.global ttable_entry0;

.section .ttable_entry0, "ax",@progbits
.align 8
.type ttable_entry0,@function
ttable_entry0:
	setwd   wsz = 13, nfx = 1; /* TODO Try to figure out what these values mean */
	rrd %ctpr1, %dr1
	rrd %ctpr2, %dr2
	rrd %ctpr3, %dr3
	rrd %ilcr,  %dr4
	rrd %lsr,   %dr5

	/* sizeof pt_regs */
	getsp -(E2K_PT_REGS_SIZE), %dr0

	std %dr1, [%dr0 + E2K_PT_REGS_CTRP1] /* regs->ctpr1 = ctpr1 */
	std %dr2, [%dr0 + E2K_PT_REGS_CTRP2] /* regs->ctpr2 = ctpr2 */
	std %dr3, [%dr0 + E2K_PT_REGS_CTRP3] /* regs->ctpr3 = ctpr3 */

	std %dr4, [%dr0 + E2K_PT_REGS_ILCR]  /* regs->ilcr = ilcr */
	std %dr5, [%dr0 + E2K_PT_REGS_LSR]   /* regs->lsr = lsr */

	disp    %ctpr1, e2k_entry
	ct      %ctpr1
.size  $ttable_entry0,.-$ttable_entry0


.global ttable_entry1;

.section .ttable_entry1, "ax",@progbits
.align 8
.type ttable_entry1,@function
ttable_entry1:
	{
	setwd wsz = 0x4
	disp %ctpr1, cpuSyscall
	}
	{
	puttagd %dr0, 0, %dr0
	puttagd %dr1, 0, %dr1
	}
	{
	puttagd %dr2, 0, %dr2
	puttagd %dr3, 0, %dr3
	}
	{
	puttagd %dr4, 0, %dr4
	ct %ctpr1
	}
.size  $ttable_entry1,.-$ttable_entry1



.global ttable_entry2;

.section .ttable_entry2, "ax",@progbits
.align 8
.type ttable_entry2,@function
ttable_entry2:
	{
	setwd wsz = 0x4
	disp %ctpr1, cpuJmpSyscall
	}
	{
	puttagd %dr0, 0, %dr0
	puttagd %dr1, 0, %dr1
	}
	{
	puttagd %dr2, 0, %dr2
	puttagd %dr3, 0, %dr3
	}
	{
	puttagd %dr4, 0, %dr4
	ct %ctpr1
	}
.size $ttable_entry2,.-$ttable_entry2
