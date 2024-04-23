/**
 * @file
 *
 * @brief
 *
 * @date 23.07.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_ENTRY_H_
#define MIPS_ENTRY_H_

#include <asm/ptrace.h>
#include <asm/mipsregs.h>

#ifndef __ASSEMBLER__

#define EBASE 0x80000000
/* this function must be copied to address EBASE */
extern void mips_first_exception_handler(void);

#define MIPS_EXCEPTIONS_QUANTITY  32 /* max number of exceptions handlers */

enum MIPS_EXCEPTION_TYPE {
	MIPS_EXCEPTION_TYPE_IRQ=0,
	MIPS_EXCEPTION_TYPE_SYSCALL=8
};

typedef  enum MIPS_EXCEPTION_TYPE mips_exception_type_t;


/*
 *  type for second exception handlers these handlers is hold in array and called
 * from first exception handler
 */
typedef void (*second_exception_handler_t)(pt_regs_t *);

extern void mips_exception_setup(mips_exception_type_t type, second_exception_handler_t handler);

/* MIPS general second exception handler */
extern void mips_second_exception_handler(void);

/* MIPS interrupt handler it's one of second exception handler */
extern void mips_interrupt_handler(void);


#else /* __ASSEMBLER__ */

#include <asm/asm.h>

	.macro SAVE_SOME
		.set    push

		move    $k0, $sp
		addi    $k0,  PT_SIZE
		LONG_S  $k0, (PT_SP) ($sp); /* save sp in a new frame */

		LONG_S  $2, (PT_R2) ($sp);
		LONG_S  $3, (PT_R3) ($sp);
		LONG_S  $4, (PT_R4) ($sp);
		LONG_S  $5, (PT_R5) ($sp);
		LONG_S  $6, (PT_R6) ($sp);
		LONG_S  $7, (PT_R7) ($sp);

		LONG_S  $28, (PT_GP) ($sp);
		LONG_S  $30, (PT_FP) ($sp);
		LONG_S  $31, (PT_RA) ($sp);

		/* Save special MIPS registers */
		mfc0    $k0, $CP0_STATUS
		LONG_S  $k0, PT_STATUS ($sp);
		mfhi    $k0;
		LONG_S  $k0, (PT_HI) ($sp);
		mflo    $k0;
		LONG_S  $k0, (PT_LO) ($sp);
		mfc0    $k0, $CP0_EPC;
		LONG_S  $k0, (PT_PC) ($sp);

		.set    pop
	.endm

	/* save at (r1) register without warning */
	.macro  SAVE_AT
		.set    push
		.set    noat
		LONG_S  $1, (PT_R1)($sp)
		.set    pop
	.endm

	.macro	SAVE_TEMP
		LONG_S	$8, PT_R8($sp)
		LONG_S	$9, PT_R9($sp)
		LONG_S	$10, PT_R10($sp)
		LONG_S	$11, PT_R11($sp)
		LONG_S	$12, PT_R12($sp)
		LONG_S	$13, PT_R13($sp)
		LONG_S	$14, PT_R14($sp)
		LONG_S	$15, PT_R15($sp)
		LONG_S	$24, PT_R24($sp);   /* t0 */
		LONG_S  $25, PT_R25($sp); /* t1 */
	.endm

	.macro  SAVE_STATIC
		LONG_S  $16, PT_R16($sp) /* s0 */
		LONG_S  $17, PT_R17($sp) /* s1 */
		LONG_S  $18, PT_R18($sp) /* s2 */
		LONG_S  $19, PT_R19($sp) /* s3 */
		LONG_S  $20, PT_R20($sp) /* s4 */
		LONG_S  $21, PT_R21($sp) /* s5 */
		LONG_S  $22, PT_R22($sp) /* s6 */
		LONG_S  $23, PT_R23($sp) /* s7 */
		LONG_S  $30, PT_FP($sp)  /* s8 */
	.endm

	/* restore without stack frame */
	.macro RESTORE_SOME
		.set    push

		LONG_L  $2, (PT_R2) ($sp);
		LONG_L  $3, (PT_R3) ($sp);
		LONG_L  $4, (PT_R4) ($sp);
		LONG_L  $5, (PT_R5) ($sp);
		LONG_L  $6, (PT_R6) ($sp);
		LONG_L  $7, (PT_R7) ($sp);

		LONG_L  $28, (PT_GP) ($sp);
		LONG_L  $30, (PT_FP) ($sp);
		LONG_L  $31, (PT_RA) ($sp);

		/* Restore special MIPS registers */
		mfc0    $k0, $12
		andi    $k0, $k0, 0xff00 /* preserve interrupt mask */
		LONG_L  $k1, PT_STATUS ($sp);
		andi    $k1, $k1, 0x00ff /* load only status bits */
		or      $k0, $k0, $k1 /* combine them together */
		mtc0    $k0, $CP0_STATUS
		LONG_L  $k0, (PT_HI) ($sp);
		mthi    $k0;
		LONG_L  $k0, (PT_LO) ($sp);
		mtlo    $k0;
		LONG_L  $k0, (PT_PC) ($sp);
		mtc0    $k0, $CP0_EPC;

		.set    pop
	.endm

	.macro	RESTORE_TEMP
		LONG_L	$8, PT_R8($sp)
		LONG_L	$9, PT_R9($sp)
		LONG_L	$10, PT_R10($sp)
		LONG_L	$11, PT_R11($sp)
		LONG_L	$12, PT_R12($sp)
		LONG_L	$13, PT_R13($sp)
		LONG_L	$14, PT_R14($sp)
		LONG_L	$15, PT_R15($sp)
		LONG_L	$24, PT_R24($sp); /* t0 */
		LONG_L  $25, PT_R25($sp); /* t1 */
	.endm

	.macro RESTORE_AT
		/* restore at (r1) register without warning */
		.set    push
		.set    noat
		LONG_L  $1, (PT_R1)($sp)
		.set    pop
	.endm

	.macro  RESTORE_STATIC
		LONG_L  $16, PT_R16($sp) /* s0 */
		LONG_L  $17, PT_R17($sp) /* s1 */
		LONG_L  $18, PT_R18($sp) /* s2 */
		LONG_L  $19, PT_R19($sp) /* s3 */
		LONG_L  $20, PT_R20($sp) /* s4 */
		LONG_L  $21, PT_R21($sp) /* s5 */
		LONG_L  $22, PT_R22($sp) /* s6 */
		LONG_L  $23, PT_R23($sp) /* s7 */
		LONG_L  $30, PT_FP($sp)  /* s8 */
	.endm

	/* Return from exception */
	.macro	RESTORE_SP_AND_RET
		LONG_L	$sp, PT_SP($sp) /* Restore stack */
		.set	mips3
		eret                   /* PC <= EPC; EXL <= 0 */
		.set	mips0
	.endm

	.macro SAVE_ALL
		LONG_ADDI $sp, -PT_SIZE
		SAVE_SOME
		SAVE_AT
		SAVE_TEMP
		SAVE_STATIC
	.endm

	.macro RESTORE_ALL
		RESTORE_TEMP
		RESTORE_STATIC
		RESTORE_AT
		RESTORE_SOME
		RESTORE_SP_AND_RET
	.endm

#endif /* __ASSEMBLER __ */


#endif /* MIPS_ENTRY_H_ */
