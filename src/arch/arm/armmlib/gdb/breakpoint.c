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

size_t _gdb_bpt_count;

int gdb_set_bpt(void *addr) {
	int i;

	for (i = 0; i < _gdb_bpt_count; i++) {
		if ((REG32_LOAD(FP_COMP(i)) & ~0x1) == 0) {
			REG32_STORE(FP_COMP(i), (uint32_t)addr | 0x1);
			break;
		}
	}
	return (i != _gdb_bpt_count);
}

int gdb_remove_bpt(void *addr) {
	int i;

	for (i = 0; i < _gdb_bpt_count; i++) {
		if ((REG32_LOAD(FP_COMP(i)) & ~0x1) == (uint32_t)addr) {
			REG32_STORE(FP_COMP(i), 0);
			break;
		}
	}
	return (i != _gdb_bpt_count);
}

void gdb_remove_all_bpts(void) {
	int i;

	for (i = 0; i < _gdb_bpt_count; i++) {
		REG32_STORE(FP_COMP(i), 0);
	}
}
