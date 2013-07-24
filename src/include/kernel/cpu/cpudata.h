/**
 * @file
 * @brief
 *
 * @date 24.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CPU_CPUDATA_H_
#define KERNEL_CPU_CPUDATA_H_

#include <module/embox/kernel/cpu/cpudata_api.h>
#include <hal/cpu.h>

/**
 * Get data from current CPU
 */
#define cpudata_ptr(name) \
	__cpudata_ptr(name)

#define cpudata_var(name) \
	(*__cpudata_ptr(&(name)))

#define __cpudata_ptr(name) \
	__cpudata_cpu_ptr(cpu_get_id(), name)

/**
 * Get data from all CPUs
 */
#define cpudata_cpu_ptr(cpu_id, name) \
	__cpudata_cpu_ptr(cpu_id, name)

#define cpudata_cpu_var(cpu_id, name) \
	(*__cpudata_cpu_ptr(cpu_id, &(name)))

#endif /* !KERNEL_CPU_CPUDATA_H_ */
