/**
 * @file
 * @brief
 *
 * @author  Denis Deryugin
 * @date    29 Mar 2015
 */
#include <errno.h>
#include <fcntl.h>

#include <util/err.h>

#include <fs/dvfs.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>

int open(const char *path, int __oflag, ...) {
	struct idesc *idesc;
	struct idesc_table *it;
	int res;
	struct lookup lookup = {0, 0};
	struct inode  *i_no;

	if ((res = dvfs_lookup(path, &lookup))) {
		return SET_ERRNO(-res);
	}

	if (!lookup.item) {
		if (__oflag & O_CREAT) {
			char *last_name = strrchr(path, '/');
			if (dvfs_create_new(last_name ? last_name + 1 : path, &lookup, __oflag)) {
				return SET_ERRNO(ENOSPC);
			}
		} else {
			return SET_ERRNO(ENOENT);
		}
	}

	i_no = lookup.item->d_inode;
	if (i_no == NULL)
		return SET_ERRNO(ENOENT);

	if ((i_no->flags & S_IFMT) == S_IFDIR) {
		dvfs_destroy_dentry(lookup.item);
		return SET_ERRNO(EISDIR);
	}
	idesc = lookup.item->d_sb->sb_ops->open_idesc(&lookup, __oflag);
	if (err(idesc)) {
		return SET_ERRNO(err(idesc));
	}
	switch(O_ACCESS_MASK & __oflag) {
	case O_RDONLY:
		idesc->idesc_amode = S_IROTH;
		break;
	case O_WRONLY:
		idesc->idesc_amode = S_IWOTH;
		break;
	case O_RDWR:
		idesc->idesc_amode = S_IROTH | S_IWOTH;
		break;
	default:
		idesc->idesc_amode = 0;
		break;
	}

	it = task_resource_idesc_table(task_self());
	res = idesc_table_add(it, idesc, 0);
	if (res < 0) {
		idesc->idesc_ops->close(idesc);
		assert(0);
		//TODO free resources
		return SET_ERRNO(-res);
	}

	dentry_ref_inc(lookup.item);

	return res;
}

