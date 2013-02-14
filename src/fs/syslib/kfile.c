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
#include <fs/path.h>

static int perm_mask(struct node *node) {
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

static int perm_check(struct node *node, int fd_flags) {
	/* Here, we rely on the fact that fd_flags correspond to OTH perm bits. */
	return (fd_flags & ~perm_mask(node)) ? -EACCES : 0;
}

static int perm_lookup(const char *path, const char **pathlast, struct node **node, mode_t *mode) {
	struct node *child;
	size_t len = 0;
	int ret;

	if (path[0] == '/') {
		path = path_next(path, NULL);
	}
	*pathlast = path;

	*node = vfs_get_root();

	while (1) {
		*mode = perm_mask(*node);

		if (NULL == (path = path_next(path + len, &len))) {
			return 0;
		}

		if (0 != (ret = perm_check(*node, S_IXOTH))) {
			return ret;
		}

		if (NULL == (child = vfs_lookup_childn(*node, path, len))) {
			return -ENOENT;
		}

		*node = child;
		*pathlast = path + len + 1;
	}

	return 0;
}

struct file_desc *kopen(const char *path, int flag, mode_t mode) {
	struct node *node;
	struct nas *nas;
	struct file_desc *desc;
	const struct kfile_operations *ops;
	int ret;
	mode_t dirmode;
	int perm_flags;

	assert(path);
	ret = perm_lookup(path, &path, &node, &dirmode);

	if (-ENOENT == ret) {
		fs_drv_t *drv;
		struct node *child;

		if (!(flag & O_CREAT)) {
			SET_ERRNO(ENOENT);
			return NULL;
		}

		if (NULL != strchr(path, '/')) {
			SET_ERRNO(ENOENT);
			return NULL;
		}

		if (0 == (dirmode & S_IWOTH)) {
			SET_ERRNO(EACCES);
			return NULL;
		}

		child = vfs_create(node, path, S_IFREG | mode);

		if (!child) {
			SET_ERRNO(ENOMEM);
			return NULL;
		}

		/* check drv of parents */
		drv = node->nas->fs->drv;
		if (!drv || !drv->fsop->create_node) {
			SET_ERRNO(EBADF);
			vfs_del_leaf(node);
			return NULL;
		}

		if (0 != (ret = drv->fsop->create_node(node, child))) {
			SET_ERRNO(-ret);
			vfs_del_leaf(node);
			return NULL;
		}

		node = child;

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
	perm_flags = ((flag & O_WRONLY || flag & O_RDWR) ? FDESK_FLAG_WRITE : 0)
		| ((flag & O_WRONLY) ? 0 : FDESK_FLAG_READ);
	desc->flags = perm_flags | ((flag & O_APPEND) ? FDESK_FLAG_APPEND : 0);
	desc->cursor = 0;

	if (0 > (ret = perm_check(node, perm_flags))) {
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
	fs_drv_t *drv;

	nas = node->nas;
	drv = nas->fs->drv;

	if (NULL == drv->fsop->truncate) {
		errno = EPERM;
		return -1;
	}

	if (0 > (ret = perm_check(node, FDESK_FLAG_WRITE))) {
		SET_ERRNO(-ret);
		return -1;
	}

	if (node->mode & S_IFDIR) {
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

	if (!(file->flags & FDESK_FLAG_WRITE)) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (NULL == file->ops->write) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (file->flags & FDESK_FLAG_APPEND) {
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

	if (!(desc->flags & FDESK_FLAG_READ)) {
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
