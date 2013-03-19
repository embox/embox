/**
 * @file
 *
 * @brief TODO No  single standard. Must move from POSIX
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#ifndef SYS_IOCTL_H_
#define SYS_IOCTL_H_

#include <sys/cdefs.h>

__BEGIN_DECLS


// TODO part of tty_ioctl, not termios -- Eldar
struct winsize {
	unsigned short ws_row;
	unsigned short ws_col;
	unsigned short ws_xpixel;   /* unused */
	unsigned short ws_ypixel;   /* unused */
};


/* ioctl number for tty devices */
#define	TIOCGWINSZ 0x1000

/* Ioctls applicable to any descriptor */
#define FIONBIO   0x00005421

extern int ioctl(int d, int request, ...);

__END_DECLS

#endif /* SYS_IOCTL_H_ */
