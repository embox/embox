/**
 * @file
 * @brief
 *
 * @date 16.06.23
 * @author Aleksey Zhmulin
 */

#ifndef ARM_ARMV5TE_CPU_IDLE_H_
#define ARM_ARMV5TE_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	__asm__ __volatile__("mcr p15, 0, %0, c7, c0, 4" : : "r"(0));
}

#endif /* ARM_ARMV5TE_CPU_IDLE_H_ */
