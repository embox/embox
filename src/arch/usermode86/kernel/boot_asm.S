/**
 * @file
 * @brief <CURSOR>
 *
 * @author  Anton Kozlov
 * @date    09.03.2013
 */

.global _start

.extern _start2
.extern kernel_start
.extern __stack

_start:
	call _start2

	movl $__stack - 4, %esp

	call kernel_start
