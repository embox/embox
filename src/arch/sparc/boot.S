/**
 * @file boot.S
 *
 * @brief Low-level boot code.
 *
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#include "asm/leon_config.h"
#include "asm/memory_map.h"
#include "asm/sparc_regs.h"
#include "autoconf.h"

 	.weak hardware_init_hook
 	.weak software_init_hook
	.section    ".text"
	.align  4

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
	.global boot_start, warm_start
boot_start:
	/* set PSR initial value */
	set	(PSR_PS | PSR_S), %g1
	wr	%g1, %g0, %psr
	 WRITE_PAUSE

	/* clear wim to iterate over the windows */
	wr %g0, %g0, %wim
	 WRITE_PAUSE

	/* setup trap table address */
	set _trap_table, %g1
	wr  %g1, %g0, %tbr
	 WRITE_PAUSE

	/* reset some control regs */
	wr %g0, %g0, %y
	 WRITE_PAUSE

	/* clear global registers. */
	mov %g0, %g1
	mov %g0, %g2
	mov %g0, %g3
	mov %g0, %g4
	mov %g0, %g5
	mov %g0, %g6
	mov %g0, %g7

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

	/* setup wim*/
	mov (1 << 1), %g1
	wr %g1, %g0, %wim

	/* setup MMU */
// TODO move somewhere -- Eldar
#define MMU_CTRL_REG            0x000
#define MMU_CTXTBL_PTR          0x100
#define MMU_CTX_REG             0x200
#define MMU_FSR                 0x300
#define MMU_FAR                 0x400
	mov MMU_CTRL_REG, %g1
	sta %g0, [%g1] 0x19
	mov MMU_CTXTBL_PTR, %g1
	sta %g0, [%g1] 0x19
	mov MMU_CTX_REG, %g1
	sta %g0, [%g1] 0x19
	mov	MMU_FSR, %g1
	lda	[%g1] 0x19, %g0 /* reset FSR */
	/* Flush MMU TLB. */
	set 0x400, %g1
	sta %g0, [%g1] 0x18

	flush
	 nop; nop; nop; nop; nop;
	/* turn cache on */
	set 0x81000f, %g1 // TODO wtf? -- Eldar
	sta %g1, [%g0] 0x2

	/* give us a stack! */
	set __stack, %fp
	sub %fp, 0x60, %sp

	PSR_BIT_TOGGLE_SHORT(PSR_ET, g1)
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
