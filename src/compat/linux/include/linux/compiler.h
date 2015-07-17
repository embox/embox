/**
 * @file
 * @brief Describes compiler specific features
 *
 * @date 13.01.10
 * @author Anton Bondarev
 */

#ifndef COMPAT_LINUX_LINUX_COMPILER_H_
#define COMPAT_LINUX_LINUX_COMPILER_H_

/* In linux it defined as "noderef". It means that pointer cannot be
 * dereferenced. And use in sys_calls because this variable must be copied by
 * opy_from_user and copy_to_user functions
 *
 * It is defined as noderef only when using strict checker, e.g. SPARSE. -- Eldar
 */
# define __user
# define __kernel
# define __safe
# define __force
# define __nocast
# define __iomem
# define __chk_user_ptr(x) (void)0
# define __chk_io_ptr(x) (void)0
# define __builtin_warning(x, ...) (1)
# define __acquires(x)
# define __releases(x)
# define __acquire(x) (void)0
# define __release(x) (void)0
# define __cond_lock(x,c) (c)

/* Helps compiler to generate more optimal code.
 * It shows which branch has higher probability.
 * See gcc documentation for __builtin_expect()
 */
#define likely(x)	(__builtin_constant_p(x) ? !!(x) :  __builtin_expect(!!(x), 1))
#define unlikely(x)	(__builtin_constant_p(x) ? !!(x) :  __builtin_expect(!!(x), 0))

#if !defined(__STDC__) || !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#define __barrier()
#else
#define __barrier() \
	__asm__ __volatile__("" : : : "memory")

#endif
#endif /* COMPAT_LINUX_LINUX_COMPILER_H_ */
