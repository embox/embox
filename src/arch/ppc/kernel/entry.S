/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 12.11.12
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/traps.h>

	.section .text

ENTRY trap_stub
	stw  r0, PT_REGS_TRAPNO(r1) /* save type of the trap into pt_regs */
	mr   r3, r1                 /* set pointer to pt_regs as first argument */
	subi r1, r1, 8              /* correct stack pointer */
	bl   trap_handle            /* trap handling */
	addi r1, r1, 8              /* restore stack pointer */
	b die                       /* and jump to die */

ENTRY irq_stub
	subi r1, r1, 8              /* correct stack before call C code */
	mr   r3, r0                 /* set interrupt type as first argument */
	bl   interrupt_handle       /* handling interrupt */
	addi r1, r1, 8              /* restore stack pointer */
	RESTORE_ALL                 /* restore all registers */
	rfi                         /* return from interrupt */

die:                            /* infinite loop */
	b die

