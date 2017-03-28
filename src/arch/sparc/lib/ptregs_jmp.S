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

	.global ptregs_jmp
ptregs_jmp:
	call winflush
	 nop
	mov %o0, %g2

	ld [%o0 + PT_PSR], %g1;
	//TODO resotore PSR is required?
	/* wr %g1, %g0, %psr; */
	nop;nop;nop;

	ld [%g2 + PT_NPC], %o6;
	LOAD_WINDOW(sp)

	mov %g2, %o0

	ld  [%o0 + PT_G1], %g1;
	ldd [%o0 + PT_G2], %g2;
	ldd [%o0 + PT_G4], %g4;
	ldd [%o0 + PT_G6], %g6;

	ldd [%o0 + PT_I0], %i0;
	ldd [%o0 + PT_I2], %i2;
	ldd [%o0 + PT_I4], %i4;
	ldd [%o0 + PT_I6], %i6;

	ld [%o0 + PT_PC],  %o7;
	ld [%o0 + PT_NPC], %o6;
	ld [%o0 + PT_I0],  %o0;

	restore
	retl
	 nop

