/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 19.07.23
 */
#ifndef SYS_MSG_H_
#define SYS_MSG_H_

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/cdefs.h>

struct msqid_ds {
	struct ipc_perm msg_perm; /* Ownership and permissions */

	// time_t msg_stime;         /* Time of last `msgsnd` */
	// time_t msg_rtime;         /* Time of last `msgrcv` */
	// time_t msg_ctime;         /* Time of last change */
	// size_t msg_qbytes;        /* Maximum number of bytes allowed in queue */
	// size_t msg_qnum;          /* Current number of messages in queue */
	// pid_t msg_lspid;          /* PID of last `msgsnd` */
	// pid_t msg_lrpid;          /* PID of last `msgrcv` */
	// unsigned long msg_cbytes; /* Current number of bytes in
	//                              iqueue (nonstandard) */
};

__BEGIN_DECLS

/**
 * @brief Message queue control operation.
 *
 * @param msqid
 * @param cmd
 * @param buf
 *
 * @return
 */
extern int msgctl(int msqid, int cmd, struct msqid_ds *buf);

/**
 * @brief Get messages queue.
 *
 * @param key
 * @param msgflg
 *
 * @return
 */
extern int msgget(key_t key, int msgflg);

/**
 * @brief Receive message from message queue.
 *
 * @param msqid
 * @param msgp
 * @param msgsz
 * @param msgtyp
 * @param msgflg
 *
 * @return
 */
extern ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
    int msgflg);

/**
 * @brief Send message to sessage queue.
 *
 * @param msqid
 * @param msgp
 * @param msgsz
 * @param msgflg
 *
 * @return
 */
extern int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

__END_DECLS

#endif /* SYS_MSG_H_ */
