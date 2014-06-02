/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */


	.text
	.global stack_ptr

stack_ptr:
	mov	r0, sp
	mov 	pc, lr
