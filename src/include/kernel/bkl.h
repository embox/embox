/**
 * @file
 * @brief
 *
 * @date 13.02.2012
 * @author Anton Bulychev
 */

#ifndef KERNEL_SMP_BKL_H_
#define KERNEL_SMP_BKL_H_

#include <module/embox/kernel/smp/bkl_api.h>

extern void bkl_lock(void);
extern void bkl_unlock(void);

#endif /* !KERNEL_SMP_BKL_H_ */
