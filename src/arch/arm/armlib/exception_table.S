/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    10.09.2012
 */

/**
 * exceptions table:
 * 0x00 - reset
 * 0x04 - undefined
 * 0x08 - SWI
 * 0x0C - prefetch abort
 * 0x10 - data abort
 * 0x14 - reserved
 * 0x18 - IRQ
 * 0x1C - FIQ (_fast IRQ)
 */

#include <asm/modes.h>
#include <asm/regs.h>
#include <framework/mod/options.h>

#define IRQ_STACK_SIZE OPTION_GET(NUMBER,irq_stack_size)

.extern irq_handler
.extern setup_excpt_table

.section .trap_table, "x"
.global arm_trap_table
arm_trap_table:
/* trap table
 * this code specific for ARM7TDMI cores
 *
 * actually, this is general for all ARM cores
 */
    ldr pc, reset_handler_addr
    ldr pc, undef_handler_addr
    ldr pc, swi_handler_addr
    ldr pc, prefetch_abt_handler_addr
    b data_abt_prepare
    nop /*bad exception not_used:		.word not_used*/
    ldr pc, irq_handler_addr
    ldr pc, fiq_handler_addr

reset_handler_addr:
    .word reset_handler
undef_handler_addr:
    .word undef_handler
swi_handler_addr:
    .word swi_handler
prefetch_abt_handler_addr:
    .word prefetch_abt_handler
.global data_abt_handler_addr
data_abt_handler_addr:
    .word data_abt_handler
irq_handler_addr:
    .word irq_handler
fiq_handler_addr:
    .word fiq_handler

data_abt_prepare:
	/* Prepare stack and call handler */
	stmfd r13!, {r14}
	add r14, pc, #0
	ldr pc, data_abt_handler_addr
	ldmfd r13!, {r14}
	mrs r0, cpsr
	orr r0, r0, #0x1F
	msr cpsr_c, r0
	mov pc, r14

.text
.global start
start:
reset_handler:
    /* Make sure MMU is off */
    mrc p15, 0, r0, c1, c0, 0
    and r0, r0, #~CR_M
    mcr p15, 0, r0, c1, c0, 0

    ldr r0, =_stack_top

    msr CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_UND | I_BIT | F_BIT
    mov sp, r0
    msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #IRQ_STACK_SIZE

    msr CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
    mov sp, r0

    bl setup_excpt_table

    b bootldr_start

undef_handler:
prefetch_abt_handler:
fiq_handler:
    sub r14, r14, #4
    b arm_unresolvable_exception
.global data_abt_handler
data_abt_handler:
    sub r14, r14, #8

arm_unresolvable_exception:
    nop
    stmfd sp!, {r0-r12, r14}
    mrs r0, CPSR
    mrs r1, SPSR
    stmfd sp!, {r0, r1}
    mov r0, sp
    b arm_exception_handler
