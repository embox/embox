/**
 * @file cpu_context.S
 *
 * @date Jun 05, 2009
 * @author Eldar Abusalimov
 */

#include "cpu_context.h"
#include "sparc_regs.h"

	.section ".text"
	.align 4

#define g_base    g1
#define g_iter    g2
#define g_scratch g3
#define g_wim     g4
#define g_psr     g5
#define g_retpc   g6

// TODO cpu_context_save comments
	.global cpu_context_save
cpu_context_save:

	/* save global registers */
	STORE_CC_GLOBALS(l_base)
	/* At this point we can quietly use globals */

	STORE_CC_PRIV(l_base, g_scratch)

	/* save PSR and WIM to restore them later */
	rd %wim, %g_wim
	rd %psr, %g_psr

	mov %l_base, %g_base

	/*  reset PSR_CWP and WIM */
	wr %g0, %g0, %wim
	PSR_BIT_CLEAR_SHORT(PSR_CWP, g_scratch)
	 WRITE_PAUSE
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
	wr %g_wim, %g0, %wim
	wr %g_psr, %g0, %psr
	 WRITE_PAUSE
	/* We're in the initial window pointed at the call time now */

	/* restore affected globals */
	LOAD_CC_GLOBALS(l_base)

	jmpl %l_retpc + 8, %g0
	 nop

// TODO cpu_context_restore comments
	.global cpu_context_restore
cpu_context_restore:

	/* save arguments to globals as far as
	 * locals will be completely overwtitten
	 */
	mov %l_retpc, %g_retpc
	mov %l_base, %g_base

	/*  reset PSR_CWP and WIM */
	wr %g0, %g0, %wim
	PSR_BIT_CLEAR_SHORT(PSR_CWP, g_scratch)
	 WRITE_PAUSE
	/* We're in window #0 now */

	add %g_base, REG_WINDOW_SZ * (CORE_NWINDOWS-1), %g_iter
	/* iterate over the windows */
	load_regwin:
		LOAD_CC_REG_WINDOW(g_iter)
		sub %g_iter, REG_WINDOW_SZ, %g_iter
	cmp  %g_iter,%g_base
	bge load_regwin
	 save

	LOAD_CC_PRIV(l_base, g_scratch)
	/* We're in the window pointed at the _save_ call time now */

	/* restore arguments */
	mov %g_retpc, %l_retpc
	mov %g_base, %l_base

	/* restore globals */
	LOAD_CC_GLOBALS(l_base)

	jmpl %l_retpc + 8, %g0
	 nop

