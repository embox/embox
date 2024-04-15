/**
 * @file
 * @brief
 *
 * @date 02.12.23
 * @author Aleksey Zhmulin
 */
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <hal/cache.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <util/field.h>

static uint32_t dcache_get_line_size(void) {
	uint32_t ctr_el0;

	ctr_el0 = ARCH_REG_LOAD(CTR_EL0);

	return (4 << FIELD_GET(ctr_el0, CTR_EL0_DLINE));
}

void dcache_inval(const void *data, size_t size) {
	uint64_t start, end;
	uint32_t line_size;

	start = (uint64_t)(uintptr_t)data;
	end = (uint64_t)(uintptr_t)data + size;

	line_size = dcache_get_line_size();
	start &= ~(line_size - 1);

	for (; start < end; start += line_size) {
		ARCH_REG_STORE(DC_IVAC, start);
		dsb(sy);
	}
}

void dcache_flush(const void *data, size_t size) {
	uint64_t start, end;
	uint32_t line_size;

	start = (uint64_t)(uintptr_t)data;
	end = (uint64_t)(uintptr_t)data + size;

	line_size = dcache_get_line_size();
	start &= ~(line_size - 1);

	for (; start < end; start += line_size) {
		ARCH_REG_STORE(DC_CIVAC, start);
		dsb(sy);
	}
}
