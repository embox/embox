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
#include <fs/idesc_event.h>
#include <fs/poll_table.h>


static int select_table_prepare(struct idesc_poll_table *pt,
		int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds) {
	struct idesc *idesc;
	int cnt;

	assert(pt);

	for (cnt = 0;nfds >= 0; nfds--, cnt++) {
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

	pt->size = cnt;

	return cnt;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int fd_cnt;
	clock_t ticks;
	struct idesc_poll_table pt;
	int ret;

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

