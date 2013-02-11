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

#include <kernel/thread/event.h>
#include <kernel/thread/sched_lock.h>
#include <kernel/time/time.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

/**
 * @brief Save only descriptors with active op.
 * */
static int filter_out_with_op(int nfds, fd_set *set, char op, int update);

/** @brief Search for active descriptors in all sets.
 * @param update - show if we filter out inactive fds or only count them.
 * @return count of active descriptors
 * @retval -EBAFD if some descriptor is invalid */
static int filter_out(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfd, int update);

/**
 * @brief Link each desc in fd_set to event.
 */
static void set_event_for_fds(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfd, struct event *event);

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int fd_cnt;
	struct event event;
	clock_t ticks;

	fd_cnt = 0;
	ticks = (timeout == NULL ? EVENT_TIMEOUT_INFINITE : ns2jiffies(timeval_to_ns(timeout)));

	sched_lock();

	/* Install event on each descriptor to be notified if
	 * some of them can to perform corresponding operations (read/write) */
	event_init(&event, "select_event");
	set_event_for_fds(nfds, readfds, writefds, exceptfds, &event);

	/* Search active descriptor and build event set.
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

	event_wait_ms(&event, ticks);

	fd_cnt = filter_out(nfds, readfds, writefds, exceptfds, 1);

out:
	sched_unlock();
	/* Unset event for all remaining fds */
	set_event_for_fds(nfds, readfds, writefds, exceptfds, NULL);
	return fd_cnt;
}

/* Suppose that set != NULL */
static int filter_out_with_op(int nfds, fd_set *set, char op, int update) {
	int fd, fd_cnt;
	struct idx_desc *desc;
	struct idx_io_op_state *state;

	fd_cnt = 0;

	for (fd = 0; fd < nfds; fd++) {
		if (FD_ISSET(fd, set)) {
			if (!(desc = task_self_idx_get(fd))) {
				return -EBADF;
			} else {
				state = op == 'r' ? &desc->data->read_state
						: &desc->data->write_state;
				if (state->can_perform_op) {
					fd_cnt++;
				} else {
					/* Filter out inactive descriptor and unset corresponding event.*/
					if (update) {
						desc->data->read_state.unblock = NULL;
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

	/* Try to find active fd in readfds*/
	if (readfds != NULL) {
		res = filter_out_with_op(nfds, readfds, 'r', update);
		if (res < 0) {
			return -EBADF;
		} else {
			fd_cnt += res;
		}
	}

	/* Try to find active fd in writefds*/
	if (writefds != NULL) {
		res = filter_out_with_op(nfds, writefds, 'w', update);
		if (res < 0) {
			return -EBADF;
		} else {
			fd_cnt += res;
		}
	}

	return fd_cnt;
}

static void set_event_for_fds(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfd, struct event *event) {
	int fd;
	struct idx_desc *desc;

	for (fd = 0; fd < nfds; fd++) {
		if (readfds && FD_ISSET(fd, readfds)) {
			desc = task_self_idx_get(fd);
			desc->data->read_state.unblock = event;
		}

		if (writefds && FD_ISSET(fd, writefds)) {
			desc = task_self_idx_get(fd);
			desc->data->write_state.unblock = event;
		}
	}
}
