/**
 * @file
 * @brief POSIX Standard: 7.1-2 General Terminal Interface.
 * @details Defines the structure of the termios file, which provides
 *          the terminal interface for POSIX compatibility.
 *
 * @date 07.11.10
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_TERMIOS_H_
#define COMPAT_POSIX_TERMIOS_H_

#include <sys/ioctl.h>
#include <sys/types.h>

/* Values for termios c_iflag bit map.  POSIX Table 7-2. */
#define BRKINT    0x0001 /* signal interrupt on break */
#define ICRNL     0x0002 /* map CR to NL on input */
#define IGNBRK    0x0004 /* ignore break */
#define IGNCR     0x0008 /* ignore CR */
#define IGNPAR    0x0010 /* ignore characters with parity errors */
#define INLCR     0x0020 /* map NL to CR on input */
#define INPCK     0x0040 /* enable input parity check */
#define ISTRIP    0x0080 /* mask off 8th bit */
#define IXOFF     0x0100 /* enable start/stop input control */
#define IXON      0x0200 /* enable start/stop output control */
#define PARMRK    0x0400 /* mark parity errors in the input queue */
#define IXANY     0x0800
#define CRTSCTS   0x1000 /* (not in POSIX) Enable RTS/CTS flow control.*/

/* Values for termios c_oflag bit map.  POSIX Sec. 7.1.2.3. */
#define OPOST     0x0001 /* perform output processing */
#define ONLCR     0x0002 /* map NL to CR-NL (ala CRMOD) */
#define OXTABS    0x0004 /* expand tabs to spaces */
#define ONOEOT    0x0008 /* discard EOT's (^D) on output) */

/* Values for termios c_cflag bit map.  POSIX Table 7-3. */
#define CLOCAL    0x0001 /* ignore modem status lines */
#define CREAD     0x0002 /* enable receiver */
#define CSIZE     0x000C /* number of bits per character */
#define CS5       0x0000 /* if CSIZE is CS5, characters are 5 bits */
#define CS6       0x0004 /* if CSIZE is CS6, characters are 6 bits */
#define CS7       0x0008 /* if CSIZE is CS7, characters are 7 bits */
#define CS8       0x000C /* if CSIZE is CS8, characters are 8 bits */
#define CSTOPB    0x0010 /* send 2 stop bits if set, else 1 */
#define HUPCL     0x0020 /* hang up on last close */
#define PARENB    0x0040 /* enable parity on output */
#define PARODD    0x0080 /* use odd parity if set, else even */

/* Values for termios c_lflag bit map.  POSIX Table 7-4. */
#define ECHO      0x0001 /* enable echoing of input characters */
#define ECHOE     0x0002 /* echo ERASE as backspace */
#define ECHOK     0x0004 /* echo KILL */
#define ECHONL    0x0008 /* echo NL */
#define ICANON    0x0010 /* canonical input (erase and kill enabled) */
#define IEXTEN    0x0020 /* enable extended functions */
#define ISIG      0x0040 /* enable signals */
#define NOFLSH    0x0080 /* disable flush after interrupt or quit */
#define TOSTOP    0x0100 /* send SIGTTOU (job control, not implemented*/

/* Values for the baud rate settings (c_cflag bits).  POSIX Table 7-6. */
#define B0        0x0000 /* hang up the line */
#define B50       0x0100 /* 50 baud */
#define B75       0x0200 /* 75 baud */
#define B110      0x0300 /* 110 baud */
#define B134      0x0400 /* 134.5 baud */
#define B150      0x0500 /* 150 baud */
#define B200      0x0600 /* 200 baud */
#define B300      0x0700 /* 300 baud */
#define B600      0x0800 /* 600 baud */
#define B1200     0x0900 /* 1200 baud */
#define B1800     0x0a00 /* 1800 baud */
#define B2400     0x0b00 /* 2400 baud */
#define B4800     0x0c00 /* 4800 baud */
#define B9600     0x0d00 /* 9600 baud */
#define B19200    0x0e00 /* 19200 baud */
#define B38400    0x0f00 /* 38400 baud */

/*
 * cc_c - Control characters array.
 */

/* Indices into c_cc array.  Default values in parentheses. POSIX Table 7-5. */
#define VEOF      0  /* cc_c[VEOF]   = EOF char (^D) */
#define VEOL      1  /* cc_c[VEOL]   = EOL char (undef) */
#define VERASE    2  /* cc_c[VERASE] = ERASE char (^H) */
#define VINTR     3  /* cc_c[VINTR]  = INTR char (DEL) */
#define VKILL     4  /* cc_c[VKILL]  = KILL char (^U) */
#define VMIN      5  /* cc_c[VMIN]   = MIN amount for reading */
#define VQUIT     6  /* cc_c[VQUIT]  = QUIT char (^\) */
#define VTIME     7  /* cc_c[VTIME]  = TIME value for timer */
#define VSUSP     8  /* cc_c[VSUSP]  = SUSP (^Z, ignored) */
#define VSTART    9  /* cc_c[VSTART] = START char (^S) */
#define VSTOP     10 /* cc_c[VSTOP]  = STOP char (^Q) */

#define NCCS      11 /* Size of the array of control characters.*/

/* Optional actions for tcsetattr().  POSIX Sec. 7.2.1.2. */
#define TCSANOW   1 /* changes take effect immediately */
#define TCSADRAIN 2 /* changes take effect after output is done */
#define TCSAFLUSH 3 /* wait for output to finish and flush input */

/* Queue_selector values for tcflush().  POSIX Sec. 7.2.2.2. */
#define TCIFLUSH  1 /* flush accumulated input data */
#define TCOFLUSH  2 /* flush accumulated output data */
#define TCIOFLUSH 3 /* flush accumulated input and output data */

/* Action values for tcflow().  POSIX Sec. 7.2.2.2. */
#define TCOOFF    1 /* suspend output */
#define TCOON     2 /* restart suspended output */
#define TCIOFF    3 /* transmit a STOP character on the line */
#define TCION     4 /* transmit a START character on the line */

typedef unsigned char cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

struct termios {
	tcflag_t c_iflag; /* input mode flags */
	tcflag_t c_oflag; /* output mode flags */
	tcflag_t c_cflag; /* control mode flags */
	tcflag_t c_lflag; /* local mode flags */
	cc_t c_cc[NCCS];  /* control characters */
};

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int tcgetattr(int fd, struct termios *);
extern int tcsetattr(int fd, int opt, const struct termios *);

extern int tcflush(int fd, int queue_selector);

extern pid_t tcgetpgrp(int fd);
extern int tcsetpgrp(int fd, pid_t pgrp);

extern speed_t cfgetispeed(const struct termios *termios);
extern speed_t cfgetospeed(const struct termios *termios);
extern int cfsetispeed(struct termios *termios, speed_t speed);
extern int cfsetospeed(struct termios *termios, speed_t speed);

__END_DECLS

/* Non-standard */
#include <asm/termbits.h>

#endif /* COMPAT_POSIX_TERMIOS_H_ */
