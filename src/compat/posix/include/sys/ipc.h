/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.07.23
 */
#ifndef SYS_IPC_H_
#define SYS_IPC_H_

#include <sys/types.h>
#include <sys/cdefs.h>

/* Mode bits for `msgget', `semget', and `shmget'. */
#define IPC_CREAT 01000 /* Create key if key does not exist. */
#define IPC_EXCL  02000 /* Fail if key exists. */

/* Mode bits for `msgrcv' and `semsnd'. */
#define IPC_NOWAIT  04000  /* Return error on wait. */
#define MSG_NOERROR 010000 /* No error if message is too big */

/* Control commands for `msgctl', `semctl', and `shmctl'. */
#define IPC_RMID 0 /* Remove identifier. */
#define IPC_SET  1 /* Set `ipc_perm' options. */
#define IPC_STAT 2 /* Get `ipc_perm' options. */

/* Special key values. */
#define IPC_PRIVATE ((key_t)0) /* Private key. */

struct ipc_perm {
	key_t key; /* Key supplied to `msgget', `semget', or `shmget'. */

	// uid_t uid;           /* Effective UID of owner */
	// gid_t gid;           /* Effective GID of owner */
	// uid_t cuid;          /* Effective UID of creator */
	// gid_t cgid;          /* Effective GID of creator */
	// unsigned short mode; /* Permissions */
	// unsigned short seq;  /* Sequence number */
};

#endif /* SYS_IPC_H_ */
