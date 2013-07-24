/**
 * @file
 * @brief
 *
 * @date 19.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CPU_NO_CPUDATA_IMPL_H_
#define KERNEL_CPU_NO_CPUDATA_IMPL_H_

#define __cpudata__

#define __cpudata_cpu_ptr(cpu_id, name) \
	(name)

#endif /* !KERNEL_CPU_NO_CPUDATA_IMPL_H_ */
