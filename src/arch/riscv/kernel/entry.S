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
	la      ra, riscv_trap_exit
	csrr    a1, mcause
	srli    t0, a1, (__riscv_xlen - 1)
	beqz    t0, 1f
	j       riscv_interrupt_handler
1:
	mv      a0, sp
	j       riscv_exception_handler

.align 2
riscv_trap_exit:
	RESTORE_ALL
	mret
