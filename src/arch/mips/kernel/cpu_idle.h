/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 03.05.24
 */
#ifndef MIPS_CPU_IDLE_H_
#define MIPS_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	__asm__ __volatile__("wait");
}

#endif /* MIPS_CPU_IDLE_H_ */
