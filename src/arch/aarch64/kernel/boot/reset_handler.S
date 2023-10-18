/**
 * @file
 * @brief
 *
 * @date 17.10.23
 * @author Aleksey Zhmulin
 */

.section .text
.global aarch64_reset_handler

aarch64_reset_handler:
	/* Setup stack. Note: you can't write SP directly */
	ldr     x0, =_stack_top
	add     sp, x0, #0

	/* Enable SIMD for q0-q7 register access */
	mov     x1, #(0x3 << 20)
	msr     cpacr_el1, x1

	/* Assume current EL is EL1 */
	adr     x0, aarch64_trap_table
	msr     vbar_el1, x0
	mov     x0, #3 << 20
	msr     cpacr_el1, x0 /* Enable FP/SIMD */

	/* Zero .bss */
	ldr     x0, =_bss_vma
	mov     x1, #0
	ldr     x2, =_bss_len
bss_loop:
	str     x1, [x0, 0]
	add     x0, x0, #8
	subs    x2, x2, #8
	b.gt    bss_loop

	/* Copy .data */
	ldr     x0, =_data_vma
	ldr     x1, =_data_lma
	ldr     x2, =_data_len
data_loop:
	ldr     x3, [x1, 0]
	str     x3, [x0, 0]
	add     x0, x0, #8
	add     x1, x1, #8
	subs    x2, x2, #8
	b.gt    data_loop

	/* Jump to arch-independent part */
	bl      kernel_start

	/* returning from kernel_start */
die:
	b       die
