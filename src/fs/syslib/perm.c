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
	return (fd_flags & ~fs_perm_mask(node)) ? -EACCES : 0;
}

int fs_perm_lookup(struct node *root, const char *path, const char **pathlast,
		struct node **node) {
	struct node *child;
	size_t len = 0;
	int ret;

	if (path[0] == '/') {
		path = path_next(path, NULL);
	}

	if (pathlast != NULL) {
		*pathlast = path;
	}

	*node = root ? root : vfs_get_root();

	while (1) {
		if (NULL == (path = path_next(path + len, &len))) {
			return 0;
		}

		if (0 != (ret = fs_perm_check(*node, S_IXOTH))) {
			return ret;
		}

		if (NULL == (child = vfs_lookup_childn(*node, path, len))) {
			return -ENOENT;
		}

		*node = child;

		if (pathlast != NULL) {
			*pathlast = path_next(path + len, NULL);
		}
	}

	return 0;
}


