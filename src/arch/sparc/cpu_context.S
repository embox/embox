/**
 * \file cpu_context.S
 *
 * \date Jun 05, 2009
 * \author Eldar Abusalimov
 */

#include "cpu_context.h"
#include "sparc_regs.h"

	.section ".text"
	.align 4

#define l_y     l3
#define l_wim   l4
#define l_tbr   l5

#define g_base    g1
#define g_iter    g2
#define g_scratch g3
#define g_wim     g4
#define g_cwp     g5
#define g_retpc   g6

// TODO cpu_context_save comments
	.global cpu_context_save
cpu_context_save:

	/* First of all, read control/status registers to locals */
	mov %y, %l_y
	mov %wim, %l_wim
	mov %tbr, %l_tbr

	/* save global registers */
	STORE_CC_GLOBALS(l_base)
	/* At this point we can quietly use globals */

	/* save PSR_CWP and WIM to restore them later */
	mov %wim, %g_wim
	mov %psr, %g_cwp
	and %g_cwp, PSR_CWP, %g_cwp
	mov %l_base, %g_base

	STORE_CC_PSR_CWP(l_base, g_cwp)

	/*  reset PSR_CWP and WIM */
	mov %g0, %wim
	mov %psr, %g_scratch
	andn %g_scratch, PSR_CWP, %g_scratch
	mov %g_scratch, %psr
	 nop; nop; nop
	/* We're in window #0 now */

	add %g_base, REG_WINDOW_SZ * (CORE_NWINDOWS-1), %g_iter
	/* iterate over the windows */
	store_regwin:
		STORE_CC_REG_WINDOW(g_iter)
		sub %g_iter, REG_WINDOW_SZ, %g_iter
	cmp %g_iter,%g_base
	bge store_regwin
	 save

	/* restore PSR_CWP and WIM */
	mov %g_wim, %wim
	mov %psr, %g_scratch
	andn %g_scratch, PSR_CWP, %g_scratch
	or  %g_scratch, %g_cwp, %g_scratch
	mov %g_scratch, %psr
	 nop; nop; nop
	/* We're in the initial window pointed at the call time now */

	/* restore affected globals */
	LOAD_CC_GLOBALS(l_base)

	jmpl %l_retpc + 8, %g0
	 nop

// TODO cpu_context_restore comments
	.global cpu_context_restore
cpu_context_restore:

	/*
	 * save arguments to globals as far as
	 * locals will be completely overwtitten
	 */
	mov %l_retpc, %g_retpc
	mov %l_base, %g_base

	/*  reset PSR_CWP and WIM */
	mov %g0, %wim
	mov %psr, %g_scratch
	andn %g_scratch, PSR_CWP, %g_scratch
	mov %g_scratch, %psr
	/* We're in window #0 now */

	add %g_base, REG_WINDOW_SZ * (CORE_NWINDOWS-1), %g_iter
	/* iterate over the windows */
	load_regwin:
		LOAD_CC_REG_WINDOW(g_iter)
		sub %g_iter, REG_WINDOW_SZ, %g_iter
	cmp  %g_iter,%g_base
	bge load_regwin
	 save

	LOAD_CC_PSR_CWP(g_base, g_cwp)

	/* set PSR_CWP */
	mov %psr, %g_scratch
	andn %g_scratch, PSR_CWP, %g_scratch
	or  %g_scratch, %g_cwp, %g_scratch
	mov %g_scratch, %psr
	 nop; nop; nop
	/* We're in the window pointed at the _save_ call time now */

	/*
	 * _save_ call time PSR, PC and nPC
	 * have already been restored to l_psr, l_pc and l_npc respectively.
	 */

	/* write control/status registers */
	mov %l_y, %y
	mov %l_wim, %wim
	mov %l_tbr, %tbr

	/* restore arguments */
	mov %g_retpc, %l_retpc
	mov %g_base, %l_base

	/* restore globals */
	LOAD_CC_GLOBALS(l_base)

	jmpl %l_retpc + 8, %g0
	 nop

