/**
 * @file
 * @brief
 *
 * @date 26.12.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_CPU_H_
#define KERNEL_CPU_H_

#include <module/embox/arch/smp.h>

#define NCPU __NCPU

#if NCPU > 1
# define SMP
#endif

#ifdef SMP

#ifndef __ASSEMBLER__

extern unsigned int cpu_get_id(void);

#endif /* __ASSEMBLER__ */

#else /* SMP */

#define cpu_get_id() (0)

#endif /* SMP */

#endif /* KERNEL_CPU_H_ */
