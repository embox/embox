/**
 * @file
 *
 * @brief
 *
 * @date 29.11.2019
 * @author Nastya Nizharadze
 */

#include <asm/entry.h>
#include <asm/ptrace.h>

.align 4
.global trap_handler
trap_handler:

	SAVE_ALL
	csrr    t6, mcause
	srli    t6, t6, 31
	beqz    t6, riscv_exception_handler
	mv      a0, sp
	jal     riscv_interrupt_handler
	RESTORE_ALL
	mret

.align 4
riscv_exception_handler:

	lw      t6, PT_PC(sp)
	addi    t6, t6, 4
	sw      t6, PT_PC(sp)
	RESTORE_ALL
	mret
