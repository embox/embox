/**
 * @file
 *
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_IOCTL_H_
#define SYS_IOCTL_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

/* Ioctls applicable to any descriptor */
#define FIONBIO   0x00005421

extern int ioctl(int d, int request, ...);

__END_DECLS

#endif /* SYS_IOCTL_H_ */
