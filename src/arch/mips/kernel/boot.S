/**
 * @file
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#include <asm/asm.h>

.section .text
.align 4
.weak hardware_init_hook

ENTRY(_start)
	/* Clean general registers */
.set noat
	move    $1, $zero /* $at */
.set at
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
#if 0
	move    $28, $zero
#endif
	move    $29, $zero
	move    $30, $zero
	move    $31, $zero

	/* Clean hi and lo registers */
	mtlo    $zero
	mthi    $zero

	/* Check hardware_init_hook and call if needed */
	PTR_LA  $13, hardware_init_hook
	beqz    $13, 1f
	nop
	jal     hardware_init_hook
	nop
1:

	/* Setup stack pointer */
	PTR_LA  $sp, _stack_top

	/* Zero .bss section */
	PTR_LA  $a0, _bss_vma
	move    $a1, $zero
	PTR_LA  $a2, _bss_len
	jal     memset
	nop

	/* Copy .data section */
	PTR_LA  $a2, _data_len
	PTR_LA  $a1, _data_lma
	PTR_LA  $a0, _data_vma
	jal     memcpy
	nop

	/* Init exceptions */
	jal     mips_exception_init
	nop

	/* Start kernel */
	j       kernel_start
	nop
END(_start)
