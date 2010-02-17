/**
 * @file
 * @brief Initial boot code for SPARC. Also includes trap table.
 *
 * @author Eldar Abusalimov
 */

#include <asm/head.h>
#include <asm/regs.h>
#include <asm/psr.h>

	.weak hardware_init_hook
	.weak software_init_hook

	.section ".trap_table.text", #alloc, #execinstr
	.align 0x1000

	.global start
start:
	.global trap_table
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
	set (PSR_PS | PSR_S | PSR_PIL), %g1
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

	set hardware_init_hook, %g1
	tst %g1
	be  1f
	 nop
	call %g1
1:	 nop

	/* give us a stack! */
	set __stack, %g1
	mov %g1, %fp
	sub %g1, 0x60, %sp

	rd %psr, %g1
	wr %g1, PSR_ET, %psr
	 WRITE_PAUSE

	set _data_vma, %o0
	set _data_lma, %o1
	subcc %o0, %o1, %g0
	be 1f
	 nop
	set _data_len, %o2
	call memcpy
1:	 nop

	set _bss_vma, %o0
	set _bss_len, %o2
	call memset
	 mov %g0, %o1

	set software_init_hook, %g1
	tst %g1
	be  1f
	 nop
	call %g1
1:	 nop

	call main
	 nop

	ta 0 ! die
	 nop
