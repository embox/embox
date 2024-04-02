/**
 * @file
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 * @author Eldar Abusalimov
 * @author Aleksey Zhmulin
 */

#ifndef COMPAT_LINUX_SYS_IOCTL_H_
#define COMPAT_LINUX_SYS_IOCTL_H_

#include <sys/cdefs.h>

/* 16-bit IOCTL numbers */
#define _IOC_DIRBITS  0
#define _IOC_SIZEBITS 0
#define _IOC_TYPEBITS 8
#define _IOC_NRBITS   8

#if 0 /* 32-bit IOCTL numbers */
#define _IOC_DIRBITS  2
#define _IOC_SIZEBITS 14
#define _IOC_TYPEBITS 8
#define _IOC_NRBITS   8
#endif

/* Direction bits. */
#define _IOC_NONE      0U
#define _IOC_WRITE     1U
#define _IOC_READ      2U

#define _IOC_DIRMASK   ((1U << _IOC_DIRBITS) - 1)
#define _IOC_SIZEMASK  ((1U << _IOC_SIZEBITS) - 1)
#define _IOC_TYPEMASK  ((1U << _IOC_TYPEBITS) - 1)
#define _IOC_NRMASK    ((1U << _IOC_NRBITS) - 1)

#define _IOC_DIRSHIFT  0U
#define _IOC_SIZESHIFT (_IOC_DIRSHIFT + _IOC_DIRBITS)
#define _IOC_TYPESHIFT (_IOC_SIZESHIFT + _IOC_SIZEBITS)
#define _IOC_NRSHIFT   (_IOC_TYPESHIFT + _IOC_TYPEBITS)

#define _IOC(dir, type, nr, size)                       \
	(((_IOC_DIRMASK & (dir)) << _IOC_DIRSHIFT)          \
	    | ((_IOC_SIZEMASK & (size)) << _IOC_SIZESHIFT)) \
	    | ((_IOC_TYPEMASK & (type)) << _IOC_TYPESHIFT)  \
	    | ((_IOC_NRMASK & (nr)) << _IOC_NRSHIFT)

/* Encoding IOCTL numbers */
#define _ION(type, nr)       _IOC(_IOC_NONE, type, nr, 0)
#define _IO(type, nr)        _IOC(_IOC_NONE, type, nr, 0)
#define _IOR(type, nr, arg)  _IOC(_IOC_READ, type, nr, sizeof(arg))
#define _IOW(type, nr, arg)  _IOC(_IOC_WRITE, type, nr, sizeof(arg))
#define _IORW(type, nr, arg) _IOC(_IOC_READ | _IOC_WRITE, type, nr, sizeof(arg))

/* Decoding */
#define _IOC_DIR(ioc_nr)     (((ioc_nr) >> _IOC_DIRSHIFT) & _IOC_DIRMASK)
#define _IOC_TYPE(ioc_nr)    (((ioc_nr) >> _IOC_TYPESHIFT) & _IOC_TYPEMASK)
#define _IOC_NR(ioc_nr)      (((ioc_nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)
#define _IOC_SIZE(ioc_nr)    (((ioc_nr) >> _IOC_SIZESHIFT) & _IOC_SIZEMASK)

/* clang-format off */

/**
 * Socket Control Operations
 */
#define SIOCSPGRP          _IOW('s', 0, int)                /* Set the process group ID */
#define SIOCGPGRP          _IOR('s', 1, int)                /* Get the process group ID */
#define SIOCATMARK         _IOR('s', 2, int)                /* Equivalent to sockatmark */
#define SIOCADDRT          _IOW('s', 3, struct ortentry)    /* Add a route entry in the routing table */
#define SIOCDELRT          _IOW('s', 4, struct ortentry)    /* Delete a route entry from the routing table */
#define SIOCSIFADDR        _IOW('s', 5, struct ifreq)       /* Set ifnet address */
#define SIOCGIFADDR        _IOR('s', 6, struct ifreq)       /* Get ifnet address */
#define SIOCSIFDSTADDR     _IOW('s', 7, struct ifreq)       /* Set p-p address */
#define SIOCGIFDSTADDR     _IOR('s', 8, struct ifreq)       /* Get p-p address */
#define SIOCSIFFLAGS       _IOW('s', 9, struct ifreq)       /* Set ifnet flags */
#define SIOCGIFFLAGS       _IOR('s', 10, struct ifreq)      /* Get ifnet flags */
#define SIOCGIFBRDADDR     _IOR('s', 11, struct ifreq)      /* Get broadcast addr */
#define SIOCSIFBRDADDR     _IOW('s', 12, struct ifreq)      /* Set broadcast addr */
#define SIOCGIFCONF        _IOR('s', 13, struct ifconf)     /* Get ifnet list */
#define SIOCGIFNETMASK     _IOR('s', 14, struct ifreq)      /* Get net addr mask */
#define SIOCSIFNETMASK     _IOW('s', 15, struct ifreq)      /* Set net addr mask */
#define SIOCGIFMETRIC      _IOR('s', 16, struct ifreq)      /* Get IF metric */
#define SIOCSIFMETRIC      _IOW('s', 17, struct ifreq)      /* Set IF metric */
#define SIOCDIFADDR        _IOW('s', 18, struct ifreq)      /* Delete IF addr */
#define SIOCAIFADDR        _IOW('s', 19, struct ifaliasreq) /* Add/chg IF alias */
#define SIOCSARP           _IOW('s', 20, struct arpreq)     /* Set arp entry */
#define SIOCGARP           _IOR('s', 21, struct arpreq)     /* Get arp entry */
#define SIOCDARP           _IOW('s', 22, struct arpreq)     /* Delete arp entry */

/**
 * TTY Control Operations
 */
#define TCGETS             _IOR('t', 0, struct termios)  /* Equivalent to tcgetattr(fd, argp) */
#define TCSETS             _IOW('t', 1, struct termios)  /* Equivalent to tcsetattr(fd, TCSANOW, argp) */
#define TCSETSW            _IOW('t', 2, struct termios)  /* Equivalent to tcsetattr(fd, TCSADRAIN, argp) */
#define TCSETSF            _IOW('t', 3, struct termios)  /* Equivalent to tcsetattr(fd, TCSAFLUSH, argp) */
#define TIOCGLCKTRMIOS     _IOR('t', 4, struct termios)  /* Get the locking status */
#define TIOCSLCKTRMIOS     _IOW('t', 5, struct termios)  /* Set the locking status */
#define TCGETS2            _IOR('t', 6, struct termios2) /* Like TCGETS, but with struct termios2 */
#define TCSETS2            _IOW('t', 7, struct termios2) /* Like TCSETS, but with struct termios2 */
#define TCSETSW2           _IOW('t', 8, struct termios2) /* Like TCSETSW, but with struct termios2 */
#define TCSETSF2           _IOW('t', 9, struct termios2) /* Like TCSETSF, but with struct termios2 */
#define TCGETA             _IOR('t', 10, struct termio)  /* Like TCGETS, but with struct termio */
#define TCSETA             _IOW('t', 11, struct termio)  /* Like TCSETS, but with struct termio */
#define TCSETAW            _IOW('t', 12, struct termio)  /* Like TCSETSW, but with struct termio */
#define TCSETAF            _IOW('t', 13, struct termio)  /* Like TCSETSF, but with struct termio */
#define TIOCGWINSZ         _IOR('t', 14, struct winsize) /* Get window size */
#define TIOCSWINSZ         _IOW('t', 15, struct winsize) /* Set window size */
#define TCSBRK             _ION('t', 16 /*int*/)         /* Equivalent to tcsendbreak(fd, arg) */
#define TCSBRKP            _ION('t', 17 /*int*/)         /* So-called "POSIX version" of TCSBRK */
#define TIOCSBRK           _ION('t', 18)                 /* Turn break on, start sending zero bits */
#define TIOCCBRK           _ION('t', 19)                 /* Turn break off, stop sending zero bits */
#define TCXONC             _ION('t', 20 /*int*/)         /* Equivalent to tcflow(fd, arg) */
#define TCFLSH             _ION('t', 21 /*int*/)         /* Equivalent to tcflush(fd, arg) */
#define TIOCOUTQ           _IOR('t', 22, int)            /* Get the number of bytes in the output buffer */
#define TIOCINQ            _IOR('t', 23, int)            /* Get the number of bytes in the input buffer */
#define TIOCSTI            _IOW('t', 24, char)           /* Insert the given byte in the input queue */
#define TIOCCONS           _ION('t', 25)                 /* Redirect /dev/console output */
#define TIOCSCTTY          _ION('t', 26 /*int*/)         /* Make the given terminal the controlling terminal */
#define TIOCNOTTY          _ION('t', 27)                 /* Give up this controlling terminal */
#define TIOCGPGRP          _IOR('t', 28, pid_t)          /* Equivalent to tcgetpgrp(fd) */
#define TIOCSPGRP          _IOW('t', 29, pid_t)          /* Equivalent to tcsetpgrp(fd, *arg) */
#define TIOCGSID           _IOR('t', 30, pid_t)          /* Get the session ID of the given terminal */
#define TIOCGEXCL          _IOR('t', 31, pid_t)          /* Get the terminal mode */
#define TIOCEXCL           _ION('t', 32)                 /* Put the terminal into exclusive mode */
#define TIOCNXCL           _ION('t', 33)                 /* Disable exclusive mode */
#define TIOCGETD           _IOR('t', 34, int)            /* Get the line discipline of the terminal */
#define TIOCSETD           _IOW('t', 35, int)            /* Set the line discipline of the terminal */
#define TIOCPKT            _IOW('t', 36, int)            /* Enable or disable packet mode */
#define TIOGCPKT           _IOR('t', 37, int)            /* Get the current packet mode */
#define TIOCSPTLCK         _IOW('t', 38, int)            /* Set or remove the pseudoterminal slave device */
#define TIOCGPTLCK         _IOR('t', 39, int)            /* Get lock state of the pseudoterminal slave device */
#define TIOCMGET           _IOR('t', 40, int)            /* Get the status of modem bits */
#define TIOCMSET           _IOW('t', 41, int)            /* Set the status of modem bits */
#define TIOCMBIC           _IOW('t', 42, int)            /* Clear the indicated modem bits */
#define TIOCMBIS           _IOW('t', 43, int)            /* Set the indicated modem bits */
#define TIOCMIWAIT         _ION('t', 44 /*int*/)         /* Wait for any of the specified modem bits to change */
#define TIOCGSOFTCAR       _IOR('t', 46, int)            /* Get the status of the CLOCAL flag */
#define TIOCSSOFTCAR       _IOW('t', 47, int)            /* Set the status of the CLOCAL flag */

/* Used for packet mode */
#define TIOCPKT_DATA       0x000
#define TIOCPKT_FLUSHREAD  0x001
#define TIOCPKT_FLUSHWRITE 0x002
#define TIOCPKT_STOP       0x004
#define TIOCPKT_START      0x008
#define TIOCPKT_NOSTOP     0x010
#define TIOCPKT_DOSTOP     0x020
#define TIOCPKT_IOCTL      0x040

/* modem lines */
#define TIOCM_LE           0x001
#define TIOCM_DTR          0x002
#define TIOCM_RTS          0x004
#define TIOCM_ST           0x008
#define TIOCM_SR           0x010
#define TIOCM_CTS          0x020
#define TIOCM_CAR          0x040
#define TIOCM_RNG          0x080
#define TIOCM_DSR          0x100
#define TIOCM_CD           TIOCM_CAR
#define TIOCM_RI           TIOCM_RNG

/**
 * File Control Operations
 */
#define FIONREAD           _IOR('f', 0, int) /* Get the number of bytes available for reading */
#define FIOCLEX            _IOW('f', 1, int) /* Set "close-on-exec" bit */
#define FIONCLEX           _IOW('f', 2, int) /* Clear "close-on-exec" bit */
#define FIONBIO            _IOW('f', 3, int) /* Set or remove O_NONBLOCK flag */
#define FIOASYNC           _IOW('f', 3, int) /* Set or remove O_SYNC flag */

/* clang-format on */

struct winsize {
	unsigned short int ws_row;
	unsigned short int ws_col;
	unsigned short int ws_xpixel;
	unsigned short int ws_ypixel;
};

__BEGIN_DECLS

extern int ioctl(int fd, int request, ...);

__END_DECLS

#endif /* COMPAT_LINUX_SYS_IOCTL_H_ */
