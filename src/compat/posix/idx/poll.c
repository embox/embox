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

#include <fs/index_descriptor.h>
#include <fs/idesc.h>
#include <fs/flags.h>

#include <kernel/task.h>
//#include <kernel/task/idx.h>
//#include <kernel/task/io_sync.h>

#include <fs/poll_table.h>


static int table_prepare(struct idesc_poll_table *pt, struct pollfd fds[],
		nfds_t nfds) {
	int i;
	struct idesc *idesc;
	int cnt = 0;
	int poll_mask;

	for (i = 0; i < nfds; ++i) {
		fds[i].revents = 0;

		if (!idesc_index_valid(fds[i].fd)) {
			continue;
		}

		idesc = index_descriptor_get(fds[i].fd);
		if (idesc == NULL) {
			fds[i].revents |= POLLNVAL;
			pt->idesc_poll[cnt].idesc = NULL;
			cnt++;
			continue;
		}

		if ((fds[i].events & POLLIN) && (idesc->idesc_amode & FS_MAY_READ)) {
			poll_mask = POLLIN;
		}

		if ((fds[i].events & POLLOUT) && (idesc->idesc_amode & FS_MAY_WRITE)) {
			poll_mask |= POLLOUT;
		}

		if ((fds[i].events & POLLERR)) {
			poll_mask |= POLLERR;
		}

		if (poll_mask) {
			pt->idesc_poll[cnt].idesc = idesc;
			pt->idesc_poll[cnt].i_poll_mask = poll_mask;
			pt->idesc_poll[cnt].o_poll_mask = 0;

			pt->idesc_poll[cnt].fd = fds[i].fd;
			cnt++;
		}
	}
	pt->size = cnt;

	return cnt;
}
static int fds_setup(struct idesc_poll_table *pt, struct pollfd fds[],
		nfds_t nfds) {
	int i;
	int cnt = 0;

	for (i = 0; (i < nfds) && (i < pt->size); ++i) {
		if (!idesc_index_valid(fds[i].fd)) {
			continue;
		}

		if (pt->idesc_poll[i].fd != fds[i].fd) {
			cnt++;//TODO
			continue;
		}

		if (pt->idesc_poll[i].o_poll_mask) {
			cnt++;
			fds[i].revents = (short) pt->idesc_poll[i].o_poll_mask;
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

	fd_cnt = poll_table_count(&pt);
	fd_cnt = fds_setup(&pt, fds, nfds);

	if (ticks == 0) {
		/* If  both  fields  of  the  timeval  stucture  are zero, then select()
		 *  returns immediately.
		 */
		return fd_cnt;
	}

	if (0 != fd_cnt) {
		return fd_cnt;
	}

	ret = poll_table_wait(&pt, ticks);

	fd_cnt = poll_table_cleanup(&pt);
	fd_cnt = poll_table_count(&pt);

	fd_cnt = fds_setup(&pt, fds, nfds);

	if (ret == -EINTR) {
		SET_ERRNO(EINTR);
		return -1;
	}

	return fd_cnt;
}
