/**
 * @file
 * @brief https://pubs.opengroup.org/onlinepubs/7908799/xsh/sysmsg.h.html
 *
 * @author Aleksey Zhmulin
 * @date 19.07.23
 */
#ifndef SYS_MSG_H_
#define SYS_MSG_H_

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/cdefs.h>

/* Message operation flags */
#define MSG_NOERROR 010000 /* No error if message is too big */

typedef unsigned long msgqnum_t;
typedef unsigned long msglen_t;

struct msqid_ds {
	struct ipc_perm msg_perm; /* Operation permission structure */
	msgqnum_t msg_qnum;       /* Number of messages currently on queue */
	msglen_t msg_qbytes;      /* Maximum number of bytes allowed on queue */
	pid_t msg_lspid;          /* Process ID of last msgsnd() */
	pid_t msg_lrpid;          /* Process ID of last msgrcv() */
	time_t msg_stime;         /* Time of last msgsnd() */
	time_t msg_rtime;         /* Time of last msgrcv() */
	time_t msg_ctime;         /* Time of last change */
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
