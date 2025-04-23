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

void kclose(struct file_desc *desc) {
	assert(desc);
	assert(desc->f_ops);

	if (desc->f_ops->close) {
		desc->f_ops->close(desc);
	}

	file_desc_destroy(desc);
}

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
	if (size > desc->f_inode->i_size - file_get_pos(desc)) {
		size = desc->f_inode->i_size - file_get_pos(desc);
	}

	ret = desc->f_ops->read(desc, buf, size);
	if (ret > 0) {
		file_set_pos(desc, file_get_pos(desc) + ret);
	}

end:
	return ret;
}

#include <drivers/block_dev.h> /* block_dev_block_size */
#include <string.h>

static int inode_fill_stat(struct inode *node, struct stat *sb) {
	memset(sb, 0 , sizeof(struct stat));

	sb->st_size = inode_size(node);
	sb->st_mode = node->i_mode;
	sb->st_uid = node->i_owner_id;
	sb->st_gid = node->i_group_id;
	sb->st_ctime = inode_ctime(node);
	sb->st_mtime = inode_mtime(node);
	sb->st_blocks = sb->st_size;

    if (node->i_sb->bdev) {
        sb->st_blocks /= block_dev_block_size(node->i_sb->bdev);
	    sb->st_blocks += (sb->st_blocks % block_dev_block_size(node->i_sb->bdev) != 0);	
    }

	return 0;
}

int kfstat(struct file_desc *desc, struct stat *sb) {
	if ((NULL == desc) || (sb == NULL)) {
		return -EBADF;
	}

	inode_fill_stat(desc->f_inode, sb);

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
