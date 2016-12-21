/**
 * @file
 *
 * @date 19.12.2016
 * @author Anton Bondarev
 */


#include <asm/linkage.h>
#include <asm/entry.h>

C_ENTRY(ptregs_jmp):
	move    $sp, $a0                     /* Arg 0: saved regs. */
	RESTORE_TEMP
	RESTORE_STATIC
	RESTORE_AT
	RESTORE_SOME
	LONG_L       $sp, PT_SP($sp) /* Restore stack */
	j       $ra
	nop
