/**
 * @file
 * @brief
 *
 * @date 18.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_SMP_NO_BKL_IMLP_H_
#define KERNEL_SMP_NO_BKL_IMLP_H_

static inline void bkl_lock(void) { }
static inline void bkl_unlock(void) { }

#endif /* !KERNEL_SMP_NO_BKL_IMPL_H_ */
