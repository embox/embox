/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.02.2013
 */

#include <errno.h>
#include <unistd.h>
#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/flags.h>

#include <security/security.h>

int fs_perm_mask(struct node *node) {
	int perm = node->mode & S_IRWXA;
	uid_t uid = getuid();

	if (uid == 0) {
		/* super user */
		return S_IRWXO;
	}

	if (node->uid == uid) {
		perm >>= 6;
	} else if (node->gid == getgid()) {
		perm >>= 3;
	}
	perm &= S_IRWXO;

	return perm;
}

int fs_perm_check(struct node *node, int fd_flags) {
	/* Here, we rely on the fact that fd_flags correspond to OTH perm bits. */
	return (fd_flags & ~fs_perm_mask(node)) ? -EACCES :
		security_node_permissions(node, fd_flags);
}

int fs_perm_lookup(struct node *root, const char *path, const char **pathlast,
		struct node **nodelast) {
	struct node *node;
	size_t len = 0;
	int ret;

	if (!path) {
		return -EINVAL;
	}

	node = root ? root : vfs_get_leaf();

	while (1) {

		path = path_next(path + len, &len);

		*nodelast = node;

		if (pathlast != NULL) {
			*pathlast = path;
		}

		if (!path) {
			return 0;
		}

		if (0 != (ret = fs_perm_check(node, FS_MAY_EXEC))) {
			return ret;
		}

		if (NULL == (node = vfs_lookup_childn(node, path, len))) {
			return -ENOENT;
		}

	}

	return 0;
}
