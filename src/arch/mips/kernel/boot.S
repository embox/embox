
#include <asm/linkage.h>


	.section    ".text"
	.align  4
 	.weak hardware_init_hook
 	.weak software_init_hook


C_ENTRY(_start):
		.set noat
		move    $1, $zero
		move    $2, $zero
		move    $3, $zero
		move    $4, $zero
		move    $5, $zero
		move    $6, $zero
		move    $7, $zero
		move    $8, $zero
		move    $9, $zero
		move    $10, $zero
		move    $11, $zero
		move    $12, $zero
		move    $13, $zero
		move    $14, $zero
		move    $15, $zero
		move    $16, $zero
		move    $17, $zero
		move    $18, $zero
		move    $19, $zero
		move    $20, $zero
		move    $21, $zero
		move    $22, $zero
		move    $23, $zero
		move    $24, $zero
		move    $25, $zero
		move    $26, $zero
		move    $27, $zero
		move    $28, $zero
		move    $29, $zero
		move    $30, $zero
		move    $31, $zero
		mtlo    $zero
		mthi    $zero

		PTR_LA	$sp, __stack
		j       kernel_start
		nop
		.set at

	.section    ".text"
	.align  4

