/**
 * @file
 * @brief
 *
 * @date 29.07.2014
 * @author Alexander Kalmuk
 */

#include <asm/asi.h>
#include <asm/entry.h>
#include <asm/mmu_consts.h>
#include <asm/stack.h>
#include <asm/psr.h>

	.text
	.align 4

	.global srmmu_fault_entry
srmmu_fault_entry:
	SAVE_ALL

	mov LEON_CNR_FADDR, %l5
	mov LEON_CNR_F, %l4

	lda [%l4] ASI_M_MMUREGS, %o0 ! read sfsr

	or %t_psr, PSR_PIL, %g1
	wr %g1, %g0, %psr
	 WRITE_PAUSE
	wr %g1, PSR_ET, %psr
	 WRITE_PAUSE

	call mmu_handle_page_fault
	 lda [%l5] ASI_M_MMUREGS, %o1 ! read sfar

	RESTORE_ALL
