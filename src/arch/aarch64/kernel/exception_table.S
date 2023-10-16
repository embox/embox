/**
 * @file exception_table.S
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 17.07.2019
 */

.section .trap_table
.global aarch64_exception_table

.align 11
aarch64_exception_table:

/* All handlers are aligned by 128 bytes */

/* Current exception level with SP0 */
/* Synchronous exception */
.align 7
	b       aarch64_reset_handler

/* IRQ exception */
.align 7
	b       exception_stub

/* FIQ exception */
.align 7
	b       exception_stub

/* System error exception */
.align 7
	b       exception_stub

/* Current exception level with SPx */
/* Synchronous exception */
.align 7
	b       exception_stub

/* IRQ exception */
.align 7
	stp     x29, x30, [sp, #-16]!
	bl      exception_entry
	bl      interrupt_handle
	b       exception_exit

/* FIQ exception */
.align 7
	stp     x29, x30, [sp, #-16]!
	bl      exception_entry
	bl      interrupt_handle
	b       exception_exit

.align 7
	/* System error exception */
	b       exception_stub

/* Lower exception level using Aarch64 */
/* Synchronous exception */
.align 7
	b       exception_stub

/* IRQ exception */
.align 7
	b       exception_stub

/* FIQ exception */
.align 7
	b       exception_stub

/* System error exception */
.align 7

/* Lower exception level using Aarch32 */
/* Synchronous exception */
.align 7
	b       exception_stub

/* IRQ exception */
.align 7
	b       exception_stub

/* FIQ exception */
.align 7
	b       exception_stub

/* System error exception */
.align 7
	b       exception_stub

aarch64_reset_handler:
	/* Setup stack. Note: you can't write SP directly */
	ldr     x0, =_stack_top
	add     sp, x0, #0

	/* Enable SIMD for q0-q7 register access */
	mov     x1, #(0x3 << 20)
	msr     cpacr_el1, x1

	/* Assume current EL is EL1 */
	adr     x0, aarch64_exception_table
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
	b.gt bss_loop

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
	b.gt data_loop

	/* Jump to arch-independent part */
	b       kernel_start

exception_stub:
	b       exception_stub

exception_entry:
	stp     x27, x28, [sp, #-16]!
	stp     x25, x26, [sp, #-16]!
	stp     x23, x24, [sp, #-16]!
	stp     x21, x22, [sp, #-16]!
	stp     x19, x20, [sp, #-16]!
	stp     x17, x18, [sp, #-16]!
	stp     x15, x16, [sp, #-16]!
	stp     x13, x14, [sp, #-16]!
	stp     x11, x12, [sp, #-16]!
	stp     x9, x10, [sp, #-16]!
	stp     x7, x8, [sp, #-16]!
	stp     x5, x6, [sp, #-16]!
	stp     x3, x4, [sp, #-16]!
	stp     x1, x2, [sp, #-16]!

	mrs     x1, esr_el1
	mrs     x2, elr_el1
	mrs     x3, daif
	mrs     x4, vbar_el1
	mrs     x5, spsr_el1
	sub     x6, sp, #(8*30)
	mrs     x7, sctlr_el1
	mov     x8, #0
	mrs     x9, ttbr0_el1
	stp     x2, x0, [sp, #-16]!
	stp     x3, x1, [sp, #-16]!
	stp     x5, x4, [sp, #-16]!
	stp     x7, x6, [sp, #-16]!
	stp     x9, x8, [sp, #-16]!
	mov     x0, sp
	ret

exception_exit:
	add     sp, sp, #(8*8)
	ldp     x2, x0, [sp], #16
	msr     elr_el1, x2
	ldp     x1, x2, [sp], #16
	ldp     x3, x4, [sp], #16
	ldp     x5, x6, [sp], #16
	ldp     x7, x8, [sp], #16
	ldp     x9, x10, [sp], #16
	ldp     x11, x12, [sp], #16
	ldp     x13, x14, [sp], #16
	ldp     x15, x16, [sp], #16
	ldp     x17, x18, [sp], #16
	ldp     x19, x20, [sp], #16
	ldp     x21, x22, [sp], #16
	ldp     x23, x24, [sp], #16
	ldp     x25, x26, [sp], #16
	ldp     x27, x28, [sp], #16
	ldp     x29, x30, [sp], #16
	eret
