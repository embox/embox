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
#include <hal/clock.h>

#include <sys/time.h>
#include <sys/select.h>

#include <kernel/manual_event.h>
#include <kernel/time/time.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

/**
 * @brief Save only descriptors with active op.
 * */
static int filter_out_with_op(int nfds, fd_set *set, enum io_sync_op op, int update);

/** @brief Search for active descriptors in all sets.
 * @param update - show if we filter out inactive fds or only count them.
 * @return count of active descriptors
 * @retval -EBAFD if some descriptor is invalid */
static int filter_out(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfd, int update);

static int set_monitoring(int nfds, fd_set *set, enum io_sync_op op, struct manual_event *m_event);

static int unset_monitoring(int nfds, fd_set *set, enum io_sync_op op);

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int fd_cnt;
	struct manual_event wait_on;
	clock_t ticks;

	fd_cnt = 0;
	ticks = (timeout == NULL ? MANUAL_EVENT_TIMEOUT_INFINITE : timeval_to_ms(timeout));
	manual_event_init(&wait_on, 0);

	set_monitoring(nfds, readfds, IO_SYNC_READING, &wait_on);
	set_monitoring(nfds, writefds, IO_SYNC_WRITING, &wait_on);

	/* Search active descriptor and build wait_on set.
	 * First try to find some active descriptor before going to sleep */
	fd_cnt = filter_out(nfds, readfds, writefds, exceptfds, 0);

	if (fd_cnt < 0) {
		SET_ERRNO(-fd_cnt);
		fd_cnt = -1;
		goto out;
	} else if (fd_cnt > 0 || !ticks) {
		/* We know there are some active descriptors in three sets.
		 * So we can simply update the sets and return fd_cnt. */
		fd_cnt = filter_out(nfds, readfds, writefds, exceptfds, 1);
		goto out;
	}

	manual_event_wait(&wait_on, ticks);

	fd_cnt = filter_out(nfds, readfds, writefds, exceptfds, 1);

out:
	unset_monitoring(nfds, readfds, IO_SYNC_READING);
	unset_monitoring(nfds, writefds, IO_SYNC_WRITING);
	return fd_cnt;
}

/* Suppose that set != NULL */
static int filter_out_with_op(int nfds, fd_set *set, enum io_sync_op op, int update) {
	int fd, fd_cnt;
	struct idx_desc *desc;

	fd_cnt = 0;

	for (fd = 0; fd < nfds; fd++) {
		if (FD_ISSET(fd, set)) {
			if (!(desc = task_self_idx_get(fd))) {
				return -EBADF;
			} else {
				if (io_sync_ready(task_idx_indata(desc)->ios, op)) {
					fd_cnt++;
				} else {
					/* Filter out inactive descriptor and unset corresponding monitor. */
					if (update) {
						io_sync_notify(task_idx_indata(desc)->ios, op, NULL);
						FD_CLR(fd, set);
					}
				}
			}
		}
	}

	return fd_cnt;
}

static int filter_out(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfd, int update) {
	int fd_cnt, res;

	fd_cnt = 0;

	/* FIXME process exception conditions */
	if (exceptfd) {
		if (update) {
			FD_ZERO(exceptfd);
		}
	}

	/* Try to find active fd in readfds*/
	if (readfds != NULL) {
		res = filter_out_with_op(nfds, readfds, IO_SYNC_READING , update);
		if (res < 0) {
			return -EBADF;
		} else {
			fd_cnt += res;
		}
	}

	/* Try to find active fd in writefds*/
	if (writefds != NULL) {
		res = filter_out_with_op(nfds, writefds, IO_SYNC_WRITING, update);
		if (res < 0) {
			return -EBADF;
		} else {
			fd_cnt += res;
		}
	}

	return fd_cnt;
}

static int set_monitoring(int nfds, fd_set *set, enum io_sync_op op,
		struct manual_event *m_event) {
	int fd;
	struct idx_desc *desc;

	for (fd = 0; fd < nfds; fd++) {
		if (FD_ISSET(fd, set)) {
			if (task_self_idx_get(fd)) {
				if (NULL == (desc = task_self_idx_get(fd))) {
					return -1;
				}
				io_sync_notify(task_idx_indata(desc)->ios, op, m_event);
			}
		}
	}

	return 0;
}

static int unset_monitoring(int nfds, fd_set *set, enum io_sync_op op) {
	int fd;
	struct idx_desc *desc;

	for (fd = 0; fd < nfds; fd++) {
		if (FD_ISSET(fd, set)) {
			if (NULL == (desc = task_self_idx_get(fd))) {
				return -1;
			}
			io_sync_notify(task_idx_indata(desc)->ios, op, NULL);
		}
	}

	return 0;
}
