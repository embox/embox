/**
 * @file
 *
 * @date: Jul 18, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_UCONTEXT
#define SRC_COMPAT_POSIX_INCLUDE_UCONTEXT

#include <defines/sigset_t_define.h>
#include <defines/stack_t_define.h>

typedef int mcontext_t;

#include <defines/ucontext_t_define.h>

#endif /* SRC_COMPAT_POSIX_INCLUDE_UCONTEXT */
