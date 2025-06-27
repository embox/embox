/**
 * @file
 * @brief
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#ifndef SPARC_KERNEL_CPU_IDLE_H_
#define SPARC_KERNEL_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	__asm__ __volatile__("wr %g0, %asr19");
}

#endif /* SPARC_KERNEL_CPU_IDLE_H_ */
