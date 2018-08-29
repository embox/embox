/**
 * @file
 *
 * @date 29.08.18
 * @author Alexander Kalmuk
 */

#ifndef HAL_IPL_H_
# error "Do not include this file directly!"
#endif /* HAL_IPL_H_ */

#include <stdint.h>

#include <cortexm/arch.h>

#define IRQ_MIN_NONFAULT_PRIO (1 << IRQ_PRIO_SHIFT)

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
	uint32_t ipl = 0;
	__asm__ __volatile__ (
		"cpsie i \n\t");

	__asm__ __volatile__ (
		"msr BASEPRI, %0;\n\t"
		:
		: "r"(ipl));
}

static inline __ipl_t ipl_save(void) {
	uint32_t r;
	uint32_t prio = IRQ_MIN_NONFAULT_PRIO;
	__asm__ __volatile__ (
		"mrs %0, BASEPRI;\n\t"
		: "=r"(r));
	__asm__ __volatile__ (
		"msr BASEPRI, %0;\n\t"
		:
		: "r"(prio));
	return r;
}

static inline void ipl_restore(__ipl_t ipl) {
	__asm__ __volatile__ (
		"msr BASEPRI, %0;\n\t"
		:
		: "r"(ipl));
}
