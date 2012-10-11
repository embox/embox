/**
 * @file
 *
 * @date 11.10.2012
 * @author Anton Bulychev
 */

#include <asm/gdt.h>

	.global usermode_enter
 usermode_enter:
 	cli

 	/* Save stack pointer */
 	mov %esp, %ebx
 	//

 	/* Pushes info for callee. */
 	mov 8(%ebx), %eax
 	push %eax
 	push $func_ret
	//

	/* Setup segment selectors. */
	mov $(__USER_DS), %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	//

	/* Build info for iret. */
	mov %esp, %eax
	pushl $(__USER_DS)
	pushl %eax
	pushf
	pop %eax
	or $0x200, %eax
	push %eax
	pushl $(__USER_CS)
	mov 4(%ebx), %eax
	push %eax
	//

	/* Now we are going into function. */
	iret

	/*
	 * Return from function. Actually, there is no return: task should
	 * call exit syscall. So it's only for debug goals.
	 */
func_ret:
	add $4, %esp
	ret
