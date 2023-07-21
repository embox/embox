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

#include <posix_errno.h>
#include <util/dlist.h>
#include <util/msg_buff.h>
#include <mem/misc/pool.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/spinlock.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/thread/thread_sched_wait.h>
#include <framework/mod/options.h>

#define MSG_QUEUE_COUNT    OPTION_GET(NUMBER, msg_queue_count)
#define MSG_QUEUE_BUF_SIZE OPTION_GET(NUMBER, msg_queue_buf_size)

#define MSG_QUEUE_FULL    1
#define MSG_QUEUE_EMPTY   2
#define MSG_QUEUE_DELETED 3
#define MSG_QUEUE_OTHER   4

struct msgbuf {
	long mtype;
	char mtext[0];
};

struct waiting_thread {
	struct dlist_head dlist_item;
	struct thread *th;
};

struct msg_queue {
	struct dlist_head dlist_item;
	struct dlist_head receivers;
	struct dlist_head senders;
	struct msg_buff buf;
	struct msqid_ds ds;
	spinlock_t lock;
	key_t key;
	int state;
	int id;
	char storage[MSG_QUEUE_BUF_SIZE];
};

POOL_DEF(msq_pool, struct msg_queue, MSG_QUEUE_COUNT);

static DLIST_DEFINE(msq_list_head);

static void wakeup_waiting_threads(struct dlist_head *list) {
	struct waiting_thread *tmp;

	dlist_foreach_entry(tmp, list, dlist_item) {
		sched_wakeup(&tmp->th->schedee);
	}
}

static struct msg_queue *msg_queue_get(int msqid) {
	struct msg_queue *tmp;

	dlist_foreach_entry(tmp, &msq_list_head, dlist_item) {
		if (msqid == tmp->id) {
			return tmp;
		}
	}

	return NULL;
}

static struct msg_queue *msg_queue_create(key_t key) {
	static int msqid_counter = 0;

	struct msg_queue *queue;

	queue = pool_alloc(&msq_pool);

	if (queue) {
		spin_init(&queue->lock, __SPIN_UNLOCKED);
		msg_buff_init(&queue->buf, queue->storage, MSG_QUEUE_BUF_SIZE);

		dlist_init(&queue->receivers);
		dlist_init(&queue->senders);

		queue->id = msqid_counter++;
		queue->key = key;
		queue->state = MSG_QUEUE_EMPTY;

		queue->ds.msg_qnum = 0;

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
	ipl_t ipl;
	int ret;

	queue = msg_queue_get(msqid);

	ipl = spin_lock_ipl(&queue->lock);

	if (!queue) {
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	if (queue->state == MSG_QUEUE_DELETED) {
		ret = SET_ERRNO(EIDRM);
		goto out;
	}

	switch (cmd) {
	case IPC_STAT:
		if (!buf) {
			ret = SET_ERRNO(EFAULT);
			goto out;
		}
		memcpy(buf, &queue->ds, sizeof(struct msqid_ds));
		break;

	case IPC_SET:
		if (!buf) {
			ret = SET_ERRNO(EFAULT);
			goto out;
		}
		queue->ds.msg_perm.uid = buf->msg_perm.uid;
		queue->ds.msg_perm.gid = buf->msg_perm.gid;
		queue->ds.msg_perm.mode = buf->msg_perm.mode;
		queue->ds.msg_qbytes = buf->msg_qbytes;
		break;

	case IPC_RMID:
		msg_queue_free(queue);
		queue->state = MSG_QUEUE_DELETED;
		wakeup_waiting_threads(&queue->receivers);
		wakeup_waiting_threads(&queue->senders);
		break;

	default:
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	ret = 0;

out:
	spin_unlock_ipl(&queue->lock, ipl);

	return ret;
}

int msgget(key_t key, int msgflg) {
	struct msg_queue *queue;
	int ret;

	if (key == IPC_PRIVATE) {
		queue = msg_queue_create(key);
		if (queue) {
			ret = queue->id;
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
				ret = queue->id;
			}
			goto out;
		}
	}

	if (msgflg & IPC_CREAT) {
		queue = msg_queue_create(key);
		if (queue) {
			ret = queue->id;
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
	struct waiting_thread receiver;
	struct msg_queue *queue;
	size_t msgbuf_size;
	size_t nread;
	ipl_t ipl;
	int ret;

	assertf(!msgtyp, "msgrcv(): msgtyp > 0 is not supported yet");

	queue = msg_queue_get(msqid);

	ipl = spin_lock_ipl(&queue->lock);

	msgbuf_size = msgsz + sizeof(long);

	if (!queue || (msgtyp < 0)) {
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	if (queue->state == MSG_QUEUE_DELETED) {
		ret = SET_ERRNO(EIDRM);
		goto out;
	}

	for (;;) {
		nread = msg_buff_dequeue(&queue->buf, msgp, msgbuf_size);

		if (nread) {
			queue->ds.msg_qnum -= 1;

			if (queue->state == MSG_QUEUE_FULL) {
				queue->state = MSG_QUEUE_OTHER;
				wakeup_waiting_threads(&queue->senders);
			}

			ret = nread - sizeof(long);
			goto out;
		}

		if (msg_buff_empty(&queue->buf)) {
			if (msgflg & IPC_NOWAIT) {
				ret = SET_ERRNO(ENOMSG);
				goto out;
			}

			queue->state = MSG_QUEUE_EMPTY;

			receiver.th = thread_self();
			dlist_init(&receiver.dlist_item);
			dlist_add_next(&receiver.dlist_item, &queue->receivers);

			spin_unlock_ipl(&queue->lock, ipl);

			SCHED_WAIT(queue->state != MSG_QUEUE_EMPTY);

			ipl = spin_lock_ipl(&queue->lock);

			if (queue->state & MSG_QUEUE_DELETED) {
				ret = SET_ERRNO(EIDRM);
				goto out;
			}
		}
		else {
			if (!(msgflg & MSG_NOERROR)) {
				ret = SET_ERRNO(E2BIG);
				goto out;
			}

			queue->ds.msg_qnum -= 1;

			if (queue->state == MSG_QUEUE_FULL) {
				queue->state = MSG_QUEUE_OTHER;
				wakeup_waiting_threads(&queue->senders);
			}

			ret = msgsz;
			goto out;
		}
	}

out:
	spin_unlock_ipl(&queue->lock, ipl);

	return ret;
}

int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	struct waiting_thread sender;
	const struct msgbuf *msgbuf;
	struct msg_queue *queue;
	size_t msgbuf_size;
	size_t nwritten;
	ipl_t ipl;
	int ret;

	queue = msg_queue_get(msqid);

	ipl = spin_lock_ipl(&queue->lock);

	msgbuf = (const struct msgbuf *)msgp;
	msgbuf_size = msgsz + sizeof(long);

	if (!queue || (msgbuf->mtype < 0)) {
		ret = SET_ERRNO(EINVAL);
		goto out;
	}

	if (queue->state == MSG_QUEUE_DELETED) {
		ret = SET_ERRNO(EIDRM);
		goto out;
	}

	for (;;) {
		nwritten = msg_buff_enqueue(&queue->buf, msgbuf, msgbuf_size);

		if (nwritten == msgbuf_size) {
			queue->ds.msg_qnum += 1;

			if (queue->state == MSG_QUEUE_EMPTY) {
				queue->state = MSG_QUEUE_OTHER;
				wakeup_waiting_threads(&queue->receivers);
			}

			ret = 0;
			goto out;
		}
		else {
			if (msgflg & IPC_NOWAIT) {
				ret = SET_ERRNO(EAGAIN);
				goto out;
			}

			msgbuf_size -= nwritten;
			queue->state = MSG_QUEUE_FULL;

			sender.th = thread_self();
			dlist_init(&sender.dlist_item);
			dlist_add_next(&sender.dlist_item, &queue->senders);

			spin_unlock_ipl(&queue->lock, ipl);

			SCHED_WAIT(queue->state != MSG_QUEUE_FULL);

			ipl = spin_lock_ipl(&queue->lock);

			if (queue->state == MSG_QUEUE_DELETED) {
				ret = SET_ERRNO(EIDRM);
				goto out;
			}
		}
	}

out:
	spin_unlock_ipl(&queue->lock, ipl);

	return ret;
}
