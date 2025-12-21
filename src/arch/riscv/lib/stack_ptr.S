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
	mv	a0, sp
	ret
