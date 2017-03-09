/**
 * @file
 * @brief
 *
 * @date 09.03.17
 * @author Anton Bondarev
 */

#include <asm/entry.h>
#include <asm/ptrace.h>

	.text
	.align 4

	.global ptregs_jmp
ptregs_jmp:
	RESTORE_ALL

