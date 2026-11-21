/**
 * @file
 * @brief POSIX record locks for DVFS: fcntl(F_GETLK, F_SETLK, F_SETLKW)
 *
 * @date 24.09.26
 */

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#include <fs/dvfs.h>
#include <fs/file_desc.h>
#include <kernel/sched/waitq.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/thread/waitq.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>
#include <util/atomic_rmw.h>

/* Locks held at once across every file and task; running out is ENOLCK. */
#define DVFS_RLOCK_MAX 256

/* "To the end of the file, however far it grows": l_len == 0. */
#define RLOCK_EOF LONG_MAX

/* One locked range, owned by the task: a POSIX record lock belongs to the
 * process, not to the descriptor (flock(2)) or the thread. One owner's ranges
 * on a file never overlap; a new lock replaces what it covers. */
struct dvfs_rlock {
	struct dlist_head link;
	struct inode *in;
	const struct task *owner;
	off_t start;
	off_t end; /* inclusive; RLOCK_EOF for l_len == 0 */
	short type; /* F_RDLCK or F_WRLCK */
};

POOL_DEF(dvfs_rlock_pool, struct dvfs_rlock, DVFS_RLOCK_MAX);

/* Everything below is under dvfs_lock(), as the flock(2) state is. */
static DLIST_DEFINE(dvfs_rlock_list);

/* One queue for every waiter, as for flock(2): a release wakes them all and
 * each looks again. */
static struct waitq dvfs_rlock_wq = WAITQ_INIT(dvfs_rlock_wq);
static unsigned long dvfs_rlock_gen;

static void rlock_changed(void) {
	atomic_rmw_add_fetch(&dvfs_rlock_gen, 1, __ATOMIC_RELEASE);
	waitq_wakeup_all(&dvfs_rlock_wq);
}

/* The absolute, inclusive range FL names on DESC's file. */
static int rlock_range(struct file_desc *desc, const struct flock *fl,
    off_t *start, off_t *end) {
	off_t base, s, e;

	switch (fl->l_whence) {
	case SEEK_SET:
		base = 0;
		break;
	case SEEK_CUR:
		base = desc->f_pos;
		break;
	case SEEK_END:
		base = (off_t)inode_size(desc->f_inode);
		break;
	default:
		return -EINVAL;
	}

	if (__builtin_add_overflow(base, fl->l_start, &s)) {
		return -EOVERFLOW;
	}

	if (fl->l_len > 0) {
		if (__builtin_add_overflow(s, fl->l_len - 1, &e)) {
			return -EOVERFLOW;
		}
	}
	else if (fl->l_len == 0) {
		e = RLOCK_EOF;
	}
	else {
		/* A negative length locks the bytes before l_start. */
		e = s - 1;
		if (__builtin_add_overflow(s, fl->l_len, &s)) {
			return -EINVAL;
		}
	}

	if (s < 0 || e < s) {
		return -EINVAL;
	}

	*start = s;
	*end = e;
	return 0;
}

/* A lock of some other task that a TYPE lock on [S, E] would collide with. */
static struct dvfs_rlock *rlock_conflict(struct inode *in,
    const struct task *owner, off_t s, off_t e, short type) {
	struct dvfs_rlock *l;

	dlist_foreach_entry(l, &dvfs_rlock_list, link) {
		if (l->in != in || l->owner == owner) {
			continue;
		}
		if (l->end < s || l->start > e) {
			continue;
		}
		if (type == F_WRLCK || l->type == F_WRLCK) {
			return l;
		}
	}

	return NULL;
}

/* Make OWNER's locks on IN say TYPE for [S, E] (F_UNLCK removes), cutting or
 * splitting the ranges it covers. Entries are taken first, so ENOLCK leaves
 * the old locks unchanged. */
static int rlock_apply(struct inode *in, const struct task *owner,
    off_t s, off_t e, short type) {
	struct dvfs_rlock *l, *spare[2] = {NULL, NULL};
	int need = (type != F_UNLCK) ? 1 : 0;
	int i, used = 0;

	dlist_foreach_entry(l, &dvfs_rlock_list, link) {
		if (l->in == in && l->owner == owner && l->start < s && l->end > e) {
			need++; /* at most one range of the owner can surround [S, E] */
			break;
		}
	}

	for (i = 0; i < need; i++) {
		spare[i] = pool_alloc(&dvfs_rlock_pool);
		if (!spare[i]) {
			while (i-- > 0) {
				pool_free(&dvfs_rlock_pool, spare[i]);
			}
			return -ENOLCK;
		}
	}

	dlist_foreach_entry(l, &dvfs_rlock_list, link) {
		if (l->in != in || l->owner != owner) {
			continue;
		}
		if (l->end < s || l->start > e) {
			continue;
		}

		if (l->start < s && l->end > e) {
			struct dvfs_rlock *tail = spare[used++];

			tail->in = l->in;
			tail->owner = l->owner;
			tail->type = l->type;
			tail->start = e + 1;
			tail->end = l->end;
			dlist_head_init(&tail->link);
			/* At the list's end, where this walk meets it past [S, E] */
			dlist_add_prev(&tail->link, &dvfs_rlock_list);
			l->end = s - 1;
		}
		else if (l->start < s) {
			l->end = s - 1;
		}
		else if (l->end > e) {
			l->start = e + 1;
		}
		else {
			dlist_del(&l->link);
			pool_free(&dvfs_rlock_pool, l);
		}
	}

	if (type != F_UNLCK) {
		l = spare[used++];
		l->in = in;
		l->owner = owner;
		l->type = type;
		l->start = s;
		l->end = e;
		dlist_head_init(&l->link);
		dlist_add_prev(&l->link, &dvfs_rlock_list);
	}

	return 0;
}

/**
 * @brief fcntl(F_GETLK, F_SETLK, F_SETLKW) on an open file
 *
 * @return 0 or -errno: -EAGAIN for F_SETLK when another task holds a
 *         conflicting lock, -EBADF for a lock the descriptor's access mode does
 *         not allow, -ENOLCK when the table is full, -EINTR when an F_SETLKW
 *         wait is broken off, -EINVAL for anything that is not a lock
 *
 * No deadlock detection: POSIX allows EDEADLK, it does not require it.
 */
int dvfs_lockf(struct file_desc *desc, int cmd, struct flock *fl) {
	const struct task *owner = task_self();
	struct inode *in = desc->f_inode;
	struct dvfs_rlock *c;
	unsigned long seen;
	off_t s, e;
	int acc, res;

	if (in == NULL) {
		return -EINVAL;
	}
	if (fl->l_type != F_RDLCK && fl->l_type != F_WRLCK
	    && fl->l_type != F_UNLCK) {
		return -EINVAL;
	}

	res = rlock_range(desc, fl, &s, &e);
	if (res) {
		return res;
	}

	if (cmd == F_GETLK) {
		dvfs_lock();
		c = (fl->l_type == F_UNLCK)
		        ? NULL
		        : rlock_conflict(in, owner, s, e, fl->l_type);
		if (c) {
			fl->l_type = c->type;
			fl->l_whence = SEEK_SET;
			fl->l_start = c->start;
			fl->l_len = (c->end == RLOCK_EOF) ? 0 : c->end - c->start + 1;
			fl->l_pid = task_get_id(c->owner);
		}
		else {
			fl->l_type = F_UNLCK;
		}
		dvfs_unlock();
		return 0;
	}

	if (cmd != F_SETLK && cmd != F_SETLKW) {
		return -EINVAL;
	}

	acc = desc->f_idesc.idesc_flags & O_ACCMODE;
	if ((fl->l_type == F_RDLCK && acc == O_WRONLY)
	    || (fl->l_type == F_WRLCK && acc == O_RDONLY)) {
		return -EBADF;
	}

	for (;;) {
		dvfs_lock();

		c = (fl->l_type == F_UNLCK)
		        ? NULL
		        : rlock_conflict(in, owner, s, e, fl->l_type);
		if (!c) {
			res = rlock_apply(in, owner, s, e, fl->l_type);
			dvfs_unlock();
			if (res == 0) {
				/* An unlock or a downgrade may let a waiter in */
				rlock_changed();
			}
			return res;
		}

		seen = atomic_rmw_load(&dvfs_rlock_gen, __ATOMIC_ACQUIRE);
		dvfs_unlock();

		if (cmd == F_SETLK) {
			return -EAGAIN;
		}

		res = WAITQ_WAIT(&dvfs_rlock_wq,
		    atomic_rmw_load(&dvfs_rlock_gen, __ATOMIC_ACQUIRE) != seen);
		if (res) {
			return -EINTR;
		}
	}
}

/**
 * @brief Drop OWNER's record locks on IN, or on every file when IN is NULL
 *
 * POSIX: closing any descriptor of a file drops every lock the process holds
 * on it, and so does the process ending.
 */
void dvfs_lockf_release(struct inode *in, const struct task *owner) {
	struct dvfs_rlock *l;
	int dropped = 0;

	dvfs_lock();
	dlist_foreach_entry(l, &dvfs_rlock_list, link) {
		if (l->owner != owner || (in && l->in != in)) {
			continue;
		}
		dlist_del(&l->link);
		pool_free(&dvfs_rlock_pool, l);
		dropped = 1;
	}
	dvfs_unlock();

	if (dropped) {
		rlock_changed();
	}
}

/* These override the weak stubs fcntl() and close() in compat/posix/idx carry.
 * DVFS core is linked as an object, so they win over the archive's stubs. */
extern const struct idesc_ops idesc_file_ops;

int idesc_fcntl_lock(struct idesc *idesc, int cmd, struct flock *fl) {
	if (idesc->idesc_ops != &idesc_file_ops) {
		/* A device, a pipe or a socket has no bytes to lock */
		return -EINVAL;
	}
	if (!fl) {
		return -EFAULT;
	}

	return dvfs_lockf(file_desc_from_idesc(idesc), cmd, fl);
}

void idesc_close_locks(struct idesc *idesc) {
	struct file_desc *desc;

	if (idesc->idesc_ops != &idesc_file_ops) {
		return;
	}

	desc = file_desc_from_idesc(idesc);
	if (desc->f_inode) {
		dvfs_lockf_release(desc->f_inode, task_self());
	}
}

/* Drops what the task still holds; the resource exists only for this hook. */
static void task_lockf_deinit(const struct task *task) {
	dvfs_lockf_release(NULL, task);
}

TASK_RESOURCE_DECLARE(static, task_lockf_desc, char,
	.deinit = task_lockf_deinit,
);
