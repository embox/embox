/**
 * @file
 * @brief flock(2) for DVFS: advisory locks that belong to an open file.
 *
 * @date 19.09.26
 */

#include <errno.h>
#include <sys/file.h>

#include <fs/dvfs.h>
#include <fs/file_desc.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread/waitq.h>
#include <util/atomic_rmw.h>

/* One queue for every lock: a release wakes all waiters and each looks again.
 * Locks are rare and short, not worth a waitq in every pooled inode. */
static struct waitq dvfs_flock_wq = WAITQ_INIT(dvfs_flock_wq);
static unsigned long dvfs_flock_gen;

static int flock_free_for(struct inode *in, struct file_desc *desc, int op) {
	int others_sh = in->i_flock_sh - (desc->f_flock == LOCK_SH ? 1 : 0);

	if (in->i_flock_ex != NULL && in->i_flock_ex != desc) {
		return 0;
	}
	if (op == LOCK_EX && others_sh > 0) {
		return 0;
	}
	return 1;
}

/* Lets go of what DESC holds. Under dvfs_lock. */
static void flock_release_locked(struct inode *in, struct file_desc *desc) {
	if (desc->f_flock == LOCK_EX && in->i_flock_ex == desc) {
		in->i_flock_ex = NULL;
	} else if (desc->f_flock == LOCK_SH) {
		in->i_flock_sh--;
	}
	desc->f_flock = 0;
}

static void flock_changed(void) {
	atomic_rmw_add_fetch(&dvfs_flock_gen, 1, __ATOMIC_RELEASE);
	waitq_wakeup_all(&dvfs_flock_wq);
}

/**
 * @brief Take, convert or drop the advisory lock DESC holds on its file
 *
 * @return 0 or -errno: -EWOULDBLOCK for LOCK_NB when it is taken, -EINVAL
 *         for an operation that is not one, -EINTR when a wait is broken off
 *
 * The lock belongs to the descriptor, not the thread. Asking for the other
 * kind converts; kclose() drops it through dvfs_flock_release().
 */
int dvfs_flock(struct file_desc *desc, int operation) {
	struct inode *in = desc->f_inode;
	int op = operation & (LOCK_EX | LOCK_SH | LOCK_UN);
	unsigned long seen;
	int res;

	if (op != LOCK_EX && op != LOCK_SH && op != LOCK_UN) {
		return -EINVAL;
	}
	if (in == NULL) {
		return -EINVAL;
	}

	for (;;) {
		dvfs_lock();

		if (op == LOCK_UN) {
			if (desc->f_flock) {
				flock_release_locked(in, desc);
				dvfs_unlock();
				flock_changed();
				return 0;
			}
			dvfs_unlock();
			return 0;
		}

		if (desc->f_flock == op) {
			dvfs_unlock();
			return 0;
		}

		if (flock_free_for(in, desc, op)) {
			int had = desc->f_flock;

			if (had) {
				flock_release_locked(in, desc);
			}
			if (op == LOCK_EX) {
				in->i_flock_ex = desc;
			} else {
				in->i_flock_sh++;
			}
			desc->f_flock = op;
			dvfs_unlock();
			if (had == LOCK_EX) {
				/* A conversion down lets shared waiters in */
				flock_changed();
			}
			return 0;
		}

		seen = atomic_rmw_load(&dvfs_flock_gen, __ATOMIC_ACQUIRE);
		dvfs_unlock();

		if (operation & LOCK_NB) {
			return -EWOULDBLOCK;
		}

		res = WAITQ_WAIT(&dvfs_flock_wq,
		    atomic_rmw_load(&dvfs_flock_gen, __ATOMIC_ACQUIRE) != seen);
		if (res) {
			return -EINTR;
		}
	}
}

void dvfs_flock_release(struct file_desc *desc) {
	if (!desc->f_flock || !desc->f_inode) {
		return;
	}

	dvfs_lock();
	flock_release_locked(desc->f_inode, desc);
	dvfs_unlock();

	flock_changed();
}
