/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.04.2014
 */

#ifndef DROPBEAR_2014_63_STUBS_H_
#define DROPBEAR_2014_63_STUBS_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>

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


static inline void openlog(const char *ident, int option, int facility) {
	(void) ident;
	(void) option;
	(void) facility;
}

void closelog(void);

#define TCP_NODELAY 0
#define UNIX_PATH_MAX 255
struct sockaddr_un {
               sa_family_t sun_family;               /* AF_UNIX */
               char        sun_path[UNIX_PATH_MAX];  /* pathname */
};

typedef unsigned int u_int;

struct rlimit;
static inline int setrlimit(int resource, const struct rlimit *rlp) {
	(void) resource;
	(void) rlp;
	return SET_ERRNO(ENOSYS);
}

static inline pid_t setsid(void) {
	return 0;
}

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

#endif /* DROPBEAR_2014_63_STUBS_H_ */
