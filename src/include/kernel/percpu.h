/**
 * @file
 * @brief
 *
 * @date 26.12.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef KERNEL_PERCPU_H_
#define KERNEL_PERCPU_H_

#include <kernel/cpu.h>
#include <module/embox/kernel/smp/cpu_api.h>

/**
 * Get data from current CPU
 */
#define percpu_ptr(name) \
	__percpu_ptr(name)

#define percpu_var(name) \
	(*__percpu_ptr(&(name)))

#define __percpu_ptr(name) \
	__percpu_cpu_ptr(cpu_get_id(), name)

/**
 * Get data from all CPUs
 */
#define percpu_cpu_ptr(cpu_id, name) \
	__percpu_cpu_ptr(cpu_id, name)

#define percpu_cpu_var(cpu_id, name) \
	(*__percpu_cpu_ptr(cpu_id, &(name)))

#endif /* !KERNEL_PERCPU_H_ */
