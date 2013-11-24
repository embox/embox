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

static int select_wait(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, clock_t ticks) {
	//struct wait_link wait_link;

	//wait_queue_prepare(&wait_link);
	return 0;
}


int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int fd_cnt;
	clock_t ticks;

	fd_cnt = 0;
	/* If  timeout is NULL (no timeout), select() can block indefinitely. */
	ticks = (timeout == NULL ? SCHED_TIMEOUT_INFINITE : timeval_to_ms(timeout));

	fd_cnt = select_count_descriptors(nfds, readfds, writefds, exceptfds);

	if (ticks == 0) {
		/* If  both  fields  of  the  timeval  stucture  are zero, then select()
		 *  returns immediately.
		 */
		return fd_cnt;
	}

	if (0 != fd_cnt) {
		return fd_cnt;
	}
	select_wait(nfds, readfds, writefds, exceptfds, ticks);

	fd_cnt = select_count_descriptors(nfds, readfds, writefds, exceptfds);

	return fd_cnt;
}

