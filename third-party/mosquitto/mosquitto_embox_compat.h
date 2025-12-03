/*
 * mosquitto_embox_compat.h
 *
 *  Created on: 2023-03-01
 *      Author: Yuriy Paramonov
 */

#ifndef MOSQUITTO_EMBOX_COMPAT_H_
#define MOSQUITTO_EMBOX_COMPAT_H_

#ifdef linux
#undef linux
#endif

#ifdef __linux__
#undef __linux__
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>

#if 1
#define DPRINT() printf(">>> mosq CALL %s\n", __FUNCTION__)
#else
#define DPRINT()
#endif

#include <ctype.h>

#include <errno.h>

__BEGIN_DECLS

#include <pthread.h>

static inline int initgroups(const char *user, gid_t group) {
	(void)user;
	(void)group;
	return 0;
}

static inline pid_t getppid(void) {
	DPRINT();
	return 0;
}

static inline pid_t fork() {
	printf(">>> fork()\n");
	errno = ENOSYS;
	return -1;
}

#include <sys/socket.h>

static inline unsigned int alarm(unsigned int seconds) {
	return 0;
}

#define pselect select

__END_DECLS

#endif /* MOSQUITTO_EMBOX_COMPAT_H_ */
