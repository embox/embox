/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

	.text
	.global vfork

#include <asm/ptrace.h>

vfork:
	CALL_WPTREGS vfork_body

