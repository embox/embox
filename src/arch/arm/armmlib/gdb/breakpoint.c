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

#include <hal/reg.h>
#include <asm/arm_m_regs.h>

static size_t max_breakpoints;

int gdb_set_bpt(void *addr) {
	uint32_t fp_ctrl;
	int i;

	if (max_breakpoints == 0) {
		fp_ctrl = REG32_LOAD(FP_CTRL);
		max_breakpoints = (((fp_ctrl >> 12) & 0x7) << 4) | ((fp_ctrl >> 4) & 0xf);
	}

	for (i = 0; i < max_breakpoints; i++) {
		if ((REG32_LOAD(FP_COMP(i)) & ~0x1) == 0) {
			REG32_STORE(FP_COMP(i), (uint32_t)addr | 0x1);
			break;
		}
	}
	return (i != max_breakpoints);
}

int gdb_remove_bpt(void *addr) {
	int i;

	for (i = 0; i < max_breakpoints; i++) {
		if ((REG32_LOAD(FP_COMP(i)) & ~0x1) == (uint32_t)addr) {
			REG32_STORE(FP_COMP(i), 0);
			break;
		}
	}
	return (i != max_breakpoints);
}

void gdb_remove_all_bpts(void) {
	int i;

	for (i = 0; i < max_breakpoints; i++) {
		REG32_STORE(FP_COMP(i), 0);
	}
}
