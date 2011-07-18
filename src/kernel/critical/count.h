/**
 * @file
 * @brief Critical count operations.
 *
 * @date 30.05.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_COUNT_H_
#define KERNEL_CRITICAL_COUNT_H_

/** Optimization barrier.
 * TODO move somewhere */
#define __barrier() \
	__asm__ __volatile__("" : : : "memory")

typedef long __critical_t;

static inline __critical_t __critical_count_get(void) {
	extern __critical_t __critical_count;
	return __critical_count;
}

static inline void __critical_count_set_bit(__critical_t mask) {
	extern __critical_t __critical_count;
	__critical_count |= mask;
}

static inline void __critical_count_clr_bit(__critical_t mask) {
	extern __critical_t __critical_count;
	__critical_count &= ~mask;
}

static inline void __critical_count_add_nobarrier(__critical_t count) {
	extern __critical_t __critical_count;
	__critical_count += count;
}

static inline void __critical_count_sub_nobarrier(__critical_t count) {
	extern __critical_t __critical_count;
	__critical_count -= count;
}

static inline void __critical_count_add(__critical_t count) {
	__critical_count_add_nobarrier(count);
	__barrier();
}

static inline void __critical_count_sub(__critical_t count) {
	__barrier();
	__critical_count_sub_nobarrier(count);
}

#endif /* KERNEL_CRITICAL_API_IMPL_H_ */
