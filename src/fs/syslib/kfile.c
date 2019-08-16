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
#include <sys/types.h>

#include <util/err.h>

#include <fs/vfs.h>
#include <fs/hlpr_path.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <fs/kfsop.h>
#include <fs/perm.h>
#include <security/security.h>

extern struct node *kcreat(struct path *dir, const char *path, mode_t mode);

extern struct idesc *char_dev_open(struct node *node, int flags);

struct idesc *kopen(struct node *node, int flag) {
	struct nas *nas;
	struct file_desc *desc;
	const struct file_operations *ops;
	int ret;
	struct idesc *idesc;

	assert(node);
	assertf(!(flag & (O_CREAT | O_EXCL)), "use kcreat() instead kopen()");
	assertf(!(flag & O_DIRECTORY), "use mkdir() instead kopen()");


	nas = node->nas;
	/* if we try open a file (not special) we must have the file system */
	if (NULL == nas->fs) {
		SET_ERRNO(ENOSUPP);
		return NULL;
	}

	if (node_is_directory(node)) {
		ops = nas->fs->file_op;
	} else {
		if (NULL == nas->fs->drv) {
			SET_ERRNO(ENOSUPP);
			return NULL;
		}

		if (!node_is_file(node)) {
			return char_dev_open(node, flag);
		}

		ops = nas->fs->drv->file_op;
	}

	if(ops == NULL) {
		SET_ERRNO(ENOSUPP);
		return NULL;
	}

	desc = file_desc_create(node, flag);
	if (0 != err(desc)) {
		SET_ERRNO(-(uintptr_t)desc);
		return NULL;
	}
	desc->ops = ops;

	idesc = desc->ops->open(node, desc, flag);
	if (err(idesc)){
		ret = (uintptr_t)idesc;
		goto free_out;
	}
	if ((struct idesc *)idesc == &desc->idesc) {
		goto out;
	} else {
		file_desc_destroy(desc);
		return idesc;
	}

free_out:
	if (ret < 0) {
		file_desc_destroy(desc);
		SET_ERRNO(-ret);
		return NULL;
	}

out:
	return &desc->idesc;
}


ssize_t kwrite(const void *buf, size_t size, struct file_desc *file) {
	ssize_t ret;

	if (!file) {
		ret = -EBADF;
		goto end;
	}

	if (!idesc_check_mode(&file->idesc, S_IWOTH)) {
		ret = -EBADF;
		goto end;
	}

	if (NULL == file->ops->write) {
		ret = -EBADF;
		goto end;
	}

	if (file->file_flags & O_APPEND) {
		kseek(file, 0, SEEK_END);
	}

	ret = file->ops->write(file, (void *)buf, size);

end:
	return ret;
}

ssize_t kread(void *buf, size_t size, struct file_desc *desc) {
	ssize_t ret;

	if (NULL == desc) {
		ret = -EBADF;
		goto end;
	}

	if (!idesc_check_mode(&desc->idesc, S_IROTH)) {
		ret = -EBADF;
		goto end;
	}

	if (NULL == desc->ops->read) {
		ret = -EBADF;
		goto end;
	}

	ret = desc->ops->read(desc, buf, size);

end:
	return ret;
}


void kclose(struct file_desc *desc) {
	assert(desc);
	assert(desc->ops);
	assert(desc->ops->close);
	desc->ops->close(desc);

	file_desc_destroy(desc);
}

int kseek(struct file_desc *desc, long int offset, int origin) {
	struct nas *nas;
	struct node_info *ni;

	if (NULL == desc) {
		return -EBADF;
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
			return -EINVAL;
	}

	return desc->cursor;
}

int kfstat(struct file_desc *desc, struct stat *stat_buff) {
	if ((NULL == desc) || (stat_buff == NULL)) {
		return -EBADF;
	}

	kfile_fill_stat(desc->node, stat_buff);

	return 0;
}

int kioctl(struct file_desc *desc, int request, void *data) {
	int ret;

	if (NULL == desc) {
		return -EBADF;
	}

	if (NULL == desc->ops->ioctl) {
		return -ENOSUPP;
	}

	ret = desc->ops->ioctl(desc, request, data);

	if (ret < 0) {
		return ret;
	}

	return 0;
}

int kftruncate(struct file_desc *desc, off_t length) {
	int ret;

	ret = ktruncate(desc->node, length);
	if (0 > ret) {
		/* XXX ktruncate sets errno */
		return -errno;
	}

	if (desc->cursor > length) {
		desc->cursor = length;
	}

	return 0;
}
