#ifndef _LIBPCAP_SYS_EVENTFD_H_
#define _LIBPCAP_SYS_EVENTFD_H_

#define NETLINK_GENERIC                16

enum
  {
    EFD_SEMAPHORE = 00000001,
#define EFD_SEMAPHORE EFD_SEMAPHORE
    EFD_CLOEXEC = 02000000,
#define EFD_CLOEXEC EFD_CLOEXEC
    EFD_NONBLOCK = 00004000
#define EFD_NONBLOCK EFD_NONBLOCK
  };

#include <sys/cdefs.h>
__BEGIN_DECLS

static inline int eventfd (unsigned int interval, int flags) {
  return 1;
}

__END_DECLS

#endif /*_LIBPCAP_SYS_EVENTFD_H_*/
