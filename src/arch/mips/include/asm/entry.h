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
	LONG_S  $1, (PT_R1)($sp)
	.set    pop
	.endm

	.macro  SAVE_STATIC
	LONG_S  $16, PT_R16($sp)
	LONG_S  $17, PT_R17($sp)
	LONG_S  $18, PT_R18($sp)
	LONG_S  $19, PT_R19($sp)
	LONG_S  $20, PT_R20($sp)
	LONG_S  $21, PT_R21($sp)
	LONG_S  $22, PT_R22($sp)
	LONG_S  $23, PT_R23($sp)
	LONG_S  $30, PT_FP($sp)
	.endm

	.macro	RESTORE_SP_AND_RET
	.set	push
	.set	noreorder
	LONG_L	$k0, (PT_PC)($sp)
	LONG_L	$sp, (PT_SP)($sp)
	jr	$k0
	 rfe
	 nop
	.set	pop
	.endm

	//move  $k0, $sp;
#define SAVE_ALL \
	addi  $k0, $sp, PT_SIZE;      \
	sw    $k0, (PT_SP) ($sp);     \
	mfc0  $k0, $CP0_STATUS;       \
	sw    $k0, (PT_STATUS) ($sp); \
	/* Save general registers */  \
    .set noat;                    \
	sw    $1, (PT_R1) ($sp);      \
	.set at;                      \
	sw    $2, (PT_R2) ($sp);      \
	sw    $3, (PT_R3) ($sp);      \
	sw    $4, (PT_R4) ($sp);      \
	sw    $5, (PT_R5) ($sp);      \
	sw    $6, (PT_R6) ($sp);      \
	sw    $7, (PT_R7) ($sp);      \
	sw    $8, (PT_R8) ($sp);      \
	sw    $9, (PT_R9) ($sp);      \
	sw    $10, (PT_R10) ($sp);    \
	sw    $11, (PT_R11) ($sp);    \
	sw    $12, (PT_R12) ($sp);    \
	sw    $13, (PT_R13) ($sp);    \
	sw    $14, (PT_R14) ($sp);    \
	sw    $15, (PT_R15) ($sp);    \
	sw    $16, (PT_R16) ($sp);    \
	sw    $17, (PT_R17) ($sp);    \
	sw    $18, (PT_R18) ($sp);    \
	sw    $19, (PT_R19) ($sp);    \
	sw    $20, (PT_R20) ($sp);    \
	sw    $21, (PT_R21) ($sp);    \
	sw    $22, (PT_R22) ($sp);    \
	sw    $23, (PT_R23) ($sp);    \
	sw    $24, (PT_R24) ($sp);    \
	sw    $25, (PT_R25) ($sp);    \
	sw    $28, (PT_GP) ($sp);     \
	sw    $30, (PT_FP) ($sp);     \
	sw    $31, (PT_RA) ($sp);     \
	/* Save special registers */  \
	mfhi $k0;                     \
	sw   $k0, (PT_HI) ($sp);      \
	mflo $k0;                     \
	sw   $k0, (PT_LO) ($sp);      \
	mfc0 $k0, $CP0_EPC;           \
	sw   $k0, (PT_PC) ($sp);


#define RESTORE_ALL \
	lw   $a0, (PT_LO*4) ($sp);   \
	mtlo $a0;                    \
	lw   $a0, (PT_HI*4) ($sp);   \
	mthi $a0;                       \
	/* Load general registers */    \
	.set noat;                      \
	lw    $1, (PT_R1) ($sp);   \
	.set at;                   \
	lw    $2, (PT_R2) ($sp);   \
	lw    $3, (PT_R3) ($sp);   \
	lw    $4, (PT_R4) ($sp);   \
	lw    $5, (PT_R5) ($sp);   \
	lw    $6, (PT_R6) ($sp);   \
	lw    $7, (PT_R7) ($sp);   \
	lw    $8, (PT_R8) ($sp);   \
	lw    $9, (PT_R9) ($sp);   \
	lw    $10, (PT_R10) ($sp); \
	lw    $11, (PT_R11) ($sp); \
	lw    $12, (PT_R12) ($sp); \
	lw    $13, (PT_R13) ($sp); \
	lw    $14, (PT_R14) ($sp); \
	lw    $15, (PT_R15) ($sp); \
	lw    $16, (PT_R16) ($sp); \
	lw    $17, (PT_R17) ($sp); \
	lw    $18, (PT_R18) ($sp); \
	lw    $19, (PT_R19) ($sp); \
	lw    $20, (PT_R20) ($sp); \
	lw    $21, (PT_R21) ($sp); \
	lw    $22, (PT_R22) ($sp); \
	lw    $23, (PT_R23) ($sp); \
	lw    $24, (PT_R24) ($sp); \
	lw    $25, (PT_R25) ($sp); \
	lw    $28, (PT_GP) ($sp);  \
	lw    $30, (PT_FP) ($sp);


#endif


#endif /* MIPS_ENTRY_H_ */
