/**
 * @file
 * @brief
 *
 * @date 13.02.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SMP_BKL_H_
#define KERNEL_SMP_BKL_H_

#include <kernel/cpu.h>

#ifndef SMP

#define bkl_lock() do { } while (0)
#define bkl_unlock() do { } while (0)

#else /* !SMP */

extern void bkl_lock(void);
extern void bkl_unlock(void);

#endif /* !SMP */

#endif /* KERNEL_SMP_BKL_H_ */
