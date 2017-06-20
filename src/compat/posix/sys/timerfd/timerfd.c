/**
 * @file
 * @brief Timers that notify via file descriptors.
 *
 * @date 29.03.17
 * @author Kirill Smirenko
 */

#include <errno.h>
#include <stddef.h>
#include <sys/stat.h>
#include <time.h>

#include <fs/idesc.h>
#include <fs/idesc_event.h>
#include <kernel/thread/sync/mutex.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
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
	struct itimerspec *timerspec; /**< The initial expiration and interval of the timer */
	struct mutex mutex; /**< Global timerfd mutex */

	struct idesc_timerfd read_desc; /**< The descriptor of the timer */
};

// ----------------------------------------------------------------------------
// Idesc operations

static const struct idesc_ops idesc_timerfd_ops;

static ssize_t timerfd_read(struct idesc *idesc, const struct iovec *iov,
		int cnt) {
	// TODO
	return 0;
}

static void timerfd_close(struct idesc *idesc) {
	// TODO
}

static const struct idesc_ops idesc_timerfd_ops = {
		.id_readv = timerfd_read,
		.close = timerfd_close
};

// ----------------------------------------------------------------------------
// Allocation and deallocation

static struct timerfd *timerfd_alloc(void) {
	struct timerfd *timerfd;

	timerfd = sysmalloc(sizeof(struct timerfd));
	if (!timerfd) {
		return NULL;
	}
	mutex_init(&timerfd->mutex);

	return timerfd;
}

static void timerfd_free(struct timerfd *timerfd) {
	sysfree(timerfd);
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

	idesc_init(&timerfd->read_desc.idesc, &idesc_timerfd_ops, S_IROTH);
	timerfd->read_desc.timerfd = timerfd;

	fd = idesc_table_add(it, &timerfd->read_desc.idesc, flags);
	if (fd < 0) {
		result = ENOMEM;
		goto out_err;
	}

	return 0;

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
	// TODO
	return 42;
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
	// TODO
	return 42;
}
