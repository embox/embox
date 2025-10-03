/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#include <asm/asm.h>
#include <asm/ptrace.h>

.text
.syntax unified

ENTRY(vfork)
	CALL_WPTREGS vfork_body
END(vfork)	
