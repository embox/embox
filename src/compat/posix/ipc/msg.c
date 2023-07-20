/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.07.23
 */
#include <errno.h>
#include <assert.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <util/dlist.h>
#include <posix_errno.h>
#include <mem/misc/pool.h>
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

struct msg_queue {
	struct msg_buff buf;
	struct msqid_ds msqid_ds;
	struct dlist_head dlist_item;
	char storage[MSG_QUEUE_BUF_SIZE];
	int msqid;
	key_t key;
};

POOL_DEF(msq_pool, struct msg_queue, MSG_QUEUE_COUNT);

static DLIST_DEFINE(msq_list_head);

static int msqid_indexator;

static struct msg_queue *msg_queue_get(int msqid) {
	struct msg_queue *tmp;

	dlist_foreach_entry(tmp, &msq_list_head, dlist_item) {
		if (msqid == tmp->msqid) {
			return tmp;
		}
	}

	return NULL;
}

static struct msg_queue *msg_queue_create(key_t key) {
	struct msg_queue *queue;

	queue = pool_alloc(&msq_pool);

	if (queue) {
		msg_buff_init(&queue->buf, queue->storage, MSG_QUEUE_BUF_SIZE);

		queue->msqid = msqid_indexator++;
		queue->key = key;

		dlist_init(&queue->dlist_item);
		dlist_add_next(&queue->dlist_item, &msq_list_head);
	}

	return queue;
}

static void msg_queue_free(struct msg_queue *queue) {
	dlist_del_init(&queue->dlist_item);
	pool_free(&msq_pool, queue);
}

int msgctl(int msqid, int cmd, struct msqid_ds *buf) {
	struct msg_queue *queue;
	int ret;

	queue = msg_queue_get(msqid);

	if (!queue) {
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	switch (cmd) {
	case IPC_STAT:
		if (!buf) {
			ret = SET_ERRNO(EFAULT);
			break;
		}
		buf = &queue->msqid_ds;
		ret = 0;
		break;

	case IPC_SET:
		if (!buf) {
			ret = SET_ERRNO(EFAULT);
			break;
		}
		queue->msqid_ds.msg_perm.uid = buf->msg_perm.uid;
		queue->msqid_ds.msg_perm.gid = buf->msg_perm.gid;
		queue->msqid_ds.msg_perm.mode = buf->msg_perm.mode;
		queue->msqid_ds.msg_qbytes = buf->msg_qbytes;
		ret = 0;
		break;

	case IPC_RMID:
		msg_queue_free(queue);
		ret = 0;
		break;

	default:
		ret = SET_ERRNO(EINVAL);
		break;
	}

out:
	return ret;
}

int msgget(key_t key, int msgflg) {
	struct msg_queue *queue;
	int ret;

	if (key == IPC_PRIVATE) {
		queue = msg_queue_create(key);
		if (queue) {
			ret = queue->msqid;
		}
		else {
			ret = SET_ERRNO(ENOSPC);
		}
		goto out;
	}

	dlist_foreach_entry(queue, &msq_list_head, dlist_item) {
		if (key == queue->key) {
			if ((msgflg & IPC_CREAT) && (msgflg & IPC_EXCL)) {
				ret = SET_ERRNO(EEXIST);
			}
			else {
				ret = queue->msqid;
			}
			goto out;
		}
	}

	if (msgflg & IPC_CREAT) {
		queue = msg_queue_create(key);
		if (queue) {
			ret = queue->msqid;
		}
		else {
			ret = SET_ERRNO(ENOSPC);
		}
	}
	else {
		ret = SET_ERRNO(ENOENT);
	}

out:
	return ret;
}

ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
	struct msg_queue *queue;
	int ret;

	queue = msg_queue_get(msqid);

	if (!queue || (msgtyp < 0)) {
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	/* TODO: */
	assertf(!msgtyp, "msgrcv(): msgtyp > 0 is not supported yet");

	msgsz += sizeof(long);

	ret = msg_buff_dequeue(&queue->buf, msgp, msgsz);
	if (!ret) {
		ret = SET_ERRNO(ENOMSG);
	}
	else {
		/* TODO: notify that msg sent */
		ret = 0;
	}

out:
	return ret;
}

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	const struct msgbuf *msgbuf;
	struct msg_queue *queue;
	int ret;

	queue = msg_queue_get(msqid);
	msgbuf = (const struct msgbuf *)msgp;

	if (!queue || (msgbuf->mtype < 0)) {
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	msgsz += sizeof(long);

	/* TODO: */

	// do {
	// 	msgsz -= msg_buff_enqueue(&queue->buf, msgp, msgsz);
	// 	/* Wait until there is a free space */
	// } while (msgsz);

	if (0 == msg_buff_enqueue(&queue->buf, msgp, msgsz)) {
		ret = SET_ERRNO(EAGAIN);
	}
	else {
		/* TODO: notify that msg received */
		ret = 0;
	}

out:
	return ret;
}
