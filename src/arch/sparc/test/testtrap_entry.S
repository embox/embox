/**
 * @file
 *
 * @date 25.03.2010
 * @author Alexander Batyukov
 */

#include <asm/entry.h>
#include <asm/psr.h>
	.text
	.align 4

	.global test_soft_trap_entry
test_soft_trap_entry:

	add %t_pc, 4, %t_pc
	add %t_npc, 4, %t_npc

	SAVE_ALL

	mov %tbr, %o0
	srl %o0, 4, %o0
	and %o0, 0xFF, %o0		! pass TT as parameter

	or %t_psr, PSR_PIL, %g1
	wr %g1, %g0, %psr
	 WRITE_PAUSE
	wr %g1, PSR_ET, %psr
	 WRITE_PAUSE

	call test_soft_trap_handler
	 nop

	RESTORE_ALL

