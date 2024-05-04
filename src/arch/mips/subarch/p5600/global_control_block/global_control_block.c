/**
 * @file
 *
 * @date Mar 20, 2020
 * @author Anton Bondarev
 */
#include <stdint.h>

#include <framework/mod/options.h>
#include <hal/reg.h>

#include "global_control_block.h"

#define GCB_BASE OPTION_GET(NUMBER, base_addr)

uint32_t mips32_gcb_set_register(uint32_t offset, uint32_t val) {
	REG32_STORE(GCB_BASE + offset, val);
	return REG32_LOAD(GCB_BASE + offset);
}

uint32_t mips32_gcb_get_register(uint32_t offset) {
	return REG32_LOAD(GCB_BASE + offset);
}

uint32_t mips_cm_l2_line_size(void) {
	unsigned long l2conf, line_sz;

	l2conf = REG32_LOAD(GCB_BASE + GCR_L2_CONFIG);

	line_sz = l2conf >> GCR_L2_CONFIG_LINESZ_SHIFT;
	line_sz &= ((1 << GCR_L2_CONFIG_LINESZ_BITS) - 1);
	return line_sz ? (2 << line_sz) : 0;
}
