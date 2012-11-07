/**
 * @file
 * @brief Utilities for work with PowerPC Microprocessor Family registers
 *
 * @date 07.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_PSR_H_
#define PPC_PSR_H_

#ifndef __ASSEMBLER__

/**
 * MSR register
 */
static inline unsigned int __get_msr(void) {
	unsigned int retval;
	__asm__ __volatile__ (
		"mfmsr %0"
		: "=r" (retval)
		: /* no input */
		: "memory"
	);
	return retval;
}

static inline void __set_msr(unsigned int val) {
	__asm__ __volatile__ (
		"mtmsr %0"
		: /* no output */
		: "r" (val)
		: "memory"
	);
}

#endif /* __ASSEMBLER__ */

#endif /* PPC_PSR_H_ */
