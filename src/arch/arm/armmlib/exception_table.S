/**
 * @file
 *
 * @date 28.03.10
 * @author Anton Kozlov

 */

#include <module/embox/kernel/irq_api.h>
#include <framework/mod/options.h>

#define IRQ_STACK_SIZE OPTION_GET(NUMBER, irq_stack_size)

.type start, %function
.type exp_default_entry, %function

.global start
.global exp_default_entry

.section .trap_table, "x"
	.word irq_stack_top
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

.section .text.bootcode
.thumb
.syntax unified

exp_default_entry:
	mov  r0, sp
	mrs  r1, IPSR
	bl   exc_default_handler

start:
	# Init PSP stack
	ldr r0, =_stack_top
	msr psp, r0

	# Switch to PSP stack
	mrs  r0, CONTROL
	movs r1, #0x2
	orrs r0, r0, r1
	msr  CONTROL, r0
	isb

	ldr r1, =bootldr_start
	bx r1
die:
	b 	    die

.data
.align 3
irq_stack_start:
	.rept   IRQ_STACK_SIZE
	.byte   0x50
	.endr
irq_stack_top:
