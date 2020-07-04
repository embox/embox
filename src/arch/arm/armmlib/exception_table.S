/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov

 */

#include <module/embox/kernel/irq_api.h>

.type start, %function
.type exp_default_entry, %function

.global start
.global exp_default_entry

.section .trap_table, "x"
    .word _stack_top
    .word start
    .word exc_entry2
    .word exc_entry3
    .word exc_entry4
    .word exc_entry5
    .word exc_entry6
    .word exc_entry7
    .word exc_entry8
    .word exc_entry9
    .word exc_entry10
    .word exc_entry11
    .word exc_entry12
    .word exc_entry13
    .word exc_entry14
    .word exc_entry15

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
