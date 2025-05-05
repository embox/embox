/**
 * @file
 * @brief
 *
 * @date 22.08.2024
 * @author Zeng Zixian
 */

#include <asm/linkage.h>
#include <asm/regs.h>
#include <asm/asm.h>

	.section .text

C_ENTRY(__ap_trampoline):

	/* Reload the global pointer */
	la      gp, __global_pointer$

	la      t0, riscv_trap_handler
#if OPTION_GET(BOOLEAN,mik32_fixup)
	la      t0, _start
#endif
	csrw    TRAP_VECTOR_REG, t0

	/* register cpuid with small stack */
	la      t0, __tmp_small_stack
	addi    t0, t0, -0x80
	mv      sp, t0
	call    cpuid_register

	/* a0 returned with cpuid */
	addi    a0, a0, -1
	li      a1, 0x4
	mul     a1, a1, a0

	/* Setting up stack */
	la      t0, __ap_sp
	PTR_L   t1, 0(t0)
	mv      sp, t1

	/* Jumping forward */
	jal    startup_ap

	/* Should not return */
loop:
	la      t0, loop
	csrw    TRAP_VECTOR_REG, t0
	j       loop

	.section .data
	.align 0x4
C_LABEL(__ap_sp):
	.space 4

	.align 16
C_LABEL(__tmp_small_stack):
	.space 0x80

