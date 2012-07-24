#include <asm/linkage.h>
#include <asm/mipsregs.h>
#include <asm/entry.h>



//http://stackoverflow.com/questions/4211555/clock-implementation-in-mips
C_ENTRY(_exeption):
	SAVE_ALL

	move $a0, $sp
	addi $a0, 16            /* Arg 0: saved regs. */
	jal  exception_handler  /* Call C code. */
	lui  $gp, 0x8000        /* Set global pointer (delay slot) */


	RESTORE_ALL

/* Do not use k0/k1 here, as interrupts are still enabled */
	lw    $31, (16+FRAME_STATUS*4) ($sp)  /* K0 = saved status */
	ori   $31, ST0_EXL                     /* Set EXL */
	mtc0  $k0, $CP0_STATUS          /* put SR back: disable interrupts */
		ehb /* hazard barier */


	lw    $k0, (16+FRAME_PC*4) ($sp)    /* K0 = EPC */
	mtc0  $k0, $CP0_EPC                 /* put PC in EPC */
//            ext     $k1, $31, 27, 1                 # get RP bit: single-step request

	lw    $31, (16+FRAME_RA*4) ($sp)
	lw    $sp, (16+FRAME_SP*4) ($sp)  /* Restore stack */

	/* Return from exception */
	ehb
	eret   /* PC <= EPC; EXL <= 0 */
