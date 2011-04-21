/**
 * @file
 * @brief bootstrap the kernel
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#include <asm/multiboot.h>

	.section .multiboot

	.globl	start, _start
start:
_start:
	jmp	multiboot_entry

	/* Align 32 bits boundary.  */
	.align	4

	/* Multiboot header.  */
multiboot_header:
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
	/* header_addr */
	.long	multiboot_header
	/* load_addr */
	.long	_start
	/* load_end_addr */
	.long	_edata
	/* bss_end_addr */
	.long	_end
	/* entry_addr */
	.long	multiboot_entry
#endif /* ! __ELF__ */

	.section .text
	.align	4
multiboot_entry:
	/* interrrupts disable */
	cli
	cld
	/* Initialize the stack pointer. */
	movl	$_stack, %esp

	/* Reset EFLAGS. */
	pushl	$0
	popf


	/* Push the pointer to the Multiboot information structure. */
//	pushl	%ebx
	/* Push the magic value. */
//	pushl	%eax
//	call	EXT_C(multiboot_check)
//	addl    $8, %esp ;


	movl   $_bss_len, 0x8(%esp)
	movl   $0x0, 0x4(%esp)
	movl   $_bss_vma, 0x0(%esp)
	call   memset

	mov   $_data_vma, %eax
	cmp   $_data_lma, %eax
	je    1f
	/*data section*/
	movl   $_data_len, 0x4(%esp)
	movl   $_data_lma, 0x4(%esp)
	movl   $_data_vma, 0x0(%esp)
	call   memcpy

1:

	call	EXT_C(kernel_start)

loop:	hlt
	jmp	loop

	.section .text
