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
#include <unistd.h>
#include <fcntl.h>

#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/printk.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/time/time.h>
#include <mem/sysmalloc.h>

// ----------------------------------------------------------------------------
// Base structures

#define idesc_to_timerfd(desc) ((struct idesc_timerfd *) desc)->timerfd

struct timerfd;

struct idesc_timerfd {
	struct idesc idesc;
	struct timerfd *timerfd;
};

struct timerfd {
	clockid_t clk_id; /**< Clock ID of this timer */
	time64_t expiration;
	time64_t interval;
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
	timerfd->expiration = 0LL;
	timerfd->interval = 0LL;
	mutex_init(&timerfd->mutex);

	return timerfd;
}

static void timerfd_free(struct timerfd *timerfd) {
	sysfree(timerfd);
}

// ----------------------------------------------------------------------------
// Helpers

static struct timerfd *timerfd_find_by_fd(int fd) {
	struct idesc_table *it;
	struct idesc *idesc;

	it = task_resource_idesc_table(task_self());
	assert(it);
	idesc = idesc_table_get(it, fd);
	return idesc_to_timerfd(idesc);
}

static void timerfd_rearm(struct timerfd *timerfd, time64_t expiration,
		time64_t interval) {
	timerfd->expiration = expiration;
	timerfd->interval = interval;
}

static void timerfd_disarm(struct timerfd *timerfd) {
	timerfd->expiration = 0LL;
	timerfd->interval = 0LL;
}

// ----------------------------------------------------------------------------
// Idesc operations

static const struct idesc_ops idesc_timerfd_ops;

static ssize_t timerfd_read(struct idesc *idesc, const struct iovec *iov,
		int cnt) {
	struct timerfd *timerfd;
	void *buf;
	struct timespec ts_now;
	int error_code;
	time64_t now, remaining, next_expiration;
	uint64_t expirations_count = 0LL;
	size_t read_size = sizeof(uint64_t);

	assert(iov);
	assert(idesc->idesc_ops == &idesc_timerfd_ops);
	assert((idesc->idesc_flags & O_ACCESS_MASK) != O_WRONLY);
	buf = iov->iov_base;

	if (iov->iov_len < sizeof(uint64_t)) {
		return -EINVAL;
	}
	timerfd = idesc_to_timerfd(idesc);

	mutex_lock(&timerfd->mutex);
	if (!timerfd->expiration && !timerfd->interval) {
		// disarmed; return 0
		error_code = 0;
		goto out_err;
	}

	clock_gettime(timerfd->clk_id, &ts_now);
	now = timespec_to_ns(&ts_now);
	remaining = timerfd->expiration - now;

	next_expiration = timerfd->expiration;
	if (remaining > 0) {
		usleep(remaining / USEC_PER_MSEC);
	}

	do {
		next_expiration += timerfd->interval;
		++expirations_count;
	} while (next_expiration < now);

	if (timerfd->interval > 0) {
		timerfd_rearm(timerfd, next_expiration, timerfd->interval);
	} else {
		timerfd_disarm(timerfd);
	}

	mutex_unlock(&timerfd->mutex);
	memcpy(buf, &expirations_count, read_size);
	return read_size;

out_err:
	mutex_unlock(&timerfd->mutex);
	return error_code;
}

static void timerfd_close(struct idesc *idesc) {
	struct timerfd *timerfd;

	assert(idesc);
	assert(idesc->idesc_ops == &idesc_timerfd_ops);

	timerfd = idesc_to_timerfd(idesc);

	timerfd_free(timerfd);
}

static const struct idesc_ops idesc_timerfd_ops = {
		.id_readv = timerfd_read,
		.close = timerfd_close
};

// ----------------------------------------------------------------------------
// Logic of the interface functions

static void do_timerfd_settime(struct timerfd *timerfd,
		const struct itimerspec *new_value, int flags) {
	struct timespec ts_now;
	time64_t now, expiration, interval;

	assert(timerfd);
	assert(new_value);

	clock_gettime(timerfd->clk_id, &ts_now);
	now = timespec_to_ns(&ts_now);

	if (flags & TFD_TIMER_ABSTIME) {
		expiration = timespec_to_ns(&new_value->it_value);
	} else {
		expiration = now + timespec_to_ns(&new_value->it_value);
	}
	interval = timespec_to_ns(&new_value->it_interval);

	mutex_lock(&timerfd->mutex);
	timerfd_rearm(timerfd, expiration, interval);
	mutex_unlock(&timerfd->mutex);
}

static void do_timerfd_gettime(struct timerfd *timerfd,
		struct itimerspec *curr_value) {
	struct timespec out_value, out_interval;
	struct timespec ts_now;
	time64_t now, remaining, next_expiration;

	assert(timerfd);
	assert(curr_value);

	mutex_lock(&timerfd->mutex);
	out_interval = ns_to_timespec(timerfd->interval);
	memcpy(&curr_value->it_interval, &out_interval, sizeof(struct timespec));

	clock_gettime(timerfd->clk_id, &ts_now);
	now = timespec_to_ns(&ts_now);
	remaining = timerfd->expiration - now;

	if (remaining > 0) {
		// armed, not expired
		out_value = ns_to_timespec(remaining);
	} else if (timerfd->interval > 0) {
		// expired but rearmed; seek the nearest expiration time
		next_expiration = timerfd->expiration;
		do {
			next_expiration += timerfd->interval;
		} while (next_expiration < now);
		out_value = ns_to_timespec(next_expiration - now);
	} else {
		// expired and disarmed
		out_value = ns_to_timespec(0);
	}
	memcpy(&curr_value->it_value, &out_value, sizeof(struct timespec));
	mutex_unlock(&timerfd->mutex);
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
	int fd = -1;
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
	idesc_init(&timerfd->read_desc.idesc, &idesc_timerfd_ops, O_RDONLY);
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

	assert(new_value);
	if (flags & ~TFD_SETTIME_FLAGS) {
		return -1;
	}

	timerfd = timerfd_find_by_fd(fd);
	assert(timerfd);

	if (old_value) {
		do_timerfd_gettime(timerfd, old_value);
	}
	do_timerfd_settime(timerfd, new_value, flags);

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
