/**
 * @file
 * @brief TODO
 *
 * @date 20.04.2010
 * @author Eldar Abusalimov
 */

#define HAL_CONTEXT_H_
#include <asm/hal/context.h>

#include <asm/regs.h>
#include <asm/wim.h>
#include <asm/ptrace.h>
#include <asm/stack.h>

	.text
	.align 4

	.global context_switch
context_switch:
	save %sp, -STACK_FRAME_SZ, %sp

	st %fp, [%i0 + CTX_KSP]
	st %i7, [%i0 + CTX_RET]
	st %i0, [%i0 + CTX_ARG]

	rd %psr, %t_psr
	st %t_psr, [%i0 + CTX_PSR]

	call winflush
	 nop

	mov %i1, %g1

	ld [%g1 + CTX_PSR], %g2
	wr %g2, %g0, %psr
	 ! don't wait

	mov WIM_INIT, %t_wim
	sll %t_wim, %g2, %t_wim
	srl %t_wim, CONFIG_NWINDOWS, %temp
	wr %t_wim, %temp, %wim
	 ! don't wait

	ld [%g1 + CTX_KSP], %fp
	ld [%g1 + CTX_RET], %i7
	ld [%g1 + CTX_ARG], %i0

	ret
	 restore
