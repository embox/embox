/**
 * @file
 * @brief
 *
 * @date 21.10.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <kernel/manual_event.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/io_sync.h>
#include <poll.h>

static int test_and_set_fds(struct pollfd fds[], nfds_t nfds,
		struct manual_event *m_event) {
	int fds_cnt;
	nfds_t i;
	struct idx_desc *fd_desc;
	struct io_sync *fd_ios;

	fds_cnt = 0;
	for (i = 0; i < nfds; ++i) {
		fds[i].revents = 0;
		if (fds[i].fd < 0) {
			continue;
		}

		fd_desc = task_self_idx_get(fds[i].fd);
		if (fd_desc == NULL) {
			fds[i].revents |= POLLNVAL;
			++fds_cnt;
			continue;

		}

		assert(task_idx_indata(fd_desc) != NULL);
		fd_ios = task_idx_indata(fd_desc)->ios;
		assert(fd_ios != NULL);

		if (fds[i].events & POLLIN) {
			io_sync_notify(fd_ios, IO_SYNC_READING, m_event);
		}
		if (fds[i].events & POLLOUT) {
			io_sync_notify(fd_ios, IO_SYNC_WRITING, m_event);
		}

		if (io_sync_ready(fd_ios, IO_SYNC_READING)) {
			fds[i].revents |= POLLIN;
		}
		if (io_sync_ready(fd_ios, IO_SYNC_WRITING)) {
			fds[i].revents |= POLLOUT;
		}

		fds[i].revents &= fds[i].events;
		fds_cnt += !!fds[i].revents;
	}

	return fds_cnt;
}

int poll(struct pollfd fds[], nfds_t nfds, int timeout) {
	int ret;
	struct manual_event wait_on;

	manual_event_init(&wait_on, 0);

	if ((0 == timeout)
			|| (0 != test_and_set_fds(fds, nfds, &wait_on))) {
		return test_and_set_fds(fds, nfds, NULL);
	}

	ret = manual_event_wait(&wait_on,
			timeout > 0 ? timeout : MANUAL_EVENT_TIMEOUT_INFINITE);
	if ((ret != 0) && (ret != -ETIMEDOUT)) {
		test_and_set_fds(fds, nfds, NULL);
		SET_ERRNO(-ret);
		return -1;
	}

	return test_and_set_fds(fds, nfds, NULL);
}
