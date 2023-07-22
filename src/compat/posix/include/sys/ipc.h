/**
 * @file
 * @brief https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysipc.h.html
 *
 * @author Aleksey Zhmulin
 * @date 19.07.23
 */
#ifndef SYS_IPC_H_
#define SYS_IPC_H_

#include <sys/types.h>
#include <sys/cdefs.h>

/* Mode bits */
#define IPC_CREAT  01000 /* Create key if key does not exist */
#define IPC_EXCL   02000 /* Fail if key exists */
#define IPC_NOWAIT 04000 /* Error if request must wait */

/* Control commands */
#define IPC_RMID 0 /* Remove identifier */
#define IPC_SET  1 /* Set options */
#define IPC_STAT 2 /* Get options */

/* Special key values */
#define IPC_PRIVATE ((key_t)0) /* Private key */

struct ipc_perm {
	uid_t uid;   /* Owner's user ID */
	gid_t gid;   /* Owner's group ID */
	uid_t cuid;  /* Creator's user ID */
	gid_t cgid;  /* Creator's group ID */
	mode_t mode; /* Read/write permission */
};

__BEGIN_DECLS

/**
 * @brief Generate an IPC key.
 *
 * @param path
 * @param id
 *
 * @return
 */
extern key_t ftok(const char *path, int id);

__END_DECLS

#endif /* SYS_IPC_H_ */
