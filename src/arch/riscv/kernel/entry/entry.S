/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 04.08.25
 */

#include <asm/asm.h>
#include <asm/csr.h>
#include <asm/context.h>
#include <util/binalign.h>

#include <framework/mod/options.h>

#define TVEC_ALIGNMENT OPTION_GET(NUMBER, tvec_alignment)

#define __context_status_offset (CALLER_SAVED_REGS_SIZE + REG_SIZE_X * 0)
#define __context_epc_offset    (CALLER_SAVED_REGS_SIZE + REG_SIZE_X * 1)
#define __context_ra_offset     (CALLER_SAVED_REGS_SIZE + REG_SIZE_X * 2)

/* The stack pointer must always be 16-byte aligned when a function is entered */
#define __context_size binalign_bound((CALLER_SAVED_REGS_SIZE + REG_SIZE_X * 3), 16)

.section .text
.global riscv_trap_handler

.align TVEC_ALIGNMENT
riscv_trap_handler:
	/* Allocate stack space for context */
	addi    sp, sp, -__context_size

	/* Store caller-saved X registers */
	store_caller_saved_x base=sp, offset=0

	/* Store [m|s]status, [m|s]epc and ra registers */
	csrr    t0, CSR_STATUS
	csrr    t1, CSR_EPC
	REG_S   t0, __context_status_offset(sp)
	REG_S   t1, __context_epc_offset(sp)
	REG_S   ra, __context_ra_offset(sp)

#ifdef __riscv_f
	/* Check if [m|s]status.FS is Clean or Dirty */
	lui     t1, (CSR_STATUS_FS_USED >> 12)
	and     t0, t0, t1
	beqz    t0, 1f

	/* Store caller-saved F registers */
	store_caller_saved_f base=sp, offset=0

	/* [m|s]status.FS = Clean */
	lui     t1, ((CSR_STATUS_FS_DIRTY ^ CSR_STATUS_FS_CLEAN) >> 12)
	csrc    CSR_STATUS, t1
1:
#endif

	/* Check "interrupt" bit (MSB) */
	csrr    a0, CSR_CAUSE
	blt     a0, zero, 2f

	/* Exception handling */
	mv      a1, sp
	call    riscv_exception_handler
	j       riscv_trap_exit
2:
	/* Interrupt handling */
	and     a0, a0, 0x1f
	call    riscv_interrupt_handler

riscv_trap_exit:
	/* Load [m|s]status, [m|s]epc and ra registers */
	REG_L   t0, __context_status_offset(sp)
	REG_L   t1, __context_epc_offset(sp)
	REG_L   ra, __context_ra_offset(sp)
	csrrw   t2, CSR_STATUS, t0
	csrw    CSR_EPC, t1

#ifdef __riscv_f
	/* Check if FPU was used inside the interrupt handler */
	lui     t1, (CSR_STATUS_FS_DIRTY >> 12)
	and     t0, t0, t2
	and     t0, t0, t1
	xor     t0, t0, t1
	bnez    t0, 3f

	/* Load caller-saved F registers */
	load_caller_saved_f base=sp, offset=0
3:
#endif

	/* Load caller-saved X registers */
	load_caller_saved_x base=sp, offset=0

	/* Restore stack pointer */
	addi    sp, sp, __context_size

	/* Return from exception/interrupt */
#if RISCV_SMODE
	sret
#else
	mret
#endif
