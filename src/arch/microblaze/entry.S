	#define C_ENTRY(name)	.globl name; .align 4; name
	.section ".text"

C_ENTRY(_hardreset):
	brai boot_start

C_ENTRY(_interrupt_handler):


C_ENTRY(_user_exception):


C_ENTRY(_debug_exception):


C_ENTRY(_hw_exception_handler):



