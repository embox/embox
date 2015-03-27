
.section .vectors, "ax", @progbits
	.word irq16
	.word irq17
	.word irq18
	.word irq19
	.word irq20
	.word irq21
	.word irq22
	.word irq23
	.word irq24
	.word irq25
	.word irq26
	.word irq27
	.word irq28
	.word irq29
	.word irq30
	.word _start

.section .text
.global _start
.weak interrupt_handle

.weak software_init_hook
.weak hardware_init_hook

.macro irq_head n
irq\n:
	push r0
	jmp irq_body
.endm

#define WDTCTL	0x0120
#define WDTHOLD	0x5a80

_start:
	# disabling Watchdog right here
	mov #WDTHOLD, &WDTCTL

	mov #_stack_top, r1
	push r0

	call #hardware_init_hook

	call #software_init_hook

	jmp kernel_start
die:
	jmp die


irq_base:
irq_head 16
irq_head 17
irq_head 18
irq_head 19
irq_head 20
irq_head 21
irq_head 22
irq_head 23
irq_head 24
irq_head 25
irq_head 26
irq_head 27
irq_head 28
irq_head 29
irq_head 30

irq_body:
	push r12
	push r13
	push r14
	push r15

	mov #irq_base, r14
	mov 8(r1), r15
	sub r14, r15
	rra r15
	rra r15
	add #16, r15

	mov #interrupt_handle, r14
	tst r14
	jz 1f
	call r14
1:
	pop r15
	pop r14
	pop r13
	pop r12

	add #2, r1

	reti

