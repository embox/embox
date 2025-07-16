/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.03.2013
 */

#include <kernel/host.h>

#ifndef USERMODE86_KERNEL_CPU_IDLE_H_
#define USERMODE86_KERNEL_CPU_IDLE_H_

static inline void arch_cpu_idle(void) {
	host_pause();
}

#endif /* USERMODE86_KERNEL_CPU_IDLE_H_ */
