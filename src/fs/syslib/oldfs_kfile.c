/**
 * @file
 *
 * @date 06.08.09
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include <fs/inode.h>
#include <fs/file_desc.h>
#include <fs/kfile.h>
#include <fs/kfsop.h>

ssize_t kwrite(struct file_desc *file, const void *buf, size_t size) {
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

ssize_t kread(struct file_desc *desc, void *buf, size_t size) {
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
