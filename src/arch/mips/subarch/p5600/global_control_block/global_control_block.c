/**
 * @file
 *
 * @date Mar 20, 2020
 * @uthor Anton Bondarev
 */
#include <stdint.h>

#include <hal/reg.h>

#include <framework/mod/options.h>

#include "global_control_block.h"

#define GCB_BASE     OPTION_GET(NUMBER,base_addr)

uint32_t mips32_gcb_set_register(uint32_t offset, uint32_t val) {
	REG32_STORE(GCB_BASE + offset, val);
	return REG32_LOAD(GCB_BASE + offset);
}

uint32_t mips32_gcb_get_register(uint32_t offset) {
	return REG32_LOAD(GCB_BASE + offset);
}
