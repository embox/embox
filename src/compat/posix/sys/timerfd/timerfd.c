/**
 * @file
 * @brief Timers that notify via file descriptors.
 *
 * @date 29.03.17
 * @author Kirill Smirenko
 */

#include <sys/timerfd.h>

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <time.h>

#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/printk.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <mem/sysmalloc.h>
#include <timespec_utils.h>

// ----------------------------------------------------------------------------
// Base structures

#define idesc_to_timerfd(desc) ((struct idesc_timerfd *) desc)->timerfd

struct timerfd;

struct idesc_timerfd {
	struct idesc idesc;
	struct timerfd *timerfd;
};

struct timerfd {
	clockid_t clk_id; /** Clock ID of this timer */
	/**
	 * Current setting - initial expiration and interval
	 */
	struct itimerspec curr_value;
	struct mutex mutex; /**< Global timerfd mutex */

	struct idesc_timerfd read_desc; /**< The descriptor of the timer */
};

// ----------------------------------------------------------------------------
// Allocation and deallocation

static struct timerfd *timerfd_alloc(void) {
	struct timerfd *timerfd;

	timerfd = sysmalloc(sizeof(struct timerfd));
	if (!timerfd) {
		return NULL;
	}

	memset(&timerfd->curr_value, 0, sizeof(struct itimerspec));
	mutex_init(&timerfd->mutex);

	return timerfd;
}

static void timerfd_free(struct timerfd *timerfd) {
	sysfree(timerfd);
}

// ----------------------------------------------------------------------------
// Idesc operations

static const struct idesc_ops idesc_timerfd_ops;

static ssize_t timerfd_read(struct idesc *idesc, const struct iovec *iov,
		int cnt) {
	printk("timerfd_read() invoked.\n");
	/*
	struct timerfd *timerfd;
	void *buf;
	uint64_t expirations_count = 0LL;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == sizeof(uint64_t));
	assert(idesc->idesc_ops == &idesc_timerfd_ops);
	assert(idesc->idesc_amode == S_IROTH);

	if (!iov->iov_len) {
		return 0;
	}

	timerfd = idesc_to_timerfd(idesc);
	mutex_lock(&timerfd->mutex);
	mutex_unlock(&timerfd->mutex);

	// return sizeof(uint64_t);
	*/
	// TODO
	return 0;
}

static void timerfd_close(struct idesc *idesc) {
	struct idesc_timerfd *idesc_timerfd;
	struct timerfd *timerfd;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_timerfd_ops);

	idesc_timerfd = (struct idesc_timerfd *)idesc;
	timerfd = idesc_to_timerfd(idesc);

	mutex_lock(&timerfd->mutex);
	idesc_timerfd->idesc.idesc_amode = 0;
	mutex_unlock(&timerfd->mutex);

	timerfd_free(timerfd);
}

static const struct idesc_ops idesc_timerfd_ops = {
		.id_readv = timerfd_read,
		.close = timerfd_close
};

// ----------------------------------------------------------------------------
// Helpers

/*
static int idesc_timerfd_isclosed(struct idesc_timerfd *idesc_timerfd) {
	return idesc_timerfd->idesc.idesc_amode == 0;
}
*/

static struct timerfd *timerfd_find_by_fd(int fd) {
	struct idesc_table *it;
	struct idesc *idesc;

	it = task_resource_idesc_table(task_self());
	assert(it);
	idesc = idesc_table_get(it, fd);
	return idesc_to_timerfd(idesc);
}

static void do_timerfd_gettime(struct timerfd *timerfd,
		struct itimerspec *curr_value) {
	struct timespec *it_value, *it_interval;
	struct timespec current_time, diff;

	assert(curr_value);

	mutex_lock(&timerfd->mutex);
	it_value = &timerfd->curr_value.it_value;
	it_interval = &timerfd->curr_value.it_interval;
	mutex_unlock(&timerfd->mutex);

	// copy timer interval to output
	memcpy(&curr_value->it_interval, it_interval, sizeof(struct timespec));

	// calculate time remaining until next expiration
	clock_gettime(timerfd->clk_id, &current_time);
	timespec_diff(&current_time, it_value, &diff);
	if (timespec_is_non_negative(&diff)) {
		// timer has not expired for the first time
		memcpy(&curr_value->it_value, &diff, sizeof(struct timespec));
	} else if (timespec_is_positive(it_interval)) {
		// timer has expired for the first time
		// seek the nearest expiration time (after one or several intervals)
		do {
			timespec_inc_by(&diff, it_interval);
		} while (!timespec_is_non_negative(&diff));
		// now diff contains the next expiration time;
		// we need to return relative time
		timespec_diff(&current_time, &diff, &curr_value->it_value);
	} else {
		// timer has already expired and been disarmed
		memset(&curr_value->it_value, 0, sizeof(struct timespec));
	}
}

// ----------------------------------------------------------------------------
// Timerfd interface

/**
 * timerfd_create - creates a new timer object, and returns a file descriptor
 * that refers to that timer.
 *
 * @clockid: the clock that is used to mark the progress of the timer
 * @flags: ignored in this version
 */
int timerfd_create(int clockid, int flags) {
	int fd;
	struct idesc_table *it;
	struct timerfd *timerfd;
	int result = 0;

	it = task_resource_idesc_table(task_self());
	assert(it);

	timerfd = timerfd_alloc();
	if (!timerfd) {
		result = ENOMEM;
		goto out_err;
	}

	timerfd->clk_id = clockid;
	idesc_init(&timerfd->read_desc.idesc, &idesc_timerfd_ops, S_IROTH);
	timerfd->read_desc.timerfd = timerfd;

	fd = idesc_table_add(it, &timerfd->read_desc.idesc, 0);
	if (fd < 0) {
		result = ENOMEM;
		goto out_err;
	}

	return fd;

	out_err:
	if (fd >= 0) {
		idesc_table_del(it, fd);
	}
	if (timerfd) {
		timerfd_free(timerfd);
	}

	SET_ERRNO(result);
	return -1;
}

/**
 * timerfd_settime - arms (starts) or disarms (stops) the timer
 *
 * @fd: the file descriptor that refers to the timer
 * @flags: ignored
 * @new_value: specifies the initial expiration and interval for the timer
 * @old_value: if not null, is used to return the setting of the timer that was
 *   current at the time of the call
 */
int timerfd_settime(int fd, int flags, const struct itimerspec *new_value,
		struct itimerspec *old_value) {
	struct timerfd *timerfd;
	struct timespec current_time;

	assert(new_value);
	if (flags & ~TFD_SETTIME_FLAGS) {
		return -1;
	}

	timerfd = timerfd_find_by_fd(fd);
	assert(timerfd);
	clock_gettime(timerfd->clk_id, &current_time);

	if (old_value) {
		do_timerfd_gettime(timerfd, old_value);
	}

	mutex_lock(&timerfd->mutex);
	if (flags & TFD_TIMER_ABSTIME) {
		memcpy(&timerfd->curr_value.it_value, &new_value->it_value,
			sizeof(struct timespec));
	} else {
		timespec_inc_by(&current_time, &new_value->it_value);
		memcpy(&timerfd->curr_value.it_value, &current_time,
			sizeof(struct timespec));
	}
	memcpy(&timerfd->curr_value.it_interval, &new_value->it_interval,
		sizeof(struct timespec));
	mutex_unlock(&timerfd->mutex);

	return 0;
}

/**
 * timerfd_gettime - returns an itimerspec structure that contains
 * the current setting of the timer
 *
 * The it_value field returns the amount of time until the timer will
 * next expire. If both fields of this structure are zero, then the
 * timer is currently disarmed.
 * The it_interval field returns the interval of the timer. If both
 * fields of this structure are zero, then the timer is set to expire
 * just once, at the time specified by curr_value.it_value.
 *
 * @fd: the file descriptor that refers to the timer
 * @curr_value: is used to return the current setting of the timer
 */
int timerfd_gettime(int fd, struct itimerspec *curr_value) {
	struct timerfd *timerfd;

	timerfd = timerfd_find_by_fd(fd);
	assert(timerfd);

	do_timerfd_gettime(timerfd, curr_value);

	return 0;
}
