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

int _open(const char *path, int __oflag) {
	struct idesc *idesc;
	struct idesc_table *it;
	struct inode *i_no;
	char *last_name;
	struct lookup lookup;
	int res;

	lookup.item = NULL;
	lookup.parent = NULL;

	if ((res = dvfs_lookup(path, &lookup))) {
		return SET_ERRNO(-res);
	}

	if (!lookup.item) {
		if (!(__oflag & O_CREAT)) {
			return SET_ERRNO(ENOENT);
		}

		last_name = strrchr(path, '/');
		last_name = (last_name) ? (last_name + 1) : (char *)path;

		if (dvfs_create_new(last_name, &lookup, __oflag)) {
			return SET_ERRNO(ENOSPC);
		}
	}

	i_no = lookup.item->d_inode;
	if (i_no == NULL) {
		return SET_ERRNO(ENOENT);
	}

	if (S_ISDIR(i_no->i_mode)) {
		if (!(__oflag & O_PATH)) {
			dentry_ref_dec(lookup.item);
			dvfs_destroy_dentry(lookup.item);
			return SET_ERRNO(EISDIR);
		}
		idesc = dvfs_file_open_idesc(&lookup, __oflag);
	}
	else {
		idesc = lookup.item->d_sb->sb_ops->open_idesc(&lookup, __oflag);
	}

	if (err(idesc)) {
		return SET_ERRNO(err(idesc));
	}

	idesc->idesc_flags = __oflag;
	it = task_resource_idesc_table(task_self());

	res = idesc_table_add(it, idesc, 0);
	if (res < 0) {
		idesc->idesc_ops->close(idesc);
		assert(0);
		//TODO free resources
		return SET_ERRNO(-res);
	}

	if (S_ISLNK(i_no->i_mode)) {
		char buff[NAME_MAX + 1];
		read(res, buff, sizeof(buff));
		close(res);
		return _open(buff, __oflag);
	}

	return res;
}

int open(const char *path, int __oflag, ...) {
	return _open(path, __oflag);
}
