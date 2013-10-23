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
.extern _stack_top

_start:
	call _start2

	movl $_stack_top - 4, %esp

	call kernel_start
