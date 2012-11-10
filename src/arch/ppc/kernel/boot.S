/**
 * @file
 *
 * @date 17.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/regs.h>

/**
 * Kernel start point
 */
C_ENTRY(_start)
	/**
	 * Setup Machine State Register
	 */
	mfmsr   r3                 /* load MSR */
#if 0
	lis     r4, MSR_ILE@h      /* setup exception handling to big-endian mode */
	ori     r4, r4, MSR_ILE@l
	andc    r3, r3, r4
	lis     r4, MSR_LE@h       /* setup big-endian mode */
	ori     r4, r4, MSR_LE@l
	andc    r3, r3, r4
#endif
	lis     r4, MSR_PR@h       /* allow user- and supervisor-level instructions */
	ori     r4, r4, MSR_PR@l
	andc    r3, r3, r4
#if 0
	lis     r4, MSR_IP@h       /* setup exception prefix to 0x000n_nnnn */
	ori     r4, r4, MSR_IP@l
	andc    r3, r3, r4
#endif
	mtmsr   r3                 /* set a new MSR */
	isync                      /* synchromizing */

#if 0
	li	r3, 0			/* Init interrupt nest count */
	mtsprg2 3

	li	r3, 0			/* Reset timebase */
	mttbl	r3
	mttbu	r3
	mttbl	r3
#endif

	/**
	 * Keep kernel stack
	 */
	lis     r1, _stack_vma@h
	ori     r1, r1, _stack_vma@l
	lis     r2, _stack_len@h
	ori     r2, r2, _stack_len@l
	add     r1, r1, r2
	mtsprg0 r1

	/**
	 * Clear kernel BSS
	 */
	lis     r3, _bss_vma@h
	ori     r3, r3, _bss_vma@l
	lis     r4, _bss_len@h
	ori     r4, r4, _bss_len@l
	add     r4, r3, r4
	li      r0, 0
1:
	stwu    r0, 4(r3)
	cmplw   cr0, r3, r4
	blt     1b

	/**
	 * Call C code
	 */
	b       kernel_start
