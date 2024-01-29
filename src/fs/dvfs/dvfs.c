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
#include <util/err.h>
#include <util/log.h>

/* Utility functions */
extern int inode_fill(struct super_block *, struct inode *, struct dentry *);
extern int dvfs_update_root(void);

/**
 * @brief Create new inode
 * @param name   Directory name for new inode
 * @param lookup Structure containing parent dentry; lookup->item should be NULL
 * @param flags  Flags passed to FS driver
 *
 * @return Negative error number
 * @retval       0 Ok
 * @retval -ENOMEM New dentry can't be allocated
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

	sb = lookup->parent->d_sb;
	lookup->item = d = dvfs_alloc_dentry();
	if (d == NULL) {
		return -ENOMEM;
	}
	dentry_ref_inc(d);

	new_inode = dvfs_alloc_inode(sb);
	if (!new_inode) {
		dentry_ref_dec(d);
		dvfs_destroy_dentry(d);
		return -ENOMEM;
	}
	dentry_fill(sb, new_inode, d, lookup->parent);

	while (*name == '/') {
		name++;
	}

	strncpy(d->name, name, NAME_MAX - 1);

	/* Remove possible trailing slashes */
	slash = strchr(d->name, '/');
	if (slash != NULL) {
		*slash = '\0';
	}

	inode_fill(sb, new_inode, d);

	d->flags |= flags;
	new_inode->i_mode |= flags;
	if (flags & VFS_DIR_VIRTUAL) {
		res = 0;
		d->d_sb = NULL;
		dentry_ref_inc(d);
		lookup->parent->flags |= DVFS_CHILD_VIRTUAL;
	}
	else {
		if (!sb->sb_iops->create) {
			res = -ENOSUPP;
		}
		else {
			if (!S_ISDIR(flags)) {
				new_inode->i_mode |= S_IFREG;
			}
			res = sb->sb_iops->create(new_inode, lookup->parent->d_inode,
			    flags);
			if (res == -ENOMEM) {
				/* This may be due to lack of some internal FS
				 * resources,  so  we  can  try  to free  some
				 * dentry and try again */
				if (dvfs_fs_dentry_try_free(sb)) {
					res = sb->sb_iops->create(new_inode,
					    lookup->parent->d_inode, flags);
				}
			}
		}
	}

	if (res) {
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
 * @retval       0 Ok
 * @retval -ENOENT File is directory or file not found and
 *                 creating is not requested
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
		return err_ptr(ENOMEM);
	}

	d = lookup->item;
	i_no = d->d_inode;

	*desc = (struct file_desc){
	    .f_dentry = lookup->item,
	    .f_inode = i_no,
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
			return err_ptr(ENOENT);
		}
	}

	if ((__oflag & O_TRUNC) && (desc->f_inode)) {
		if (i_no->i_ops && i_no->i_ops->truncate) {
			if (i_no->i_ops->truncate(desc->f_inode, 0)) {
				dvfs_destroy_file(desc);
				return err_ptr(ENOENT);
			}
		}
		inode_size_set(i_no, 0);
	}
	if ((__oflag & O_APPEND) && (desc->f_inode)) {
		desc->pos = desc->f_inode->i_size;
	}

	return &desc->f_idesc;
}

/**
 * @brief Delete file from storage
 * @param path Path to file
 *
 * @return Negative error code
 * @retval  0 Ok
 * @retval -1 File not found
 */
int dvfs_remove(const char *path) {
	struct lookup lookup = {};
	struct inode *i_no;
	int res;

	dvfs_lookup(path, &lookup);

	if (!lookup.item) {
		return -ENOENT;
	}

	dentry_ref_dec(lookup.item);
	i_no = lookup.item->d_inode;

	assert(i_no->i_ops);

	if (!i_no->i_ops->remove)
		return -EPERM;

	if (lookup.item->usage_count > 0) {
		return -EBUSY;
	}

	res = i_no->i_ops->remove(i_no);

	if (res == 0) {
		res = dvfs_destroy_dentry(lookup.item);
		if (res != 0) {
			log_error("Failed to destroy dentry");
		}
	}
	else {
		log_error("Failed to remove inode");
	}

	return res;
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
 * @retval -ENOENT Mount point or device not found
 */
int dvfs_mount(const char *source, const char *dest, const char *fs_type,
    int flags) {
	struct lookup lookup = {};
	struct super_block *sb;
	struct dentry *d = NULL;
	int err;

	assert(dest);
	assert(fs_type);

	if (NULL == (sb = super_block_alloc(fs_type, source))) {
		return -ENOMEM;
	}

	if (!strcmp(dest, "/")) {
		set_rootfs_sb(sb);
		dvfs_update_root();
	}
	else {
		dvfs_lookup(dest, &lookup);

		if (lookup.item == NULL) {
			err = -ENOENT;
			goto err_free_all;
		}

		if (lookup.item->flags & DVFS_MOUNT_POINT) {
			err = -EBUSY;
			goto err_free_all;
		}

		if (!(lookup.item->flags & S_IFDIR)) {
			err = -EINVAL;
			goto err_free_all;
		}

		dentry_disconnect(lookup.item);

		d = dvfs_alloc_dentry();
		dentry_ref_inc(d);

		d->flags |= VFS_DIR_VIRTUAL;
		dentry_fill(sb, sb->sb_root, d, lookup.parent);
		strcpy(d->name, lookup.item->name);

		d->flags |= S_IFDIR | DVFS_MOUNT_POINT;

		dentry_ref_dec(lookup.item);
	}

	return 0;
err_free_all:
	if (lookup.item != NULL) {
		dentry_ref_dec(lookup.item);
	}

	if (d != NULL) {
		dvfs_destroy_inode(d->d_inode);
		dentry_reconnect(d->parent, d->name);
		dentry_ref_dec(d);
		dvfs_destroy_dentry(d);
	}

	if (sb) {
		super_block_free(sb);
	}

	return err;
}

/**
 * @brief Recoursive dentry freeing
 *
 * @param d Dentry to be freed
 *
 * @return Negative error code or zero if succed
 */
static int _dentry_destroy(struct dentry *parent) {
	struct dentry *child;
	int err;
	dlist_foreach_entry(child, &parent->children, children_lnk) {
		if ((err = _dentry_destroy(child)))
			return err;
	}

	return dvfs_destroy_dentry(parent);
}

/**
 * @brief Perform unmount operation
 *
 * @param mpoint Dentry of FS root
 *
 * @return Negative error code or zero if succeed
 * @retval 0 Success
 * @retval -ENOBUSY Some files in FS tree are being used, can't unmount
 */
int dvfs_umount(struct dentry *mpoint) {
	int err;
	struct super_block *sb;

	sb = mpoint->d_sb;

	if (sb->sb_ops && sb->sb_ops->umount_begin) {
		if ((err = sb->sb_ops->umount_begin(sb)))
			return err;
	}

	/* TODO what if mount point was renamed? */
	dentry_reconnect(mpoint->parent, mpoint->name);
	dentry_ref_dec(mpoint);

	if ((err = super_block_free(sb))) {
		return err;
	}

	if ((err = _dentry_destroy(mpoint))) {
		return err;
	}

	return 0;
}

static struct dentry *iterate_virtual(struct lookup *lookup,
    struct dir_ctx *ctx) {
	struct dentry *next_dentry;
	struct dlist_head *l;
	int i;

	i = 0;
	dlist_foreach(l, &lookup->parent->children) {
		next_dentry = mcast_out(l, struct dentry, children_lnk);

		if (next_dentry->flags & VFS_DIR_VIRTUAL) {
			if (i++ == (ctx->flags & ~DVFS_CHILD_VIRTUAL)) {
				ctx->flags++;
				dentry_ref_inc(next_dentry);
				lookup->item = next_dentry;

				return next_dentry;
			}
		}
	}

	return NULL;
}

/**
 * @brief Get next entry in the directory
 * @param lookup  Contains directory dentry (.parent) and
 *                previous element (.item)
 * @param dir_ctx Position to be found in directory
 *
 * @return Negative error value
 * @retval 0 Ok
 */
int dvfs_iterate(struct lookup *lookup, struct dir_ctx *ctx) {
	struct super_block *sb;
	struct inode *next_inode;
	struct dentry *next_dentry, *cached;
	int res;

	assert(ctx);
	assert(lookup);
	assert(lookup->parent);

	if (lookup->parent->flags & VFS_DIR_VIRTUAL && !lookup->parent->d_sb) {
		/* Clean virtual dir, no files */
		lookup->item = NULL;
		return 0;
	}

	assert(lookup->parent->d_sb);
	assert(lookup->parent->d_inode);

	sb = lookup->parent->d_sb;
	assert(sb->sb_iops && sb->sb_iops->iterate);

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

	dentry_ref_inc(next_dentry);
	lookup->item = next_dentry;

	res = sb->sb_iops->iterate(next_inode, next_dentry->name,
	    lookup->parent->d_inode, ctx);
	if (res) {
		/* iterate virtual */
		dentry_ref_dec(next_dentry);
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

	if ((cached = dvfs_cache_get(next_dentry->name, lookup))) {
		/* This node is already in the VFS tree */
		dentry_ref_dec(next_dentry);
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
		lookup->item = next_dentry;
	}

	return 0;
}
