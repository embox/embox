/**
 * @file
 *
 * @date Jul 5, 2013
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <kernel/sched.h>

static inline unsigned int cpu_get_stack(void) {
	unsigned ret;
	__asm__ __volatile__ (
		"mov %%esp, %0\n\t"
		: "=r"(ret)
	);
	return ret;

}


int irq_stack_protection(void) {
	struct thread *th;
	unsigned int threshold;

	th = sched_current();
	threshold = (uint32_t)th->stack - th->stack_sz + 0x400;

	if(threshold < cpu_get_stack()) {
		return 0;
	}
	return 1;
}
