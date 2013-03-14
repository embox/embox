/**
 * @file
 * @brief
 *
 * @date 26.12.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_PERCPU_H_
#define KERNEL_PERCPU_H_

#include <kernel/cpu.h>

#ifdef SMP

#include <stddef.h>

extern char _percpu_vma, _percpu_len;

#define __percpu__     __attribute__((section(".data.percpu")))
#define __PERCPU_START ((char *) &_percpu_vma)
#define __PERCPU_LEN   ((size_t) &_percpu_len)

#define __PERCPU_CPU_VAR_PTR(cpu_id, name) \
		((typeof(name)) (((char *) (name)) + (cpu_id * __PERCPU_LEN)))


#else

#define __percpu__

#define __PERCPU_CPU_VAR_PTR(cpu_id, name) \
		(name)

#endif /* SMP */

#define __PERCPU_VAR_PTR(name) \
		__PERCPU_CPU_VAR_PTR(cpu_get_id(), name)

/* Get data from current CPU */

#define percpu_ptr(name) \
		__PERCPU_VAR_PTR(name)

#define percpu_var(name) \
		(*percpu_ptr(&(name)))

/* Get data from all CPUs */

#define percpu_cpu_ptr(cpu_id, name) \
		__PERCPU_CPU_VAR_PTR(cpu_id, name)

#define percpu_cpu_var(cpu_id, name) \
		(*percpu_cpu_ptr(cpu_id, &(name)))

#endif /* KERNEL_PERCPU_H_ */
