/**
 * @file
 * @brief Describes compiler specific features
 *
 * @date 13.01.2010
 * @author Anton Bondarev
 */

#ifndef COMPILER_H_
#define COMPILER_H_

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

#endif /* COMPILER_H_ */
