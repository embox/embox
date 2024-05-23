/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 25.10.2012
 */

#include <arm/fpu.h>
#include <asm/ptrace.h>

.text
.global context_switch
context_switch:
	@ Storing struct context
	stmia   r0!, {r0-lr}
	stmia   r0!, {lr}
	mrs     r3, cpsr
	stmia   r0!, {r3}
	save_fpu_inc tmp=r2, stack=r0

	@ Restoring struct context
	add     r1, r1, #PTREGS_SIZE
	ldmdb   r1, {r3}
	msr     cpsr, r3
	load_fpu_inc tmp=r2, stack=r1
	sub     r1, r1, #PTREGS_SIZE
	ldmia   r1, {r0-pc}
