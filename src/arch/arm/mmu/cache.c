/**
 * @file
 * @brief
 *
 * @date 26.11.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include <hal/cache.h>
#include <util/log.h>

static uint32_t get_cache_line_size(void) {
#if __ARM_ARCH == 6 || __ARM_ARCH == 5
	/*
	 * I am not sure about if it is arch defined cache line size,
	 * but Linux uses 32 bytes cache line size for both I and D caches
	 * for any ARMV6: arch/arm/mm/cache-v6.S
	 *
	 * For ARMv5 I am not sure at all, just add this to fix arm/qemu.
	 *
	 * P.S. It is added because of ARM1176JZF-S (Ra Pi model B rev 2.0)
	 */
	return 32;
#elif __ARM_ARCH >= 7 || __ARM_ARCH == 4
	uint32_t csir;
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (csir));
	return (1 << ((csir & 7) + 2)) * 4;
#else
	#error Unsupported ARM arch
#endif
}

void dcache_inval(const void *data, size_t size) {
	uint32_t start, end, line_size;

	start = (uint32_t)(uintptr_t)data;
	end = (uint32_t)(uintptr_t)data + size;

	line_size = get_cache_line_size();
	if (start & ~(line_size - 1)) {
		log_error("Unaligned start = 0x%08"PRIu32"", start);
	}
	start &= ~(line_size - 1);
	while (start < end) {
		asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (start));
		start += line_size;
	}
}

void dcache_flush(const void *data, size_t size) {
	uint32_t start, end, line_size;

	start = (uint32_t)(uintptr_t)data;
	end = (uint32_t)(uintptr_t)data + size;

	line_size = get_cache_line_size();
	if (start & ~(line_size - 1)) {
		log_warning("Unaligned start = 0x%08"PRIu32"", start);
	}
	start &= ~(line_size - 1);
	while (start < end) {
		asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (start));
		start += line_size;
	}
}
