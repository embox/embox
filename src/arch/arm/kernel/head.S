/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov
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

    .global start
.extern IRQ_handler

.section .trap_table, "x"
/* trap table
 * this code specific for ARM7TDMI cores
 *
 * actually, this is general for all ARM cores
 */
start:
    ldr pc, reset_handler_addr
    ldr pc, undef_handler_addr
    ldr pc, SWI_handler_addr
    ldr pc, prefetch_abt_handler_addr
    ldr pc, data_abt_handler_addr
    nop /*bad exception not_used:		.word not_used*/
    ldr pc, IRQ_handler_addr
    ldr pc, FIQ_handler_addr

reset_handler_addr:
    .word reset_handler
undef_handler_addr:
    .word undef_handler
SWI_handler_addr:
    .word SWI_handler
prefetch_abt_handler_addr:
    .word prefetch_abt_handler
data_abt_handler_addr:
    .word data_abt_handler
IRQ_handler_addr:
    .word IRQ_handler
FIQ_handler_addr:
    .word FIQ_handler

/* stubs */
undef_handler:
    B undef_handler
prefetch_abt_handler:
    B prefetch_abt_handler
data_abt_handler:
    B data_abt_handler
FIQ_handler:
    B FIQ_handler

.text
/* initialize stack */
.RAM_TOP:
    .word _stack_top
reset_handler:
    msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    ldr	    r0, =0
    ldr	    r1, =start
loop_excpt:
    ldr	    r2, [r1], #0x4
    str	    r2, [r0], #0x4
    cmp	    r0, #0x20
    bne    loop_excpt

    ldr r0, .RAM_TOP

    msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    mov sp, r0
    sub r0, r0, #CONFIG_ARM_IRQ_STACK_SIZE

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

    msr CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
    mov sp, r0

/* initialize memory */
/* clear bss */
    mov     r0, #0
    ldr     r1, =_bstart
    ldr     r2, =_bend
loopZI:
    cmp     r1, r2
    strlo   r0, [r1], #4
    blo     loopZI

/* copy data section */
    ldr     r1, =_data_lma
    ldr     r2, =_data_vma
    ldr     r3, =_data_end
loop_cp_data:
    cmp     r2, r3
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo     loop_cp_data

    ldr     r1, =_text_lma
    ldr     r2, =_text_vma
    ldr     r3, =_text_end
loop_cp_text:
    cmp     r2, r3
    ldrlo   r0, [r1], #4
    strlo   r0, [r2], #4
    blo     loop_cp_text

init_hook:
    nop
    ldr     r0, =hardware_init_hook
    blne    hardware_init_hook

    ldr     r0, =software_init_hook
    blne    software_init_hook

/* enabling interrupts */
    msr	    CPSR_c, #ARM_MODE_SVC

/* kernel start */
	bl		kernel_start
die:
    b 	    die
