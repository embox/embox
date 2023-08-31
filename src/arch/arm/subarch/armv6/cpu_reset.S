/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 31.08.23
 */
.extern arm_trap_table
.global hardware_init_hook

hardware_init_hook:
	/* setup trap table base address */
	ldr     r0, =arm_trap_table
	mcr     p15, 0, r0, c12, c0, 0

	bx      lr
