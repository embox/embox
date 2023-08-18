/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <asm/cp14.h>
#include <debug/gdbstub.h>

#define MAX_HW_BPTS 0x10

#define DBGBCR_EN      (1U << 0U)
#define DBGBCR_PMC_ANY (0b11U << 1U)
#define DBGBCR_BAS_ANY (0b1111U << 5U)

#define DBGDSCR_ITREN            (1U << 13U)
#define DBGDSCR_MDBGEN           (1U << 15U)
#define DBGDSCR_EXTDCCMODE_MASK  (3U << 20U)
#define DBGDSCR_EXTDCCMODE_STALL (1U << 20U)

static bool hw_bpts_enabled = false;
static bool hw_bpts_activated = false;

static void *hw_bpts[MAX_HW_BPTS];
static size_t hw_bpt_count;

static void store_bpt_regs(int num, uint32_t dbgbvr, uint32_t dbgbcr) {
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

bool arm_set_hw_bpt(void *addr) {
	int i;

	if (!hw_bpts_enabled) {
		return false;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] == NULL) {
			hw_bpts[i] = addr;
			if (hw_bpts_activated) {
				store_bpt_regs(i, (uint32_t)addr,
				    DBGBCR_EN | DBGBCR_PMC_ANY | DBGBCR_BAS_ANY);
			}
			break;
		}
	}

	return (i != hw_bpt_count);
}

bool arm_remove_hw_bpt(void *addr) {
	int i;

	if (!hw_bpts_enabled) {
		return false;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] == addr) {
			hw_bpts[i] = NULL;
			if (hw_bpts_activated) {
				store_bpt_regs(i, 0, 0);
			}
			break;
		}
	}

	return (i != hw_bpt_count);
}

void arm_remove_hw_bpts(void) {
	int i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			hw_bpts[i] = NULL;
			if (hw_bpts_activated) {
				store_bpt_regs(i, 0, 0);
			}
		}
	}
}

void arm_activate_hw_bpts(void) {
	int i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			store_bpt_regs(i, (uint32_t)hw_bpts[i],
			    DBGBCR_EN | DBGBCR_PMC_ANY | DBGBCR_BAS_ANY);
		}
	}

	hw_bpts_activated = true;
}

void arm_deactivate_hw_bpts(void) {
	int i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			store_bpt_regs(i, 0, 0);
		}
	}

	hw_bpts_activated = false;
}

void arm_enable_hw_bpts(void) {
	uint32_t dbgdscr;

	hw_bpt_count = ((CP14_LOAD(DBGDIDR) >> 24) & 0b1111) + 1;

	dbgdscr = CP14_LOAD(DBGDSCRext);
	dbgdscr &= ~DBGDSCR_EXTDCCMODE_MASK;
	dbgdscr |= DBGDSCR_MDBGEN | DBGDSCR_ITREN | DBGDSCR_EXTDCCMODE_STALL;
	CP14_STORE(DBGDSCRext, dbgdscr);

	hw_bpts_enabled = true;
	hw_bpts_activated = false;
}

void arm_disable_hw_bpts(void) {
	if (hw_bpts_enabled) {
		CP14_CLEAR(DBGDSCRext, DBGDSCR_MDBGEN | DBGDSCR_ITREN);
		hw_bpts_enabled = false;
	}
}
