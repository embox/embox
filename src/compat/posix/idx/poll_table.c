/**
 * @file
 *
 * @date Nov 26, 2013
 * @author: Anton Bondarev
 */
#include <poll.h>

//#include <kernel/task.h>
#include <kernel/sched/waitq.h>
//#include <kernel/task/idx.h>

#include <fs/idesc.h>
#include <fs/poll_table.h>

int poll_table_count(struct idesc_poll_table *pt) {
	int cnt = 0;
	int i;
	struct idesc *idesc;
	int poll_mask;

	for (i = 0; i < pt->size; i++) {
		idesc = pt->idesc_poll[i].idesc;
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

	for (i = 0; i < pt->size; i++) {
		struct idesc_poll *idesc_poll = &pt->idesc_poll[i];
		struct waitq *wq;

		assert(idesc_poll->idesc);

		wq = &idesc_poll->idesc->idesc_waitq;
		waitq_wait_cleanup(wq, &idesc_poll->wait_link.link);
	}

	return 0;
}

static int poll_table_wait_prepare(struct idesc_poll_table *pt, clock_t ticks) {
	int i;

	for (i = 0; i < pt->size; i++) {
		struct idesc_poll *idesc_poll = &pt->idesc_poll[i];

		assert(idesc_poll->idesc);
		idesc_wait_prepare(idesc_poll->idesc, &idesc_poll->wait_link,
				idesc_poll->i_poll_mask);
	}

	return 0;
}

int poll_table_wait(struct idesc_poll_table *pt, clock_t ticks) {
	int ret = 0;

	poll_table_wait_prepare(pt, ticks);

	ret = SCHED_WAIT_TIMEOUT(poll_table_count(pt), ticks);

	poll_table_cleanup(pt);

	return ret;
}
