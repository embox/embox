/**
 * @file
 * @brief rename() for DVFS, in one hold of the tree lock
 *
 * @date 19.09.26
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>

#include <fs/dvfs.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <util/err.h>
#include <util/log.h>

#define REMOVE_FILE 1
#define REMOVE_DIR  2

extern int dvfs_remove_locked(struct lookup *lu, int kind);
extern int dvfs_cache_add(struct dentry *dentry);
extern int dvfs_cache_del(struct dentry *dentry);

/* Whether ANCESTOR is DIR or one of the directories above it. */
static int dentry_is_under(struct dentry *dir, struct dentry *ancestor) {
	struct dentry *root = dvfs_root();

	for (;;) {
		if (dir == ancestor) {
			return 1;
		}
		if (dir == root || dir->parent == dir || dir->parent == NULL) {
			return 0;
		}
		dir = dir->parent;
	}
}

/* For a file system with no rename of its own: a new file under NAME in
 * DSTLU's directory with SRC's bytes. The caller removes SRC afterwards. */
static int dvfs_copy_file(struct dentry *src, struct lookup *dstlu,
    const char *name) {
	struct lookup in_lu = {}, out_lu = {};
	struct idesc *in_id, *out_id;
	struct file_desc *in, *out;
	char buf[256];
	int res, got, put;

	out_lu.parent = dstlu->parent;
	res = dvfs_create_new(name, &out_lu, S_IFREG);
	if (res) {
		return res;
	}

	out_id = dvfs_file_open_idesc(&out_lu, O_WRONLY);
	if (ptr2err(out_id)) {
		dentry_ref_dec(out_lu.item);
		return ptr2err(out_id);
	}
	out = file_desc_from_idesc(out_id);

	dentry_ref_inc(src);
	in_lu.item = src;
	in_id = dvfs_file_open_idesc(&in_lu, O_RDONLY);
	if (ptr2err(in_id)) {
		dentry_ref_dec(src);
		kclose(out);
		return ptr2err(in_id);
	}
	in = file_desc_from_idesc(in_id);

	res = 0;
	while ((got = kread(in, buf, sizeof(buf))) > 0) {
		put = kwrite(out, buf, got);
		if (put != got) {
			res = put < 0 ? put : -EIO;
			break;
		}
	}
	if (got < 0) {
		res = got;
	}

	kclose(in);
	kclose(out);

	return res;
}

/**
 * @brief Give FROM the name TO
 *
 * @return 0 or -errno, as rename(2):
 *   -ENOENT   FROM does not exist, or TO's directory does not
 *   -EBUSY    either is a mount point or the root
 *   -EISDIR   FROM is a file and TO an existing directory
 *   -ENOTDIR  FROM is a directory and TO an existing file
 *   -ENOTEMPTY TO is a directory with something in it
 *   -EINVAL   TO is inside FROM
 *   -EXDEV    they are on different file systems, or FROM is a directory on
 *             one that cannot rename
 *
 * An existing TO is replaced; if it is open, it becomes DYING as with unlink.
 */
int dvfs_rename(const char *from, const char *to) {
	struct lookup src = {}, dst = {};
	struct dentry *s, *np, *old_parent;
	struct inode_operations *iops;
	char last[NAME_MAX];
	const char *name;
	int res;

	assert(from);
	assert(to);

	dvfs_lock();

	if ((res = dvfs_lookup_at(NULL, from, &src, NULL))) {
		goto out;
	}
	if (src.item == NULL) {
		res = -ENOENT;
		goto out;
	}
	if ((res = dvfs_lookup_at(NULL, to, &dst, last))) {
		goto out;
	}

	s = src.item;
	np = dst.parent;

	if (s == dvfs_root() || (s->flags & (DVFS_MOUNT_POINT | DVFS_DYING))) {
		res = (s->flags & DVFS_DYING) ? -ENOENT : -EBUSY;
		goto out;
	}

	if (dst.item == s) {
		res = 0;
		goto out;
	}

	name = dst.item ? dst.item->name : last;

	if (dst.item && (dst.item == dvfs_root()
	                 || (dst.item->flags & DVFS_MOUNT_POINT))) {
		res = -EBUSY;
		goto out;
	}

	if (S_ISDIR(s->flags)) {
		if (dst.item && !S_ISDIR(dst.item->flags)) {
			res = -ENOTDIR;
			goto out;
		}
		if (dentry_is_under(np, s)) {
			res = -EINVAL;
			goto out;
		}
	} else if (dst.item && S_ISDIR(dst.item->flags)) {
		res = -EISDIR;
		goto out;
	}

	if (s->d_sb == NULL || s->d_sb != np->d_sb) {
		res = -EXDEV;
		goto out;
	}

	iops = s->d_sb->sb_iops;
	if (!iops->ino_rename && S_ISDIR(s->flags)) {
		res = -EXDEV;
		goto out;
	}

	if (dst.item) {
		res = dvfs_remove_locked(&dst,
		    S_ISDIR(s->flags) ? REMOVE_DIR : REMOVE_FILE);
		if (res) {
			goto out;
		}
		/* NAME points into the dying dentry, which dst still holds */
	}

	if (iops->ino_rename) {
		res = iops->ino_rename(s->d_inode, np->d_inode, name);
		if (res) {
			goto out;
		}

		/* The dentry moves: whoever holds it holds the renamed file */
		dvfs_cache_del(s);
		dlist_del_init(&s->children_lnk);
		strncpy(s->name, name, sizeof(s->name) - 1);
		s->name[sizeof(s->name) - 1] = '\0';

		old_parent = s->parent;
		if (old_parent != np) {
			dentry_ref_inc(np);
			s->parent = np;
		}
		dlist_add_prev(&s->children_lnk, &np->children);
		dvfs_cache_add(s);
		if (old_parent != np) {
			dentry_ref_dec(old_parent);
		}
	} else {
		res = dvfs_copy_file(s, &dst, name);
		if (res == 0) {
			res = dvfs_remove_locked(&src, REMOVE_FILE);
		}
	}

out:
	dvfs_lookup_put(&dst);
	dvfs_lookup_put(&src);
	dvfs_unlock();

	return res;
}
