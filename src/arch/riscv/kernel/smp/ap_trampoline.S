/**
 * @file
 * @brief
 *
 * @date 22.08.2024
 * @author Zeng Zixian
 */

#include <asm/linkage.h>
#include <asm/csr.h>
#include <asm/asm.h>

	.section .text

C_ENTRY(__ap_trampoline):

	/* Reload the global pointer */
	la      gp, __global_pointer$

	la      t0, riscv_trap_handler
	csrw    CSR_TVEC, t0

	/* register cpuid with small stack */
	la      t0, __tmp_small_stack
	mv      sp, t0
	call    cpuid_register

	/* Setting up stack */
	la      t0, __ap_sp
	REG_L   t1, 0(t0)
	mv      sp, t1

	/* Jumping forward */
	jal    startup_ap

	/* Should not return */
loop:
	la      t0, loop
	csrw    CSR_TVEC, t0
	j       loop

	.section .data
	.align 0x4
C_LABEL(__ap_sp):
	.space 4

	.align 16
	.space 0x80
C_LABEL(__tmp_small_stack):
