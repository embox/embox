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
 * General exception vector for all other CPUs. It saves all registers and
 * calls c handler for special type exception.
 * This code is called from mips_first_exception handler. Register k1 contains
 * exception type.
 */
 LEAF(mips_second_exception_handler)
	SAVE_ALL                             /* save all needed registers */

	PTR_L   $k0, exception_handlers($k1) /* exception number is an offset in array */
	PTR_LA  $ra, restore_from_exception  /* return address for exit from exception */
	move    $a0, $sp                     /* Arg 0: saved regs. */
	jr      $k0                          /* Call C code. */
	nop

restore_from_exception:                  /* label for exception return address */
	RESTORE_ALL                          /* restore all registers and return from exception */
END(mips_second_exception_handler)

/*
 * General exception vector for all other CPUs.
 *
 * Be careful when changing this, it has to be at most 128 bytes
 * to fit into space reserved for the exception handler.
 *
 * This code must place into EBASE+0x180 address
 */
NESTED(mips_first_exception_handler, 0, $sp)
	.set  push                 /* save the current status of flags */

	mfc0  $k1, $CP0_CAUSE
	andi  $k1, $k1, 0x7c       /* read exception number */

	j     mips_second_exception_handler /* jump to real exception handler */
	nop

	.set  pop                  /* restore the previous status of flags */
END(mips_first_exception_handler)
