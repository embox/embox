/**
 * @file
 * @brief
 *
 * @date 29.04.2025
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_SYS_SYSCALL_H_
#define COMPAT_POSIX_SYS_SYSCALL_H_

#include <sys/cdefs.h>


#define SYS_gettid    0x0

__BEGIN_DECLS

extern long syscall(long number, ...);

__END_DECLS

#endif /* COMPAT_POSIX_SYS_SYSCALL_H_ */
