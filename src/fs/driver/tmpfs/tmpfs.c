/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <stdio.h>

#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <fs/tmpfs.h>
#include <util/array.h>
#include <embox/device.h>
#include <embox/block_dev.h>

/* tmp filesystem description pool */
//POOL_DEF(tmp_fs_pool, struct tmpfs_fs_description, OPTION_GET(NUMBER,tmpfs_descriptor_quantity));

/* tmp file description pool */
//POOL_DEF(tmp_file_pool, struct tmpfs_file_description, OPTION_GET(NUMBER,tmpfs_inode_quantity));

static const fs_drv_t tmpfs_drv;
static file_operations_t tmpfs_fop;

static uint8_t sector_buff[SECTOR_SIZE];
static uint32_t bytecount;

const fs_drv_t *tmpfs_get_fs(void) {
    return &tmpfs_drv;
}

static int tmpfs_init(void * par) {
	return 0;
}

static int tmpfs_format(void *par) {
	return 0;
}

static int tmpfs_mount(void *par) {
	return 0;
}

static int tmpfs_create(void *params) {
	return 0;
}

static int tmpfs_delete(const char *fname) {
	return 0;
}

/*
 * file_operation
 */
static int tmpfs_fseek(void *file, long offset, int whence) {
	struct file_desc *desc;
	tmpfs_file_description_t *fd;
	uint32_t curr_offset;

	curr_offset = offset;

	desc = (struct file_desc *) file;
	fd = (tmpfs_file_description_t *)desc->node->fd;

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		curr_offset += fd->fi.pointer;
		break;
	case SEEK_END:
		curr_offset = fd->fi.filelen + offset;
		break;
	default:
		return -1;
	}

	//tmpfs_fseek(fd, curr_offset, sector_buff);
	return 0;
}

static uint32_t tmpfs_open_file(void *fdsc, uint8_t *path, uint8_t mode,
		uint8_t *p_scratch) {
	return 1;
}

static void *tmpfs_fopen(struct file_desc *desc, const char *mode) {
	node_t *nod;
	uint8_t _mode;
	uint8_t path [MAX_LENGTH_PATH_NAME];
	tmpfs_file_description_t *fd;

	nod = desc->node;
	fd = (tmpfs_file_description_t *)nod->fd;

	if ('r' == *mode) {
		_mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		_mode = O_WRONLY;
	}
	else {
		_mode = O_RDONLY;
	}

	set_path ((char *) path, nod);
	cut_mount_dir((char *) path, (char *) fd->fs->root_name);

	if(tmpfs_open_file(fd, (uint8_t *)path, _mode, sector_buff)) {
		return desc;
	}

	return NULL;
}

static int tmpfs_fclose(struct file_desc *desc) {
	return 0;
}

static uint32_t tmpfs_read_file(void *fdsc, uint8_t *p_scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len) {
	return 0;
}

static size_t tmpfs_fread(void *buf, size_t size, size_t count, void *file) {
	size_t size_to_read;
	struct file_desc *desc;
	size_t rezult;
	tmpfs_file_description_t *fd;

	size_to_read = size * count;
	desc = (struct file_desc *) file;
	fd = (tmpfs_file_description_t *)desc->node->fd;

	rezult = tmpfs_read_file(fd, sector_buff, buf, &bytecount, size_to_read);
	if (DFS_OK == rezult) {
		return bytecount;
	}
	return rezult;
}

static uint32_t tmpfs_write_file(void *fdsc, uint8_t *p_scratch,
		uint8_t *buffer, uint32_t *successcount, uint32_t len) {
	return 0;
}

static size_t tmpfs_fwrite(const void *buf, size_t size,
	size_t count, void *file) {
	size_t size_to_write;
	struct file_desc *desc;
	size_t rezult;
	tmpfs_file_description_t *fd;

	size_to_write = size * count;
	desc = (struct file_desc *) file;

	fd = (tmpfs_file_description_t *)desc->node->fd;

	rezult = tmpfs_write_file(fd, sector_buff, (uint8_t *)buf,
			&bytecount, size_to_write);
	if (DFS_OK == rezult) {
		return bytecount;
	}
	return rezult;
}

static int tmpfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

static fsop_desc_t tmpfs_fsop = { tmpfs_init, tmpfs_format, tmpfs_mount,
		tmpfs_create, tmpfs_delete};

static file_operations_t tmpfs_fop = { tmpfs_fopen, tmpfs_fclose, tmpfs_fread,
		tmpfs_fwrite, tmpfs_fseek, tmpfs_ioctl, NULL };

static const fs_drv_t tmpfs_drv = {
	.name = "tmpfs",
	.file_op = &tmpfs_fop,
	.fsop = &tmpfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(tmpfs_drv);
