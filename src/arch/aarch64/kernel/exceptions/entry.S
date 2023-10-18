/**
 * @file
 * @brief
 *
 * @date 17.07.2019
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

.macro excpt_handler
.endm

.section .text

excpt_entry:
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

excpt_exit:
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

excpt_stub:
	b       excpt_stub

.section .trap_table

.global aarch64_trap_table

.align 11
aarch64_trap_table:
/* All handlers are aligned by 128 bytes */

/* Current exception level with SP0 */

.align 7
	/* Synchronous exception */
	b       aarch64_reset_handler

.align 7
	/* IRQ exception */
	b       excpt_stub

.align 7
	/* FIQ exception */
	b       excpt_stub

.align 7
	/* System error exception */
	b       excpt_stub

/* Current exception level with SPx */

.align 7
	/* Synchronous exception */
	b       excpt_stub

.align 7
	/* IRQ exception */
	stp     x29, x30, [sp, #-16]!
	bl      excpt_entry
	bl      interrupt_handle
	b       excpt_exit

.align 7
	/* FIQ exception */
	stp     x29, x30, [sp, #-16]!
	bl      excpt_entry
	bl      interrupt_handle
	b       excpt_exit

.align 7
	/* System error exception */
	b       excpt_stub

/* Lower exception level using Aarch64 */

.align 7
	/* Synchronous exception */
	b       excpt_stub

.align 7
	/* IRQ exception */
	b       excpt_stub

.align 7
	/* FIQ exception */
	b       excpt_stub

.align 7
	/* System error exception */
	b       excpt_stub

/* Lower exception level using Aarch32 */

	/* Synchronous exception */
.align 7
	b       excpt_stub

.align 7
	/* IRQ exception */
	b       excpt_stub

.align 7
	/* FIQ exception */
	b       excpt_stub

.align 7
	/* System error exception */
	b       excpt_stub
