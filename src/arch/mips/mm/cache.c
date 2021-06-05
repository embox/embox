/**
 * @file
 *
 * @date Mar 11, 2020
 * @Author Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>

extern void flush_dcache_range(unsigned long start_addr, unsigned long stop);
extern void invalidate_dcache_range(unsigned long start_addr, unsigned long stop);
void dcache_inval(const void *p, size_t size) {
	invalidate_dcache_range((uintptr_t)(p), (uintptr_t)(p + size));
}

void dcache_flush(const void *p, size_t size) {
	flush_dcache_range((uintptr_t)(p), (uintptr_t)(p + size));
}
