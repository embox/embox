/**
 * @file
 *
 * @brief
 *
 * @date 13.09.2011
 * @author Anton Bondarev
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>

#include <hal/clock.h>

#include <kernel/time/time.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>


static inline int select_get_mask(int idx, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
	int mask = 0;
	if (FD_ISSET(idx, readfds)) {
		mask |= IDESC_STAT_READ;
	}

	if (FD_ISSET(idx, writefds)) {
		mask |= IDESC_STAT_WRITE;
	}

	if (FD_ISSET(idx, exceptfds)) {
		mask |= IDESC_STAT_EXEPT;
	}

	return mask;
}


struct idesc_poll {
	struct idesc *idesc;
	int poll_mask;
	struct wait_link wait_link;
};

struct idesc_poll_table {
	struct idesc_poll idesc_poll[IDESC_QUANTITY];
	struct wait_link wait_link;
	int size;
};

static int select_count_descriptors(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
	struct idesc *idesc;
	int cnt = 0;

	for (;nfds >= 0; nfds--) {
		idesc = idesc_common_get(nfds);
		assert(idesc);

		if (FD_ISSET(nfds, readfds)) {
			if (idesc->idesc_ops->status(idesc, IDESC_STAT_READ)) {
				cnt++;
			}
		}
		if (FD_ISSET(nfds, writefds)) {
			if (idesc->idesc_ops->status(idesc, IDESC_STAT_WRITE)) {
				cnt++;
			}
		}
		if (FD_ISSET(nfds, exceptfds)) {
			if (idesc->idesc_ops->status(idesc, IDESC_STAT_EXEPT)) {
				cnt++;
			}
		}
	}

	return cnt;
}

static int select_wait(struct idesc_poll_table *pt, clock_t ticks) {
	int i;

	for (i = 0; i < pt->size; pt->size++) {
		struct idesc *idesc;
		struct idesc_wait_link *waitl;
		int poll_mask;

		idesc = pt->idesc_poll[i].idesc
		waitl = &pt->idesc_poll[i].wait_link;
		poll_mask = pt->idesc_poll[i].poll_mask;

		idesc_wait_prepare(idesc, waitl, poll_mask);
	}

	//wait_queue_prepare(&pt->wait_link)
	//sched_prepare_wait()

	return 0;
}



static int select_table_prepare(struct idesc_poll_table *pt,
		int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
	struct idesc *idesc;

	assert(pt);

	for (pt->size = 0;nfds >= 0; nfds--, pt->size++) {
		idesc = idesc_common_get(nfds);
		assert(idesc);

		if (FD_ISSET(nfds, readfds)) {
			pt->idesc_poll[pt->size].idesc = idesc;
			pt->idesc_poll[pt->size].poll_mask = IDESC_STAT_READ;
		}

		if (FD_ISSET(nfds, writefds)) {
			pt->idesc_poll[pt->size].idesc = idesc;
			pt->idesc_poll[pt->size].poll_mask = IDESC_STAT_WRITE;
		}

		if (FD_ISSET(nfds, exceptfds)) {
			pt->idesc_poll[pt->size].idesc = idesc;
			pt->idesc_poll[pt->size].poll_mask = IDESC_STAT_EXEPT;
		}
	}

	return pt->size;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int fd_cnt;
	clock_t ticks;
	struct idesc_poll_table pt;

	fd_cnt = 0;
	/* If  timeout is NULL (no timeout), select() can block indefinitely. */
	ticks = (timeout == NULL ? SCHED_TIMEOUT_INFINITE : timeval_to_ms(timeout));

	fd_cnt = select_table_prepare(&pt, nfds, readfds, writefds, exceptfds);

	if (ticks == 0) {
		/* If  both  fields  of  the  timeval  stucture  are zero, then select()
		 *  returns immediately.
		 */
		return fd_cnt;
	}

	if (0 != fd_cnt) {
		return fd_cnt;
	}
	select_wait(pt, ticks);

	fd_cnt = select_count_descriptors(nfds, readfds, writefds, exceptfds);

	return fd_cnt;
}

