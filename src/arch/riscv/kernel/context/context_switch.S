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

#define __context_status_offset (CALLEE_SAVED_REGS_SIZE + REG_SIZE_X * 0)
#define __context_fcsr_offset   (CALLEE_SAVED_REGS_SIZE + REG_SIZE_X * 1)

.section .text
.global context_switch

.align 2
context_switch:
	/* Store callee-saved X registers */
	store_callee_saved_x base=a0, offset=0

	/* Store [m|s]status register */
	csrr    t0, CSR_STATUS
	REG_S   t0, __context_status_offset(a0)

#ifdef __riscv_f
	/* Check if [m|s]status.FS is Clean or Dirty */
	lui     t1, (CSR_STATUS_FS_USED >> 12)
	and     t0, t0, t1
	beqz    t0, 1f

	/* Store fcsr register */
	frcsr   t0
	sw      t0, __context_fcsr_offset(a0)

	/* Store callee-saved F registers */
	store_callee_saved_f base=a0, offset=0
1:
#endif

	/* Load [m|s]status register */
	REG_L   t0, __context_status_offset(a1)
	csrw    CSR_STATUS, t0

#ifdef __riscv_f
	/* Check if [m|s]status.FS is Clean or Dirty */
	and     t0, t0, t1
	beqz    t0, 2f

	/* Load callee-saved F registers */
	load_callee_saved_f base=a1, offset=0

	/* Load fcsr register */
	lw      t0, __context_fcsr_offset(a1)
	fscsr   t0

	/* [m|s]status.FS = Dirty */
	lui     t1, (CSR_STATUS_FS_DIRTY >> 12)
	csrs    CSR_STATUS, t1
2:
#endif

	/* Load callee-saved X registers */
	load_callee_saved_x base=a1, offset=0

	ret
