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

#include <asm/cp14.h>
#include <asm/debug.h>
#include <debug/breakpoint.h>
#include <util/bitmap.h>

#define MAX_HW_BPTS 16

static BITMAP_DECL(hw_bpt_usage_table, MAX_HW_BPTS);

static void store_debug_regs(int num, uint32_t dbgbvr, uint32_t dbgbcr) {
	switch (num) {
	case 0:
		CP14_STORE(DBGBVR0, dbgbvr);
		CP14_STORE(DBGBCR0, dbgbcr);
		break;
	case 1:
		CP14_STORE(DBGBVR1, dbgbvr);
		CP14_STORE(DBGBCR1, dbgbcr);
		break;
	case 2:
		CP14_STORE(DBGBVR2, dbgbvr);
		CP14_STORE(DBGBCR2, dbgbcr);
		break;
	case 3:
		CP14_STORE(DBGBVR3, dbgbvr);
		CP14_STORE(DBGBCR3, dbgbcr);
		break;
	case 4:
		CP14_STORE(DBGBVR4, dbgbvr);
		CP14_STORE(DBGBCR4, dbgbcr);
		break;
	case 5:
		CP14_STORE(DBGBVR5, dbgbvr);
		CP14_STORE(DBGBCR5, dbgbcr);
		break;
	case 6:
		CP14_STORE(DBGBVR6, dbgbvr);
		CP14_STORE(DBGBCR6, dbgbcr);
		break;
	case 7:
		CP14_STORE(DBGBVR7, dbgbvr);
		CP14_STORE(DBGBCR7, dbgbcr);
		break;
	case 8:
		CP14_STORE(DBGBVR8, dbgbvr);
		CP14_STORE(DBGBCR8, dbgbcr);
		break;
	case 9:
		CP14_STORE(DBGBVR9, dbgbvr);
		CP14_STORE(DBGBCR9, dbgbcr);
		break;
	case 10:
		CP14_STORE(DBGBVR10, dbgbvr);
		CP14_STORE(DBGBCR10, dbgbcr);
		break;
	case 11:
		CP14_STORE(DBGBVR11, dbgbvr);
		CP14_STORE(DBGBCR11, dbgbcr);
		break;
	case 12:
		CP14_STORE(DBGBVR12, dbgbvr);
		CP14_STORE(DBGBCR12, dbgbcr);
		break;
	case 13:
		CP14_STORE(DBGBVR13, dbgbvr);
		CP14_STORE(DBGBCR13, dbgbcr);
		break;
	case 14:
		CP14_STORE(DBGBVR14, dbgbvr);
		CP14_STORE(DBGBCR14, dbgbcr);
		break;
	case 15:
		CP14_STORE(DBGBVR15, dbgbvr);
		CP14_STORE(DBGBCR15, dbgbcr);
		break;
	default:
		break;
	}
}

static void arm_hw_bpt_set(struct bpt *bpt) {
	bpt->num = bitmap_find_zero_bit(hw_bpt_usage_table, MAX_HW_BPTS, 0);
	bitmap_set_bit(hw_bpt_usage_table, bpt->num);

	store_debug_regs(bpt->num, (uint32_t)bpt->addr,
	    DBGBCR_EN | DBGBCR_PMC_ANY | DBGBCR_BAS_ANY);
}

static void arm_hw_bpt_remove(struct bpt *bpt) {
	store_debug_regs(bpt->num, 0, 0);

	bitmap_clear_bit(hw_bpt_usage_table, bpt->num);
}

static void arm_hw_bpt_enable(void) {
	uint32_t dbgdscr;

	dbgdscr = CP14_LOAD(DBGDSCRext);
	dbgdscr &= ~DBGDSCR_EXTDCCMODE_MASK;
	dbgdscr |= DBGDSCR_MDBGEN | DBGDSCR_ITREN | DBGDSCR_EXTDCCMODE_STALL;
	CP14_STORE(DBGDSCRext, dbgdscr);
}

static void arm_hw_bpt_disable(void) {
	CP14_CLEAR(DBGDSCRext, DBGDSCR_MDBGEN | DBGDSCR_ITREN);
}

static const struct bpt_info *arm_hw_bpt_info(void) {
	extern char _text_vma, _text_len;

	static bool initialized = false;
	static struct bpt_info info;

	if (!initialized) {
		info.count = ((CP14_LOAD(DBGDIDR) >> 24) & 0b1111) + 1;
		info.start = (void *)&_text_vma;
		info.end = (void *)&_text_vma + (uintptr_t)&_text_len;
		initialized = true;
	}

	return &info;
}

HW_BREAKPOINT_OPS_DEF({
    .set = arm_hw_bpt_set,
    .remove = arm_hw_bpt_remove,
    .enable = arm_hw_bpt_enable,
    .disable = arm_hw_bpt_disable,
    .info = arm_hw_bpt_info,
});
