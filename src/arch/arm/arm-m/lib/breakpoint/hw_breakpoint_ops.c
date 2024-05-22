/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.08.23
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/arm_m_regs.h>
#include <debug/breakpoint.h>
#include <hal/reg.h>
#include <lib/libds/bitmap.h>

#define MAX_HW_BPTS 16

static BITMAP_DECL(hw_bpt_usage_table, MAX_HW_BPTS);

static void arm_m_hw_bpt_set(struct bpt *bpt) {
	bpt->addr = (void *)((uint32_t)bpt->addr & ~0x1);
	bpt->num = bitmap_find_zero_bit(hw_bpt_usage_table, MAX_HW_BPTS, 0);
	bitmap_set_bit(hw_bpt_usage_table, bpt->num);

	REG32_STORE(FP_COMP(bpt->num), (uint32_t)bpt->addr | 0x1);
}

static void arm_m_hw_bpt_remove(struct bpt *bpt) {
	REG32_STORE(FP_COMP(bpt->num), 0);

	bitmap_clear_bit(hw_bpt_usage_table, bpt->num);
}

static void arm_m_hw_bpt_enable(void) {
	uint32_t fp_ctrl;

	fp_ctrl = REG32_LOAD(FP_CTRL);
	REG32_STORE(FP_LAR, FP_LAR_UNLOCK_KEY);
	REG32_STORE(FP_CTRL, fp_ctrl | 0x3);
	REG32_ORIN(DCB_DEMCR, DCB_DEMCR_MON_EN);
}

static void arm_m_hw_bpt_disable(void) {
	REG32_STORE(FP_CTRL, (REG32_LOAD(FP_CTRL) & ~0x1) | 0x2);
	REG32_STORE(FP_LAR, 0);
	REG32_CLEAR(DCB_DEMCR, DCB_DEMCR_MON_EN);
}

static const struct bpt_info *arm_m_hw_bpt_info(void) {
	extern char _text_vma, _text_len;

	static bool initialized = false;
	static struct bpt_info info;

	uint32_t fp_ctrl;

	if (!initialized) {
		fp_ctrl = REG32_LOAD(FP_CTRL);
		info.count = (((fp_ctrl >> 12) & 0x7) << 4) | ((fp_ctrl >> 4) & 0xf);
		info.start = (void *)&_text_vma;
		info.end = (void *)&_text_vma + (uintptr_t)&_text_len;
		initialized = true;
	}

	return &info;
}

HW_BREAKPOINT_OPS_DEF({
    .set = arm_m_hw_bpt_set,
    .remove = arm_m_hw_bpt_remove,
    .enable = arm_m_hw_bpt_enable,
    .disable = arm_m_hw_bpt_disable,
    .info = arm_m_hw_bpt_info,
});
