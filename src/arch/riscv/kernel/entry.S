/**
 * @file
 *
 * @brief
 *
 * @date 29.11.2019
 * @author Nastya Nizharadze
 */
#include <asm/asm.h>
#include <asm/regs.h>
#include <asm/entry.h>
#include <asm/ptrace.h>

#include <framework/mod/options.h>

#define TRAP_ALIGN    OPTION_GET(NUMBER,trap_align)

.section .text

.global riscv_trap_handler

.align TRAP_ALIGN
riscv_trap_handler:
	SAVE_ALL
	csrr    a1, CAUSE_REG
	srli    t0, a1, (__riscv_xlen - 1)
	bnez    t0, 1f

	csrr    a2, INTERRUPT_REG
	csrr    a3, INTPENDING_REG
	csrr    a4, EPC_REG
	csrr    a5, TRAP_VECTOR_REG
#if !SMODE
	csrr    a6, mhartid
#else
	add	a6, tp, x0
#endif

	/* exception handling */
	add     a0, sp, x0
	call    riscv_exception_handler
	j       riscv_trap_exit
1:
	/* interrupt handling */
	call    riscv_interrupt_handler

riscv_trap_exit:
	RESTORE_ALL
	RET_INSTR
