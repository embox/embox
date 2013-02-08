/**
 * @file
 * @brief Implements switching to usermode.
 *
 * @date 11.10.2012
 * @author Anton Bulychev
 */

#include <asm/gdt.h>

	.global usermode_entry, usermode_set_stack

usermode_set_stack:
	mov 4(%esp), %esp
	ret

usermode_entry:
 	cli

	/* Get pointer to tu_data */
 	mov 4(%esp), %ebx

	/* Setup segment selectors. */
	mov $(__USER_DS), %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	//

	/* Build info for iret. */
	pushl $(__USER_DS)
	pushl 4(%ebx)

	/* Work with EFLAGS */
	pushf
	popl %eax
	or $0x200, %eax        /* Enable IF flag */
	and $0xffffcfff, %eax  /* Set IOPL equals 0 */
	pushl %eax

	pushl $(__USER_CS)
	pushl (%ebx)
	//


	/* Now we are going into function. */
	iret
