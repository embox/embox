/**
 * @file
 * @brief
 *
 * @date 06.11.22
 * @author Aleksey Zhmulin
 */

#include <arm/fpu.h>
#include <asm/cp15.h>
#include <asm/modes.h>

.macro excpt_entry correction=0
.if \correction
	@ Correct return address
	sub     lr, lr, #\correction
.endif
	@ Move sp, lr, spsr to temp regs
	stmfd   sp!, {r0-r2}
	mov     r0, sp
	mov     r1, lr
	mrs     r2, SPSR

	@ Return stack pointer to prev value
	add     sp, sp, #(3 * 4)

	@ Return to previous CPU mode with disabled interrupts
	msr     CPSR, #(ARM_MODE_SYS | I_BIT | F_BIT)

	@ Store struct pt_regs on the stack
	stmfd   sp!, {r1, r2}
	add     r1, sp, #(2 * 4)
	stmfd   sp!, {r1, lr}
	ldmfd   r0, {r0-r2}
	stmfd   sp!, {r0-r12}

	@ Move struct pt_regs addr to r0
	mov     r0, sp
.endm

.macro excpt_exit prev_mode
	@ Return to previous CPU mode with disabled interrupts
	msr     CPSR, #(ARM_MODE_SYS | I_BIT | F_BIT)

	@ Load struct pt_regs and return from exception
	add     r0, sp, #(17 * 4)
	ldmdb   r0!, {r1, r2}
	ldmdb   r0!, {r3, lr}
	mov     sp, r3
	msr     CPSR, #(\prev_mode | I_BIT | F_BIT)
	msr     SPSR, r2
	stmfd   sp!, {r1}
	ldmdb   r0, {r0-r12}
	ldmfd   sp!, {pc}^
.endm

.macro turn_off_mmu
	mrc     p15, 0, sp, c1, c0, 0
	and     sp, sp, #~CR_M
	mcr     p15, 0, sp, c1, c0, 0
	ldr     sp, =_stack_top
.endm

.text
/* Needs for mmu_test */
.global data_abt_handler_addr
.global data_abt_handler

.align 4
undef_handler:
	excpt_entry correction=4
	bl      arm_undef_handler
	excpt_exit prev_mode=ARM_MODE_UND

swi_handler:
	excpt_entry correction=4
	bl      swi_handle
	excpt_exit prev_mode=ARM_MODE_SVC

prefetch_abt_handler:
	excpt_entry correction=4
	mrc     p15, 0, r1, c5, c0, 1
	bl      arm_pabt_handler
#if KEEP_GOING
	excpt_exit prev_mode=ARM_MODE_ABT
#endif

data_abt_handler:
	excpt_entry correction=8
	mrc     p15, 0, r1, c5, c0, 0
	bl      arm_dabt_handler
#if KEEP_GOING
	excpt_exit prev_mode=ARM_MODE_ABT
#endif

.align 4
irq_handler:
	excpt_entry correction=4
#if ARM_FPU_VFP
	ARM_FPU_CONTEXT_SAVE_DEC(r1, sp)
#endif
	bl      interrupt_handle
#if ARM_FPU_VFP
	ARM_FPU_CONTEXT_LOAD_DEC(r1, sp)
#endif
	excpt_exit prev_mode=ARM_MODE_IRQ

fiq_handler:
	excpt_entry correction=4
	bl      arm_fiq_handler
#if KEEP_GOING
	excpt_exit prev_mode=ARM_MODE_FIQ
#endif

/**
 * Exceptions table (general for all ARM cores):
 * 0x00 - reset
 * 0x04 - undefined
 * 0x08 - SWI
 * 0x0C - prefetch abort
 * 0x10 - data abort
 * 0x14 - reserved
 * 0x18 - IRQ
 * 0x1C - FIQ (fast IRQ)
 */
.section .trap_table, "x"
.global arm_trap_table
arm_trap_table:
	ldr     pc, reset_handler_addr
	ldr     pc, undef_handler_addr
	ldr     pc, swi_handler_addr
	ldr     pc, prefetch_abt_handler_addr
	ldr     pc, data_abt_handler_addr
	ldr     pc, .
	ldr     pc, irq_handler_addr
	ldr     pc, fiq_handler_addr

reset_handler_addr:
	.word   reset_handler
undef_handler_addr:
	.word   undef_handler
swi_handler_addr:
	.word   swi_handler
prefetch_abt_handler_addr:
	.word   prefetch_abt_handler
data_abt_handler_addr:
	.word   data_abt_handler
irq_handler_addr:
	.word   irq_handler
fiq_handler_addr:
	.word   fiq_handler
