/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    10.09.2012
 */

#include <asm/modes.h>
#include <asm/regs.h>
#include <framework/mod/options.h>


	.pushsection .monitor_trap_table, "ax"

	.align	5
/* the vector table for secure state and HYP mode */
.global _monitor_vectors
_monitor_vectors:
	.word 0	/* reset */
	.word 0 /* undef */
	adr pc, _secure_monitor
	.word 0
	.word 0
	.word 0
	.word 0
	.word 0

_secure_monitor:
    stmfd sp!, {r0-r12, r14}
    mrs r0, CPSR
    mrs r1, SPSR
    stmfd sp!, {r0, r1}
    mov r0, sp
    b arm_exception_handler

    .popsection
