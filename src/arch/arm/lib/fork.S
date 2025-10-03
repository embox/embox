/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */


#include <asm/asm.h>
#include <asm/ptrace.h>

.text
.syntax unified

ENTRY(fork)
	CALL_WPTREGS fork_body
END(fork)

