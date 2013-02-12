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

static int check_perm(struct node *node, int fd_flags) {
	int perm = node->mode & S_IRWXA;
	uid_t uid = getuid();

	if (uid == 0) {
		/* super user */
		return 0;
	}

	if (node->uid == uid) {
		perm >>= 6;
	} else if (node->gid == getgid()) {
		perm >>= 3;
	}
	perm &= S_IRWXO;

	/* Here, we rely on the fact that fd_flags correspond to OTH perm bits. */
	return (fd_flags & ~perm) ? -EACCES : 0;
}

struct file_desc *kopen(const char *path, int flag, mode_t mode) {
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

	if (NULL == (node = vfs_lookup(NULL, path))) {
		char tpath[MAX_LENGTH_PATH_NAME];
		char node_name[MAX_LENGTH_FILE_NAME];
		fs_drv_t *drv;
		size_t path_offset, path_len, name_len;
		struct node *node, *parent;

		if (!(flag & O_CREAT)) {
			SET_ERRNO(ENOENT);
			return NULL;
		}

		/* get last exist node */
		node = vfs_get_exist_path(path, tpath, sizeof(tpath));
		if (NULL == node) {
			SET_ERRNO(ENOENT);
			return NULL;
		}

		mode &= S_IRWXU | S_IRWXG | S_IRWXO; /* leave only permission bits */

		path_len = strlen(path);
		path_offset = strlen(tpath);

		while (1) {
			path_get_next_name(path + path_offset, node_name, sizeof(node_name));
			name_len = strlen(node_name);

			if (path_offset + name_len + 1 < path_len) {
				path_offset += name_len + 1;
			} else {
				break;
			}

			if (-1 == kmkdir(node, node_name, mode)) {
				return NULL;
			}

			node = vfs_lookup_child(node, node_name);
			assert(node);
		}

		parent = node;
		node = vfs_create(node, node_name, S_IFREG | mode);

		if (!node) {
			SET_ERRNO(ENOMEM);
			return NULL;
		}

		/* check drv of parents */
		drv = parent->nas->fs->drv;
		if (!drv || !drv->fsop->create_node) {
			SET_ERRNO(EBADF);
			vfs_del_leaf(node);
			return NULL;
		}

		if (0 != (ret = drv->fsop->create_node(parent, node))) {
			SET_ERRNO(-ret);
			vfs_del_leaf(node);
			return NULL;
		}

	} else if (flag & O_CREAT && flag & O_EXCL) {
			SET_ERRNO(EEXIST);
			return NULL;
	}

	if (NULL == (node = vfs_lookup(NULL, path))) {
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
	desc->flags = ((flag & O_WRONLY || flag & O_RDWR) ? FDESK_FLAG_WRITE : 0)
		| ((flag == O_RDONLY || flag & O_RDWR) ? FDESK_FLAG_READ : 0)
		| ((flag & O_APPEND) ? FDESK_FLAG_APPEND : 0);
	desc->cursor = 0;

	if (0 > (ret = check_perm(node, desc->flags))) {
		goto free_out;
	}

	if (0 > (ret = desc->ops->open(node, desc, flag))) {
		goto free_out;
	}

free_out:
	if (ret < 0) {
		file_desc_free(desc);
		SET_ERRNO(-ret);
		return NULL;
	}

	return desc;
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
