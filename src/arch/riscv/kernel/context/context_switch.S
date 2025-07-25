/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#include <asm/asm.h>
#include <asm/regs.h>
#include <asm/context.h>

#define __context_x_offset      0
#define __context_f_offset      CALLEE_SAVED_SIZE_X
#define __context_status_offset (CALLEE_SAVED_SIZE_X + CALLEE_SAVED_SIZE_F)

.section .text
.global context_switch

.align 2
context_switch:
	/* Store callee-saved X registers */
	store_callee_saved_x base=a0, offset=__context_x_offset

	/* Store [m|s]status registers */
	csrr    s0, STATUS_REG
	REG_S   s0, __context_status_offset(a0)

#if REG_SIZE_F > 0
	/* Check if FPU state is "dirty" */
	lui     s1, 0b100
	and     s0, s0, s1
	beqz    s0, 1f

	/* Store callee-saved F registers */
	store_callee_saved_f base=a0, offset=__context_f_offset
1:
#endif

	/* Load [m|s]status registers */
	REG_L   s0, __context_status_offset(a1)
	csrw    STATUS_REG, s0

#if REG_SIZE_F > 0
	/* Check if FPU state is "dirty" */
	and     s0, s0, s1
	beqz    s0, 2f

	/* Load callee-saved F registers */
	load_callee_saved_f base=a1, offset=__context_f_offset
2:
#endif

	/* Load callee-saved X registers */
	load_callee_saved_x base=a1, offset=__context_x_offset

	ret
