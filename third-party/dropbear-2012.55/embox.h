/**
 * @file
 *
 * @brief
 * @date 23.11.2012
 * @author Alexander Kalmuk
 */

#ifndef EMBOX_DROPBEAR_EMBOX_H_
#define EMBOX_DROPBEAR_EMBOX_H_

#define TCP_NODELAY 0

#include <signal.h>


/* <unistd.h */
static inline pid_t setsid(void) {
	return -1;
}

static inline int chown(const char *path, uid_t owner, gid_t group) {
	return -1;
}

extern char *crypt(char *key, const char *salt);
/* end <unistd.h> */

/* <grp.h> */
/* initgroups NOT POSIX */
static inline int initgroups(const char *user, gid_t group) {
	return 0;
}

#include <asm/termbits.h>
#include <sys/select.h>
#include <linux/types.h>

static inline int fork(void) {
	return -1;
}

#endif /* EMBOX_DROPBEAR_EMBOX_H_ */
