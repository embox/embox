/**
 * @file
 *
 * @brief
 *
 * @date 18.06.2020
 * @author sksat
 */

#include <asm/entry.h>
#include <asm/ptrace.h>

.align 8
.global trap_handler
trap_handler:
	SAVE_ALL

	csrr    t6, mcause
	srli    t6, t6, 63
	beqz    t6, riscv64_exception_handler
	mv      a0, sp
	jal     riscv64_interrupt_handler

	RESTORE_ALL
	mret

.align 8
riscv64_exception_handler:

	RESTORE_ALL
	mret
