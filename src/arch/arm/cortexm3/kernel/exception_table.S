/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov

 */

.type start, %function
.type undef_handler, %function
.type hard_fault, %function

.global trap_table_start
.global trap_table_end
.global start

.section .trap_table, "x"
trap_table_start:
    .word _stack_top
    .word start
    .word undef_handler
    .word hard_fault
trap_table_end:



.section .text, "x"

start:
    ldr r1, =bootldr_start
    bx r1
die:
    b 	    die

/* stubs */
undef_handler:
    b undef_handler

hard_fault:
    b hard_fault

