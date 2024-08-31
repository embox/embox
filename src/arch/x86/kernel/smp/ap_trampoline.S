/**
 * @file
 * @brief
 *
 * @date 21.12.2012
 * @author Anton Bulychev
 */

#include <asm/linkage.h>
#include <asm/gdt.h>
#include <hal/cpu.h>

	.section .data
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

	ljmpl $(__KERNEL_CS), $__ap_trampoline_32

	.align 0x4

C_LABEL(__ap_sp):
	.space 4 * (NCPU - 1)
C_LABEL(__ap_gdtr):
	.space 8
C_LABEL(__ap_gdt):
	.space GDT_ENTRIES * GDT_ENTRY_SIZE
C_LABEL(__tmp_small_stack):
	/* this stack in only for calling cpu_get_id */
	.space 0x40
C_LABEL(__ap_trampoline_end):

	.section .text
	.code32

__ap_trampoline_32:
	/* Setting up selectors */
	movw $(__KERNEL_DS), %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

	/* Setting up stack */
	lea	__tmp_small_stack, %esp
	addl $0x40, %esp
	call cpu_get_id

	/* eax get the returned cpuid value */
	dec	%eax
	lea	__ap_sp(,%eax,4), %esp
	mov	(%esp), %esp

	/* Jumping forward */
	jmp startup_ap
