/**
 * @file
 *
 * @date Jul 19, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_DEFINES_UCONTEXT_T_DEFINE_H_
#define SRC_COMPAT_POSIX_INCLUDE_DEFINES_UCONTEXT_T_DEFINE_H_

#include <ucontext.h> /* mcontext_t */

#include <defines/sigset_t_define.h>
#include <defines/stack_t_define.h>

struct _ucontext {
	struct _ucontext *uc_link;     /* resumed when this context returns */
	sigset_t    uc_sigmask;  /* blocked when this context is active */
	stack_t     uc_stack;    /* the stack used by this context */
	mcontext_t  uc_mcontext; /* machine-specific representation */
};
typedef struct _ucontext ucontext_t;


#endif /* SRC_COMPAT_POSIX_INCLUDE_DEFINES_UCONTEXT_T_DEFINE_H_ */
