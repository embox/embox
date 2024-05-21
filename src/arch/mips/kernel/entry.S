/**
 * @file
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#include <asm/mipsregs.h>
#include <asm/entry.h>
#include <asm/asm.h>

/**
 * General exception vector for all other CPUs. It saves all registers and
 * calls c handler for special type exception.
 * This code is called from mips_first_exception handler. Register k1 contains
 * exception type.
 */
LEAF(mips_second_exception_handler)
	/* save all needed registers */
	SAVE_ALL

	/* return address for exit from exception */
	PTR_LA  $ra, restore_from_exception

	/* Check for pending interrupts */
	li      $t1, CAUSE_IM
	and     $t1, $k1, $t1
	beqz    $t1, 1f

	/* Call interrupt handler */
	PTR_LA  $k0, mips_interrupt_handler
	jr      $k0
	nop
1:
	/* Arg 0: saved regs */
	move    $a0, $sp

	/* Arg 1: exception number */
	andi    $a1, $k1, 0x7c

	/* Call exception handler */
	PTR_LA  $k0, mips_interrupt_handler
	jr      $k0
	nop

	/* label for exception return address */
restore_from_exception:
	/* restore all registers and return from exception */
	RESTORE_ALL
END(mips_second_exception_handler)

/**
 * General exception vector for all other CPUs.
 *
 * Be careful when changing this, it has to be at most 128 bytes
 * to fit into space reserved for the exception handler.
 *
 * This code must place into EBase+0x180 address
 */
NESTED(mips_first_exception_handler, 0, $sp)
/* save the current status of flags */
.set push
	mfc0    $k1, $CP0_CAUSE
	PTR_LA  $t1, mips_second_exception_handler
	jr      $t1
	nop
/* restore the previous status of flags */
.set pop
END(mips_first_exception_handler)
