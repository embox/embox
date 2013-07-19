/**
 * @file
 * @brief
 *
 * @date 19.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_SMP_ONECPU_IMPL_H_
#define KERNEL_SMP_ONECPU_IMPL_H_

#define __percpu__

#define __percpu_cpu_ptr(cpu_id, name) \
	(name)

#define cpu_get_id() (0)

#endif /* KERNEL_SMP_ONECPU_IMPL_H_ */
