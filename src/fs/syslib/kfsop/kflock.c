/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <fcntl.h>
#include <sys/file.h>

#include <fs/file_desc.h>
#include <fs/inode.h>

#include <kernel/spinlock.h>
#include <kernel/thread.h>

#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#define MAX_FLOCK_QUANTITY OPTION_GET(NUMBER, flock_quantity)
POOL_DEF(flock_pool, struct flock_shared, MAX_FLOCK_QUANTITY);

static int flock_shared_get(struct node_flock *flock) {
	struct flock_shared *shlock;
	struct thread *current = thread_self();

	shlock = pool_alloc(&flock_pool);
	if (NULL == shlock) {
		return -ENOMEM;
	}
	dlist_add_next(dlist_head_init(&shlock->flock_link), &flock->shlock_holders);
	shlock->holder = current;
	flock->shlock_count++;

	return -ENOERR;
}

static int flock_shared_put(struct node_flock *flock) {
	struct flock_shared *shlock;
	struct thread *current = thread_self();

	dlist_foreach_entry(shlock, &flock->shlock_holders, flock_link) {
		if (current == shlock->holder) {
			dlist_del(&shlock->flock_link);
			pool_free(&flock_pool, shlock);
			flock->shlock_count--;
		}
	}

	return -ENOERR;
}

static inline void flock_exclusive_get(struct mutex *exlock) {
	mutex_lock(exlock);
}

static inline int flock_exclusive_tryget(struct mutex *exlock) {
	return mutex_trylock(exlock);
}

static inline void flock_exclusive_put(struct mutex *exlock) {
	mutex_unlock(exlock);
}

/**
 * Apply advisory lock to specified fd
 * @param File descriptor number
 * @param Operation: one of LOCK_EX, LOCK_SH, LOCK_UN plus possible LOCK_NB
 * @return ENOERR if operation succeed or -1 and set errno in other way
 */
int kflock(int fd, int operation) {
	int rc;
	struct node_flock *flock;
	struct mutex *exlock;
	spinlock_t *flock_guard;
	long *shlock_count;
	struct file_desc *fdesc;
	struct thread *current = thread_self();

	/**
	 * Base algorithm:
	 * - Validate operation
	 * - Validate fd
	 * - Get lock pointer and other preparations
	 * - Determine operation (total 2 x 2 + 1 = 5)
	 *    1. Exclusive lock, blocking
	 *        - If shared block is acquired only by current thread convert it
	 *          to exclusive
	 *        - If shared or exclusive lock is acquired then block
	 *        - Else acquire exclusive lock
	 *    2. Exclusive lock, non-blocking
	 *        - The same as 1 but return EWOULDBLOCK instead of blocking
	 *    3. Shared lock, blocking
	 *        - If exclusive block is acquired by current thread then convert
	 *          it to shared
	 *        - If exclusive lock is acquired then block
	 *        - Else acquire shared lock
	 *    4. Shared lock, non-blocking
	 *        - The same as 3 but return EWOULDBLOCK instead of blocking
	 *    5. Unlock, blocking and non-blocking
	 *        - If any lock is acquired by current thread then remove it
	 *          blocking and non-blocking the same because mutex_unlock
	 *          never blocks thread in current implementation
	 */

	/* Validate operation */
	if (((LOCK_EX | LOCK_SH | LOCK_UN) & operation) != LOCK_EX && \
			((LOCK_EX | LOCK_SH | LOCK_UN) & operation) != LOCK_SH && \
			((LOCK_EX | LOCK_SH | LOCK_UN) & operation) != LOCK_UN)
		return -EINVAL;

	/* - Find locks and other properties for provided file descriptor number
	 * - fd is validated inside task_self_idx_get */
	fdesc = file_desc_get(fd);
	flock = &(fdesc)->f_inode->flock;
	exlock = &flock->exlock;
	shlock_count = &flock->shlock_count;
	flock_guard = &flock->flock_guard;

	/* Exclusive locking operation */
	if (LOCK_EX & operation) {
		spin_lock(flock_guard);
		/* If shared lock is acquired by any thread then free up our locks and
		 * try to acquire exclusive one */
		if (*shlock_count > 0) {
			/* We can hold only one type of lock at the moment */
			assert(0 == exlock->lock_count);
			flock_shared_put(flock);
		}
		if (LOCK_NB & operation) {
			if (-EBUSY == flock_exclusive_tryget(exlock)) {
				spin_unlock(flock_guard);
				SET_ERRNO(EWOULDBLOCK);
				return -1;
			} else {
				spin_unlock(flock_guard);
			}
		} else {
			/* We should unlock flock_guard here to avoid many processes
			 * waiting on spin lock at the enter to this critical section */
			spin_unlock(flock_guard);
			flock_exclusive_get(exlock);
		}
	}

	/* Shared locking operation */
	if (LOCK_SH & operation) {
		spin_lock(flock_guard);
		/* If current thread is holder of exclusive lock then convert it to
		 * shared lock */
		if (exlock->lock_count > 0) {
			if (&current->schedee == exlock->holder) {
				/* Again no two different types of lock can be held
				 * simultaneously */
				assert(0 == *shlock_count);
				/* Exclusive lock can be acquired many times by one thread
				 * that is because of current implementation of mutexes, so
				 * if we converting lock to shared we need to free all of them
				 */
				while (exlock->lock_count != 0) flock_exclusive_put(exlock);
			} else {
				if (LOCK_NB & operation) {
					if (-EBUSY == flock_exclusive_tryget(exlock)) {
						spin_unlock(flock_guard);
						SET_ERRNO(EWOULDBLOCK);
						return -1;
					}
				} else {
					/* The same as for exclusive lock we are unlocking
					 * flock_guard to avoid many processes waiting on
					 * spin lock at the enter to this critical section */
					spin_unlock(flock_guard);
					flock_exclusive_get(exlock);
					spin_lock(flock_guard);
					flock_exclusive_put(exlock);
				}
			}
		}
		/* Acquire shared lock */
		rc = flock_shared_get(flock);
		if (-ENOERR != rc) {
			return rc;
		}
		spin_unlock(flock_guard);
	}

	/* Unlock operation */
	if (LOCK_UN & operation) {
		spin_lock(flock_guard);
		/* Handle exclusive lock free */
		if (exlock->holder == &current->schedee) {
			assert(0 == *shlock_count);
			/* mutex_unlock can't block the thread
			 * so nothing need for LOCK_NB */
			while (exlock->lock_count != 0) flock_exclusive_put(exlock);
		}
		/* Handle shared lock free */
		if (*shlock_count > 0) {
			assert(0 == exlock->lock_count);
			flock_shared_put(flock);
		}
		spin_unlock(flock_guard);
	}

	return ENOERR;
}
