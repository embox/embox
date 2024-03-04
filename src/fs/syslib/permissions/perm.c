/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.02.2013
 */

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/hlpr_path.h>

#include <security/security.h>

int fs_perm_mask(struct inode *node) {
	int perm = node->i_mode & S_IRWXA;
	uid_t uid = getuid();

	if (uid == 0) {
		/* super user */
		return S_IRWXO;
	}

	if (node->i_owner_id == uid) {
		perm >>= 6;
	} else if (node->i_group_id == getgid()) {
		perm >>= 3;
	}
	perm &= S_IRWXO;

	return perm;
}

int fs_perm_check(struct inode *node, int fd_flags) {
	/* Here, we rely on the fact that fd_flags correspond to OTH perm bits. */
	return (fd_flags & ~fs_perm_mask(node)) ? -EACCES :
		security_node_permissions(node, fd_flags);
}

int fs_perm_lookup(const char *path, const char **pathlast,
		struct path *nodelast) {
	struct path node_path;
	struct path dir_path;
	size_t len = 0;
	int ret;

	if (!path) {
		return -EINVAL;
	}

	if (path[0] == '/') {
		vfs_get_root_path(&node_path);
	} else {
		vfs_get_leaf_path(&node_path);
	}

	do {
		struct inode *dnode;

		path = path_next(path + len, &len);

		*nodelast = node_path;

		if (pathlast != NULL) {
			*pathlast = path;
		}

		if (!path) {
			return 0;
		}

		if (0 != (ret = fs_perm_check(node_path.node, S_IXOTH))) {
			return ret;
		}

		dir_path = node_path;
		if_mounted_follow_down(&node_path);
		dnode = node_path.node;
		vfs_lookup_childn(&node_path, path, len, &node_path);

		if (NULL == node_path.node) {
			if (dnode && dnode->i_ops && dnode->i_ops->ino_lookup) {
				dnode->i_ops->ino_lookup(path, dir_path.node);
			} else {
				return -ENOENT;
			}
			vfs_lookup_childn(&dir_path, path, len, &node_path);
		}

	} while (node_path.node);

	return -ENOENT;
}

int fs_perm_lookup_relative(const char *path, const char **pathlast,
		struct path *nodelast) {
	int ret = 0;

	if (0 != (ret = fs_perm_lookup(path, pathlast, nodelast))) {
		return ret;
	}

	if (!S_ISDIR(nodelast->node->i_mode)) {
		return 0;
	}

	return 0;
}
