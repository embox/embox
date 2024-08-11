/*
 *@brief RDTSC implementation
 *
 *@date 22.10.2013
 *
 *
 */

#include <stdint.h>

#include <lib/libcpu_info.h>

uint64_t get_cpu_counter(void) {
	uint32_t hi = 0, lo = 0;
	asm volatile (	"rdtsc\n\t"
					"movl %%eax, %0\n\t"
					"movl %%edx, %1\n\t"
					: "=a"(lo), "=d"(hi)
					: );
	return (((uint64_t) hi) << 32) | (uint64_t)lo;
}
