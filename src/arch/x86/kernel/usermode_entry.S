/**
 * @file
 * @brief Implements switching to usermode.
 *
 * @date 11.10.2012
 * @author Anton Bulychev
 */

#include <asm/gdt.h>

	.global usermode_entry
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
	pushf
	popl %eax
	or $0x200, %eax
	pushl %eax
	pushl $(__USER_CS)
	pushl (%ebx)
	//

	/* Now we are going into function. */
	iret
