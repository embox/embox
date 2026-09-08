/**
 * @file
 * @brief
 *
 * @date 18.07.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_CPU_NO_BKL_IMLP_H_
#define KERNEL_CPU_NO_BKL_IMLP_H_

static inline void bkl_lock(void) { }
static inline void bkl_unlock(void) { }

/* critical_enter() calls these unconditionally. */
static inline int bkl_trylock(void) { return 1; }
static inline void bkl_wait(void) { }
static inline void bkl_assert_owned(unsigned int t, unsigned int u, void *f) {
	(void)t;
	(void)u;
	(void)f;
}

#endif /* !KERNEL_CPU_NO_BKL_IMPL_H_ */
