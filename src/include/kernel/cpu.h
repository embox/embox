/**
 * @file
 * @brief
 *
 * @date 26.12.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_CPU_H_
#define KERNEL_CPU_H_

#define NCPU 2

#if defined(NCPU) && NCPU > 1
# define SMP
#endif

#ifdef SMP

extern unsigned int cpu_get_id(void);

#endif

#endif /* KERNEL_CPU_H_ */
