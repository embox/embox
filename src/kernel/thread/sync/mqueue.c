/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#include <errno.h>

#include <kernel/thread/sched.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/thread/sync/mqueue.h>

static inline int mqueue_busyspace(struct mqueue *q) {
	int res = q->tail - q->head;
	if (res < 0) {
		res += MQUEUE_SIZE;
	}
	return res;
}

static inline int mqueue_freespace(struct mqueue *q) {
	return MQUEUE_SIZE - mqueue_busyspace(q);
}

static inline void mqueue_writec(struct mqueue *q, char c) {
	q->ring_buffer[q->tail] = c;
	q->tail = (q->tail + 1) % MQUEUE_SIZE;
}

static inline char mqueue_readc(struct mqueue *q) {
	char c = q->ring_buffer[q->head];
	q->head = (q->head + 1) % MQUEUE_SIZE;
	return c;
}


void mqueue_init(struct mqueue *q) {
	sleepq_init(&q->sleepq);

	q->head = 0;
	q->tail = 0;
}

int mqueue_send(struct mqueue *q, const char *message, unsigned int len) {
	sched_lock();
	{
		if (MQUEUE_LEN_SIZE + len >= MQUEUE_SIZE) {
			sched_unlock();
			return -EINVAL;
		}

		if (mqueue_busyspace(q) + MQUEUE_LEN_SIZE + len >= MQUEUE_SIZE) {
			sched_unlock();
			return -EBUSY;
		}

		mqueue_writec(q, (len & 0x000000FF) >> 0);
		mqueue_writec(q, (len & 0x0000FF00) >> 8);
		mqueue_writec(q, (len & 0x00FF0000) >> 16);
		mqueue_writec(q, (len & 0xFF000000) >> 24);
		for ( ; len; len--, message++) {
			mqueue_writec(q, *message);
		}

		sched_wake_one(&q->sleepq);
	}
	sched_unlock();

	return ENOERR;
}

unsigned int mqueue_receive(struct mqueue *q, char *message) {
	unsigned int res, len;

	sched_lock();
	{
		if (!mqueue_busyspace(q)) {
			sched_sleep_locked(&q->sleepq, SCHED_TIMEOUT_INFINITE);
		}

		assert(mqueue_busyspace(q));

		len  = ((unsigned int) mqueue_readc(q)) << 0;
		len += ((unsigned int) mqueue_readc(q)) << 8;
		len += ((unsigned int) mqueue_readc(q)) << 16;
		len += ((unsigned int) mqueue_readc(q)) << 24;

		res = len;

		for ( ; len; len--, message++) {
			*message = mqueue_readc(q);
		}
	}
	sched_unlock();

	return res;
}
