/**
 * @file
 * @brief bootstrap the kernel
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */


#include <asm/linkage.h>
#include <hal/cpu.h>

	.section .text
	.align 4

	/* system entry point */
C_ENTRY(_start):
//	jmp	multiboot_entry

//multiboot_entry:
	/* interrrupts disable */
	cli
	cld

	/* Initialize the stack pointer. */
	movl    $_stack_top, %esp  /* load base address for system stack */

	/* Reset EFLAGS. */
	pushl   $0
	popf

	/* if we use multiboot header we must save registers which was set by loader
	eax = magic value and ebx = address of Multiboot information structure */
	/* Push the pointer to the Multiboot information structure. */
	pushl   %ebx
	/* Push the magic value. */
	pushl   %eax


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
	call    memcpy             /* copy data section */
	addl    $0xc, %esp
1:

#ifdef __SSE__
	mov     %cr0, %eax
	and     $0xFFFB, %ax       /* clear coprocessor emulation cr0.em */
	or      $0x2, %ax          /* set coprocessor monitoring CR0.MP */
	mov     %eax, %cr0
	mov     %cr4, %eax
	or      $(3 << 9), %ax     /* set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time */
	mov     %eax, %cr4
#endif

	call    gdt_init
	call    idt_init

#if NCPU > 1
	call    cpudata_init
#endif /* NCPU > 1 */

/* we already save eax and ebx on the stack */
	call    multiboot_save_info
	addl    $8, %esp


	call    kernel_start /* call C code */

loop:
	hlt             /* if we return from kernel_start it's error */
	jmp	loop

