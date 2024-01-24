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
#include <poll.h>

#include <hal/clock.h>

#include <kernel/time/time.h>
#include <kernel/sched.h>
#include <kernel/task.h>

#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_event.h>
#include <kernel/task/resource/poll_table.h>
#include <lib/libds/array.h>

static int select_fds2pt(struct idesc_poll_table *pt,
		int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
	struct idesc *idesc;
	int cnt;
	int poll_mask;

	assert(pt);

	cnt = 0;

	for (int i = 0; i < nfds; i++) {

		poll_mask = 0;

		if (readfds && FD_ISSET(i, readfds)) {
			poll_mask |= POLLIN;//IDESC_STAT_READ;
		}
		if (writefds && FD_ISSET(i, writefds)) {
			poll_mask |= POLLOUT;//IDESC_STAT_WRITE;
		}
		if (exceptfds && FD_ISSET(i, exceptfds)) {
			poll_mask |= POLLERR;//IDESC_STAT_EXEPT;
		}

		if (poll_mask) {
			struct idesc_poll *pl;

			if (!idesc_index_valid(i)
					|| (NULL == (idesc = index_descriptor_get(i)))) {
				return SET_ERRNO(EBADF);
			}

			assert(cnt < ARRAY_SIZE(pt->idesc_poll));
			pl = &pt->idesc_poll[cnt++];

			pl->fd = i;
			// pl->idesc = idesc;
			pl->i_poll_mask = poll_mask;
			pl->o_poll_mask = 0;
		}

	}

	pt->size = cnt;

	return cnt;
}

static void select_pt2fds(struct idesc_poll_table *pt,
		fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {

	if (readfds) {
		FD_ZERO(readfds);
	}
	if (writefds) {
		FD_ZERO(writefds);
	}
	if (exceptfds) {
		FD_ZERO(exceptfds);
	}

	for (int i = 0; i < pt->size; i ++) {
		struct idesc_poll *pl;

		pl = &pt->idesc_poll[i];

		if (pl->o_poll_mask) {

			if (readfds && pl->o_poll_mask & POLLIN) {
				FD_SET(pl->fd, readfds);
			}
			if (writefds && pl->o_poll_mask & POLLOUT) {
				FD_SET(pl->fd, writefds);
			}
			if (exceptfds && pl->o_poll_mask & POLLERR) {
				FD_SET(pl->fd, exceptfds);
			}
		}
	}
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	clock_t ticks;
	struct idesc_poll_table pt;
	int ret;

	ret = select_fds2pt(&pt, nfds, readfds, writefds, exceptfds);
	if (0 > ret) {
		return ret;
	}

	ticks = (timeout == NULL ? SCHED_TIMEOUT_INFINITE : timeval_to_ms(timeout));
	ret = poll_table_count(&pt);

	if (ret != 0 || ticks == 0) {
		select_pt2fds(&pt, readfds, writefds, exceptfds);
		return ret;
	}

	ret = poll_table_wait(&pt, ticks);
	if ((ret != 0) && (ret != -ETIMEDOUT)) {
		return SET_ERRNO(-ret);
	}

	ret = poll_table_count(&pt);
	select_pt2fds(&pt, readfds, writefds, exceptfds);
	return ret;
}

