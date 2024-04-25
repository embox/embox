/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

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
	li      $t1, CAUSE_IM                /*  is a mask, which mean that Interrupt bit of CAUSE register set in 1 */
	and     $t1, $k1, $t1                /* check if Timer Interrupt bit of CAUSE register is 1 */
	bnez    $t1, cp0_timer_irq_hnd       /* if Timer Interrupt bit of CAUSE register is 1 then go cp0_timer_irq_hnd */
	andi    $k1, $k1, 0x7c               /* read exception number */
	PTR_L   $k0, exception_handlers($k1) /* exception number is an offset in array */
	PTR_LA  $ra, restore_from_exception  /* return address for exit from exception */
	move    $a0, $sp                     /* Arg 0: saved regs. */

	jr      $k0                          /* Call C code. */
	nop
cp0_timer_irq_hnd:
	PTR_L   $k0, exception_handlers($zero)/* exception number is 0, that means that timer interrupt occur */
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
 * This code must place into KSEG0+0x180 address
 */
NESTED(mips_first_exception_handler, 0, $sp)
	.set  push                 /* save the current status of flags */

	mfc0  $k1, $CP0_CAUSE
	la    $t1, mips_second_exception_handler
	jr    $t1                  /* jump to real exception handler */
	nop

	.set  pop                  /* restore the previous status of flags */
END(mips_first_exception_handler)
