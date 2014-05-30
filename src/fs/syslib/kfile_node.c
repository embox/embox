/**
 * @file
 *
 * @date Oct 24, 2013
 * @author: Anton Bondarev
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fs/hlpr_path.h>
#include <fs/perm.h>
#include <fs/flags.h>
#include <security/security.h>
#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>

int kcreat(struct path *dir_path, const char *path, mode_t mode, struct path *child) {
	struct fs_driver *drv;
	int ret;

	assert(dir_path->node);

	path = path_next(path, NULL);

	if (!path) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	if (NULL != strchr(path, '/')) {
		SET_ERRNO(ENOENT);
		return -1;
	}

	if (0 != (fs_perm_check(dir_path->node, FS_MAY_WRITE))) {
		SET_ERRNO(EACCES);
		return -1;
	}

	if (0 != (ret = security_node_create(dir_path->node, S_IFREG | mode))) {
		SET_ERRNO(-ret);
		return -1;
	}

	if (0 != vfs_create(dir_path, path, S_IFREG | mode, child)) {
		SET_ERRNO(ENOMEM);
		return -1;
	}

	child->mnt_desc = dir_path->mnt_desc;
	// XXX should it be here?
	child->node->uid = getuid();
	child->node->gid = getgid();

	if(!dir_path->node->nas || !dir_path->node->nas->fs) {
		SET_ERRNO(EBADF);
		vfs_del_leaf(child->node);
		return -1;
	}

	/* check drv of parents */
	drv = dir_path->node->nas->fs->drv;
	if (!drv || !drv->fsop->create_node) {
		SET_ERRNO(EBADF);
		vfs_del_leaf(child->node);
		return -1;
	}

	if (0 != (ret = drv->fsop->create_node(dir_path->node, child->node))) {
		SET_ERRNO(-ret);
		vfs_del_leaf(child->node);
		return -1;
	}


	return 0;
}

int ktruncate(struct node *node, off_t length) {
	int ret;
	struct nas *nas;
	struct fs_driver *drv;

	nas = node->nas;
	drv = nas->fs->drv;

	if (NULL == drv->fsop->truncate) {
		errno = EPERM;
		return -1;
	}

	if (0 > (ret = fs_perm_check(node, FS_MAY_WRITE))) {
		SET_ERRNO(-ret);
		return -1;
	}

	if (node_is_directory(node)) {
		SET_ERRNO(EISDIR);
		return -1;
	}

	if (0 > (ret = drv->fsop->truncate(node, length))) {
		SET_ERRNO(-ret);
		return -1;
	}

	return ret;
}

int kfile_fill_stat(struct node *node, struct stat *stat_buff) {
	struct nas *nas;
	struct node_info *ni;

	memset(stat_buff, 0 , sizeof(struct stat));

	nas = node->nas;
	ni = &nas->fi->ni;

	stat_buff->st_size = ni->size;
	stat_buff->st_mode = node->mode;
	stat_buff->st_uid = node->uid;
	stat_buff->st_gid = node->gid;

	return 0;
}
