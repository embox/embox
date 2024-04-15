/**
 * @file
 * @brief Implementation of POSIX function "rename" for new VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-11-19
 */

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include <fs/dvfs.h>

#define FS_BUFFER_SZ 512

/**
 * @brief Change the name of the file. Actually it moves file from one location
 * to another
 *
 * @param src_name Old pathname of the file
 * @param dst_name New pathname of the file
 *
 * @return 0 if succeed and -1 if failed, errno is set
 */
int rename(const char *src_name, const char *dst_name) {
	struct lookup lu = {};
	struct dentry *from, *to, *dst_parent;
	char dst_parent_path[PATH_MAX + 1];
	int err;
	char buf[FS_BUFFER_SZ];
	int in, out;
	int pos;

	assert(src_name);
	assert(dst_name);

	/* TODO check if local name is not dot or double dot => EINVAL;
	 * 	check if dst is subdir of src => EINVAL */

	if ((err = dvfs_lookup(src_name, &lu))) {
		return SET_ERRNO(err);
	}
	from = lu.item;

	err = dvfs_lookup(dst_name, &lu);
	if (err != 0 && err != -ENOENT) {
		return SET_ERRNO(err);
	}
	to = lu.item;

	if (err == 0) {
		if (dvfs_remove(dst_name)) {
			return SET_ERRNO(EIO);
		}
	}

	if (!S_ISDIR(from->flags) && (to && S_ISDIR(to->flags))) {
		return SET_ERRNO(EISDIR);
	}

	strncpy(dst_parent_path, dst_name, sizeof(dst_parent_path) - 1);
	dst_parent_path[sizeof(dst_parent_path) - 1] = '\0';

	if ((err = dvfs_lookup(dirname(dst_parent_path), &lu))) {
		return SET_ERRNO(err);
	}

	dst_parent = lu.item;

	assert(dst_parent);
	assert(dst_parent->d_sb);
	assert(dst_parent->d_sb->sb_iops);

	if (dst_parent->d_sb == from->d_sb
	    && dst_parent->d_sb->sb_iops->ino_rename) {
		/* Same FS with rename support*/
		return dst_parent->d_sb->sb_iops->ino_rename(from->d_inode,
		    dst_parent->d_inode, basename((char *)dst_name));
	}
	else {
		/* Different FS or same FS without rename support */
		assert(from);
		assert(from->d_sb);
		assert(from->d_sb->sb_iops);

		if (!from->d_sb->sb_iops->ino_remove
		    || !dst_parent->d_sb->sb_iops->ino_create) {
			return SET_ERRNO(EROFS);
		}

		in = open(src_name, O_RDONLY);
		out = open(dst_name, O_WRONLY);
		pos = 0;

		if (!in || !out) {
			goto err_out;
		}

		while (pos < from->d_inode->i_size) {
			err = read(in, buf, sizeof(buf));
			if (err < 0) {
				goto err_out;
			}

			err = write(out, buf, err);
			if (err < 0) {
				goto err_out;
			}

			pos += FS_BUFFER_SZ;
		}

		dvfs_remove(src_name);
	}

	close(in);
	close(out);

	return 0;

err_out:
	close(in);
	close(out);

	return -1;
}
