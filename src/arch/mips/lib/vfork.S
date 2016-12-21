/**
 * @file
 *
 * @date 19.12.2016
 * @author Anton Bondarev
 */


#include <asm/linkage.h>
#include <asm/entry.h>

C_ENTRY(vfork):
	SAVE_ALL
	move    $a0, $sp                     /* Arg 0: saved regs. */
	jal vfork_body
	nop
