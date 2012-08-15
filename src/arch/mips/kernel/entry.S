/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/mipsregs.h>
#include <asm/entry.h>
#include <asm/asm.h>


/*
 * General exception vector for all other CPUs.
 *
 * Be careful when changing this, it has to be at most 128 bytes
 * to fit into space reserved for the exception handler.
 *
 * This code must place into EBASE+0x180 address
 */
NESTED(mips_first_exception_handler, 0, $sp)
	.set      push                 /* save the current status of flags */

	mfc0      $k1, $CP0_CAUSE
	andi      $k1, $k1, 0x7c       /* read exception number */

	PTR_L     $k0, exception_handlers($k1) /* exception number is an offset in array */
	jr        $k0                  /* call special handler. $k1 contains exception number */

	.set      pop                  /* restore the previous status of flags */
END(mips_first_exception_handler)

NESTED(mips_second_exception_handler, PT_SIZE, $sp)
	SAVE_ALL

	move $a0, $sp                  /* Arg 0: saved regs. */
	jal  mips_c_exception_handler  /* Call C code. */
	nop

	RESTORE_ALL
END(mips_second_exception_handler)


NESTED(mips_interrupt_handler, PT_SIZE, $sp)
	SAVE_ALL

	move $a0, $sp                  /* Arg 0: saved regs. */
	jal  mips_c_interrupt_handler  /* Call C code. */
	nop

	RESTORE_ALL
END(mips_interrupt_handler)
