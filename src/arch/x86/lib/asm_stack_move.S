/**
 * @file
 *
 * @data May 7, 2014
 * @author Anton Bondarev
 */


	.global cpu_stack_move
cpu_stack_move:
	movl    4(%esp), %eax /* destanation stack */
	movl    8(%esp), %edx /* source stack */

	movl    %edx, %ecx
	subl    %esp, %ecx

	subl    %ecx, %eax
	subl    %ecx, %edx

	pushl   %ecx
	pushl   %edx
	pushl   %eax
	call    memcpy
	popl    %eax
	popl    %edx
	popl    %ecx

/* switch stack pointer */
	subl    %eax, %edx

	subl    %edx, %esp
	subl    %edx, %ebp

	ret
