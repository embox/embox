/**
 * @file
 *
 * @date 29.08.18
 * @author Alexander Kalmuk
 */

#ifndef HAL_IPL_IMPL_H_
#define HAL_IPL_IMPL_H_

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
	uint32_t prio = NVIC_MAX_PRIO;
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

#endif /* HAL_IPL_IMPL_H_ */
