/**
 * @file boot.S
 *
 * @brief Low-level boot code.
 *
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include "leon_config.h"
#include "memory_map.h"
#include "sparc_regs.h"

 	.weak hardware_init_hook
 	.weak software_init_hook
	.section    ".text"
	.align  4

	.global boot_start, warm_start
boot_start:
	/* setup trap table address */
	set _trap_table, %g1
	wr  %g1, %g0, %tbr
	 WRITE_PAUSE

	/* clear wim to iterate over the windows */
	wr %g0, %g0, %wim
	 WRITE_PAUSE
	/* reset pil, icc and jump into window #0 */
	PSR_BIT_CLEAR((PSR_PIL | PSR_ICC | PSR_CWP), t0, t1)
	 WRITE_PAUSE

	/* clean all register windows */
	save // get into the last window
		rd %psr, %g1
	restore // return to window #0
	and	%g1, PSR_CWP, %g1

register_window_clean:
	mov %g0, %i0
	mov %g0, %i1
	mov %g0, %i2
	mov %g0, %i3
	mov %g0, %i4
	mov %g0, %i5
	mov %g0, %i6
	mov %g0, %i7
	mov %g0, %l0
	mov %g0, %l1
	mov %g0, %l2
	mov %g0, %l3
	mov %g0, %l4
	mov %g0, %l5
	mov %g0, %l6
	mov %g0, %l7
	deccc %g1
 	bg register_window_clean
	 save

/*
 * At the boot time we are in an arbitrary register window.
 * Therefore we reset CWP field of PSR to zero
 * and mark the _previous_ window as invalid (set WIM to 1 << 1).
 * So it is possible to do (CORE_NWINDOWS - 1) SAVEs
 * before the WOF trap will occure for the first time.
 *
 * WIM layout:
 *
 * <-- restore, rett                 trap, save -->
 *  ----------------------------------------------
 *  | win# | ... | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *  | bits |     |   |   |   |   |   |   | * |   |
 *  ----------------------------------------------
 *                                             ^-CWP
 */
	/* setup wim*/
	mov (1 << 1), %g1
	wr %g1, %g0, %wim

	/* clear global registers. TODO what for? x_0 -- Eldar */
	mov %g0, %g1
	mov %g0, %g2
	mov %g0, %g3
	mov %g0, %g4
	mov %g0, %g5
	mov %g0, %g6
	mov %g0, %g7

	/* reset some control regs */
	wr %g0, %g0, %y
	 WRITE_PAUSE

	flush
	 nop;nop;nop
	/* turn cache on */
	set 0x81000F, %g1 // TODO wtf? -- Eldar
	sta %g1, [%g0] 2

#define src g1
#define dst g2
#define end g3
#define tmp0 g4
#define tmp1 g5
	/* copy data sections */
	set _endtext, %src
	set _data_start, %dst
	set _data_end, %end
	cmp %dst, %end
	bge 2f
	 nop

1:	ldd [%src], %tmp0
	std %tmp0, [%dst]
	add %dst, 8, %dst
	cmp %dst, %end
	bl  1b
	 add %src, 8, %src

	/* Zero bss */
2:	set _bstart, %dst
	set _bend, %end
	mov %g0, %tmp0
	mov %g0, %tmp1
	cmp %dst, %end
	bge 2f
	 sub %end, 8, %end

1:	std %tmp0, [%dst]
	cmp %dst, %end
	bl  1b
	 add %dst, 8, %dst

2:

	/* give us a stack! */
	set __stack, %g1
	mov %g1, %fp
	sub %g1, 0x60, %sp

	PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
	 WRITE_PAUSE

	set hardware_init_hook, %g1
	tst %g1
	be  1f
	 nop
	call %g1
1:	 nop

	set software_init_hook, %g1
	tst %g1
	be  1f
	 nop
	call %g1
1:	 nop

	call main
	 nop

	ta 0  // die
	 nop
