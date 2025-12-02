/**
 * @file
 *
 * @date 01.12.25
 * @author Anton Bondarev
 */

#ifndef __SYS_EVENTFD_H__
#define __SYS_EVENTFD_H__

#include <sys/cdefs.h>

/* Flags for eventfd.  */
enum
  {
    EFD_SEMAPHORE = 000000001,
#define EFD_SEMAPHORE EFD_SEMAPHORE
    EFD_CLOEXEC   = 010000000,
#define EFD_CLOEXEC EFD_CLOEXEC
    EFD_NONBLOCK  = 000000004
#define EFD_NONBLOCK EFD_NONBLOCK
  };

__BEGIN_DECLS

extern int eventfd(unsigned int initval, int flags);

__END_DECLS

#endif /* __SYS_EVENTFD_H__ */