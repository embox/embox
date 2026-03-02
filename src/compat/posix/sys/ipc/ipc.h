/**
 * @file
 * @brief https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysipc.h.html
 *
 * @author Aleksey Zhmulin
 * @date 19.07.23
 */
#ifndef COMPAT_POSIX_SYS_IPC_H_
#define COMPAT_POSIX_SYS_IPC_H_

#include <sys/types.h>

/* Bits of 'msgflg', 'shmflg', 'semget' args of msgget(), shmget(), semget() */
#define IPC_MODE   0x01ff /* Mode bit mask */
#define IPC_CREAT  0x0200 /* Create key if key does not exist */
#define IPC_EXCL   0x0400 /* Fail if key exists */
#define IPC_NOWAIT 0x0800 /* Error if request must wait */

/* Special value of the 'key' arg of msgget(), shmget(), semget() */
#define IPC_PRIVATE ((key_t)0) /* Private key */

/* Values of the 'cmd' arg of msgctl(), shmctl() and semclt() */
#define IPC_RMID 0 /* Remove identifier */
#define IPC_SET  1 /* Set options */
#define IPC_STAT 2 /* Get options */

struct ipc_perm {
	uid_t uid;   /* Owner's user ID */
	gid_t gid;   /* Owner's group ID */
	uid_t cuid;  /* Creator's user ID */
	gid_t cgid;  /* Creator's group ID */
	mode_t mode; /* Read/write permission */
};

/**
 * @brief Generate an IPC key.
 *
 * @param path
 * @param id
 *
 * @return
 */
static inline key_t ftok(const char *path, int id) {
	return (key_t)(id & 0xff);
}

#endif /* COMPAT_POSIX_SYS_IPC_H_ */
