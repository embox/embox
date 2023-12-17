/**
 * @file
 * @brief
 *
 * @date 17.07.2019
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

.macro excpt_vector el:req fn:req
/* All vectors are 128 byte aligned and limited to 32 instructions */
.align 7
	sub     sp, sp, #(8 * 34)
	/* Store x0-x29 regs */
	stp     x0, x1, [sp, #(8 * 0)]
	stp     x2, x3, [sp, #(8 * 2)]
	stp     x4, x5, [sp, #(8 * 4)]
	stp     x6, x7, [sp, #(8 * 6)]
	stp     x8, x9, [sp, #(8 * 8)]
	stp     x10, x11, [sp, #(8 * 10)]
	stp     x12, x13, [sp, #(8 * 12)]
	stp     x14, x15, [sp, #(8 * 14)]
	stp     x16, x17, [sp, #(8 * 16)]
	stp     x18, x19, [sp, #(8 * 18)]
	stp     x20, x21, [sp, #(8 * 20)]
	stp     x22, x23, [sp, #(8 * 22)]
	stp     x24, x25, [sp, #(8 * 24)]
	stp     x26, x27, [sp, #(8 * 26)]
	stp     x28, x29, [sp, #(8 * 28)]
	/* Store lr, sp, pc, psr */
.if \el == 0
	mrs     x10, sp_el0
.else
	add     x10, sp, #(8 * 34)
.endif
	mrs     x11, elr_el1
	mrs     x12, spsr_el1
	stp     x30, x10, [sp, #(8 * 30)]
	stp     x11, x12, [sp, #(8 * 32)]
	/* Call the handler */
	mov     x0, sp
	ldr     x30, =excpt_exit
	b       \fn
.endm

.section .text.kernel_vectors
.global aarch64_kernel_vectors

/* Vector tables must be placed at a 2KB-aligned address */
.align 11
aarch64_kernel_vectors:
	/* EL1t */
	excpt_vector 1, aarch64_sync_handler
	excpt_vector 1, aarch64_irq_handler
	excpt_vector 1, aarch64_fiq_handler
	excpt_vector 1, aarch64_serror_handler
	/* EL1h */
	excpt_vector 1, aarch64_sync_handler
	excpt_vector 1, aarch64_irq_handler
	excpt_vector 1, aarch64_fiq_handler
	excpt_vector 1, aarch64_serror_handler
	/* EL0 64-bit */
	excpt_vector 0, aarch64_sync_handler
	excpt_vector 0, aarch64_irq_handler
	excpt_vector 0, aarch64_fiq_handler
	excpt_vector 0, aarch64_serror_handler
	/* EL0 32-bit */
	excpt_vector 0, aarch64_sync_handler
	excpt_vector 0, aarch64_irq_handler
	excpt_vector 0, aarch64_fiq_handler
	excpt_vector 0, aarch64_serror_handler

excpt_exit:
	/* Load lr, pc, psr */
	ldp     x30, x10, [sp, #(8 * 30)]
	ldp     x11, x12, [sp, #(8 * 32)]
	msr     elr_el1, x11
	msr     spsr_el1, x12
	/* Load x0-x29 regs */
	ldp     x0, x1, [sp, #(8 * 0)]
	ldp     x2, x3, [sp, #(8 * 2)]
	ldp     x4, x5, [sp, #(8 * 4)]
	ldp     x6, x7, [sp, #(8 * 6)]
	ldp     x8, x9, [sp, #(8 * 8)]
	ldp     x10, x11, [sp, #(8 * 10)]
	ldp     x12, x13, [sp, #(8 * 12)]
	ldp     x14, x15, [sp, #(8 * 14)]
	ldp     x16, x17, [sp, #(8 * 16)]
	ldp     x18, x19, [sp, #(8 * 18)]
	ldp     x20, x21, [sp, #(8 * 20)]
	ldp     x22, x23, [sp, #(8 * 22)]
	ldp     x24, x25, [sp, #(8 * 24)]
	ldp     x26, x27, [sp, #(8 * 26)]
	ldp     x28, x29, [sp, #(8 * 28)]
	/* Return from exception */
	add     sp, sp, #(8 * 34)
	eret
