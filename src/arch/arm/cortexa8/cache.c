/**
 * @file
 * @brief
 *
 * @date 26.11.13
 * @author Ilia Vaprol
 */

#include <stddef.h>
#include <stdint.h>

void dcache_inval(const void *p, size_t size) {
	uint32_t start, end, csir, line_size;

	start = (uint32_t)(uintptr_t)p;
	end = (uint32_t)(uintptr_t)p + size;
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (csir));
	line_size = (1 << ((csir & 7) + 2)) * 4;
	start &= ~(line_size - 1);
	while (start < end) {
		asm volatile ("mcr p15, 0, %0, c7, c6, 1" : : "r" (start));
		start += line_size;
	}
}

void dcache_flush(const void *p, size_t size) {
	uint32_t start, end, csir, line_size;

	start = (uint32_t)(uintptr_t)p;
	end = (uint32_t)(uintptr_t)p + size;
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (csir));
	line_size = (1 << ((csir & 7) + 2)) * 4;
	start &= ~(line_size - 1);
	while (start < end) {
		asm volatile ("mcr p15, 0, %0, c7, c14, 1" : : "r" (start));
		start += line_size;
	}
}
