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

/* Live descriptors, so that kumount() can refuse to free a volume under
 * one; the OBJALLOC_DEF pool cannot be walked. Sized as the pool.
 *
 * fdesc_live_lock guards it, with interrupts masked; nothing blocks inside.
 * objalloc() and objfree() take the pool's own lock and stay outside it, so
 * the two never nest. Without SMP masked interrupts alone guard it. */
#define FDESC_LIVE_MAX OPTION_GET(NUMBER, fdesc_quantity)
static struct file_desc *fdesc_live[FDESC_LIVE_MAX];
#if defined(SMP)
static spinlock_t fdesc_live_lock = SPIN_STATIC_UNLOCKED;
#endif

static ipl_t fdesc_live_enter(void) {
	ipl_t ipl = ipl_save();

#if defined(SMP)
	__spin_lock(&fdesc_live_lock);
#endif

	return ipl;
}

static void fdesc_live_leave(ipl_t ipl) {
#if defined(SMP)
	__spin_unlock(&fdesc_live_lock);
#endif
	ipl_restore(ipl);
}

/* Descriptors allocated with no slot left to record them. Non-zero means a
 * slot leaked and kumount()'s check is no longer reliable. */
unsigned long fdesc_live_lost;

static struct file_desc *file_desc_alloc(struct inode *node) {
	struct file_desc *desc = objalloc(&desc_pool);
	struct super_block *sb;
	ipl_t ipl;
	int i;
	int sealed = 0;

	if (desc) {
		ipl = fdesc_live_enter();
		/* Checked under the same lock as the count, so kumount()'s answer
		 * holds through the teardown. i_sb is loaded once: umount on another
		 * core clears it. A stale but live sb is safe, as sb_unmounting is set
		 * before the inodes are detached. */
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
		fdesc_live_leave(ipl);

		if (sealed) {
			objfree(&desc_pool, desc);
			return NULL;
		}
		if (i == FDESC_LIVE_MAX) {
			atomic_rmw_add_fetch(&fdesc_live_lost, 1, __ATOMIC_RELAXED);
		}
	}
	return desc;
}

static void file_desc_free(struct file_desc *desc) {
	ipl_t ipl;
	int i;

	ipl = fdesc_live_enter();
	for (i = 0; i < FDESC_LIVE_MAX; i++) {
		if (fdesc_live[i] == desc) {
			fdesc_live[i] = NULL;
			break;
		}
	}
	fdesc_live_leave(ipl);

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
	ipl = fdesc_live_enter();
	n = fdesc_count_on_sb(sb);
	fdesc_live_leave(ipl);

	return n;
}

int file_desc_seal_sb(struct super_block *sb) {
	ipl_t ipl;
	int n;

	if (sb == NULL) {
		return 0;
	}
	ipl = fdesc_live_enter();
	n = fdesc_count_on_sb(sb);
	if (n == 0) {
		sb->sb_unmounting = 1;
	}
	fdesc_live_leave(ipl);

	return n;
}

/* Both must stay zero: uses of a descriptor that no longer names the file
 * it was opened on. */
unsigned long fdesc_stale_gen;
unsigned long fdesc_dead_inode;

int file_desc_valid(struct file_desc *desc) {
	struct inode *node;

	assert(desc);

	node = desc->f_inode;
	if (node == NULL) {
		return 0;
	}

	/* The pool slot changed hands: a valid pointer to another file. */
	if (node->i_gen != desc->f_gen) {
		atomic_rmw_add_fetch(&fdesc_stale_gen, 1, __ATOMIC_RELAXED);
		return 0;
	}

	/* The inode left the tree (unmount or unlink); its filesystem data is
	 * already released. */
	if (node->i_dying) {
		atomic_rmw_add_fetch(&fdesc_dead_inode, 1, __ATOMIC_RELAXED);
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

	/* Hold the inode so that unlink() defers its free until close. Failure
	 * means it was unlinked after the lookup. */
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

	/* Drops a name; the last one takes the inode out of the tree. */
	inode_del(node);

	/* After inode_del(): if that was the last name, releasing this
	 * reference frees the inode. */
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
