/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 30.03.13
 */

#ifndef COMPAT_POSIX_POSIX_ENVIRON_H_
#define COMPAT_POSIX_POSIX_ENVIRON_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern char *** task_self_environ_ptr(void);

#define environ (* task_self_environ_ptr())

__END_DECLS

#endif /* COMPAT_POSIX_POSIX_ENVIRON_H_ */
