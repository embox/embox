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

#ifdef __ASSEMBLER__

	.macro  SAVE_AT
	.set    push
	.set    noat
	LONG_S  $1, PT_R1(sp)
	.set    pop
	.endm

	.macro  SAVE_STATIC
	LONG_S  $16, PT_R16(sp)
	LONG_S  $17, PT_R17(sp)
	LONG_S  $18, PT_R18(sp)
	LONG_S  $19, PT_R19(sp)
	LONG_S  $20, PT_R20(sp)
	LONG_S  $21, PT_R21(sp)
	LONG_S  $22, PT_R22(sp)
	LONG_S  $23, PT_R23(sp)
	LONG_S  $30, PT_R30(sp)
	.endm


#define SAVE_ALL \
	mfc0  $k0, $CP0_STATUS;                \
	move  $k1, $sp;                       \
	addi  $sp, -16-PT_WORDS*4;         \
	sw    $k0, (16+PT_STATUS*4) ($sp); \
	sw    $k1, (16+PT_SP*4) ($sp);     \
	/* Save general registers */          \
    .set noat;                            \
	sw    $1, (16+PT_R1*4) ($sp);   \
	.set at
	sw    $2, (16+PT_R2*4) ($sp);   \
	sw    $3, (16+PT_R3*4) ($sp);   \
	sw    $4, (16+PT_R4*4) ($sp);   \
	sw    $5, (16+PT_R5*4) ($sp);   \
	sw    $6, (16+PT_R6*4) ($sp);   \
	sw    $7, (16+PT_R7*4) ($sp);   \
	sw    $8, (16+PT_R8*4) ($sp);   \
	sw    $9, (16+PT_R9*4) ($sp);   \
	sw    $10, (16+PT_R10*4) ($sp); \
	sw    $11, (16+PT_R11*4) ($sp); \
	sw    $12, (16+PT_R12*4) ($sp); \
	sw    $13, (16+PT_R13*4) ($sp); \
	sw    $14, (16+PT_R14*4) ($sp); \
	sw    $15, (16+PT_R15*4) ($sp); \
	sw    $16, (16+PT_R16*4) ($sp); \
	sw    $17, (16+PT_R17*4) ($sp); \
	sw    $18, (16+PT_R18*4) ($sp); \
	sw    $19, (16+PT_R19*4) ($sp); \
	sw    $20, (16+PT_R20*4) ($sp); \
	sw    $21, (16+PT_R21*4) ($sp); \
	sw    $22, (16+PT_R22*4) ($sp); \
	sw    $23, (16+PT_R23*4) ($sp); \
	sw    $24, (16+PT_R24*4) ($sp); \
	sw    $25, (16+PT_R25*4) ($sp); \
	# Skip $26 - K0                    \
	# Skip $27 - K1                    \
	sw    $28, (16+PT_GP*4) ($sp);  \
	# Skip $29 - SP                    \
	sw    $30, (16+PT_FP*4) ($sp);  \
	sw    $31, (16+PT_RA*4) ($sp);  \
	/* Save special registers */           \
	mfhi $k0;                              \
	sw   $k0, (16+PT_HI*4) ($sp);       \
	mflo $k0;                              \
	sw   $k0, (16+PT_LO*4) ($sp);       \
	mfc0 $k0, $CP0_EPC;                     \
	sw   $k0, (16+PT_PC*4) ($sp);


#define RESTORE_ALL \
	lw   $a0, (16+PT_LO*4) ($sp);   \
	mtlo $a0;                          \
	lw   $a0, (16+PT_HI*4) ($sp);   \
	mthi $a0;                          \
	/* Load general registers */      \
	.set noat;                         \
	lw    $1, (16+PT_R1*4) ($sp);   \
	lw    $2, (16+PT_R2*4) ($sp);   \
	lw    $3, (16+PT_R3*4) ($sp);   \
	lw    $4, (16+PT_R4*4) ($sp);   \
	lw    $5, (16+PT_R5*4) ($sp);   \
	lw    $6, (16+PT_R6*4) ($sp);   \
	lw    $7, (16+PT_R7*4) ($sp);   \
	lw    $8, (16+PT_R8*4) ($sp);   \
	lw    $9, (16+PT_R9*4) ($sp);   \
	lw    $10, (16+PT_R10*4) ($sp); \
	lw    $11, (16+PT_R11*4) ($sp); \
	lw    $12, (16+PT_R12*4) ($sp); \
	lw    $13, (16+PT_R13*4) ($sp); \
	lw    $14, (16+PT_R14*4) ($sp); \
	lw    $15, (16+PT_R15*4) ($sp); \
	lw    $16, (16+PT_R16*4) ($sp); \
	lw    $17, (16+PT_R17*4) ($sp); \
	lw    $18, (16+PT_R18*4) ($sp); \
	lw    $19, (16+PT_R19*4) ($sp); \
	lw    $20, (16+PT_R20*4) ($sp); \
	lw    $21, (16+PT_R21*4) ($sp); \
	lw    $22, (16+PT_R22*4) ($sp); \
	lw    $23, (16+PT_R23*4) ($sp); \
	lw    $24, (16+PT_R24*4) ($sp); \
	lw    $25, (16+PT_R25*4) ($sp); \
	# Skip $26 - K0                   \
	# Skip $27 - K1                   \
	lw    $28, (16+PT_GP*4) ($sp);  \
	# Skip $29 - SP                   \
	lw    $30, (16+PT_FP*4) ($sp);  \
	.set  at;


#endif


#endif /* MIPS_ENTRY_H_ */
