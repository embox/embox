/**
 * @file
 * @brief
 *
 * @date 26.12.12
 * @author Anton Bulychev
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CPU_H_
#define KERNEL_CPU_H_

#include <framework/mod/options.h>
#include <module/embox/kernel/cpu/cpu_api.h>

/* FIXME hack to get abstract module variable */
#if OPTION_MODULE_DEFINED(embox__kernel__cpu__onecpu, NUMBER, cpu_count)
#define NCPU OPTION_MODULE_GET(embox__kernel__cpu__onecpu, NUMBER, cpu_count)
#elif OPTION_MODULE_DEFINED(embox__kernel__cpu__ncpu, NUMBER, cpu_count)
#define NCPU OPTION_MODULE_GET(embox__kernel__cpu__ncpu, NUMBER, cpu_count)
#endif

#ifndef __ASSEMBLER__

#include <sys/types.h>

struct thread;

/**
 * Get cpu id
 */
extern unsigned int cpu_get_id(void);

/**
 * Get/set cpu specific data (libthread)
 */
extern void cpu_set_idle_thread(struct thread *idle);
extern struct thread * cpu_get_idle_thread(void);
extern clock_t cpu_get_total_time(unsigned int cpu_id);
extern clock_t cpu_get_idle_time(unsigned int cpu_id);

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

#endif /* !__ASSEMBLER__ */

#endif /* !KERNEL_CPU_H_ */
