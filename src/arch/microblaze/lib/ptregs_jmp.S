/**
 * @file
 *
 * @date 09.03.17
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/setjmp.h>
#include <asm/ptrace.h>

    .section .text

C_ENTRY(ptregs_jmp):
	addik   r1, r5, -PT_OFFSET
	RESTORE_REGS

	lwi	r1, r1, PT_R1;
	addik   r1, r1, STATE_SAVE_SIZE	/* Clean up stack space.  */

	rtsd    r15, 8            /* normal return */
	nop

