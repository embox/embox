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
#if 1
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
#endif

	lis	    r1, _stack_vma@ha
	ori	r1, r1, _stack_vma@l
	lis    r3, _stack_len@ha
	ori    r3, r3, _stack_len@l
	add r1,r1, r3

	mtspr	SPR_SPRG0, r1		/* Keep kernel stack */

	/*
	 * Clear kernel BSS
	 */
	lis	r3, _bss_vma@ha
	addi	r3, r3, _bss_vma@l
	lis	r4, _bss_len@ha
	addi	r4, r4, _bss_len@l
	add r4, r3, r4
	li	r0, 0
1:
	stwu	r0, 4(r3)
	cmplw	cr0, r3, r4
	blt	1b

	b	kernel_start
