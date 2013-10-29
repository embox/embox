/*
 *@brief RDTSC implementation
 *
 *@date 22.10.2013
 *
 *
 */

#include <stdint.h>

#include <hal/cpu_info.h>

uint64_t get_cpu_counter(void) {
	uint64_t hi = 0, lo = 0;
	asm volatile (	"rdtsc\n\t"
					"movl %%eax, %0\n\t"
					"movl %%edx, %1\n\t" :
					"=r"(lo), "=r"(hi) :);
	return (hi << 32) + lo;
}
