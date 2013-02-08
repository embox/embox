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

#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <fs/fs_drv.h>
#include <fs/file_operation.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <fs/kfsop.h>


struct file_desc *kopen(const char *path, int flag) {
	struct node *node;
	struct nas *nas;
	struct file_desc *desc;
	const struct kfile_operations *ops;
	int ret;
	int path_len;

	assert(path);

	path_len = strlen(path);

	if (!path_len || path[path_len - 1] == '/') {
		return NULL; /* this can't be a directory */
	}

	if (NULL == (node = vfs_find_node(path, NULL))) {
		/* we try create file */
		/* check the mode */
		if ((O_WRONLY != flag) && (O_APPEND != flag)) {
			SET_ERRNO(ENOENT);
			return NULL;
		}

		/* create file */
		if (kcreat(NULL, path, 0) < 0) {
			return NULL;
		}
	}

	if (NULL == (node = vfs_find_node(path, NULL))) {
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
		ops = (struct kfile_operations *)nas->fs->drv->file_op;
	} else {
		ops = nas->fs->file_op;
	}

	/* allocate new descriptor */
	if (NULL == (desc = file_desc_alloc())) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	desc->node = node;
	desc->ops = ops;

	if ((flag & O_APPEND) && node_is_file(node)) {
		desc->cursor = kseek(desc, 0, SEEK_END);
	} else {
		desc->cursor = 0;
	}

	ret = desc->ops->open(node, desc, flag);
	if (ret < 0) {
		file_desc_free(desc);
		SET_ERRNO(-ret);
		return NULL;
	}

	return desc;
}

size_t kwrite(const void *buf, size_t size, struct file_desc *file) {
	size_t ret;
	struct node *node;

	if (NULL == file) {
		SET_ERRNO(EBADF);
		return -1;
	}

	node = file->node;
	if ((node != NULL) && (node->type & S_IREAD)) {
		SET_ERRNO(EPERM);
		return -1;
	}


	if (NULL == file->ops->write) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = file->ops->write(file, (void *)buf, size);
	if ((ssize_t)ret < 0) {
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

	if (NULL == desc->ops->read) {
		SET_ERRNO(EBADF);
		return -1;
	}

	ret = desc->ops->read(desc, buf, size);
	if ((ssize_t)ret < 0) {
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

	if (node_is_directory(node)) {
		stat_buff->st_mode = S_IFDIR;
		return 0;
	}
	if (node_is_file(node)){
		stat_buff->st_mode = S_IFREG;
	}

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
