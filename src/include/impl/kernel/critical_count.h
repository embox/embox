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

/* I realize what I am doing. Force this variable to be placed in common
 * storage despite of -fno-common option. */
unsigned int __attribute__((common)) __critical_count;

inline static unsigned int __critical_count_get(void) {
	return __critical_count;
}

inline static void __critical_count_add_nobarrier(unsigned int count) {
	__critical_count += count;
}

inline static void __critical_count_sub_nobarrier(unsigned int count) {
	__critical_count -= count;
}

inline static void __critical_count_add(unsigned int count) {
	__critical_count_add_nobarrier(count);
	__barrier();
}

inline static void __critical_count_sub(unsigned int count) {
	__barrier();
	__critical_count_sub_nobarrier(count);
}

