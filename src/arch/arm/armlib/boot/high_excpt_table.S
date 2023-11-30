/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 17.08.2015
 */
#include <hal/reg.h>

.section .text

.global setup_excpt_table

setup_excpt_table:
	ldr     r0, =0xffff0000
	ldr     r1, =arm_trap_table
	cmp     r0, r1
	beq     1f
	ldmia   r1!, {r2-r9}
	stmia   r0!, {r2-r9}
1:
	mrc     p15, 0, r0, c1, c0, 0
	orr     r0, r0, #SCTLR_V
	mcr     p15, 0, r0, c1, c0, 0

	bx      lr
