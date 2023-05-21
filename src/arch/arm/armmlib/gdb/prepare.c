/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <hal/reg.h>
#include <asm/arm_m_regs.h>

void gdb_prepare_arch(void) {
	REG32_STORE(FP_LAR, FP_LAR_UNLOCK_KEY);
	REG32_ORIN(FP_CTRL, 0x3);
	REG32_ORIN(DCB_DEMCR, DCB_DEMCR_MON_EN);
}

void gdb_cleanup_arch(void) {
	REG32_CLEAR(DCB_DEMCR, DCB_DEMCR_MON_EN);
	REG32_STORE(FP_CTRL, (REG32_LOAD(FP_CTRL) & ~0x1) | 0x2);
	REG32_STORE(FP_LAR, 0);
}
