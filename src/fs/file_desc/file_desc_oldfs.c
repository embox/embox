/**
 * @file
 *
 * @brief
 *
 * @date 14.09.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>

#include <fs/perm.h>
#include <hal/ipl.h>
#include <kernel/spinlock.h>
#include <mem/objalloc.h>

#include <fs/file_desc.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <embox/unit.h>
#include <util/err.h>
#include <fs/inode.h>
#include <util/atomic_rmw.h>

extern const struct idesc_xattrops file_idesc_xattrops;

OBJALLOC_DEF(desc_pool, struct file_desc, OPTION_GET(NUMBER,fdesc_quantity));

/* Which descriptors are alive, so that kumount() can refuse to
 * free a volume out from under one. The pool above is an OBJALLOC_DEF and
 * cannot be walked, so the table is kept here instead of asking it.
 *
 * A plain array of the pool's own size: the cost is one pointer per possible
 * descriptor and a linear scan at unmount, which happens rarely and matters
 * more than it costs.
 *
 * And it needs a lock, because it is a table two
 * cores reach. Two allocations scanning it at once can settle on the same
 * slot, and then one of the descriptors is not in the table at all -- so
 * kumount() sees a volume with nothing open on it and frees it under a file
 * that is open, which is the exact corruption this table exists to prevent.
 * A concurrent free can also clear a slot a scan has just read, and
 * file_desc_open_on_sb() then dereferences a descriptor that is already back
 * in the pool.
 *
 * Interrupts are masked rather than preemption disabled: every section is a
 * bounded walk over an array, nothing in them blocks, and masking makes them
 * non-preemptible for free. Same shape and the same reasoning as the
 * descriptor table's lock in kernel/task/idesc/idesc_table.c.
 *
 * objalloc() and objfree() stay OUTSIDE it. They take the pool's own lock,
 * and holding one lock at a time is what keeps "no nesting here" a fact
 * rather than something to check again later. */
#define FDESC_LIVE_MAX OPTION_GET(NUMBER, fdesc_quantity)
static struct file_desc *fdesc_live[FDESC_LIVE_MAX];
static spinlock_t fdesc_live_lock = SPIN_STATIC_UNLOCKED;

/* Descriptors allocated with no room left to record them. The table is the
 * pool's own size, so this cannot happen while the two agree: a non-zero
 * value means a slot leaked and kumount()'s answer stopped being trustworthy
 * some time earlier. */
unsigned long fdesc_live_lost;

static struct file_desc *file_desc_alloc(struct inode *node) {
	struct file_desc *desc = objalloc(&desc_pool);
	struct super_block *sb;
	ipl_t ipl;
	int i;
	int sealed = 0;

	if (desc) {
		ipl = ipl_save();
		__spin_lock(&fdesc_live_lock);
		/* The same lock that answers "how many are open" decides whether one
		 * more may be. Otherwise the answer is true only for the instant it is
		 * given, and kumount() acts on it for the whole teardown.
		 *
		 * Read i_sb ONCE. This was
		 *
		 *     sealed = node && node->i_sb && node->i_sb->sb_unmounting;
		 *
		 * which loads the pointer twice -- once to test it and once to reach
		 * the field. inode_detach_fs() clears i_sb from the umount path on
		 * another core, so the check could pass and the dereference then run
		 * on NULL. Measured: three rounds of a 170-round acceptance hour died
		 * with `Data abort ... FAR_EL1 = 0x40', always in the mount_smp round
		 * and always at this line, and 0x40 is exactly
		 * offsetof(struct super_block, sb_unmounting) -- so the base really
		 * was NULL while the test on the same line had just passed.
		 *
		 * One load closes it. A pointer that was valid when loaded is safe to
		 * follow here: interrupts are off and this core holds the lock, so
		 * nothing preempts between the load and the field, and a volume that
		 * is being taken apart has had sb_unmounting set BEFORE the walk that
		 * detaches its inodes -- so a stale-but-live pointer reads 1 and
		 * refuses, which is the right answer. */
		sb = node ? node->i_sb : NULL;
		sealed = sb && sb->sb_unmounting;
		if (!sealed) {
			for (i = 0; i < FDESC_LIVE_MAX; i++) {
				if (fdesc_live[i] == NULL) {
					fdesc_live[i] = desc;
					break;
				}
			}
		}
		__spin_unlock(&fdesc_live_lock);
		ipl_restore(ipl);

		if (sealed) {
			objfree(&desc_pool, desc);
			return NULL;
		}
		if (i == FDESC_LIVE_MAX) {
			atomic_add_fetch(&fdesc_live_lost, 1, __ATOMIC_RELAXED);
		}
	}
	return desc;
}

static void file_desc_free(struct file_desc *desc) {
	ipl_t ipl;
	int i;

	ipl = ipl_save();
	__spin_lock(&fdesc_live_lock);
	for (i = 0; i < FDESC_LIVE_MAX; i++) {
		if (fdesc_live[i] == desc) {
			fdesc_live[i] = NULL;
			break;
		}
	}
	__spin_unlock(&fdesc_live_lock);
	ipl_restore(ipl);

	objfree(&desc_pool, desc);
}

/* How many open descriptors name a file on this superblock. */
static int fdesc_count_on_sb(const struct super_block *sb) {
	int n = 0;
	int i;

	for (i = 0; i < FDESC_LIVE_MAX; i++) {
		if (fdesc_live[i] != NULL && fdesc_live[i]->f_inode != NULL
		    && fdesc_live[i]->f_inode->i_sb == sb) {
			n++;
		}
	}

	return n;
}

int file_desc_open_on_sb(const struct super_block *sb) {
	ipl_t ipl;
	int n;

	if (sb == NULL) {
		return 0;
	}
	ipl = ipl_save();
	__spin_lock(&fdesc_live_lock);
	n = fdesc_count_on_sb(sb);
	__spin_unlock(&fdesc_live_lock);
	ipl_restore(ipl);

	return n;
}

int file_desc_seal_sb(struct super_block *sb) {
	ipl_t ipl;
	int n;

	if (sb == NULL) {
		return 0;
	}
	ipl = ipl_save();
	__spin_lock(&fdesc_live_lock);
	n = fdesc_count_on_sb(sb);
	if (n == 0) {
		sb->sb_unmounting = 1;
	}
	__spin_unlock(&fdesc_live_lock);
	ipl_restore(ipl);

	return n;
}

/* Both must stay zero; each counts a use of a descriptor that no longer
 * names what it was opened on, which would otherwise be found by its
 * consequences -- in another file. */
unsigned long fdesc_stale_gen;
unsigned long fdesc_dead_inode;

int file_desc_valid(struct file_desc *desc) {
	struct inode *node;

	assert(desc);

	node = desc->f_inode;
	if (node == NULL) {
		return 0;
	}

	/* The pool slot changed hands. The pointer is valid and it is somebody
	 * else's file -- which is exactly what a NULL guard cannot see, and what
	 * wrote one file's bytes into another. */
	if (node->i_gen != desc->f_gen) {
		atomic_add_fetch(&fdesc_stale_gen, 1, __ATOMIC_RELAXED);
		return 0;
	}

	/* The inode left the name tree -- the volume was unmounted, or the
	 * file was removed. The inode itself is still here, because this
	 * descriptor holds a reference to it, but the filesystem has already been
	 * given its data back. Answer the descriptor rather than let it reach
	 * whatever took the driver's private slot. */
	if (node->i_dying) {
		atomic_add_fetch(&fdesc_dead_inode, 1, __ATOMIC_RELAXED);
		return 0;
	}

	return 1;
}

extern const struct idesc_ops idesc_file_ops;

struct file_desc *file_desc_create(struct inode *node, int flag) {
	struct file_desc *desc;
	int perm_flags;
	int ret;

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc(node))) {
		/* Either no descriptor left, or the volume is being unmounted. */
		return err2ptr(ENOMEM);
	}
	/* setup access mode */
	perm_flags = 0;
	if ((flag & O_WRONLY) || (flag & O_RDWR)) {
		perm_flags |= S_IWOTH;
	}
	if (!(flag & O_WRONLY)) {
		perm_flags |= S_IROTH;
	}

	if (0 > (ret = fs_perm_check(node, perm_flags))) {
		file_desc_free(desc);
		return err2ptr(EACCES);
	}

	/* A descriptor outlives the lookup that found its inode, and unlink()
	 * frees the inode the moment its last name goes. Say so, so that the free
	 * waits for this descriptor instead. A refusal here means the file was
	 * unlinked between the lookup and now -- the answer to that is ENOENT, not
	 * a descriptor onto a corpse. */
	if (!inode_ref(node)) {
		file_desc_free(desc);
		return err2ptr(ENOENT);
	}

	desc->f_inode = node;
	desc->f_gen = node->i_gen;
	desc->f_pos = 0;

	idesc_init(&desc->f_idesc, &idesc_file_ops, flag);
	desc->f_idesc.idesc_xattrops = &file_idesc_xattrops;

	return desc;
}

int file_desc_destroy(struct file_desc *fdesc) {
	struct inode *node;

	assert(fdesc);

	node = fdesc->f_inode;

	/* One name fewer; this is what takes the inode out of the tree when the
	 * last one goes. */
	inode_del(node);

	/* And one user fewer. After inode_del(), because if
	 * that was the last name the inode is now dying and this is the reference
	 * whose release frees it. */
	inode_unref(node);

	file_desc_free(fdesc);
	return 0;
}

struct file_desc *file_desc_get(int idx) {
	struct idesc *idesc;
	struct idesc_table *it;

	assert(idesc_index_valid(idx));

	it = task_resource_idesc_table(task_self());
	assert(it);

	idesc = idesc_table_get(it, idx);
	assert(idesc);

	if (idesc->idesc_ops != &idesc_file_ops) {
		return NULL;
	}

	return (struct file_desc *) idesc;
}

off_t file_get_pos(struct file_desc * file) {
	return file->f_pos;
}

off_t file_set_pos(struct file_desc *file, off_t off) {
	file->f_pos = off;

	return file->f_pos;
}

size_t file_get_size(struct file_desc *file) {
	return file->f_inode->i_size;
}

void file_set_size(struct file_desc *file, size_t size) {
	file->f_inode->i_size = size;
}

void *file_get_inode_data(struct file_desc *file) {
	assert(file->f_inode);

	return file->f_inode->i_privdata;
}

struct file_desc *file_desc_from_idesc(struct idesc *idesc) {
	return (struct file_desc *)idesc;
}

void file_desc_set_file_info(struct file_desc *file, void *fi) {
	file->file_info = fi;
}
