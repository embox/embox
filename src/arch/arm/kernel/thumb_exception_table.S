/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov

 */
.weak hardware_init_hook
.weak software_init_hook
.weak kernel_start

.align 4

.extern bootldr_start

.type start, %function
.type bootldr_start, %function
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

start:
    b bootldr_start

/* stubs */
undef_handler:
    b undef_handler

hard_fault:
    b hard_fault

SWI_handler:
IRQ_handler:
FIQ_handler:
die:
    b 	    die

