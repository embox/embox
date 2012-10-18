/**
 * @file
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */
 #include <asm/cpu.h>
#include <asm/linkage.h>

/*
 * Kernel start point
 */
C_ENTRY(_start)
	/*
	 * Setup CPU registers.
	 */
	li	r3, MSR_IP		/* Establish default MSR value */
	mtmsr	r3

	li	r3, 0			/* Init interrupt nest count */
	mtspr	SPR_SPRG2, 3

	li	r3, 0			/* Reset timebase */
	mttbl	r3
	mttbu	r3
	mttbl	r3


	lis	    r1, _stack_vma@ha
	addi	r1, r1, _stack_vma@l
	addi    r1, r1, _stack_len@ha

	mtspr	SPR_SPRG0, 1		/* Keep kernel stack */

	/*
	 * Clear kernel BSS
	 */
	lis	r3, _bss_vma@ha
	addi	r3, r3, _bss_vma@l
	lis	r4, _bss_len@ha
	addi	r4, r4, _bss_len@l
	li	0, 0
1:
	stwu	0, 4(3)
	cmplw	0, 3, 4
	blt	1b

	b	kernel_start
