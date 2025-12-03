/*
 * tcl_embox_compat.h
 *
 *  Created on: Feb 23, 2014
 *      Author: alexander
 */

#ifndef TCL_EMBOX_COMPAT_H_
#define TCL_EMBOX_COMPAT_H_

#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>

#define HUGE_VAL ((double)-1)

/* (termios.h) Extensions to the termios c_iflag bit map.  */
#define IXANY           0x0800  /* allow any key to continue ouptut */

#define NI_MAXHOST 1025 // is not POSIX
#define NI_MAXSERV 32   // is not POSIX

struct utimbuf;

static inline double ldexp(double x, int exp) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline char *realpath(const char *path, char *resolved_path) {
	printf("tcl>>> %s\n", __func__);
	return (char *)0;
}

static inline int mkfifo(const char *pathname, mode_t mode) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline int mknod(const char *pathname, mode_t mode, dev_t dev) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline int symlink(const char *oldpath, const char *newpath) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline int utime(const char *filename, const struct utimbuf *times) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline int mkstemps(char *template, int suffixlen) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline double tan(double x) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline double tanh(double x) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

static inline struct group *getgrgid(gid_t gid) {
	printf("tcl>>> %s\n", __func__);
	return NULL;
}

static inline int fork(void) {
	printf("tcl>>> %s\n", __func__);
	return -1;
}

#endif /* TCL_EMBOX_COMPAT_H_ */
