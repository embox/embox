/**
 * @file
 *
 * @brief
 *
 * @date 29.11.2019
 * @author Nastya Nizharadze
 */
#include <asm/asm.h>
#include <asm/entry.h>
#include <asm/ptrace.h>

.section .text

.global riscv_trap_handler

.align 2
riscv_trap_handler:
	SAVE_ALL
	csrr    a1, CAUSE_REG
	srli    t0, a1, (__riscv_xlen - 1)
	bnez    t0, 1f

	/* exception handling */
	mv      a0, sp
	call    riscv_exception_handler
	j       riscv_trap_exit
1:
	/* interrupt handling */
	call    riscv_interrupt_handler

riscv_trap_exit:
	RESTORE_ALL
	RET_INSTR
