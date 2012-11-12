/**
 * @file
 * @brief bootstrap the kernel
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */

#include <asm/multiboot.h>
#include <asm/linkage.h>
#include <module/embox/arch/mmu.h>

	.section .multiboot

/* system entry point */
C_ENTRY(_start):
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

/* real entry code */
	.section .text
	.align	4
multiboot_entry:
	/* interrrupts disable */
	cli
	cld

#ifndef NOMMU
	/* Enable mmu */
	call    mmu_enable
#endif /* ! NOMMU */

	/* Initialize the stack pointer. */
	movl	$_stack_vma, %esp  /* load base address for system stack */
	addl    $_stack_len, %esp  /* setup correct system stack pointer */

	/* Reset EFLAGS. */
	pushl	$0
	popf

	/* Clean up bss section */
	pushl   $_bss_len          /* push bss len as 3 parameter on the stack */
	pushl   $0x0               /* push zero as 2 parameter on the stack */
	pushl   $_bss_vma          /* push bss start as 1 parameter on the stack */
	call    memset             /* clear bss section */
	addl    $0xc, %esp

	/* Check whether load data section */
	mov     $_data_vma, %eax
	cmp     $_data_lma, %eax
	je      1f        /* if data_vma = data_lma section have correct place */

	/* Copy data section to a correct place*/
	pushl   $_data_len         /* push data len as 3 parameter on the stack */
	pushl   $_data_lma         /* push data lma as 2 parameter on the stack */
	pushl   $_data_vma         /* push data vma as 1 parameter on the stack */
	call    memcpy              /* copy data section */
	addl    $0xc, %esp

1:

#ifdef CONFIG_MULTIBOOT_CHECK
	/* Push the pointer to the Multiboot information structure. */
	pushl	%ebx
	/* Push the magic value. */
	pushl	%eax
	call	multiboot_check
	addl    $8, %esp
#endif

	call	kernel_start /* call C code */

loop:
	hlt             /* if we return from kernel_start it's error */
	jmp	loop

/*
 * Reset cpu
 * Use triple fault
 */
C_ENTRY(cpu_reset):
	cli
	movl	$null_idt, %eax		/* Reset by triple fault */
	lidt	(%eax)
	int	$3
	hlt

	.align 4
null_idt:
	.word	0
	.long	0
