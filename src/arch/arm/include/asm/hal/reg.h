/**
 * @file
 * @brief ARM register operation
 *
 * @date 5.11.10
 * @author Anton Bondarev
 */
#ifndef ARM_REG_H_
#define ARM_REG_H_

#include <types.h>

// FIXME AT91_REG - platform-specific definition in common header. -- Eldar
// XXX too many TODO's ^_^ -- Eldar
/** Hardware register definition for AT91. */
typedef volatile unsigned int at91_reg_t;

static inline uint32_t get_cpsr(void) {
	volatile uint32_t reg;
	__asm__ __volatile__ ("mrs %0, CPSR;\n\t"
		: "=r"(reg)
	);
	return reg;
}

static inline void set_cpsr(uint32_t reg) {
	__asm__ __volatile__ ("msr CPSR, %0; \n\t"
		:
		: "r"(reg)
	);
}


#endif /* ARM_REG_H_ */
