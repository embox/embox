/**
 * @file
 * @brief
 *
 * @date 09.03.17
 * @author Anton Bondarev
 */

#include <asm/entry.h>
#include <asm/ptrace.h>
#include <asm/regs.h>


/** For temporal computations. */
#define g_temp       g3

	.text
	.align 4

	.global vfork
vfork:
	save %sp, -(STACK_FRAME_SZ + PTRACE_REGS_SZ) , %sp

	st  %g1, [%sp + STACK_FRAME_SZ + PT_G1];
	std %g2, [%sp + STACK_FRAME_SZ + PT_G2];
	std %g4, [%sp + STACK_FRAME_SZ + PT_G4];
	std %g6, [%sp + STACK_FRAME_SZ + PT_G6];

	std %i0, [%sp + STACK_FRAME_SZ + PT_I0];
	std %i2, [%sp + STACK_FRAME_SZ + PT_I2];
	std %i4, [%sp + STACK_FRAME_SZ + PT_I4];
	std %i6, [%sp + STACK_FRAME_SZ + PT_I6];

	st %o7, [%sp + STACK_FRAME_SZ + PT_PC];
	/* TODO */
	st %o6, [%sp + STACK_FRAME_SZ + PT_NPC];

	rd %psr, %o0
	nop; nop; nop
	st %o0, [%sp + STACK_FRAME_SZ + PT_PSR];

	call vfork_body
	 add %fp, -PTRACE_REGS_SZ, %o0

	nop /* noreturn*/

