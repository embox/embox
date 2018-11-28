/**
 * @file
 *
 * @date Nov 28, 2018
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>

#include <drivers/iomuxc.h>

#include <framework/mod/options.h>

#define IOMUXC_BASE            OPTION_GET(NUMBER, base_addr)

void iomuxc_write(uint32_t reg, uint32_t val) {
	REG32_STORE(IOMUXC_BASE + reg, val);
}

uint32_t iomuxc_read(uint32_t reg) {
	return REG32_LOAD(IOMUXC_BASE + reg);
}

static struct periph_memory_desc iomuxc_mem = {
	.start = IOMUXC_BASE,
	.len   = 0x1000,
};

PERIPH_MEMORY_DEFINE(iomuxc_mem);
