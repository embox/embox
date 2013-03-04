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

#define __PERCPU_VAR_PTR(name) \
		((typeof(name)) (((char *) (name)) + (cpu_get_id() * __PERCPU_LEN)))

#else

#define __percpu__

#define __PERCPU_VAR_PTR(name) \
		(name)

#endif /* SMP */

#define percpu_ptr(name) \
		__PERCPU_VAR_PTR(name)

#define percpu_var(name) \
		(*percpu_ptr(&(name)))

#endif /* KERNEL_PERCPU_H_ */
