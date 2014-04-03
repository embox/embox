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

.weak hardware_init_hook
.weak software_init_hook
.weak kernel_start

    .align 4

.extern irq_handler
.global start
.section .trap_table, "x"
/* trap table
 * this code specific for ARM7TDMI cores
 *
 * actually, this is general for all ARM cores
 */
    ldr pc, reset_handler_addr
    ldr pc, undef_handler_addr
    ldr pc, swi_handler_addr
    ldr pc, prefetch_abt_handler_addr
    ldr pc, data_abt_handler_addr
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
data_abt_handler_addr:
    .word data_abt_handler
irq_handler_addr:
    .word irq_handler
fiq_handler_addr:
    .word fiq_handler

/* stubs */
undef_handler:
    B undef_handler
prefetch_abt_handler:
    B prefetch_abt_handler
data_abt_handler:
    B data_abt_handler
fiq_handler:
    B fiq_handler

.text
/* initialize stacks in some modes */
start:
reset_handler:

    ldr r0, =_stack_top

    msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #CONFIG_ARM_IRQ_STACK_SIZE
#if 0
    msr CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #CONFIG_ARM_FIQ_STACK_SIZE

    msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #CONFIG_ARM_SVC_STACK_SIZE

    msr CPSR_c, #ARM_MODE_ABT | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #CONFIG_ARM_ABT_STACK_SIZE

    msr CPSR_c, #ARM_MODE_UND | I_BIT | F_BIT
    mov sp, r0
    sub r1, r0, #CONFIG_ARM_UND_STACK_SIZE
#endif
    msr CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
    mov sp, r0

    b bootldr_start
