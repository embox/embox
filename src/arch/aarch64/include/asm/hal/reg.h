/**
 * @file reg.h
 * @brief Stub header
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */

#ifndef AARCH_HAL_REG_
#define AARCH_HAL_REG_

#include <stdint.h>

static inline uint64_t get_daif(void) {
	volatile uint64_t reg;
	__asm__ __volatile__ ("mrs %0, daif;\n\t"
		: "=r"(reg)
	);
	return reg;
}

static inline void set_daif(uint64_t reg) {
	__asm__ __volatile__ ("msr daif, %0; \n\t"
		:
		: "r"(reg)
	);
}

#endif /* AARCH_HAL_REG_ */
