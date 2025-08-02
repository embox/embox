/**
 * @file
 * @brief
 *
 * @date 29.11.2019
 * @author Nastya Nizharadze
 */

#include <asm/asm.h>
#include <asm/csr.h>
#include <asm/entry.h>

#include <framework/mod/options.h>

#define TVEC_ALIGNMENT OPTION_GET(NUMBER, tvec_alignment)

.section .text
.global riscv_trap_handler

.align TVEC_ALIGNMENT
riscv_trap_handler:
	SAVE_ALL
	csrr    a1, CSR_CAUSE
	srli    t0, a1, (__riscv_xlen - 1)
	bnez    t0, 1f

	/* exception handling */
	add     a0, sp, x0
	call    riscv_exception_handler
	j       riscv_trap_exit
1:
	/* interrupt handling */
	call    riscv_interrupt_handler

riscv_trap_exit:
	RESTORE_ALL
#if RISCV_SMODE
	sret
#else
	mret
#endif
