/**
 * @file
 *
 * @date Mar 11, 2020
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <stdint.h>

#include <hal/cache.h>

extern void flush_dcache_range(unsigned long start_addr, unsigned long stop);
extern void invalidate_dcache_range(unsigned long start_addr,
    unsigned long stop);

void dcache_inval(const void *data, size_t size) {
	invalidate_dcache_range((uintptr_t)(data), (uintptr_t)(data + size));
}

void dcache_flush(const void *data, size_t size) {
	flush_dcache_range((uintptr_t)(data), (uintptr_t)(data + size));
}
