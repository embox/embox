/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */

#include <assert.h>
#include <poll.h>
#include <stddef.h>
#include <sys/types.h>

#include <kernel/sched/waitq.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_event.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/poll_table.h>
#include <kernel/thread.h>
#include <kernel/thread/thread_sched_wait.h>

static struct idesc *poll_table_idx2idesc(int idx) {
	return idx < 0 ? NULL : index_descriptor_get(idx);
}

int poll_table_count(struct idesc_poll_table *pt) {
	int cnt = 0;
	int i;
	struct idesc *idesc;
	int poll_mask;

	for (i = 0; i < pt->size; i++) {
		idesc = poll_table_idx2idesc(pt->idesc_poll[i].fd);
		//idesc = pt->idesc_poll[i].idesc;
		/* in poll we must treat wrong descriptors and setup revents in
		 * POLLNVAL value
		 */
		if (idesc == NULL) {
			cnt++;
			continue;
		}

		poll_mask = pt->idesc_poll[i].i_poll_mask;

		assert(idesc->idesc_ops);
		assert(idesc->idesc_ops->status);

		if (idesc->idesc_ops->status(idesc, POLLIN & poll_mask)) {
			pt->idesc_poll[i].o_poll_mask |= POLLIN;
		}
		if (idesc->idesc_ops->status(idesc, POLLOUT & poll_mask)) {
			pt->idesc_poll[i].o_poll_mask |= POLLOUT;
		}
		if (idesc->idesc_ops->status(idesc, POLLERR & poll_mask)) {
			pt->idesc_poll[i].o_poll_mask |= POLLERR;
		}

		if (pt->idesc_poll[i].o_poll_mask) {
			cnt++;
		}
	}

	return cnt;
}

static int poll_table_cleanup(struct idesc_poll_table *pt) {
	int i;
	struct idesc *idesc;

	for (i = 0; i < pt->size; i++) {
		struct idesc_poll *idesc_poll = &pt->idesc_poll[i];

		//assert(idesc_poll->idesc);
		idesc = poll_table_idx2idesc(idesc_poll->fd);
		if (!idesc) {
			continue;
		}
		idesc->idesc_usage_count--;
		if (idesc->idesc_usage_count > 0) {
			idesc_wait_cleanup(idesc, &idesc_poll->wait_link);
		}
		else {
			idesc_close(idesc, idesc_poll->fd);
		}
	}

	return 0;
}

static int poll_table_wait_prepare(struct idesc_poll_table *pt, clock_t ticks) {
	int i;
	struct idesc *idesc;

	for (i = 0; i < pt->size; i++) {
		struct idesc_poll *ip = &pt->idesc_poll[i];

		//assert(ip->idesc);
		idesc = poll_table_idx2idesc(ip->fd);
		if (!idesc) {
			continue;
		}
		idesc->idesc_usage_count++;

		idesc_wait_init(&ip->wait_link, ip->i_poll_mask);
		idesc_wait_prepare(idesc, &ip->wait_link);
	}

	return 0;
}

int poll_table_wait(struct idesc_poll_table *pt, clock_t ticks) {
	int ret = 0;

	threadsig_lock();
	{
		poll_table_wait_prepare(pt, ticks);

		ret = SCHED_WAIT_TIMEOUT(poll_table_count(pt), ticks);

		poll_table_cleanup(pt);
	}
	threadsig_unlock();

	return ret;
}
