/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 12.11.12
 * @author Ilia Vaprol
 */

#include <asm/entry.h>
#include <asm/regs.h>
//#include <asm/ptrace.h>

.globl ext_intr_hnd
ext_intr_hnd:
	SAVE_ALL
	bl interrupt_handle
	RESTORE_ALL
	rfi

.globl decr_hnd
decr_hnd:
//PTRACE_DEBUG
	SAVE_ALL            /* save all registers */

#if 1
	mfsprg2 18
	addi 18,18,1
	mtsprg2 18
#endif

	mftsr    r3         /* clear TSR[DIS] */
	lis      r4, TSR_DIS@h
	ori      r4, r4, TSR_DIS@l
	andc     r3, r3, r4
	mttsr    r3

	subi     r1, r1, 8  /* handle interrupt */
	bl       interrupt_handle
	addi     r1, r1, 8

#if 1
	mfsprg2 18
	subi 18,18,1
	mtsprg2 18
#endif

#if 1 /* reset decrementer */
	lis 18,   10000000@h
	ori 18,18,10000000@l
	mtdec 18
#endif

	RESTORE_ALL         /* restore all registers */
//PTRACE_DEBUG
	rfi                 /* return from interrupt */
