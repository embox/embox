/**
 * @file
 *
 * @date 19.11.10
 * @author Nikolay Korotky
 */

#ifndef X86_REGS_H_
#define X86_REGS_H_

#include <asm/flags.h>

static inline void *cpu_get_stack(void) {
	void * ret;
	__asm__ __volatile__ (
		"mov %%esp, %0\n\t"
		: "=r"(ret)
	);
	return ret;
}

#endif /* X86_REGS_H_ */
