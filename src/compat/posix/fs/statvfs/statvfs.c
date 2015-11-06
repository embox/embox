/**
 * @file
 *
 * @data 06.11.15
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <stddef.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#include <fs/dvfs.h>
#include <fs/idesc.h>
#include <fs/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>

int statvfs(const char *path, struct statvfs *buf) {
	struct stat st_buf;
	int res;
	int fd;

	assert(buf);
	assert(path);
	res = stat(path, &st_buf);
	if (res == -1) {
		return -1;
	}
	switch(st_buf.st_mode & S_IFMT) {
	case S_IFBLK:
	case S_IFDIR:
	case S_IFREG:
		break;
	default:
		return SET_ERRNO(EBADF);
	}
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		/* errno already setup */
		return -1;
	}
	res = fstatvfs(fd, buf);

	close(fd);
	/* errno already setup */
	return res;
}

static void statvfs_fill_from_file(struct file *file, struct statvfs *buf) {
	struct super_block *sb;

	assert(file);
	assert(file->f_inode);
	assert(file->f_inode->i_sb);

	sb = file->f_inode->i_sb;
	buf->f_bsize = buf->f_frsize = sb->bdev->block_size;
	buf->f_blocks = sb->bdev->block_size / sb->bdev->size;
}

int fstatvfs(int fd, struct statvfs *buf) {
	struct stat st_buf;
	struct idesc *idesc;
	int res;

	assert(buf);

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}
	res = fstat(fd, &st_buf);
	if (res == -1) {
		return -1;
	}
	memset(buf, 0, sizeof(struct statvfs));

	switch(st_buf.st_mode & S_IFMT) {
	case S_IFBLK:
		break;
	case S_IFDIR:
	case S_IFREG:
		statvfs_fill_from_file((struct file *)idesc, buf);
		break;
	default:
		return SET_ERRNO(EBADF);
	}

	return 0;
}
