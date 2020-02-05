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
#include <fs/inode.h>
#include <fs/hlpr_path.h>
#include <fs/fs_driver.h>
#include <fs/file_operation.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <fs/kfsop.h>
#include <fs/perm.h>
#include <security/security.h>

extern struct inode *kcreat(struct path *dir, const char *path, mode_t mode);

struct idesc *kopen(struct inode *node, int flag) {
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
		ops = nas->fs->sb_fops;
	} else {
		if (NULL == nas->fs->fs_drv) {
			SET_ERRNO(ENOSUPP);
			return NULL;
		}

		ops = nas->fs->fs_drv->file_op;

		if (S_ISCHR(node->i_mode)) {
			/* Note: we suppose this node belongs to devfs */
			idesc = ops->open(node, (void *) ((uintptr_t) flag));
			idesc->idesc_flags = flag;
			return idesc;
		}
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
	desc->f_ops = ops;

	if (desc->f_ops->open != NULL) {
		idesc = desc->f_ops->open(node, &desc->f_idesc);
		if (err(idesc)){
			ret = (uintptr_t)idesc;
			goto free_out;
		}
	} else {
		idesc = &desc->f_idesc;
	}

	if ((struct idesc *)idesc == &desc->f_idesc) {
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
	return &desc->f_idesc;
}


ssize_t kwrite(const void *buf, size_t size, struct file_desc *file) {
	ssize_t ret;

	if (!file) {
		ret = -EBADF;
		goto end;
	}

	if (idesc_check_mode(&file->f_idesc, O_RDONLY)) {
		ret = -EBADF;
		goto end;
	}

	if (NULL == file->f_ops->write) {
		ret = -EBADF;
		goto end;
	}

	if (file->file_flags & O_APPEND) {
		kseek(file, 0, SEEK_END);
	}

	ret = file->f_ops->write(file, (void *)buf, size);
	if (ret > 0) {
		file_set_pos(file, file_get_pos(file) + ret);
	}

end:
	return ret;
}

ssize_t kread(void *buf, size_t size, struct file_desc *desc) {
	ssize_t ret;

	if (NULL == desc) {
		ret = -EBADF;
		goto end;
	}

	if (idesc_check_mode(&desc->f_idesc, O_WRONLY)) {
		ret = -EBADF;
		goto end;
	}

	if (NULL == desc->f_ops->read) {
		ret = -EBADF;
		goto end;
	}

	/* Don't try to read past EOF */
	if (size > desc->f_inode->nas->fi->ni.size - file_get_pos(desc)) {
		size = desc->f_inode->nas->fi->ni.size - file_get_pos(desc);
	}

	ret = desc->f_ops->read(desc, buf, size);
	if (ret > 0) {
		file_set_pos(desc, file_get_pos(desc) + ret);
	}

end:
	return ret;
}


void kclose(struct file_desc *desc) {
	assert(desc);
	assert(desc->f_ops);

	if (desc->f_ops->close) {
		desc->f_ops->close(desc);
	}

	file_desc_destroy(desc);
}

int kseek(struct file_desc *desc, long int offset, int origin) {
	struct nas *nas;
	struct node_info *ni;
	off_t pos;

	pos = file_get_pos(desc);

	if (NULL == desc) {
		return -EBADF;
	}

	nas = desc->f_inode->nas;
	ni = &nas->fi->ni;

	switch (origin) {
		case SEEK_SET:
			pos = offset;
			break;

		case SEEK_CUR:
			pos += offset;
			break;

		case SEEK_END:
			pos = ni->size + offset;
			break;

		default:
			return -EINVAL;
	}

	file_set_pos(desc, pos);
	return pos;
}

int kfstat(struct file_desc *desc, struct stat *stat_buff) {
	if ((NULL == desc) || (stat_buff == NULL)) {
		return -EBADF;
	}

	kfile_fill_stat(desc->f_inode, stat_buff);

	return 0;
}

int kioctl(struct file_desc *desc, int request, void *data) {
	int ret;

	if (NULL == desc) {
		return -EBADF;
	}

	if (NULL == desc->f_ops->ioctl) {
		return -ENOSUPP;
	}

	ret = desc->f_ops->ioctl(desc, request, data);

	if (ret < 0) {
		return ret;
	}

	return 0;
}

int kftruncate(struct file_desc *desc, off_t length) {
	int ret;
	off_t pos;

	pos = file_get_pos(desc);

	ret = ktruncate(desc->f_inode, length);
	if (0 > ret) {
		/* XXX ktruncate sets errno */
		return -errno;
	}

	if (pos > length) {
		file_set_pos(desc, length);
	}

	return 0;
}
