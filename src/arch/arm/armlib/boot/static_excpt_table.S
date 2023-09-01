/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 17.08.2015
 */
.section .text

.global setup_excpt_table

setup_excpt_table:
	ldr     r0, =arm_trap_table
	mcr     p15, 0, r0, c12, c0, 0
	bx      lr
