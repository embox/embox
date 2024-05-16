/**
 * @file
 * @brief
 *
 * @date 14.05.24
 * @author Aleksey Zhmulin
 */

#ifndef RISCV_KERNEL_CPU_IDLE_H_
#define RISCV_KERNEL_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	__asm__ __volatile__("WFI");
}

#endif /* RISCV_KERNEL_CPU_IDLE_H_ */
