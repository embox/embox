/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov

 */

.type start, %function
.type exp_default_entry, %function

.global trap_table_start
.global trap_table_end
.global start
.global exp_default_entry

/* This trap table derived from Cortex-M7.
 * So for lower Cortex-M version some of these handlers
 * are unused. */
.section .trap_table, "x"
trap_table_start:
    .word _stack_top
    .word start /* Reset */
    .word exp_default_entry /* NMI */
    .word exp_default_entry /* Hard fault */
    .word exp_default_entry /* Memory management fault */
    .word exp_default_entry /* Bus fault */
    .word exp_default_entry /* Usage fault */
    .word 0 /* Reserved */
    .word 0 /* Reserved */
    .word 0 /* Reserved */
    .word 0 /* Reserved */
#if 0
    /* we use static_irq_attach for these handlers and below
     * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0497a/Babefdjc.html */
    .word exp_default_entry /* SVCall */
    .word 0 /* Reserved for debug */
    .word 0 /* Reserved */
#endif
trap_table_end:



.section .text, "x"

exp_default_entry:
    mov  r0, sp
    mrs  r1, IPSR
    bl   exc_default_handler

start:
    ldr r1, =bootldr_start
    bx r1
die:
    b 	    die
