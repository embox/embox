/**
 * @file
 * @brief
 *
 * @date 21.10.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <poll.h>

#include <fs/idesc.h>

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>

#include <fs/poll_table.h>


static int table_prepare(struct idesc_poll_table *pt, struct pollfd fds[], nfds_t nfds) {
	int i;
	struct idesc *idesc;
	int cnt;

	for (i = 0; i < nfds; ++i) {
		fds[i].revents = 0;
		if (fds[i].fd < 0) {
			continue;
		}

		idesc = idesc_common_get(fds[i].fd);
		if (idesc == NULL) {
			fds[i].revents |= POLLNVAL;
			continue;
		}

		if (fds[i].events & POLLIN) {
			pt->idesc_poll[pt->size].idesc = idesc;
			pt->idesc_poll[pt->size].poll_mask = IDESC_STAT_READ;
			cnt++;
		}

		if (fds[i].events & POLLOUT) {
			pt->idesc_poll[pt->size].idesc = idesc;
			pt->idesc_poll[pt->size].poll_mask = IDESC_STAT_READ;
			cnt++;
		}
	}
	return cnt;
}

int poll(struct pollfd fds[], nfds_t nfds, int timeout) {
	int ret;
	int fd_cnt;
	int ticks;
	struct idesc_poll_table pt;

	ticks = timeout;

	fd_cnt = table_prepare(&pt, fds, nfds);

	if (ticks == 0) {
		/* If  both  fields  of  the  timeval  stucture  are zero, then select()
		 *  returns immediately.
		 */
		return fd_cnt;
	}

	if (0 != fd_cnt) {
		return fd_cnt;
	}

	do {
		poll_table_wait_prepare(&pt, ticks);
		if ((fd_cnt = poll_table_count(&pt))) {
			break;
		}
		ret = __waitq_wait(ticks);
	} while (!ret);

	poll_table_cleanup(&pt);

	if (ret == -EINTR) {
		SET_ERRNO(EINTR);
		return -1;
	}

	fd_cnt = poll_table_cleanup(&pt);

	return fd_cnt;

}
