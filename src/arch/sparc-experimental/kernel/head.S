/**
 * @file
 *
 * @brief Initial boot code.
 *
 * @author Eldar Abusalimov
 */

#include <asm/reg_alias.h>

 	.weak hardware_init_hook
 	.weak software_init_hook

	.section ".trap_table.text"
	.align 0x1000

	.global start
start:
trap_table:
#include "trap_table.inc"

	.text
	.align 4

/*
 * At the boot time we are in an arbitrary register window.
 * Therefore we reset CWP field of PSR to zero and mark the _previous_ window
 * as invalid (set WIM to 1 << 1). So it is possible to do (NWINDOWS - 1) SAVEs
 * before the WOF trap will occure for the first time.
 *
 * WIM layout:
 *
 * <-- restore, rett                 trap, save -->
 *       +-----+---+---+---+---+---+---+---+---+
 *  win# | ... | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *  bits |     |   |   |   |   |   |   | * |   |
 *       +-----+---+---+---+---+---+---+---+---+
 *                                           ^-CWP
 */
boot_start:
	/* set PSR initial value */
	set (PSR_PS | PSR_S), %g1
	wr  %g1, %g0, %psr
	 WRITE_PAUSE

	/* setup trap table address */
	set trap_table, %g1
	wr  %g1, %g0, %tbr
	 WRITE_PAUSE

	/* reset some control regs */
	wr %g0, %g0, %y
	 WRITE_PAUSE

	/* clear wim to iterate over the windows */
	wr %g0, %g0, %wim
	 WRITE_PAUSE

	/* clean all register windows */
	save /* get into the last window */
		rd %psr, %g1
	restore /* return to window #0 */
	and %g1, PSR_CWP, %g1

1:	subcc %g1, 1, %g1
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
 	bg 1b
	 save

	/* setup WIM */
	mov (1 << 1), %g1
	wr %g1, %g0, %wim

	/* clear global registers. */
	mov %g0, %g1
	mov %g0, %g2
	mov %g0, %g3
	mov %g0, %g4
	mov %g0, %g5
	mov %g0, %g6
	mov %g0, %g7

	/* give us a stack! */
	set __stack, %g1
	mov %g1, %fp
	sub %g1, 0x60, %sp

	PSR_BIT_TOGGLE(PSR_ET, g1)
	 WRITE_PAUSE

	call hal_main
	 nop

	ta 0 ! die
	 nop
