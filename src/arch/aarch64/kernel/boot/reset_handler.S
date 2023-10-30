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
	cmp     x0, #0b1000 /* EL value is stored in bits 2 and 3. */
	beq     el2_entry
	blo     el1_entry

el3_entry:
	/* Initialize SCTLR_EL2 and HCR_EL2 before entering EL2. */
	msr     SCTLR_EL2, xzr
	msr     HCR_EL2, xzr
	/* Determine the EL2 Execution state. */
	mrs     x0, SCR_EL3
	orr     x0, x0, #(1 << 10) /* RW=1 EL2 Execution state is AArch64. */
	orr     x0, x0, #(1 << 0) /* NS EL1 is Non-secure world. */
	msr     SCR_EL3, x0
	mov     x0, #0b01001
	msr     SPSR_EL3, x0
	/* Determine EL2 entry. */
	adr     x0, el2_entry
	msr     ELR_EL3, x0
	eret

el2_entry:
	/* Initialize the SCTLR_EL1 register before entering EL1. */
	msr     SCTLR_EL1, xzr
	/* Determine the EL1 Execution state. */
	mrs     x0, HCR_EL2
	orr     x0, x0, #(1 << 31) /* RW=1 EL1 Execution state is AArch64. */
	msr     HCR_EL2, x0
	mov     x0, #0b00101
	msr     SPSR_EL2, x0
	/* Determine EL1 entry. */
	adr     x0, el1_entry
	msr     ELR_EL2, x0
	eret

el1_entry:
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
