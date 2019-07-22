/**
* @file exception_table.S
* @brief 
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 
* @date 17.07.2019
*/

.section .trap_table
.global aarch64_exception_table
aarch64_exception_table:
	b aarch64_reset_handler

aarch64_reset_handler:
	/* Setup stack. Note: you can't write SP directly */
	ldr x0, =_stack_top
	add sp, x0, #0

	/* Enable SIMD for q0-q7 register access */
	mov x1, #(0x3 << 20)
	msr cpacr_el1, x1
	b kernel_start
