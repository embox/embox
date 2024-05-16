/**
 * @file
 * @brief
 *
 * @date 25.11.22
 * @author Aleksey Zhmulin
 */

#ifndef ARM_ARMV7A_CPU_IDLE_H_
#define ARM_ARMV7A_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	__asm__ __volatile__("WFI");
}

#endif /* ARM_ARMV7A_CPU_IDLE_H_ */
