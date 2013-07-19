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

#include <module/embox/arch/smp.h>

#define NCPU __NCPU

#ifndef __ASSEMBLER__

#include <module/embox/kernel/smp/cpu_api.h>
#include <sys/types.h>

struct thread;

#ifndef cpu_get_id
extern unsigned int cpu_get_id(void);
#endif /* !cpu_get_id */

extern void cpu_set_idle_thread(struct thread *idle);
extern struct thread * cpu_get_idle_thread(void);
extern clock_t cpu_get_total_time(unsigned int cpu_id);
extern clock_t cpu_get_idle_time(unsigned int cpu_id);

#endif /* !__ASSEMBLER__ */

#endif /* !KERNEL_CPU_H_ */
