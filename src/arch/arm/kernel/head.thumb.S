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

.section .trap_table, "x"
/* trap table
 * this code specific for ARM7TDMI cores
 *
 * actually, this is general for all ARM cores
 */
start:
    b reset_handler
    b undef_handler
    b SWI_handler
    b prefetch_abt_handler
    b data_abt_handler
    nop /*bad exception not_used:		.word not_used*/
    b IRQ_handler
    b FIQ_handler

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
/* initialize stack */
.RAM_TOP:
    .word _stack_top

reset_handler:
    mov	    r0, #0xd3
    mrs	    r1, APSR
    msr	    APSR, r0
    ldr	    r0, =0
    ldr	    r1, =start
loop_excpt:
    ldr	    r2, [r1]
    str	    r2, [r0]
    add	    r1, r1, #4
    add	    r0, r0, #4
    cmp	    r0, #0x20
    bne    loop_excpt

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
die:
    b 	    die
