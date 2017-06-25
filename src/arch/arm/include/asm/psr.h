/**
 * @file
 *
 * @brief Utilities for work with ARM processor register (cpsr)
 *
 * @date 21.07.10
 * @author Anton Kozlov
 */

#ifndef ARM_PSR_H_
#define ARM_PSR_H_

#ifndef __ASSEMBLER__

static inline unsigned int __get_cpsr(void) {
	unsigned long retval;
	__asm__ __volatile__(
		"mrs  %0, cpsr\n"
		: "=r" (retval)
		:

	);
	return retval;
}

static inline void __set_cpsr(unsigned val) {
	__asm__ __volatile__(
		"msr  cpsr, %0\n"
		: /* no outputs */
		: "r" (val)
	);
}

#endif

#endif /* ARM_PSR_H_ */
