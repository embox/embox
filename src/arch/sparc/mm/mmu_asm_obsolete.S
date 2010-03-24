#include <asm/mmu_obsolete.h>
#include <asm/asi.h>

	.seg	"text"
	.proc	0
	.align PAGE_SIZE
	.global mmu_func1
mmu_func1:
	save  %sp, -112, %sp
	ret
	restore

	.align 4

	/**may be testing data cache? */
	.global mmu_double
mmu_double:
	save  %sp, -112, %sp
	save  %sp, -112, %sp
	save  %sp, -112, %sp
	save  %sp, -112, %sp
	save  %sp, -112, %sp
	save  %sp, -112, %sp
	set 0x11111111,%l0
	set 0x22222222,%l1
	set 0x33333333,%l2
	set 0x44444444,%l3
	set 0x55555555,%l4
	set 0x66666666,%l5
	set 0x77777777,%l6
	set 0x88888888,%l7
	set 0x40000000,%g0
	std %l0,[%fp-32]
	std %l2,[%fp-24]
	std %l4,[%fp-16]
	std %l6,[%fp-8]
	mov %fp,%g1
	sub %g1,32-4,%g1
	sta %g0, [%g1] 0xf //clear data cache tag??
	sub %g1,8,%g1
	sta %g0, [%g1] 0xf
	sub %g1,8,%g1
	sta %g0, [%g1] 0xf
	sub %g1,8,%g1
	sta %g0, [%g1] 0xf

	sta %g0, [%g0] 0x11//flush data cache
	save  %sp, -112, %sp
	save  %sp, -112, %sp
	restore
	restore
	nop
	nop
	ldd [%fp-32],%l0
	ldd [%fp-24],%l2
	ldd [%fp-16],%l4
	ldd [%fp-8],%l6
	nop
	nop
	restore
	restore
	restore
	restore
	restore
	ret
 	restore

/* SRMMU_TFAULT() SRMMU_DFAULT()
 * %l0 : %psr
 * %l1 = pc            :set by trap
 * %l2 = npc           :set by trap
 * %l3 : %wim
 * %l6 : 1 on inst, 0 on data
 */
	.global srmmu_fault,_srmmu_fault_svt
_srmmu_fault_svt:
	rd      %wim, %l3
srmmu_fault:
	mov	0x400, %l5
	mov	0x300, %l4
	lda	[%l5] ASI_M_MMUREGS, %l6	! read sfar first
	lda	[%l4] ASI_M_MMUREGS, %l5	! read sfsr last

	set	pth_addr,%l7			! repair pth
	ld    [%l7],%l7
	ld	[%l7],%l4
	ld	[%l7+4],%l5
	ld 	[%l7+8],%l6
	or    %l6,0x0,%l6				! one page
	sta   %g0, [%l6] ASI_M_FLUSH_PROBE

	st	%l5,[%l4]
	inc   12,%l7
	set	pth_addr,%l5
	st    %l7,[%l5]

	jmp	%l1				! reexecute command
	rett	%l2

	.seg "data"
	.align 8
	.global pth_addr, pth_addr1
pth_addr:	.word 0
	.align 8
pth_addr1:	.word 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
