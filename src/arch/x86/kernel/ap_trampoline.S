/**
 * @file
 * @brief
 *
 * @date 21.12.2012
 * @author Anton Bulychev
 */

#include <asm/linkage.h>
#include <asm/gdt.h>

	.section .text
	.code16

	.align 0x1000
C_ENTRY(__ap_trampoline):
	cli

	/* %cs has some value and we must use the same for data */
	mov	%cs, %ax
	mov	%ax, %ds

	/* load gdt prepared by bsp */
	lgdtl __ap_gdtr - __ap_trampoline

	/* switch to protected mode */
	mov %cr0, %eax
	orb $1, %al
	mov %eax, %cr0

	ljmpl $(__KERNEL_CS), $startup_ap

	.align 0x4

C_LABEL(__ap_gdtr):
	.space 8
C_LABEL(__ap_idtr):
	.space 8
C_LABEL(__ap_gdt):
	.space 128
C_LABEL(__ap_idt):
	.space 128
C_LABEL(__ap_trampoline_end):
