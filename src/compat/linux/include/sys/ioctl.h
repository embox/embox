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

/* socket i/o controls */
#define SIOCSHIWAT _IOW('s',  0, int)       /* set high watermark */
#define SIOCGHIWAT _IOR('s',  1, int)       /* get high watermark */
#define SIOCSLOWAT _IOW('s',  2, int)       /* set low watermark */
#define SIOCGLOWAT _IOR('s',  3, int)       /* get low watermark */
#define SIOCATMARK _IOR('s',  7, int)       /* at oob mark? */
#define SIOCSPGRP  _IOW('s',  8, int)       /* set process group */
#define SIOCGPGRP  _IOR('s',  9, int)       /* get process group */

#define SIOCADDRT  _IOW('r', 10, struct ortentry) /* add route */
#define SIOCDELRT  _IOW('r', 11, struct ortentry) /* delete route */

#define SIOCSIFADDR     _IOW('i', 12, struct ifreq) /* set ifnet address */
#define OSIOCGIFADDR    _IOWR('i',13, struct ifreq) /* get ifnet address */
#define SIOCGIFADDR     _IOWR('i',33, struct ifreq) /* get ifnet address */
#define SIOCSIFDSTADDR  _IOW('i', 14, struct ifreq) /* set p-p address */
#define OSIOCGIFDSTADDR _IOWR('i',15, struct ifreq) /* get p-p address */
#define SIOCGIFDSTADDR  _IOWR('i',34, struct ifreq) /* get p-p address */
#define SIOCSIFFLAGS    _IOW('i', 16, struct ifreq) /* set ifnet flags */
#define SIOCGIFFLAGS    _IOWR('i',17, struct ifreq) /* get ifnet flags */
#define OSIOCGIFBRDADDR _IOWR('i',18, struct ifreq) /* get broadcast addr */
#define SIOCGIFBRDADDR  _IOWR('i',35, struct ifreq) /* get broadcast addr */
#define SIOCSIFBRDADDR  _IOW('i',19, struct ifreq)  /* set broadcast addr */
#define OSIOCGIFCONF    _IOWR('i',20, struct ifconf) /* get ifnet list */
#define SIOCGIFCONF     _IOWR('i',36, struct ifconf) /* get ifnet list */
#define OSIOCGIFNETMASK _IOWR('i',21, struct ifreq)  /* get net addr mask */
#define SIOCGIFNETMASK  _IOWR('i',37, struct ifreq)  /* get net addr mask */
#define SIOCSIFNETMASK  _IOW('i',22, struct ifreq)   /* set net addr mask */
#define SIOCGIFMETRIC   _IOWR('i',23, struct ifreq)  /* get IF metric */
#define SIOCSIFMETRIC   _IOW('i',24, struct ifreq)   /* set IF metric */
#define SIOCDIFADDR     _IOW('i',25, struct ifreq)   /* delete IF addr */
#define SIOCAIFADDR     _IOW('i',26, struct ifaliasreq) /* add/chg IF alias */

#define SIOCSARP        _IOW('i', 30, struct arpreq)  /* set arp entry */
#define OSIOCGARP       _IOWR('i',31, struct arpreq)  /* get arp entry */
#define	SIOCGARP        _IOWR('i',38, struct arpreq)  /* get arp entry */
#define	SIOCDARP        _IOW('i', 32, struct arpreq)  /* delete arp entry */


__BEGIN_DECLS

extern int ioctl(int d, int request, ...);

__END_DECLS

#endif /* SYS_IOCTL_H_ */
