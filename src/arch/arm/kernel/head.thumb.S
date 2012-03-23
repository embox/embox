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
.extern irq_handler

.type reset_handler, %function
.type undef_handler, %function
.type irq_handler, %function

.section .trap_table, "x"
    .word _stack_top
    .word reset_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word undef_handler
    .word irq_handler


/* stubs */
undef_handler:
    b die

.text
/* initialize stack */
.RAM_TOP:
    .word _stack_top

start:
reset_handler:

/* initialize memory */
/* clear bss */
    mov     r0, #0
    ldr     r1, =_bstart
    ldr     r2, =_bend
loopZI:
    str     r0, [r1]
    add	    r1, r1, #4
    cmp     r1, r2
    bls     loopZI

/* copy data section */
    ldr     r1, =_data_lma
    ldr     r2, =_data_vma
    ldr     r3, =_data_end
loop_cp_data:
    ldr	    r0, [r1]
    str	    r0, [r2]
    add	    r1, r1, #4
    add	    r2, r2, #4
    cmp     r2, r3
    bls     loop_cp_data

hw_init_hook:
    ldr     r0, =hardware_init_hook
    cmp	    r0, #0
    beq	    sw_init_hook
    bl      hardware_init_hook

sw_init_hook:
    ldr     r0, =software_init_hook
    cmp	    r0, #0
    beq	    krn_start
    bl      software_init_hook

krn_start:
/* kernel start */
	bl		kernel_start

SWI_handler:
IRQ_handler:
FIQ_handler:
die:
    b 	    die
