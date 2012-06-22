
#include <asm/linkage.h>

	.section    ".text"
	.align  4
 	.weak hardware_init_hook
 	.weak software_init_hook


C_ENTRY(_start):

	nop

	.section    ".text"
	.align  4

