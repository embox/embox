/**
 * @file
 *
 * @data 01.02.2016
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <unistd.h>

#include <fs/path.h>
#include <fs/vfs.h>
#include <fs/inode.h>

int chown(const char *path, uid_t owner_id, gid_t group_id) {
	struct path node_path;
	int ret;

	if (!path) {
		return SET_ERRNO(EFAULT);
	}

	ret = vfs_lookup(path, &node_path);
	if (ret) {
		return SET_ERRNO(ENOENT);
	}

	node_path.node->i_owner_id = owner_id;
	node_path.node->i_group_id = group_id;

	return 0;
}
