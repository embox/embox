/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.05.2014
 */


	.text
	.global stack_ptr

stack_ptr:
	movl %esp, %eax
	addl $4, %eax
	ret

