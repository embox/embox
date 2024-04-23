/**
 * @file
 *
 * @date 19.12.2016
 * @author Anton Bondarev
 */

#include <asm/asm.h>
#include <asm/entry.h>

ENTRY(vfork)
	SAVE_ALL
	move    $a0, $sp /* Arg 0: saved regs. */
	jal     vfork_body
	nop
END(vfork)
