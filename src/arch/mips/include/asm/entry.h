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

#include <asm/mipsregs.h>

#ifdef __ASSEMBLER__

/*
 * Offsets of register values in saved context.
 */
#define FRAME_R1     0
#define FRAME_R2     1
#define FRAME_R3     2
#define FRAME_R4     3
#define FRAME_R5     4
#define FRAME_R6     5
#define FRAME_R7     6
#define FRAME_R8     7
#define FRAME_R9     8
#define FRAME_R10    9
#define FRAME_R11    10
#define FRAME_R12    11
#define FRAME_R13    12
#define FRAME_R14    13
#define FRAME_R15    14
#define FRAME_R16    15
#define FRAME_R17    16
#define FRAME_R18    17
#define FRAME_R19    18
#define FRAME_R20    19
#define FRAME_R21    20
#define FRAME_R22    21
#define FRAME_R23    22
#define FRAME_R24    23
#define FRAME_R25    24
#define FRAME_GP     25
#define FRAME_SP     26
#define FRAME_FP     27
#define FRAME_RA     28
#define FRAME_LO     29
#define FRAME_HI     30
#define FRAME_STATUS 31
#define FRAME_PC     32

#define FRAME_WORDS  33


#define SAVE_ALL \
	.set noat;                            \
	mfc0  $k0, $CP0_STATUS;                \
	move  $k1, $sp;                       \
	addi  $sp, -16-FRAME_WORDS*4;         \
	sw    $k0, (16+FRAME_STATUS*4) ($sp); \
	sw    $k1, (16+FRAME_SP*4) ($sp);     \
	/* Save general registers */         \
    .set noat;                            \
		sw    $1, (16+FRAME_R1*4) ($sp);   \
		sw    $2, (16+FRAME_R2*4) ($sp);   \
		sw    $3, (16+FRAME_R3*4) ($sp);   \
		sw    $4, (16+FRAME_R4*4) ($sp);   \
		sw    $5, (16+FRAME_R5*4) ($sp);   \
		sw    $6, (16+FRAME_R6*4) ($sp);   \
		sw    $7, (16+FRAME_R7*4) ($sp);   \
		sw    $8, (16+FRAME_R8*4) ($sp);   \
		sw    $9, (16+FRAME_R9*4) ($sp);   \
		sw    $10, (16+FRAME_R10*4) ($sp); \
		sw    $11, (16+FRAME_R11*4) ($sp); \
		sw    $12, (16+FRAME_R12*4) ($sp); \
		sw    $13, (16+FRAME_R13*4) ($sp); \
		sw    $14, (16+FRAME_R14*4) ($sp); \
		sw    $15, (16+FRAME_R15*4) ($sp); \
		sw    $16, (16+FRAME_R16*4) ($sp); \
		sw    $17, (16+FRAME_R17*4) ($sp); \
		sw    $18, (16+FRAME_R18*4) ($sp); \
		sw    $19, (16+FRAME_R19*4) ($sp); \
		sw    $20, (16+FRAME_R20*4) ($sp); \
		sw    $21, (16+FRAME_R21*4) ($sp); \
		sw    $22, (16+FRAME_R22*4) ($sp); \
		sw    $23, (16+FRAME_R23*4) ($sp); \
		sw    $24, (16+FRAME_R24*4) ($sp); \
		sw    $25, (16+FRAME_R25*4) ($sp); \
		# Skip $26 - K0                    \
		# Skip $27 - K1                    \
		sw    $28, (16+FRAME_GP*4) ($sp);  \
		# Skip $29 - SP                    \
		sw    $30, (16+FRAME_FP*4) ($sp);  \
		sw    $31, (16+FRAME_RA*4) ($sp);  \
	.set at;                               \
	/* Save special registers */           \
	mfhi $k0;                              \
	sw   $k0, (16+FRAME_HI*4) ($sp);       \
	mflo $k0;                              \
	sw   $k0, (16+FRAME_LO*4) ($sp);       \
	mfc0 $k0, $CP0_EPC;                     \
	sw   $k0, (16+FRAME_PC*4) ($sp);


#define RESTORE_ALL \
	lw   $a0, (16+FRAME_LO*4) ($sp);   \
	mtlo $a0;                          \
	lw   $a0, (16+FRAME_HI*4) ($sp);   \
	mthi $a0;                          \
	/* Load general registers */      \
	.set noat;                         \
	lw    $1, (16+FRAME_R1*4) ($sp);   \
	lw    $2, (16+FRAME_R2*4) ($sp);   \
	lw    $3, (16+FRAME_R3*4) ($sp);   \
	lw    $4, (16+FRAME_R4*4) ($sp);   \
	lw    $5, (16+FRAME_R5*4) ($sp);   \
	lw    $6, (16+FRAME_R6*4) ($sp);   \
	lw    $7, (16+FRAME_R7*4) ($sp);   \
	lw    $8, (16+FRAME_R8*4) ($sp);   \
	lw    $9, (16+FRAME_R9*4) ($sp);   \
	lw    $10, (16+FRAME_R10*4) ($sp); \
	lw    $11, (16+FRAME_R11*4) ($sp); \
	lw    $12, (16+FRAME_R12*4) ($sp); \
	lw    $13, (16+FRAME_R13*4) ($sp); \
	lw    $14, (16+FRAME_R14*4) ($sp); \
	lw    $15, (16+FRAME_R15*4) ($sp); \
	lw    $16, (16+FRAME_R16*4) ($sp); \
	lw    $17, (16+FRAME_R17*4) ($sp); \
	lw    $18, (16+FRAME_R18*4) ($sp); \
	lw    $19, (16+FRAME_R19*4) ($sp); \
	lw    $20, (16+FRAME_R20*4) ($sp); \
	lw    $21, (16+FRAME_R21*4) ($sp); \
	lw    $22, (16+FRAME_R22*4) ($sp); \
	lw    $23, (16+FRAME_R23*4) ($sp); \
	lw    $24, (16+FRAME_R24*4) ($sp); \
	lw    $25, (16+FRAME_R25*4) ($sp); \
	# Skip $26 - K0                   \
	# Skip $27 - K1                   \
	lw    $28, (16+FRAME_GP*4) ($sp);  \
	# Skip $29 - SP                   \
	lw    $30, (16+FRAME_FP*4) ($sp);  \
	.set  at;


#endif


#endif /* MIPS_ENTRY_H_ */
