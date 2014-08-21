/**
 * @file
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 */

#ifndef SYS_IOCTL_H_
#define SYS_IOCTL_H_

#include <sys/cdefs.h>

/* Encoding IOCTL numbers */

#define _IO(type, nr)         _IOC(_IOC_NONE, type, nr, 0)
#define _IOR(type, nr, arg)   _IOC(_IOC_READ, type, nr, sizeof(arg))
#define _IOW(type, nr, arg)   _IOC(_IOC_WRITE, type, nr, sizeof(arg))
#define _IOWR(type, nr, arg)  _IOC(_IOC_READ|_IOC_WRITE, type, nr, sizeof(arg))

/* Decoding */

#define _IOC_DIR(ioc_nr)     (((ioc_nr) >> _IOC_DIRSHIFT)  & _IOC_DIRMASK)
#define _IOC_TYPE(ioc_nr)    (((ioc_nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(ioc_nr)      (((ioc_nr) >> _IOC_NRSHIFT)   & _IOC_NRMASK)
#define _IOC_SIZE(ioc_nr)    (((ioc_nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

/* Direction bits. */

#define _IOC_NONE   0U
#define _IOC_WRITE  1U
#define _IOC_READ   2U

/* internals... */

#define _IOC_NRBITS     8
#define _IOC_TYPEBITS   8
#define _IOC_SIZEBITS   14
#define _IOC_DIRBITS    2

#define _IOC_NRMASK     ((0x1 << _IOC_NRBITS) - 1)
#define _IOC_TYPEMASK   ((0x1 << _IOC_TYPEBITS) - 1)
#define _IOC_SIZEMASK   ((0x1 << _IOC_SIZEBITS) - 1)
#define _IOC_DIRMASK    ((0x1 << _IOC_DIRBITS) - 1)

#define _IOC_NRSHIFT    0
#define _IOC_TYPESHIFT  (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT  (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT   (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC(dir, type, nr, size) \
	(((dir)  << _IOC_DIRSHIFT)  | \
	 ((type) << _IOC_TYPESHIFT) | \
	 ((nr)   << _IOC_NRSHIFT)   | \
	 ((size) << _IOC_SIZESHIFT))

/* Ioctls applicable to any descriptor */
#define FIONBIO   _IOW('a', 0, const int)
#define FIONREAD  _IO ('a', 1)

__BEGIN_DECLS

extern int ioctl(int d, int request, ...);

__END_DECLS

#endif /* SYS_IOCTL_H_ */
