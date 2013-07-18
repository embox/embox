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

extern char _percpu_start, _percpu_block_end, _percpu_end;

#define __percpu__         __attribute__((section(".data.percpu")))
#define __PERCPU_START     ((char *)&_percpu_start)
#define __PERCPU_BLOCK_END ((char *)&_percpu_block_end)
#define __PERCPU_BLOCK_SZ  ((size_t)(__PERCPU_BLOCK_END - __PERCPU_START))
#define __PERCPU_END       ((char *)&_percpu_end)

#define __PERCPU_CPU_VAR_PTR(cpu_id, name) \
	((typeof(name)) (((char *) (name)) + (cpu_id * __PERCPU_BLOCK_SZ)))

//		assert((__PERCPU_START <= (char *)(name))
//				&& ((char *)(name) < __PERCPU_BLOCK_END));
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
