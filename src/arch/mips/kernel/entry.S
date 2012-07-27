#include <asm/linkage.h>
#include <asm/mipsregs.h>
#include <asm/entry.h>


/*
 * General exception vector for all other CPUs.
 *
 * Be careful when changing this, it has to be at most 128 bytes
 * to fit into space reserved for the exception handler.
 *
 * This code must place into EBASE+0x180 address
 */
NESTED(mips_first_exception_handler, 0, $sp)
	.set    push                 /* save the current status of flags */

	mfc0    $k1, $CP0_CAUSE
	andi    $k1, $k1, 0x7c       /* read exception number */

	PTR_L   $k0, exception_handlers($k1) /* exception number is an offset in array */
	jr      $k0                  /* call special handler. $k1 contains exception number */

	.set    pop                  /* restore the previous status of flags */
END(mips_first_exception_handler)

//http://www.doc.ic.ac.uk/lab/secondyear/spim/node10.html
//https://www.ibm.com/developerworks/mydeveloperworks/blogs/ddou/entry/implementing_mips_interrupts_for_u_boot28?lang=en

//http://linux.junsun.net/porting-howto/
//http://winfred-lu.blogspot.com/2011/04/mips-exceptions-initialization-and.html
//http://stackoverflow.com/questions/4211555/clock-implementation-in-mips
//https://www.d.umn.edu/~cprince/courses/cs2521fall09/programming/lab8/

NESTED(mips_second_exception_handler, PT_SIZE, $sp)
	SAVE_AT
	SAVE_STATIC
	SAVE_ALL

	move $a0, $sp
	addi $a0, 16            /* Arg 0: saved regs. */
	jal  mips_c_exception_handler  /* Call C code. */
	lui  $gp, 0x8000        /* Set global pointer (delay slot) */


	RESTORE_ALL

	lw    $31, (16+PT_STATUS) ($sp) /* K0 = saved status */
	ori   $31, ST0_EXL                /* Set EXL exception level */
	mtc0  $k0, $CP0_STATUS            /* put CP0_STATUS back: disable interrupts */
		ehb /* hazard barier */


	lw    $k0, (16+PT_PC) ($sp)     /* K0 = EPC */
	mtc0  $k0, $CP0_EPC               /* put PC in EPC */

	lw    $31, (16+PT_RA) ($sp)
	lw    $sp, (16+PT_SP) ($sp)  /* Restore stack */

	/* Return from exception */
	ehb
	//eret   /* PC <= EPC; EXL <= 0 */
	rfe
	jr $k0
END(mips_second_exception_handler)
