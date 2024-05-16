/**
 * @file
 * @brief
 *
 * @date 14.05.24
 * @author Aleksey Zhmulin
 */

#ifndef X86_KERNEL_CPU_IDLE_H_
#define X86_KERNEL_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	__asm__ __volatile__("hlt");
}

#endif /* X86_KERNEL_CPU_IDLE_H_ */
