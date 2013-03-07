/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fs/vfs.h>
#include <fs/path.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <fs/kfsop.h>
#include <fs/perm.h>
#include <security/security.h>

static struct node *kcreat(struct node *dir, const char *path, mode_t mode) {
	struct fs_driver *drv;
	struct node *child;
	int ret;

	if (NULL != strchr(path, '/')) {
		SET_ERRNO(ENOENT);
		return NULL;
	}

	if (0 != (fs_perm_check(dir, FS_MAY_WRITE))) {
		SET_ERRNO(EACCES);
		return NULL;
	}

	if (0 != (ret = security_node_create(dir, S_IFREG | mode))) {
		SET_ERRNO(-ret);
		return NULL;
	}

	child = vfs_create(dir, path, S_IFREG | mode);

	if (!child) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	/* check drv of parents */
	drv = dir->nas->fs->drv;
	if (!drv || !drv->fsop->create_node) {
		SET_ERRNO(EBADF);
		vfs_del_leaf(child);
		return NULL;
	}

	if (0 != (ret = drv->fsop->create_node(dir, child))) {
		SET_ERRNO(-ret);
		vfs_del_leaf(child);
		return NULL;
	}

	return child;
}

struct file_desc *kopen(const char *path, int flag, mode_t mode) {
	struct node *node;
	struct nas *nas;
	struct file_desc *desc;
	const struct kfile_operations *ops;
	int perm_flags, ret;

	assert(path);
	ret = fs_perm_lookup(NULL, path, &path, &node);

	if (-ENOENT == ret) {
		if (!(flag & O_CREAT)) {
			SET_ERRNO(ENOENT);
			return NULL;
		}

		if (NULL == (node = kcreat(node, path, mode))) {
			return NULL;
		}

	} else if (-EACCES == ret) {
		SET_ERRNO(EACCES);
		return NULL;
	} else if (ret == 0 && flag & O_CREAT && flag & O_EXCL) {
			SET_ERRNO(EEXIST);
			return NULL;
	}

	if (node_is_directory(node)) {
		SET_ERRNO(EISDIR);
		return NULL;
	}

	nas = node->nas;
	/* if we try open a file (not special) we must have the file system */
	if (NULL == nas->fs ) {
		SET_ERRNO(ENOSUPP);
		return NULL;
	}

	if (node_is_file(node)) {
		if (NULL == nas->fs->drv) {
			SET_ERRNO(ENOSUPP);
			return NULL;
		}
		ops = (struct kfile_operations *) nas->fs->drv->file_op;
	} else {
		ops = nas->fs->file_op;
	}

	if(ops == NULL) {
		SET_ERRNO(ENOSUPP);
		return NULL;
	}
	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	desc->node = node;
	desc->ops = ops;
	perm_flags = ((flag & O_WRONLY || flag & O_RDWR) ? FS_MAY_WRITE : 0)
		| ((flag & O_WRONLY) ? 0 : FS_MAY_READ);
	desc->flags = perm_flags | ((flag & O_APPEND) ? FS_MAY_APPEND : 0);
	desc->cursor = 0;

	if (0 > (ret = fs_perm_check(node, perm_flags))) {
		goto free_out;
	}

	if (0 > (ret = desc->ops->open(node, desc, flag))) {
		goto free_out;
	}

	if (flag & O_TRUNC) {
		/*if (0 > (ret = ktruncate(desc->node, 0))) { }*/
		ktruncate(desc->node, 0);
	}

free_out:
	if (ret < 0) {
		file_desc_free(desc);
		SET_ERRNO(-ret);
		return NULL;
	}

	return desc;
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

size_t kwrite(const void *buf, size_t size, struct file_desc *file) {
	size_t ret;

	if (!file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!(file->flags & FS_MAY_WRITE)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (NULL == file->ops->write) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (file->flags & FS_MAY_APPEND) {
		kseek(file, 0, SEEK_END);
	}

	ret = file->ops->write(file, (void *)buf, size);
	if ((ssize_t) ret < 0) {
		SET_ERRNO(-(ssize_t)ret);
		return -1;
	}

	return ret;
}

size_t kread(void *buf, size_t size, struct file_desc *desc) {
	size_t ret;

	if (NULL == desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (!(desc->flags & FS_MAY_READ)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (NULL == desc->ops->read) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = desc->ops->read(desc, buf, size);
	if ((ssize_t) ret < 0) {
		SET_ERRNO(-(ssize_t)ret);
		return -1;
	}

	return ret;
}


int kclose(struct file_desc *desc) {

	if (NULL == desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (NULL == desc->ops->close) {
		SET_ERRNO(EBADF);
		return -1;
	}

	desc->ops->close(desc);
	file_desc_free(desc);

	return 0;
}

int kseek(struct file_desc *desc, long int offset, int origin) {
	struct nas *nas;
	struct node_info *ni;

	if (NULL == desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	nas = desc->node->nas;
	ni = &nas->fi->ni;

	switch (origin) {
		case SEEK_SET:
			desc->cursor = offset;
			break;

		case SEEK_CUR:
			desc->cursor += offset;
			break;

		case SEEK_END:
			desc->cursor = ni->size + offset;
			break;

		default:
			SET_ERRNO(EINVAL);
			return -1;
	}

	if (desc->cursor > ni->size) {
		desc->cursor = ni->size;
	}

	return desc->cursor;
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

int kfstat(struct file_desc *desc, struct stat *stat_buff) {
	if ((NULL == desc) || (stat_buff == NULL)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	kfile_fill_stat(desc->node, stat_buff);

	return 0;
}

int kioctl(struct file_desc *desc, int request, ...) {
	int ret;
	va_list args;
	va_start(args, request);

	if (NULL == desc) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (NULL == desc->ops->ioctl) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = desc->ops->ioctl(desc, request, args);
	if (ret < 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}
