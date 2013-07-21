/**
 * @file
 * @brief
 *
 * @date 19.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CPU_ONECPU_IMPL_H_
#define KERNEL_CPU_ONECPU_IMPL_H_

#define __cpudata__

#define __cpudata_cpu_ptr(cpu_id, name) \
	(name)

static inline unsigned int cpu_get_id(void) {
	return 0;
}

#endif /* !KERNEL_CPU_ONECPU_IMPL_H_ */
