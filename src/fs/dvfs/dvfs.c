/**
 * @file
 * @brief  DVFS interface implementation
 * @author Denis Deryugin
 * @date   11 Mar 2014
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

#include <fs/dentry.h>
#include <fs/dvfs.h>
#include <fs/super_block.h>
#include <util/atomic_rmw.h>
#include <util/err.h>
#include <util/log.h>

/* Utility functions */
extern int inode_fill(struct super_block *, struct inode *, struct dentry *);
extern int dvfs_update_root(void);
extern struct dentry *local_lookup(struct dentry *parent, char *name);

/**
 * @brief Create new inode
 * @param name   Directory name for new inode
 * @param lookup Structure containing parent dentry; lookup->item should be NULL
 * @param flags  Flags passed to FS driver
 *
 * @return Negative error number
 * @retval       0 Ok, lookup->item is the new dentry, referenced
 * @retval -ENOMEM New dentry can't be allocated
 * @retval -EEXIST The name is already in the directory
 * @retval -ENOENT The directory itself has been removed
 *
 * Called with dvfs_lock and lookup->parent held, in the same hold of the lock
 * as the dvfs_lookup_at() that found the name missing.
 */
int dvfs_create_new(const char *name, struct lookup *lookup, int flags) {
	struct super_block *sb;
	struct inode *new_inode;
	struct dentry *d;
	char *slash;
	int res;

	assert(lookup);
	assert(lookup->parent);
	assert(lookup->parent->flags & S_IFDIR);
	dvfs_lock_assert("dvfs_create_new");

	if (lookup->parent->flags & DVFS_DYING) {
		return -ENOENT;
	}

	while (*name == '/') {
		name++;
	}

	sb = lookup->parent->d_sb;
	lookup->item = d = dvfs_alloc_dentry();
	if (d == NULL) {
		return -ENOMEM;
	}

	strncpy(d->name, name, NAME_MAX - 1);

	/* Remove possible trailing slashes */
	slash = strchr(d->name, '/');
	if (slash != NULL) {
		*slash = '\0';
	}

	if (d->name[0] == '\0') {
		lookup->item = NULL;
		dvfs_destroy_dentry(d);
		return -EINVAL;
	}

	if (local_lookup(lookup->parent, d->name)) {
		lookup->item = NULL;
		dvfs_destroy_dentry(d);
		return -EEXIST;
	}

	new_inode = dvfs_alloc_inode(sb);
	if (!new_inode && !(flags & VFS_DIR_VIRTUAL)) {
		lookup->item = NULL;
		dvfs_destroy_dentry(d);
		return -ENOMEM;
	}

	if (new_inode == NULL) {
		/* A virtual directory under a virtual parent has no superblock and
		 * needs no inode. */
		dentry_fill(NULL, NULL, d, lookup->parent);
	} else {
		dentry_fill(sb, new_inode, d, lookup->parent);
		inode_fill(sb, new_inode, d);
		new_inode->i_mode |= flags;
	}
	dentry_ref_inc(d);

	d->flags |= flags;
	if (flags & VFS_DIR_VIRTUAL) {
		res = 0;
		d->d_sb = NULL;
		/* Only the tree knows a virtual directory, so it is pinned there. */
		dentry_ref_inc(d);
		lookup->parent->flags |= DVFS_CHILD_VIRTUAL;
	}
	else {
		if (!sb->sb_iops->ino_create) {
			res = -EROFS;
		}
		else {
			if (!S_ISDIR(flags)) {
				new_inode->i_mode |= S_IFREG;
			}
			res = sb->sb_iops->ino_create(new_inode, lookup->parent->d_inode,
			    flags);
			if (res == -ENOMEM) {
				/* This may be due to lack of some internal FS
				 * resources,  so  we  can  try  to free  some
				 * dentry and try again */
				if (dvfs_fs_dentry_try_free(sb)) {
					res = sb->sb_iops->ino_create(new_inode,
					    lookup->parent->d_inode, flags);
				}
			}
		}
		if (res == 0) {
			dentry_upd_flags(d);
		}
	}

	if (res) {
		lookup->item = NULL;
		dentry_ref_dec(d);
		dvfs_destroy_dentry(d);
	}

	return res;
}

/**
 * @brief Initialize file descriptor for usage according to path
 * @param path Path to the file
 * @param desc The file descriptor to be initailized
 * @param mode Defines behavior according to POSIX
 *
 * @returns Negative error number
 * @retval -ENOENT File is directory or file not found and
 *                 creating is not requested
 *
 * On success the descriptor takes over the caller's reference on
 * lookup->item; kclose() gives it back.
 */
struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag) {
	extern const struct idesc_ops idesc_file_ops;

	struct file_desc *desc;
	struct idesc *res;
	struct inode *i_no;
	struct dentry *d;

	assert(lookup);

	desc = dvfs_alloc_file();
	if (desc == NULL) {
		return err2ptr(ENFILE);
	}

	d = lookup->item;
	i_no = d->d_inode;

	*desc = (struct file_desc){
	    .f_dentry = lookup->item,
	    .f_inode = i_no,
	    .f_gen = i_no ? i_no->i_gen : 0,
	    .f_ops = d->d_sb ? d->d_sb->sb_fops : NULL,
	    .f_idesc = {.idesc_ops = &idesc_file_ops},
	};

	if (!(d->flags & VFS_DIR_VIRTUAL)) {
		assert(desc->f_ops);
	}

	if (desc->f_ops && desc->f_ops->open && !(__oflag & O_PATH)) {
		res = desc->f_ops->open(i_no, &desc->f_idesc, __oflag);
		if (res == NULL) {
			dvfs_destroy_file(desc);
			return err2ptr(ENOENT);
		}
	}

	if ((__oflag & O_TRUNC) && (desc->f_inode)) {
		if (i_no->i_ops && i_no->i_ops->ino_truncate) {
			if (i_no->i_ops->ino_truncate(desc->f_inode, 0)) {
				dvfs_destroy_file(desc);
				return err2ptr(ENOENT);
			}
		}
		inode_size_set(i_no, 0);
	}
	if ((__oflag & O_APPEND) && (desc->f_inode)) {
		file_set_pos(desc, inode_size(desc->f_inode));
	}

	return &desc->f_idesc;
}

extern int set_rootfs_sb(struct super_block *sb);
/**
 * @brief Mount file system
 * @param source  Path to the source device (e.g. /dev/sda1)
 * @param dest    Path to the mount point (e.g. /mnt)
 * @param fs_type File system type related to FS driver
 * @param flags   NIY
 *
 * @return Negative error value
 * @retval       0 Ok
 * @retval -ENOENT Mount point not found
 * @retval -ENODEV No such file system type, or it would not mount the device
 * @retval  -EBUSY Already a mount point (the root included)
 *
 * The mount hides the directory it covers: that dentry goes off its parent's
 * list, a new one for the mounted root takes its place, and the new one keeps
 * the old one in d_covered with a reference, for umount to put it back.
 */
int dvfs_mount(const char *source, const char *dest, const char *fs_type,
    int flags) {
	struct lookup lookup = {};
	struct super_block *sb = NULL;
	struct dentry *covered, *d;
	int err;

	assert(dest);
	assert(fs_type);

	if (NULL == fs_driver_find(fs_type)) {
		return -ENODEV;
	}

	dvfs_lock();

	if (!strcmp(dest, "/")) {
		if (dvfs_root()->d_sb != NULL) {
			err = -EBUSY;
			goto out;
		}
		if (NULL == (sb = super_block_alloc(fs_type, source))) {
			err = -ENODEV;
			goto out;
		}
		set_rootfs_sb(sb);
		dvfs_update_root();
		err = 0;
		goto out;
	}

	if ((err = dvfs_lookup_at(NULL, dest, &lookup, NULL))) {
		goto out;
	}

	covered = lookup.item;
	if (covered == NULL) {
		err = -ENOENT;
		goto out_put;
	}

	if (!S_ISDIR(covered->flags)) {
		err = -ENOTDIR;
		goto out_put;
	}

	if (covered->flags & (DVFS_MOUNT_POINT | DVFS_DYING)) {
		err = -EBUSY;
		goto out_put;
	}

	if (NULL == (sb = super_block_alloc(fs_type, source))) {
		err = -ENODEV;
		goto out_put;
	}

	d = dvfs_alloc_dentry();
	if (d == NULL) {
		super_block_free(sb);
		err = -ENOMEM;
		goto out_put;
	}

	/* Hide the covered directory, put the mounted root in its place. */
	dvfs_cache_del(covered);
	dlist_del_init(&covered->children_lnk);

	dentry_fill(sb, sb->sb_root, d, covered->parent);
	strcpy(d->name, covered->name);
	d->flags = S_IFDIR | VFS_DIR_VIRTUAL | DVFS_MOUNT_POINT;
	/* The mount's own pin, given back by umount. */
	d->usage_count = 1;
	/* The lookup's reference on the covered dentry becomes the mount's. */
	d->d_covered = covered;
	lookup.item = NULL;

	err = 0;
out_put:
	dvfs_lookup_put(&lookup);
out:
	dvfs_unlock();
	return err;
}

/* Anybody other than the mount point still holding a dentry of SB? Dying
 * dentries are off the tree but still on the list of all dentries. */
static int dvfs_sb_busy(struct super_block *sb, struct dentry *mpoint,
    int mpoint_refs) {
	extern struct dlist_head dentry_dlist;
	struct dentry *d;

	if (mpoint->usage_count > mpoint_refs) {
		return 1;
	}

	dlist_foreach_entry(d, &dentry_dlist, d_lnk) {
		if (d != mpoint && d->d_sb == sb && d->usage_count > 0) {
			return 1;
		}
		/* A mount inside this one: its covered dentry is ours and held, so
		 * the check above has seen it already. */
	}

	return 0;
}

/**
 * @brief Perform unmount operation
 *
 * @param mpoint Dentry of FS root, held by the caller
 *
 * @return Negative error code or zero if succeed
 * @retval 0 Success; the caller's reference is the last one and frees it
 * @retval -EBUSY Some files in FS tree are being used, can't unmount
 * @retval -EINVAL Not a mount point
 *
 * The busy check and the teardown are one hold of the lock, which every new
 * reference goes through. The superblock goes last.
 */
int dvfs_umount(struct dentry *mpoint) {
	struct super_block *sb;
	struct dentry *covered;
	int err;

	dvfs_lock();

	if (mpoint == dvfs_root()) {
		err = -EBUSY;
		goto out;
	}

	if (!(mpoint->flags & DVFS_MOUNT_POINT) || mpoint->d_covered == NULL) {
		err = -EINVAL;
		goto out;
	}

	sb = mpoint->d_sb;

	/* The cached dentries of SB go first, leaves before their parents. */
	while (dvfs_fs_dentry_try_free(sb)) {
	}

	/* The mount's own pin plus the caller's. */
	if (dvfs_sb_busy(sb, mpoint, 2)) {
		atomic_rmw_add_fetch(&dvfs_umount_busy, 1, __ATOMIC_RELAXED);
		err = -EBUSY;
		goto out;
	}

	if (sb->sb_ops && sb->sb_ops->umount_begin) {
		if ((err = sb->sb_ops->umount_begin(sb))) {
			goto out;
		}
	}

	covered = mpoint->d_covered;
	mpoint->d_covered = NULL;

	/* Take the mounted root off the tree and give the covered directory its
	 * place back. */
	dlist_del_init(&mpoint->children_lnk);
	dvfs_cache_del(mpoint);
	dlist_add_prev(&covered->children_lnk, &covered->parent->children);

	/* The root inode belongs to the superblock and goes with it. */
	mpoint->d_inode = NULL;
	mpoint->flags &= ~DVFS_MOUNT_POINT;
	mpoint->flags |= DVFS_DYING;
	mpoint->usage_count--; /* the mount's pin; the caller's is the last */

	err = super_block_free(sb);
	if (err) {
		log_error("super_block_free: %d", err);
		err = 0;
	}

	dentry_ref_dec(covered);
out:
	dvfs_unlock();
	return err;
}

int dvfs_umount_path(const char *path) {
	struct lookup lu;
	int err;

	dvfs_lock();

	if ((err = dvfs_lookup_at(NULL, path, &lu, NULL))) {
		goto out;
	}

	if (lu.item == NULL) {
		err = -ENOENT;
	} else if (!(lu.item->flags & DVFS_MOUNT_POINT)) {
		err = -EINVAL;
	} else {
		err = dvfs_umount(lu.item);
	}

	dvfs_lookup_put(&lu);
out:
	dvfs_unlock();
	return err;
}

static struct dentry *iterate_virtual(struct lookup *lookup,
    struct dir_ctx *ctx) {
	struct dentry *next_dentry;
	struct dlist_head *l;
	int i;

	i = 0;
	dlist_foreach(l, &lookup->parent->children) {
		if (l == &lookup->parent->children) {
			continue;
		}
		next_dentry = mcast_out(l, struct dentry, children_lnk);

		if (!(next_dentry->flags & VFS_DIR_VIRTUAL)) {
			continue;
		}
		/* A mount over a directory the parent's file system has: the
		 * driver's walk already listed that name. */
		if (next_dentry->d_covered
		    && !(next_dentry->d_covered->flags & VFS_DIR_VIRTUAL)) {
			continue;
		}

		if (i++ == (ctx->flags & ~DVFS_CHILD_VIRTUAL)) {
			ctx->flags++;
			dentry_ref_inc(next_dentry);
			lookup->item = next_dentry;

			return next_dentry;
		}
	}

	return NULL;
}

static int dvfs_iterate_locked(struct lookup *lookup, struct dir_ctx *ctx) {
	struct super_block *sb;
	struct inode *next_inode;
	struct dentry *next_dentry, *cached;
	int res;

	if (lookup->parent->flags & DVFS_DYING) {
		lookup->item = NULL;
		return 0;
	}

	if (lookup->parent->flags & VFS_DIR_VIRTUAL && !lookup->parent->d_sb) {
		/* A virtual directory: what it has is what is in memory */
		lookup->item = NULL;
		if (lookup->parent->flags & DVFS_CHILD_VIRTUAL) {
			ctx->flags |= DVFS_CHILD_VIRTUAL;
			lookup->item = iterate_virtual(lookup, ctx);
		}
		return 0;
	}

	assert(lookup->parent->d_sb);
	assert(lookup->parent->d_inode);

	sb = lookup->parent->d_sb;
	assert(sb->sb_iops && sb->sb_iops->ino_iterate);

	if (ctx->flags & DVFS_CHILD_VIRTUAL) {
		/* we are already in virtual iterate mode */
		lookup->item = iterate_virtual(lookup, ctx);

		return 0;
	}

	next_inode = dvfs_alloc_inode(sb);
	if (!next_inode) {
		return -ENOMEM;
	}
	next_dentry = dvfs_alloc_dentry();
	if (!next_dentry) {
		dvfs_destroy_inode(next_inode);
		return -ENOMEM;
	}

	res = sb->sb_iops->ino_iterate(next_inode, next_dentry->name,
	    lookup->parent->d_inode, ctx);
	while (res == -ENOMEM && dvfs_fs_dentry_try_free(sb)) {
		/* The driver's own pool is empty, and cached dentries of this volume
		 * hold entries of it. -ENOMEM is not the end of the directory. */
		res = sb->sb_iops->ino_iterate(next_inode, next_dentry->name,
		    lookup->parent->d_inode, ctx);
	}
	if (res == -ENOMEM) {
		dvfs_destroy_dentry(next_dentry);
		dvfs_destroy_inode(next_inode);
		lookup->item = NULL;
		return -ENOMEM;
	}
	if (res) {
		/* iterate virtual */
		dvfs_destroy_dentry(next_dentry);
		dvfs_destroy_inode(next_inode);

		lookup->item = NULL;
		if (lookup->parent->flags & DVFS_CHILD_VIRTUAL) {
			ctx->flags = DVFS_CHILD_VIRTUAL;

			lookup->item = iterate_virtual(lookup, ctx);
		}
		/* Virtual entries are always cached, so we skip cache check */
		return 0;
	}

	lookup->item = next_dentry;
	if ((cached = dvfs_cache_get(next_dentry->name, lookup))) {
		/* This node is already in the VFS tree */
		dvfs_destroy_dentry(next_dentry);
		dvfs_destroy_inode(next_inode);
		lookup->item = cached;
		dentry_ref_inc(cached);
	}
	else {
		/* Integrate next_dentry into VFS tree */
		dentry_fill(sb, next_inode, next_dentry, lookup->parent);
		inode_fill(sb, next_inode, next_dentry);
		dentry_upd_flags(next_dentry);
		dvfs_cache_add(next_dentry);
		dentry_ref_inc(next_dentry);
	}

	return 0;
}

/**
 * @brief Get next entry in the directory
 * @param lookup  Contains directory dentry (.parent), held by the caller
 * @param dir_ctx Position to be found in directory
 *
 * @return Negative error value
 * @retval 0 Ok; lookup->item is the entry, referenced, or NULL at the end
 */
int dvfs_iterate(struct lookup *lookup, struct dir_ctx *ctx) {
	int res;

	assert(ctx);
	assert(lookup);
	assert(lookup->parent);

	dvfs_lock();
	res = dvfs_iterate_locked(lookup, ctx);
	dvfs_unlock();

	return res;
}
