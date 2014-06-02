/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

	.text
	.global fork

#include <asm/ptrace.h>

fork:
	CALL_WPTREGS fork_body

