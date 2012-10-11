/**
 * @file
 *
 * @date 19.11.10
 * @author Nikolay Korotky
 */

#ifndef X86_REGS_H_
#define X86_REGS_H_

#include <types.h>
#include <asm/flags.h>

/* Read Time Stamp Counter Register */
static inline unsigned long long rdtsc(void) {
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return ((unsigned long long) lo) | (((unsigned long long) hi) << 32);
}

#endif /* X86_REGS_H_ */
