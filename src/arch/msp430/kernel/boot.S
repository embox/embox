
.section .vectors, "ax", @progbits
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word die
	.word _start

.section .text
.global _start

.weak software_init_hook
.weak hardware_init_hook

#define WDTCTL	0x0120
#define WDTHOLD	0x5a80

_start:
	# disabling Watchdog right here
	mov #WDTHOLD, &WDTCTL

	mov #__stack, r1

	call #hardware_init_hook

	call #software_init_hook

	jmp kernel_start

die:
	jmp die


