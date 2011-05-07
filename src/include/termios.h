/**
 * @file
 * @brief POSIX Standard: 7.1-2 General Terminal Interface.
 * @details Defines the structure of the termios file, which provides
 *          the terminal interface for POSIX compatibility.
 *
 * @date 07.11.10
 * @author Anton Bondarev
 */

#ifndef TERMIOS_H_
#define TERMIOS_H_

typedef unsigned char   cc_t;
typedef unsigned int    speed_t;
typedef unsigned int    tcflag_t;

#define NCCS 8
typedef struct termios {
	tcflag_t c_iflag; /* input mode flags */
	tcflag_t c_oflag; /* output mode flags */
	tcflag_t c_cflag; /* control mode flags */
	tcflag_t c_lflag; /* local mode flags */
	cc_t c_line;      /* line discipline */
	cc_t c_cc[NCCS];  /* control characters */
} termios_t;

extern int tcgetattr(int fd, struct termios *termios_p);
extern int tcsetattr(int fd, int optional_actions, struct termios *termios_p);

#endif /* TERMIOS_H_ */
