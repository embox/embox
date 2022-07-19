/**
 * @file
 *
 * @date Jul 19, 2022
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_POSIX_INCLUDE_DEFINES_STACK_T_DEFINE_H_
#define SRC_COMPAT_POSIX_INCLUDE_DEFINES_STACK_T_DEFINE_H_

#include <stddef.h>

typedef struct {
	void     *ss_sp;       /* stack base or pointer */
	size_t    ss_size;     /* stack size */
	int       ss_flags;    /* flags */
} stack_t;


#endif /* SRC_COMPAT_POSIX_INCLUDE_DEFINES_STACK_T_DEFINE_H_ */
