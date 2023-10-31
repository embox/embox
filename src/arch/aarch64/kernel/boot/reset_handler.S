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
	/* Get current exception level. */
	mrs     x0, CurrentEL
	cmp     x0, #0x8
	beq     el2_entry
	blo     el1_entry

	/* EL3 entry */
	msr     sctlr_el2, xzr
	msr     hcr_el2, xzr
	/* Determine the EL2 Execution state. */
	mrs     x0, scr_el3
	orr     x0, x0, #(1 << 10) /* RW=1 EL2 Execution state is AArch64. */
	orr     x0, x0, #(1 << 0) /* NS EL1 is Non-secure world. */
	msr     scr_el3, x0
	mov     x0, #0b01001
	msr     spsr_el3, x0
	/* Determine EL2 entry. */
	adr     x0, el2_entry
	msr     elr_el3, x0
	eret

el2_entry:
	/* Enable FP/SIMD */
	mov     x0, #0x33ff
	msr     cptr_el2, x0
	/* Set up exception vectors */
	adr     x0, aarch64_trap_table
	msr     vbar_el2, x0
	b       0f

el1_entry:
	/* Enable FP/SIMD */
	mov     x0, #(0x3 << 20)
	msr     cpacr_el1, x0
	/* Set up exception vectors */
	adr     x0, aarch64_trap_table
	msr     vbar_el1, x0

0:
	/* Setup stack. Note: you can't write SP directly */
	ldr     x0, =_stack_top
	add     sp, x0, #0

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
