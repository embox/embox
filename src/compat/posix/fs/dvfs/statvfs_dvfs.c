/**
 * @file
 *
 * @date 06.11.15
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
#include <fs/file_desc.h>
#include <drivers/block_dev.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/index_descriptor.h>
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

unsigned long fsname2fsid(const char *name) {
	if (0 == strcmp(name, "vfat")) {
		return FSID_VFAT;
	}
	if (0 == strcmp(name, "ext2")) {
		return FSID_EXT2;
	}
	if (0 == strcmp(name, "ext3")) {
		return FSID_EXT3;
	}
	if (0 == strcmp(name, "ext4")) {
		return FSID_EXT4;
	}
	return 0;
}

char *fsid2fsname(unsigned long f_sid) {
	switch(f_sid) {
	case FSID_VFAT:
		return "vfat";
	case FSID_EXT2:
		return "ext2";
	case FSID_EXT3:
		return "ext3";
	case FSID_EXT4:
		return "ext4";
	default:
		return "unknown";
	}

	return "unknown";
}

static void statvfs_fill_from_file(struct file_desc *file, struct statvfs *buf) {
	struct super_block *sb;

	assert(file);
	assert(file->f_inode);
	assert(file->f_inode->i_sb);

	sb = file->f_inode->i_sb;
	if (sb->bdev) {
		buf->f_bsize = buf->f_frsize = sb->bdev->block_size;
		buf->f_blocks = sb->bdev->block_size / sb->bdev->size;
		buf->f_fsid = fsname2fsid(sb->fs_drv->name);
	}
}

#if 0	
static struct super_block *dumb_fs_fill_sb(struct super_block *sb, struct file_desc *bdev) {
	const struct fs_driver *fs_drv;
	int res;

	assert(sb);
	assert(bdev);

	array_spread_foreach(fs_drv, fs_drivers_registry) {
		if (!fs_drv->fill_sb) {
			continue;
		}
		res = fs_drv->fill_sb(sb, bdev);
		if (!res) {
			sb->fs_drv = fs_drv;
			return sb;
		}
	}

	return NULL;
}
#endif	

static void statvfs_fill_from_bdev(struct file_desc *bdev, struct statvfs *buf) {
#if 0
	struct super_block sb_buf;
	struct super_block *sb;
	struct block_dev *dev;

	assert(bdev);
	memset(&sb_buf, 0, sizeof(struct super_block));

	sb = dumb_fs_fill_sb(&sb_buf, bdev);
	if (sb) {
		buf->f_fsid = fsname2fsid(sb->fs_drv->name);
		dev = bdev->f_inode->i_data;
		buf->f_bsize = buf->f_frsize = dev->size;
		buf->f_blocks = dev->size / dev->block_size;

	}
#endif	
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
		statvfs_fill_from_bdev((struct file_desc *)idesc, buf);
		break;
	case S_IFDIR:
	case S_IFREG:
		statvfs_fill_from_file((struct file_desc *)idesc, buf);
		break;
	default:
		return SET_ERRNO(EBADF);
	}

	return 0;
}
