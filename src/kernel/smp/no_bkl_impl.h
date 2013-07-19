/**
 * @file
 * @brief
 *
 * @date 18.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_SMP_NO_BKL_IMLP_H_
#define KERNEL_SMP_NO_BKL_IMLP_H_

#define bkl_lock() do { } while (0)
#define bkl_unlock() do { } while (0)

#endif /* !KERNEL_SMP_NO_BKL_IMPL_H_ */
