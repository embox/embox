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

multiboot_entry:
	/* interrrupts disable */
	cli
	/* Initialize the stack pointer. */
	movl	$_stack, %esp

	/* Reset EFLAGS. */
	pushl	$0
	popf

	/* Push the pointer to the Multiboot information structure. */
	pushl	%ebx
	/* Push the magic value. */
	pushl	%eax

	call	EXT_C(multiboot_check)

	call	EXT_C(kernel_start)

loop:	hlt
	jmp	loop

