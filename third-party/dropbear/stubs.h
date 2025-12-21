/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov & Mark Kovalev
 * @date    21.11.2018
 */

#ifndef DROPBEAR_STUBS_H_
#define DROPBEAR_STUBS_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define LOG_AUTH 0

#define LOG_PID 1

#define LOG_EMERG	1
#define LOG_ALERT	2
#define LOG_CRIT	3
#define LOG_ERR		4
#define LOG_WARNING	5
#define LOG_NOTICE	6
#define LOG_INFO	7
#define LOG_DEBUG	8

#define UIO_MAXIOV	1024

static inline void openlog(const char *ident, int option, int facility) {
	(void) ident;
	(void) option;
	(void) facility;
}

void closelog(void);

typedef unsigned int u_int;

static inline int initgroups(const char *user, gid_t group) {
	(void) user;
	(void) group;
	return 0;
}

static inline char *ttyname(int fd) {
	static char buf[16];
	(void) fd;

	return strcpy(buf, "/dev_pty_0");
}

extern int daemon(int nochdir, int noclose);

#endif /* DROPBEAR_STUBS_H_ */
