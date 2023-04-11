/**
 * @file
 *
 * @date Apr 11, 2023
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>

#include <drivers/common/memory.h>

#include <driver/rcm_sctl.h>

#include <framework/mod/options.h>

#define BASE_ADDR   OPTION_GET(NUMBER, base_addr)


uint32_t rcm_sctl_read_reg(int reg) {
	return REG32_LOAD(BASE_ADDR + reg);
}

void rcm_sctl_write_reg(int reg, uint32_t val) {
	REG32_STORE(BASE_ADDR + reg, val);
}

PERIPH_MEMORY_DEFINE(rcm_sctl, BASE_ADDR, 0x1000);
