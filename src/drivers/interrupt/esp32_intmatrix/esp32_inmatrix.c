#include <errno.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/esp32_intmatrix.h>
#include <framework/mod/options.h>
#include <hal/ipl.h>
#include <hal/reg.h>

#define MTX_BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define MTX_SOURCE_COUNT 84U
#define MTX_CLIC_FIRST   16U
#define MTX_CLIC_LAST    47U
#define MTX_MAP_MASK     0x3fU

int esp32_intmatrix_route(uint32_t source, uint32_t clic_int) {
	uintptr_t reg;
	uint32_t value;
	ipl_t ipl;

	if (source >= MTX_SOURCE_COUNT
	    || clic_int < MTX_CLIC_FIRST || clic_int > MTX_CLIC_LAST) {
		return -EINVAL;
	}

	reg = MTX_BASE_ADDR + sizeof(uint32_t) * source;
	ipl = ipl_save();
	/* The matrix takes the full CLIC ID, including the external IRQ offset. */
	value = REG32_LOAD(reg);
	value = (value & ~MTX_MAP_MASK) | clic_int;
	REG32_STORE(reg, value);
	ipl_restore(ipl);

	return 0;
}

PERIPH_MEMORY_DEFINE(esp32_intmatrix, MTX_BASE_ADDR, 0x800);
