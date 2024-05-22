/**
 * @file
 * @brief
 *
 * @date 06.11.22
 * @author Aleksey Zhmulin
 */
#include <arm/fpu.h>
#include <asm/ptrace.h>
#include <hal/reg.h>

/**
 * Purpose
 * : store struct excpt_context on the stack
 *
 * Params
 * : correction = increment return address to point to the exception instruction
 */
.macro excpt_entry correction=0
.if \correction
	@ Correct return address
	sub     lr, lr, #\correction
.endif
	@ Move sp, lr, spsr to temp regs
	stmfd   sp, {r0-r2}
	mov     r0, sp
	mov     r1, lr
	mrs     r2, SPSR

	@ Return to previous CPU mode with disabled interrupts
	msr     CPSR, #(PSR_M_SYS | PSR_I | PSR_F)

	@ Store struct pt_regs on the stack
	stmfd   sp!, {r1, r2}
	add     r1, sp, #(2 * 4)
	stmfd   sp!, {r1, lr}
	ldmdb   r0, {r0-r2}
	stmfd   sp!, {r0-r12}
.endm

/**
 * Purpose
 * : load struct excpt_context from the stack
 *
 * Params
 * : excpt_exit = CPU mode on exception entry
 */
.macro excpt_exit prev_mode
	@ Return to previous CPU mode with disabled interrupts
	msr     CPSR, #(PSR_M_SYS | PSR_I | PSR_F)

	@ Load struct pt_regs and return from exception
	add     r0, sp, #PTREGS_SIZE
	ldmdb   r0!, {r1, r2}
	ldmdb   r0!, {r3, lr}
	mov     sp, r3
	msr     CPSR, #(\prev_mode | PSR_I | PSR_F)
	msr     SPSR, r2
	stmfd   sp!, {r1}
	ldmdb   r0, {r0-r12}
	ldmfd   sp!, {pc}^
.endm

.text
undef_handler:
	excpt_entry correction=4
	save_fpu_dec tmp=r0, stack=sp, upd=1
	mov     r0, sp
	bl      arm_undef_handler
	load_fpu_inc tmp=r0, stack=sp, upd=1
	excpt_exit prev_mode=PSR_M_UND

swi_handler:
	excpt_entry correction=4
	save_fpu_dec tmp=r0, stack=sp, upd=1
	bl      arm_swi_handler
	load_fpu_inc tmp=r0, stack=sp, upd=1
	excpt_exit prev_mode=PSR_M_SVC

pabt_handler:
	excpt_entry correction=4
	save_fpu_dec tmp=r0, stack=sp, upd=1
	mov     r0, sp
	bl      arm_pabt_handler
	load_fpu_inc tmp=r0, stack=sp, upd=1
	excpt_exit prev_mode=PSR_M_ABT

dabt_handler:
	excpt_entry correction=8
	save_fpu_dec tmp=r0, stack=sp, upd=1
	mov     r0, sp
	bl      arm_dabt_handler
	load_fpu_inc tmp=r0, stack=sp, upd=1
	excpt_exit prev_mode=PSR_M_ABT

irq_handler:
	excpt_entry correction=4
	save_fpu_dec tmp=r0, stack=sp, upd=1
	bl      arm_irq_handler
	load_fpu_inc tmp=r0, stack=sp, upd=1
	excpt_exit prev_mode=PSR_M_IRQ

fiq_handler:
	excpt_entry correction=4
	save_fpu_dec tmp=r0, stack=sp, upd=1
	mov     r0, sp
	bl      arm_fiq_handler
	load_fpu_inc tmp=r0, stack=sp, upd=1
	excpt_exit prev_mode=PSR_M_FIQ

/**
 * Exceptions table (general for all ARM cores)
 * : 0x00 = reset
 * : 0x04 = undefined
 * : 0x08 = SWI
 * : 0x0C = prefetch abort
 * : 0x10 = data abort
 * : 0x14 = reserved
 * : 0x18 = IRQ
 * : 0x1C = FIQ (fast IRQ)
 */
.section .trap_table, "x"
.global arm_trap_table
arm_trap_table:
	b       reset_handler
	b       undef_handler
	b       swi_handler
	b       pabt_handler
	b       dabt_handler
	ldr     pc, .
	b       irq_handler
	b       fiq_handler
