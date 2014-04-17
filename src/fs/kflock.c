/**
 * @file
 * @brief
 *
 * @date Apr 3, 2014
 * @author Anton Bondarev
 */



/*
 * Below fcntl locking implementation and help functions
 */
#include <kernel/spinlock.h>
#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <fs/file_desc.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <sched.h>
#include <kernel/spinlock.h>

/*
#define MAX_KFLOCK_SHARED_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(kflock_shared_pool, kflock_shared_t, 128);
#define MAX_KFLOCK_EXCLUSIVE_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(kflock_exclusive_pool, kflock_exclusive_t, 128);
*/
//#define MAX_KFLOCK_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(kflock_lock_pool, kflock_lock_t, 128);

enum {
	FLOCK_SKIP_EXCLUSIVE,
	FLOCK_SKIP_SHARED
};

enum {
	FLOCK_SHARED_INCLUDE,
	FLOCK_SHARED_INCLUDED,
	FLOCK_SHARED_FIT,
	FLOCK_SHARED_INTERSECT_LEFT,
	FLOCK_SHARED_INTERSECT_RIGHT,
	FLOCK_SHARED_BORDER_LEFT,
	FLOCK_SHARED_BORDER_RIGHT,
	FLOCK_EXCLUSIVE_INCLUDE,
	FLOCK_EXCLUSIVE_INCLUDED,
	FLOCK_EXCLUSIVE_FIT,
	FLOCK_EXCLUSIVE_INTERSECT_LEFT,
	FLOCK_EXCLUSIVE_INTERSECT_RIGHT,
	FLOCK_EXCLUSIVE_BORDER_LEFT,
	FLOCK_EXCLUSIVE_BORDER_RIGHT,
	FLOCK_RANGE_NOT_INTERSECT
};

static inline int kflock_init(node_t *node) {
	/* kflock initialization */
	dlist_init(&node->kflock.locks);
	node->kflock.kflock_guard = SPIN_UNLOCKED;

	return ENOERR;
}

static int kflock_lock_get(kflock_t *kflock, struct flock *flock) {
	kflock_lock_t *lock;
	struct thread *current = thread_self();

	lock = pool_alloc(&kflock_lock_pool);
	if (NULL == lock) {
		return -ENOMEM;
	}
	dlist_add_next(dlist_head_init(&lock->kflock_link), &kflock->locks);
	lock->flock.l_len = flock->l_len;
	lock->flock.l_pid = (pid_t) current->task->tid;
	lock->flock.l_start = flock->l_start;
	lock->flock.l_type = flock->l_type;
	lock->flock.l_whence = flock->l_whence;
	wait_queue_init(&lock->wq);

	return -ENOERR;
}

/* Put certain shared flock if provided or
 * all shared locks of current thread if NULL */
static int kflock_lock_put(kflock_t *kflock, struct flock *flock) {
	kflock_lock_t *lock;
	struct thread *current = thread_self();

	dlist_foreach_entry(lock, &kflock->locks, kflock_link) {
		if (current->task->tid == lock->flock.l_pid) {
			if (NULL == flock || (lock->flock.l_start == flock->l_start &&
					lock->flock.l_len == flock->l_len)) {
				dlist_del(&lock->kflock_link);
				wait_queue_notify_all(&lock->wq);
				pool_free(&kflock_lock_pool, lock);
				/* Returning success if we are searching for one lock
				 * and it was found */
				if (NULL != flock)
					return -ENOERR;
			}
		}
	}
	/* If we releasing all locks return -ENOERR in other case if we are here
	 * it means that lock to release was not found and returning -EINVAL */
	if (NULL == flock) {
		return -ENOERR;
	} else {
		return -EINVAL;
	}
}

/* Return disposition of flock compare to locks in kflock */
static int kflock_get_disposition(int *intersection, kflock_t *kflock,
		struct flock *flock, kflock_lock_t *existing_kflock, unsigned int skip)
{
	kflock_lock_t *lock;
	off_t lock_end, flock_end = flock->l_start + flock->l_len;
	off_t delta_start, delta_end;

	*intersection = -1;

	dlist_foreach_entry(lock, &kflock->locks, kflock_link) {
		lock_end = lock->flock.l_start + lock->flock.l_len;
		delta_start = lock->flock.l_start - flock->l_start;
		delta_end = lock_end - flock_end;

		if (skip--)
			continue;

		if (F_WRLCK == lock->flock.l_type) {
			if (0 == delta_start && 0 == delta_end) {
				*intersection = FLOCK_EXCLUSIVE_FIT;
				break;
			}
			if (delta_start <= 0 && delta_end >= 0) {
				*intersection = FLOCK_EXCLUSIVE_INCLUDE;
				break;
			}
			if (delta_start >= 0 && delta_end <= 0) {
				*intersection = FLOCK_EXCLUSIVE_INCLUDED;
				break;
			}
			if (delta_start == lock->flock.l_len) {
				*intersection = FLOCK_EXCLUSIVE_BORDER_RIGHT;
				break;
			}
			if (delta_end == -lock->flock.l_len) {
				*intersection = FLOCK_EXCLUSIVE_BORDER_LEFT;
				break;
			}
			if (delta_start < 0 && delta_end <= 0) {
				*intersection = FLOCK_EXCLUSIVE_INTERSECT_LEFT;
				break;
			}
			if (delta_start >= 0 && delta_end > 0) {
				*intersection = FLOCK_EXCLUSIVE_INTERSECT_RIGHT;
				break;
			}
		}

		if (F_RDLCK == lock->flock.l_type) {
			if (0 == delta_start && 0 == delta_end) {
				*intersection = FLOCK_SHARED_FIT;
				break;
			}
			if (delta_start <= 0 && delta_end >= 0) {
				*intersection = FLOCK_SHARED_INCLUDE;
				break;
			}
			if (delta_start >= 0 && delta_end <= 0) {
				*intersection = FLOCK_SHARED_INCLUDED;
				break;
			}
			if (delta_start == lock->flock.l_len) {
				*intersection = FLOCK_SHARED_BORDER_RIGHT;
				break;
			}
			if (delta_end == -lock->flock.l_len) {
				*intersection = FLOCK_SHARED_BORDER_LEFT;
				break;
			}
			if (delta_start < 0 && delta_end <= 0) {
				*intersection = FLOCK_SHARED_INTERSECT_LEFT;
				break;
			}
			if (delta_start >= 0 && delta_end > 0) {
				*intersection = FLOCK_SHARED_INTERSECT_RIGHT;
				break;
			}
		}
	}
	/* If intersection has been found then copy intersect lock and return */
	if (-1 != *intersection) {
		memcpy(existing_kflock, lock, sizeof(*existing_kflock));
	} else {
		*intersection = FLOCK_RANGE_NOT_INTERSECT;
	}

	return -ENOERR;
}

static inline int kflock_validate_and_fix(struct flock *flock, int fd) {
	int file_size, cur_offset;
	struct idx_desc *idesc;
	struct stat finfo;
	short fltype = flock->l_type;

	if (-1 == fstat(fd, &finfo))
		return errno;
	file_size = finfo.st_blksize * finfo.st_blocks;

	/* Make l_start to point to flock from the start of the file */
	switch (flock->l_whence) {
	case SEEK_CUR:
		idesc = task_self_idx_get(fd);
		cur_offset = ((struct file_desc *) idesc->data->fd_struct)->cursor;
		flock->l_start += cur_offset;
		flock->l_whence = SEEK_SET;
		break;
	case SEEK_END:
		flock->l_start = file_size - flock->l_start;
		flock->l_whence = SEEK_SET;
		break;
	}

	/* Validate l_start */
	if (flock->l_start < 0 || flock->l_start > file_size)
		return -EINVAL;

	/* Convert length to positive if negative or zero value was provided
	 * or cut it until the end of the file if it is too big */
	if (0 == flock->l_len || ((flock->l_len + flock->l_start) > file_size))
		flock->l_len = file_size - flock->l_start;
	if (flock->l_len < 0) {
		flock->l_start -= flock->l_len;
		flock->l_len = -flock->l_len;
	}

	/* Validate type of the lock */
	switch (fltype) {
	case F_RDLCK:
	case F_WRLCK:
	case F_UNLCK:
		break;
	default:
		return -EINVAL;
	}

	return -ENOERR;
}

static inline int kflock_validate_cmd(int cmd) {
		switch (cmd) {
		case F_SETLK:
		case F_SETLKW:
		case F_GETLK:
			return -ENOERR;
		}
	return -EINVAL;
}

/* Split existing flock by flock
 * Should be called within critical section */
static int kflock_lock_cut(kflock_t *kflock, struct flock *existing_flock,
		struct flock *flock) {
	int rc;
	struct flock part1;
	struct flock part2;
	struct thread *current = thread_self();

	/* Check if existing lock owned by current process */
	if (current->task->tid != existing_flock->l_pid)
		return -EACCES;

	part1.l_len = flock->l_start - existing_flock->l_start;
	part1.l_pid = existing_flock->l_pid;
	part1.l_start = existing_flock->l_start;
	part1.l_type = existing_flock->l_type;
	part1.l_whence = existing_flock->l_whence;

	part2.l_len = (existing_flock->l_start + existing_flock->l_len) - \
			(flock->l_start + flock->l_len);
	part2.l_pid = existing_flock->l_pid;
	part2.l_start = (existing_flock->l_start + existing_flock->l_len) - \
			part2.l_len;
	part2.l_type = existing_flock->l_type;
	part2.l_whence = existing_flock->l_whence;

	rc = kflock_lock_put(kflock, existing_flock);
	if (-ENOERR != rc)
		return rc;

	if (part1.l_len > 0) {
		rc = kflock_lock_get(kflock, &part1);
		if (-ENOERR != rc)
			return rc;
	}
	if (part2.l_len > 0) {
		rc = kflock_lock_get(kflock, &part2);
		if (-ENOERR != rc)
			return rc;
	}

	return -ENOERR;
}

/* Combine existing lock with flock
 * Should be called within critical section */
static int kflock_lock_combine(kflock_t *kflock, struct flock *existing_flock,
		struct flock *flock) {
	int rc;
	off_t combined_start, combined_len;
	struct flock combined_flock;
	struct thread *current = thread_self();

	/* Should be called only for locks of one type */
	assert(existing_flock->l_type == flock->l_type);

	/* Check if existing lock owned by current process */
	if (current->task->tid != existing_flock->l_pid)
		return -EACCES;

	combined_start = existing_flock->l_start <= flock->l_start ?
			existing_flock->l_start : flock->l_start;
	combined_len = (existing_flock->l_start + existing_flock->l_len) >=
			(flock->l_start + flock->l_len) ?
					(existing_flock->l_start + existing_flock->l_len) :
					(flock->l_start + flock->l_len);

	combined_flock.l_start = combined_start;
	combined_flock.l_len = combined_len;
	combined_flock.l_pid = existing_flock->l_pid;
	combined_flock.l_type = existing_flock->l_type;
	combined_flock.l_whence = existing_flock->l_whence;

	/* Modify provided flock to be combined */
	memcpy(flock, &combined_flock, sizeof(*flock));

	rc = kflock_lock_put(kflock, existing_flock);
	if (-ENOERR != rc)
		return rc;

	return -ENOERR;
}

/* Handle F_GETLK command */
static int kflock_handle_getlk(kflock_t *kflock, struct flock *flock) {
	int rc;
	int intersection = -1;
	unsigned int skip_locks = 0;
	kflock_lock_t existing_kflock;

	/* Enter critical section */
	spin_lock(&kflock->kflock_guard);

	do {
		/* If both lock shared they shouldn't affect each other, so we need
		 * to find intersection with exclusive locks only */
		rc = kflock_get_disposition(&intersection, kflock, flock,
				&existing_kflock, skip_locks);
		skip_locks++;
	} while (FLOCK_RANGE_NOT_INTERSECT != intersection ||
			(flock->l_type == F_RDLCK &&
					existing_kflock.flock.l_type == F_RDLCK));

	if (-ENOERR != rc) {
		spin_unlock(&kflock->kflock_guard);
		return rc;
	}

	if (FLOCK_RANGE_NOT_INTERSECT == intersection) {
		flock->l_type = F_UNLCK;
	} else {
		memcpy(flock, &existing_kflock.flock, sizeof(*flock));
	}

	/* Exit critical section */
	spin_lock(&kflock->kflock_guard);

	return -EINVAL;
}

/* Perform all operations needed to get new exclusive lock */
/*
 * TODO: Reduce following function by creating separate functions for repeated
 * parts of code
 */
static int kflock_handle_lock(kflock_t *kflock, struct flock *flock, int cmd) {
	int rc;
	int intersection = -1;
	unsigned int skip_locks = 0;
	kflock_lock_t existing_kflock;
	struct thread *current = thread_self();

	if (cmd == F_GETLK)
		return kflock_handle_getlk(kflock, flock);

	/* Enter critical section */
	spin_lock(&kflock->kflock_guard);

	while (FLOCK_RANGE_NOT_INTERSECT != intersection) {
		rc = kflock_get_disposition(&intersection, kflock, flock,
				&existing_kflock, skip_locks);
		if (-ENOERR != rc) {
			spin_unlock(&kflock->kflock_guard);
			return rc;
		}

		switch(intersection) {
		case FLOCK_EXCLUSIVE_INCLUDE:
		case FLOCK_EXCLUSIVE_FIT:
			if (current->task->tid == existing_kflock.flock.l_pid) {
				/* Exclusive lock within current process include of fit */
				if (F_WRLCK == flock->l_type) {
					/* Lock we try to acquire is exclusive.
					 * Since lock like this or larger is already acquired
					 * we just need to return ENOERR */
					spin_unlock(&kflock->kflock_guard);
					return -ENOERR;
				} else {
					/* Lock we try to acquire is shared.
					 * We are putting shared lock held by current process to
					 * make it possible to acquire exclusive instead */
					rc = kflock_lock_put(kflock, &existing_kflock.flock);
					if (-ENOERR != rc) {
						spin_unlock(&kflock->kflock_guard);
						return rc;
					}
				}
			} else {
				/* Exclusive lock within another process include of fit.
				 * We are waiting or returning EAGAIN independent on
				 * type of lock we try to acquire */
				if (cmd & F_SETLK) {
					spin_unlock(&kflock->kflock_guard);
					return -EAGAIN;
				} else if (cmd & F_SETLKW) {
					spin_unlock(&kflock->kflock_guard);
					/* FIXME what if context switch occur here and below
					 * at the similar code points */
					wait_queue_wait(&existing_kflock.wq,
							SCHED_TIMEOUT_INFINITE);
					spin_lock(&kflock->kflock_guard);
				}
			}
			continue;

		case FLOCK_EXCLUSIVE_INCLUDED:
		case FLOCK_EXCLUSIVE_INTERSECT_LEFT:
		case FLOCK_EXCLUSIVE_INTERSECT_RIGHT:
		case FLOCK_EXCLUSIVE_BORDER_LEFT:
		case FLOCK_EXCLUSIVE_BORDER_RIGHT:
			if (current->task->tid == existing_kflock.flock.l_pid) {
				/* Intersection with exclusive lock within current process */
				if (F_WRLCK == flock->l_type) {
					/* If both locks are exclusive then combine them into one */
					rc = kflock_lock_combine(kflock, &existing_kflock.flock,
							flock);
				} else {
					/* If intersect lock is shared cut it */
					rc = kflock_lock_cut(kflock, &existing_kflock.flock, flock);
				}
				if (-ENOERR != rc) {
					spin_unlock(&kflock->kflock_guard);
					return rc;
				}
			} else {
				/* Intersection with exclusive lock within another process
				 * We are waiting or returning EAGAIN independent on
				 * type of lock we try to acquire */
				if (cmd & F_SETLK) {
					spin_unlock(&kflock->kflock_guard);
					return -EAGAIN;
				} else if (cmd & F_SETLKW) {
					spin_unlock(&kflock->kflock_guard);
					wait_queue_wait(&existing_kflock.wq,
							SCHED_TIMEOUT_INFINITE);
					spin_lock(&kflock->kflock_guard);
				}
			}
			continue;

		case FLOCK_SHARED_INCLUDE:
		case FLOCK_SHARED_INTERSECT_LEFT:
		case FLOCK_SHARED_INTERSECT_RIGHT:
		case FLOCK_SHARED_BORDER_LEFT:
		case FLOCK_SHARED_BORDER_RIGHT:
			if (current->task->tid == existing_kflock.flock.l_pid) {
				/* Intersection with shared lock within current process */
				if (F_WRLCK == flock->l_type) {
					/* Cut it if we try to acquire exclusive one */
					rc = kflock_lock_cut(kflock, &existing_kflock.flock, flock);
				} else {
					/* Combine it if we try to acquire shared one */
					rc = kflock_lock_combine(kflock, &existing_kflock.flock,
							flock);
				}
				if (-ENOERR != rc) {
					spin_unlock(&kflock->kflock_guard);
					return rc;
				}
			} else {
				/* Intersection with shared lock within another process */
				if (F_WRLCK == flock->l_type) {
					/* We are waiting or returning EAGAIN if lock we try to
					 * acquire is exclusive */
					if (cmd & F_SETLK) {
						spin_unlock(&kflock->kflock_guard);
						return -EAGAIN;
					} else if (cmd & F_SETLKW) {
						spin_unlock(&kflock->kflock_guard);
						wait_queue_wait(&existing_kflock.wq,
								SCHED_TIMEOUT_INFINITE);
						spin_lock(&kflock->kflock_guard);
					}
				} else {
					/* Here if we would like to put shared lock we can
					 * probably do it if there is no other conflicts. So we
					 * need to skip any intersection with this shared lock from
					 * different process and continue to search for conflicting
					 * locks. If no such locks were found then acquire current
					 * shared lock
					 */
					skip_locks++;
				}
			}
			continue;

		case FLOCK_SHARED_INCLUDED:
		case FLOCK_SHARED_FIT:
			if (current->task->tid == existing_kflock.flock.l_pid) {
				/* Shared lock within current process include of fit */
				if (F_WRLCK == flock->l_type) {
					/* Put held lock to replace it with exclusive one */
					rc = kflock_lock_put(kflock, &existing_kflock.flock);
					if (-ENOERR != rc) {
						spin_unlock(&kflock->kflock_guard);
						return rc;
					}
				} else {
					/* Return ENOERR since similar lock is already acquired */
					spin_unlock(&kflock->kflock_guard);
					return -ENOERR;
				}
			} else {
				/* Shared lock within another process include of fit */
				if (F_WRLCK == flock->l_type) {
					/* We are waiting or returning EAGAIN if lock we try to
					 * acquire is exclusive */
					if (cmd & F_SETLK) {
						spin_unlock(&kflock->kflock_guard);
						return -EAGAIN;
					} else if (cmd & F_SETLKW) {
						spin_unlock(&kflock->kflock_guard);
						wait_queue_wait(&existing_kflock.wq,
								SCHED_TIMEOUT_INFINITE);
						spin_lock(&kflock->kflock_guard);
					}
				} else {
					/* Here if we would like to put shared lock we can
					 * probably do it if there is no other conflicts. So we
					 * need to skip any intersection with this shared lock from
					 * different process and continue to search for conflicting
					 * locks. If no such locks were found then acquire current
					 * shared lock
					 */
					skip_locks++;
				}
			}
			continue;
		}
	}

	/* Really get new lock here */
	rc = kflock_lock_get(kflock, flock);
	if (-ENOERR != rc) {
		spin_unlock(&kflock->kflock_guard);
		return rc;
	}

	/* Leave critical section */
	spin_unlock(&kflock->kflock_guard);

	return -ENOERR;
}

static int kflock_handle_unlock(kflock_t *kflock, struct flock *flock,
		int cmd) {
	if (cmd == F_GETLK)
		return kflock_handle_getlk(kflock, flock);

	return kflock_lock_put(kflock, flock);

	return -EINVAL;
}

/**
 * Common fcntl locking implementation
 * Can be used by any fs if it doesn't have own implementation
 * @param File descriptor number
 * @param Operation: F_GETLK, F_SETLK and F_SETLKW
 * @param Structure stuct flock with lock description
 * @return ENOERR if operation succeed or -1 and set errno in other way
 */
int vfs_fcntl_lock(int fd, int cmd, struct flock *flock) {
	int rc;
	kflock_t *kflock;
	struct idx_desc *idesc;
	int perm_flags;

	/* Validate lock */
	if ((rc = kflock_validate_and_fix(flock, fd)) != -ENOERR)
		return rc;

	/* - Find locks and other properties for provided file descriptor number
	 * - fd is validated inside task_self_idx_get */
	idesc = task_self_idx_get(fd);
	kflock = &((struct file_desc *) idesc->data->fd_struct)->node->kflock;
	perm_flags = ((struct file_desc *) idesc->data->fd_struct)->flags;

	/* Check permissions to requested operation */
	if ((!(flock->l_type == F_RDLCK && (perm_flags & FS_MAY_READ))) &&
			(!(flock->l_type == F_WRLCK && (perm_flags & FS_MAY_WRITE))) &&
			(!(flock->l_type == F_UNLCK)))
		return -EPERM;

	/* Jump to requested operation */
	switch(flock->l_type) {
	case F_WRLCK:
	case F_RDLCK:
		return kflock_handle_lock(kflock, flock, cmd);
	case F_UNLCK:
		return kflock_handle_unlock(kflock, flock, cmd);
	}

	return -EINVAL;
}

int vfs_fcntl(int fd, int cmd, ... /* arg */ ) {
	int rc;
	va_list args;
	struct flock *flock;

	if ((rc = kflock_validate_cmd(cmd)) == -ENOERR) {
		/* One of the fcntl locking operations requested */
		va_start(args, cmd);
		flock = va_arg(args, struct flock *);
		va_end(args);

		return vfs_fcntl_lock(fd, cmd, flock);
	}

	return rc;
}
