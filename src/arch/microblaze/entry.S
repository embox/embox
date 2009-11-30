#include "asm/mb_regs.h"

	#define C_ENTRY(name)	.globl name; .align 4; name
	.section ".text"

C_ENTRY(_hardreset):
	brai boot_start
	nop
	nop

C_ENTRY(_interrupt_handler):
	addi r1, r1, -4
	SAVE_REGS

	brlid	r15, interrupt_handler
	nop
	nop

	RESTORE_REGS

	bra	 r14
	nop
	nop


C_ENTRY(_user_exception):
	brai boot_start
	nop
	nop


C_ENTRY(_debug_exception):
	brai boot_start
	nop
	nop

C_ENTRY(_hw_exception_handler):
	brai boot_start
	nop
	nop


