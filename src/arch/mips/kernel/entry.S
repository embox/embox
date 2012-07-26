#include <asm/linkage.h>
#include <asm/mipsregs.h>
#include <asm/entry.h>


//http://linux.junsun.net/porting-howto/
//http://winfred-lu.blogspot.com/2011/04/mips-exceptions-initialization-and.html
//http://stackoverflow.com/questions/4211555/clock-implementation-in-mips

C_ENTRY(_exeption):
	SAVE_AT
	SAVE_STATIC
	SAVE_ALL

	move $a0, $sp
	addi $a0, 16            /* Arg 0: saved regs. */
	jal  exception_handler  /* Call C code. */
	lui  $gp, 0x8000        /* Set global pointer (delay slot) */


	RESTORE_ALL

	lw    $31, (16+PT_STATUS*4) ($sp) /* K0 = saved status */
	ori   $31, ST0_EXL                /* Set EXL exception level */
	mtc0  $k0, $CP0_STATUS            /* put CP0_STATUS back: disable interrupts */
		ehb /* hazard barier */


	lw    $k0, (16+PT_PC*4) ($sp)     /* K0 = EPC */
	mtc0  $k0, $CP0_EPC               /* put PC in EPC */

	lw    $31, (16+PT_RA*4) ($sp)
	lw    $sp, (16+PT_SP*4) ($sp)  /* Restore stack */

	/* Return from exception */
	ehb
	rfe //eret   /* PC <= EPC; EXL <= 0 */
	nop
