/**
 * @file
 *
 * @date 28.03.2010
 * @author Nikolay Korotky
 */

#include <asm/asi.h>

	.seg   "text"
//	.proc  0
//	.align PAGE_SIZE

/* SRMMU_TFAULT() SRMMU_DFAULT()
 * %l0 : %psr
 * %l1 = pc            :set by trap
 * %l2 = npc           :set by trap
 * %l3 : %wim
 * %l6 : 1 on inst, 0 on data
 */
	.global srmmu_fault
srmmu_fault:
	mov 0x400, %l5
	mov 0x300, %l4
	lda [%l5] ASI_M_MMUREGS, %l6 ! read sfar first
	lda [%l4] ASI_M_MMUREGS, %l5 ! read sfsr last

	set pth_addr,%l7 ! repair pth
	ld  [%l7],%l7
	ld  [%l7],%l4
	ld  [%l7+4],%l5
	ld  [%l7+8],%l6
	or  %l6,0x0,%l6 ! one page
	sta %g0, [%l6] ASI_M_FLUSH_PROBE

	st  %l5,[%l4]
	inc 12,%l7
	set pth_addr,%l5
	st  %l7,[%l5]

	jmp  %l1 ! reexecute command
	rett %l2

	.seg    "data"
	.align  8
	.global pth_addr
pth_addr:
	.word 0
	.align 8
