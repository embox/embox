/**
 * @file
 * @brief TODO
 *
 * @date 20.04.2010
 * @author Eldar Abusalimov
 */

#include <asm/regs.h>
#include <asm/psr.h>
#include <asm/ptrace.h>
#include <asm/stack.h>
#include <asm/winmacro.h>

	.text
	.align 4

	.global context_switch
context_switch:
	save %sp, -STACK_FRAME_SZ, %sp

	call winflush
	 mov %i1, %local

	STORE_PT_ALL(i0 - STACK_FRAME_SZ, temp, temp, temp, temp)
	LOAD_PT_ALL(local - STACK_FRAME_SZ, temp, temp, temp, temp)

	ret
	 restore
