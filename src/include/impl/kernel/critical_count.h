/**
 * @file
 * @brief TODO
 *
 * @date 30.05.2010
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_CRITICAL_H_
# error "Do not include this file directly, use <kernel/critical/*.h> instead!"
#endif /* KERNEL_CRITICAL_H_ */

/** Optimization barrier.
 * TODO move somewhere */
#define __barrier() __asm__ __volatile__("" : : : "memory")

typedef long __critical_t;

/* I clearly realize what I do. Force this variable to be placed in common
 * storage despite of -fno-common option. */
__critical_t __attribute__((common)) __critical_count;

inline static __critical_t __critical_count_get(void) {
	return __critical_count;
}

inline static void __critical_count_add_nobarrier(__critical_t count) {
	__critical_count += count;
}

inline static void __critical_count_sub_nobarrier(__critical_t count) {
	__critical_count -= count;
}

inline static void __critical_count_add(__critical_t count) {
	__critical_count_add_nobarrier(count);
	__barrier();
}

inline static void __critical_count_sub(__critical_t count) {
	__barrier();
	__critical_count_sub_nobarrier(count);
}

