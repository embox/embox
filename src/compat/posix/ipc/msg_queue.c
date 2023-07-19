/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.07.23
 */
#include <errno.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <posix_errno.h>
#include <mem/sysmalloc.h>
#include <util/msg_buff.h>
#include <framework/mod/options.h>
#include <kernel/sched/sync/mutex.h>

#define MSG_QUEUE_COUNT    8
#define MSG_QUEUE_BUF_SIZE 1024

// #define MSG_QUEUE_COUNT    OPTION_GET(NUMBER, msg_queue_count)
// #define MSG_QUEUE_BUF_SIZE OPTION_GET(NUMBER, msg_queue_buf_size)

struct msgbuf {
	long mtype;
	char mtext[0];
};

static struct {
	struct msg_buff *buf;
	struct msqid_ds msqid_ds;
} msg_queues[MSG_QUEUE_COUNT];

int msgctl(int msqid, int cmd, struct msqid_ds *buf) {
	/* TODO */
	return -1;
}

int msgget(key_t key, int msgflg) {
	void *storage;
	int ret;
	int i;

	for (i = 0; i < MSG_QUEUE_COUNT; i++) {
		if (key == msg_queues[i].msqid_ds.msg_perm.key) {
			ret = i;
			break;
		}
	}

	if ((i == MSG_QUEUE_COUNT) && (msgflg & IPC_CREAT)) {
		for (i = 0; i < MSG_QUEUE_COUNT; i++) {
			if (msg_queues[i].buf == NULL) {
				break;
			}
		}
		if (i == MSG_QUEUE_COUNT) {
			ret = SET_ERRNO(ENOSPC);
		}
		else {
			storage = sysmalloc(MSG_QUEUE_BUF_SIZE);
			if (!storage) {
				ret = SET_ERRNO(ENOMEM);
			}
			msg_queues[i].buf = sysmalloc(sizeof(struct msg_buff));
			if (!msg_queues[i].buf) {
				sysfree(storage);
				ret = SET_ERRNO(ENOMEM);
			}
			else {
				ret = i;
				msg_queues[i].msqid_ds.msg_perm.key = key;
				msg_buff_init(msg_queues[i].buf, storage, MSG_QUEUE_BUF_SIZE);
			}
		}
	}
	else if (i == MSG_QUEUE_COUNT) {
		ret = SET_ERRNO(ENOENT);
	}
	else if ((msgflg & IPC_CREAT) && (msgflg & IPC_EXCL)) {
		ret = SET_ERRNO(EEXIST);
	}

	return ret;
}

ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
	int ret;

	msgsz += sizeof(long);

	if ((msqid < 0) || (msqid >= MSG_QUEUE_COUNT) ||
	    (msg_queues[msqid].buf == NULL) || (msgtyp < 0)) {
		ret = SET_ERRNO(EINVAL);
	}
	else {
		if (0 == msg_buff_dequeue(msg_queues[msqid].buf, msgp, msgsz)) {
			if (msg_buff_empty(msg_queues[msqid].buf)) {
				ret = SET_ERRNO(ENOMSG);
			}
			else {
				ret = SET_ERRNO(E2BIG);
			}
		}
		else {
			ret = 0;
		}
	}

	return ret;
}

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	const struct msgbuf *msgbuf;
	int ret;

	msgbuf = (const struct msgbuf *)msgp;
	msgsz += sizeof(long);

	if ((msqid < 0) || (msqid >= MSG_QUEUE_COUNT) ||
	    (msg_queues[msqid].buf == NULL) || (msgbuf->mtype < 0)) {
		ret = SET_ERRNO(EINVAL);
	}
	else {
		if (0 == msg_buff_enqueue(msg_queues[msqid].buf, msgp, msgsz)) {
			ret = SET_ERRNO(EAGAIN);
		}
		else {
			ret = 0;
		}
	}

	return ret;
}
