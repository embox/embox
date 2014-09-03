/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text

	.globl syscall_trap
syscall_trap:
	pushl   $(0)
	pushl   $(0x80)
	jmp	syscall_stub


	.section .traps.text

syscall_stub:
	SAVE_ALL

	mov SYSCALL_TABLE(,%eax,4), %ebx
	test %ebx,%ebx
	jnz syscall_make

	mov $-1,%eax
	jmp syscall_leave

syscall_make:
	call *SYSCALL_TABLE(,%eax,4)

syscall_leave:

	mov %eax, (PT_EAX)(%esp)  /* Save the return value */
	RESTORE_ALL
