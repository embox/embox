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

#define PERCPU_DEFINE(type, name) \
		type PERCPU_##name[NCPU]

#define PERCPU_DECLARE(type, name) \
		extern type PERCPU_##name[]

#define __PERCPU_VAR(name) \
		PERCPU_##name[cpu_get_id()]

#else

#define PERCPU_DEFINE(type, name) \
		type PERCPU_##name

#define PERCPU_DECLARE(type, name) \
		extern type PERCPU_##name

#define __PERCPU_VAR(name) \
		PERCPU_##name

#endif /* SMP */

#define percpu_get(name) \
		(__PERCPU_VAR(name))

#define percpu_set(name, val) \
		do { __PERCPU_VAR(name) = val } while (0)

#endif /* KERNEL_PERCPU_H_ */
