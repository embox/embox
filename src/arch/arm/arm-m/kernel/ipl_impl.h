/**
 * @file
 *
 * @date 01.07.10
 * @author Anton Kozlov
 */

#ifndef ARCH_ARM_ARMMLIB_IPL_IMPL_H_
#define ARCH_ARM_ARMMLIB_IPL_IMPL_H_

#ifndef HAL_IPL_H_
#error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <stdint.h>

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
	__asm__ __volatile__("cpsie i \n\t");
}

static inline __ipl_t ipl_save(void) {
	register uint32_t r;
	__asm__ __volatile__("mrs %0, PRIMASK;\n\t"
	                     "cpsid i \n\t"
	                     : "=r"(r));
	return r;
}

static inline void ipl_restore(__ipl_t ipl) {
	__asm__ __volatile__("msr PRIMASK, %0;\n\t" : : "r"(ipl));
}

#endif /* ARCH_ARM_ARMMLIB_IPL_IMPL_H_ */
