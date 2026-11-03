/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    29 Mar 2015
 */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <unistd.h>

#include <fs/dvfs.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <util/err.h>

/* Whether a descriptor made by open_idesc() holds the dentry it came from.
 * Files and block devices do (theirs is a file_desc, and kclose() puts it);
 * a character device answers with its own idesc and keeps nothing. */
static int idesc_holds_dentry(struct idesc *idesc) {
	extern const struct idesc_ops idesc_file_ops;
	extern struct idesc_ops idesc_bdev_ops;

	return idesc->idesc_ops == &idesc_file_ops
	    || idesc->idesc_ops == &idesc_bdev_ops;
}

int _open(const char *path, int __oflag) {
	struct idesc *idesc;
	struct idesc_table *it;
	struct inode *i_no;
	struct lookup lookup;
	struct super_block *sb;
	char last[NAME_MAX];
	int in_tree;
	int res;

	dvfs_lock();

	/* Resolve, create if asked, and take the reference in one hold of the
	 * lock, so two cores cannot both find a name missing and create it. */
	if ((res = dvfs_lookup_at(NULL, path, &lookup, last))) {
		dvfs_unlock();
		return SET_ERRNO(-res);
	}

	if (!lookup.item) {
		if (!(__oflag & O_CREAT)) {
			res = -ENOENT;
			goto err_put;
		}

		if ((res = dvfs_create_new(last, &lookup, S_IFREG))) {
			goto err_put;
		}
	} else if ((__oflag & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
		res = -EEXIST;
		goto err_put;
	}

	i_no = lookup.item->d_inode;

	if (S_ISDIR(lookup.item->flags)) {
		if (!(__oflag & O_PATH)) {
			res = -EISDIR;
			goto err_put;
		}
	} else {
		if (__oflag & O_DIRECTORY) {
			res = -ENOTDIR;
			goto err_put;
		}
		if (i_no == NULL) {
			res = -ENOENT;
			goto err_put;
		}
	}

	/* Only the file is kept from here on. */
	dentry_ref_dec(lookup.parent);
	lookup.parent = NULL;

	sb = lookup.item->d_sb;
	in_tree = S_ISDIR(lookup.item->flags)
	    || sb->sb_ops->open_idesc == dvfs_file_open_idesc;

	if (in_tree) {
		/* A file: opened under the lock, so an unlink cannot take the
		 * driver's data away between the lookup and the open. */
		idesc = dvfs_file_open_idesc(&lookup, __oflag);
		dvfs_unlock();
	} else {
		/* A device: its open may block and must not hold the whole tree.
		 * The dentry is held; devfs has no unlink. */
		dvfs_unlock();
		idesc = sb->sb_ops->open_idesc(&lookup, __oflag);
	}

	if (ptr2err(idesc)) {
		dentry_ref_dec(lookup.item);
		return SET_ERRNO(-ptr2err(idesc));
	}

	if (!idesc_holds_dentry(idesc)) {
		dentry_ref_dec(lookup.item);
	}

	idesc->idesc_flags = __oflag;
	it = task_resource_idesc_table(task_self());

	res = idesc_table_add(it, idesc, 0);
	if (res < 0) {
		idesc->idesc_ops->close(idesc);
		return SET_ERRNO(EMFILE);
	}

	if (i_no && S_ISLNK(i_no->i_mode)) {
		char buff[NAME_MAX + 1];
		int len;

		len = read(res, buff, sizeof(buff) - 1);
		close(res);
		if (len <= 0) {
			return SET_ERRNO(ENOENT);
		}
		buff[len] = '\0';
		return _open(buff, __oflag);
	}

	return res;

err_put:
	dvfs_lookup_put(&lookup);
	dvfs_unlock();
	return SET_ERRNO(-res);
}

int open(const char *path, int __oflag, ...) {
	return _open(path, __oflag);
}
