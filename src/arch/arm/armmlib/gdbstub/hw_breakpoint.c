/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <hal/reg.h>
#include <debug/gdbstub.h>
#include <asm/arm_m_regs.h>

#define MAX_HW_BPTS 0x10

static bool hw_bpts_enabled = false;
static bool hw_bpts_activated = false;

static void *hw_bpts[MAX_HW_BPTS];
static size_t hw_bpt_count;

bool arm_m_set_hw_bpt(void *addr) {
	int i;

	if (!hw_bpts_enabled) {
		return false;
	}

	addr = (void *)((uint32_t)addr & ~0x1);

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] == NULL) {
			hw_bpts[i] = addr;
			if (hw_bpts_activated) {
				REG32_STORE(FP_COMP(i), (uint32_t)addr | 0x1);
			}
			break;
		}
	}
	return (i != hw_bpt_count);
}

bool arm_m_remove_hw_bpt(void *addr) {
	int i;

	if (!hw_bpts_enabled) {
		return false;
	}

	addr = (void *)((uint32_t)addr & ~0x1);

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] == addr) {
			hw_bpts[i] = NULL;
			if (hw_bpts_activated) {
				REG32_STORE(FP_COMP(i), 0);
			}
			break;
		}
	}
	return (i != hw_bpt_count);
}

void arm_m_remove_hw_bpts(void) {
	int i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			hw_bpts[i] = NULL;
			if (hw_bpts_activated) {
				REG32_STORE(FP_COMP(i), 0);
			}
		}
	}
}

void arm_m_activate_hw_bpts(void) {
	int i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			REG32_STORE(FP_COMP(i), (uint32_t)hw_bpts[i] | 0x1);
		}
	}

	hw_bpts_activated = true;
}

void arm_m_deactivate_hw_bpts(void) {
	int i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			REG32_STORE(FP_COMP(i), 0);
		}
	}

	hw_bpts_activated = false;
}

void arm_m_enable_hw_bpts(void) {
	uint32_t fp_ctrl;

	fp_ctrl = REG32_LOAD(FP_CTRL);
	hw_bpt_count = (((fp_ctrl >> 12) & 0x7) << 4) | ((fp_ctrl >> 4) & 0xf);

	assert(hw_bpt_count <= MAX_HW_BPTS);

	REG32_STORE(FP_LAR, FP_LAR_UNLOCK_KEY);
	REG32_STORE(FP_CTRL, fp_ctrl | 0x3);
	REG32_ORIN(DCB_DEMCR, DCB_DEMCR_MON_EN);

	hw_bpts_enabled = true;
	hw_bpts_activated = false;
}

void arm_m_disable_hw_bpts(void) {
	if (hw_bpts_enabled) {
		REG32_STORE(FP_CTRL, (REG32_LOAD(FP_CTRL) & ~0x1) | 0x2);
		REG32_STORE(FP_LAR, 0);
		REG32_CLEAR(DCB_DEMCR, DCB_DEMCR_MON_EN);

		hw_bpts_enabled = false;
	}
}
