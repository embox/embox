/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <fs/tmpfs.h>
#include <fs/mount.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/device.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <fs/mount.h>
#include <drivers/ramdisk.h>
#include <cmd/mkfs.h>

/* tmpfs filesystem description pool */
POOL_DEF(tmpfs_fs_pool, struct tmpfs_fs_description, OPTION_GET(NUMBER,tmpfs_descriptor_quantity));

/* tmpfs file description pool */
POOL_DEF(tmpfs_file_pool, struct tmpfs_file_description, OPTION_GET(NUMBER,tmpfs_inode_quantity));

INDEX_DEF(tmpfs_file_idx,0,OPTION_GET(NUMBER,tmpfs_inode_quantity));

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,tmpfs_file_size)
#define MAX_FILESYSTEM_SIZE OPTION_GET(NUMBER,tmpfs_filesystem_size)

#define TMPFS_NAME "tmpfs"
#define TMPFS_DEV  "/dev/ram0"
#define TMPFS_DIR  "/tmp"

static fs_drv_t tmpfs_drv;
static file_operations_t tmpfs_fop;

static char sector_buff[PAGE_SIZE()];
static mkfs_params_t tmpfs_mkfs_params;
static mount_params_t mount_param;


static int tmpfs_format(void *path);
static int tmpfs_mount(void *par);

const fs_drv_t *tmpfs_get_fs(void) {
    return &tmpfs_drv;
}

static int tmpfs_init(void * par) {
	dev_ramdisk_t *ramdisk;
	mkfs_params_t *mkfs_params;

	if(NULL == par) {
		/* don't need init fs driver*/
		//mkfs_params = &tmpfs_mkfs_params;
		return 0;
	}
	else {
		mkfs_params = (mkfs_params_t *) par;
	}

	mkfs_params->blocks = MAX_FILESYSTEM_SIZE;
	mkfs_params->fs_type = 0;
	strcpy((void *)&mkfs_params->fs_name, TMPFS_NAME);
	strcpy((void *)&mkfs_params->path, TMPFS_DEV);

	if (0 != ramdisk_create((void *)mkfs_params)) {
		return -1;
	}

	mount_param.dev = TMPFS_DEV;
	mount_param.dir = TMPFS_DIR;

	if(NULL ==	(mount_param.dev_node =
			vfs_find_node(mount_param.dev, NULL))) {
		return -1;
	}
	/* set created ramdisc attribute from dev_node */
	ramdisk =
		(dev_ramdisk_t *)block_dev(mount_param.dev_node->dev_id)->privdata;

	/* format filesystem */
	if(0 != tmpfs_format((void *)&ramdisk->path)) {
		return -1;
	}

	/* mount filesystem */
	if(tmpfs_mount((void *) &mount_param)) {
		return -1;
	}
	return 0;
}

static int tmp_ramdisk_fs_init(void) {

	return tmpfs_init(&tmpfs_mkfs_params);
}
EMBOX_UNIT_INIT(tmp_ramdisk_fs_init);

static int tmpfs_format(void *path) {
	node_t *nod;
	tmpfs_fs_description_t *fs_des;
	tmpfs_file_description_t *fd;

	if (NULL == (nod = vfs_find_node((char *) path, NULL))) {
		return -ENODEV;/*device not found*/
	}

	if((NULL == (fs_des = pool_alloc(&tmpfs_fs_pool))) ||
			(NULL == (fd = pool_alloc(&tmpfs_file_pool)))) {
		return -ENOMEM;
	}
	fs_des->dev_id = nod->dev_id;
	strcpy((char *) fs_des->root_name, "\0");
	fs_des->vi.block_per_file = MAX_FILE_SIZE;
	fs_des->vi.block_size = PAGE_SIZE();
	fs_des->vi.numblocks = block_dev(nod->dev_id)->size / PAGE_SIZE();

	fd->fs = fs_des;
	nod->fs_type = &tmpfs_drv;
	nod->file_info = (void *) &tmpfs_fop;
	nod->fd = (void *)fd;

	return 0;
}

static int tmpfs_mount(void *par) {
	mount_params_t *params;
		node_t *dir_node, *dev_node;
		tmpfs_file_description_t *fd, *dev_fd;

		params = (mount_params_t *) par;
		dev_node = params->dev_node;
		if (NULL == (dir_node = vfs_find_node(params->dir, NULL))) {
			/*FIXME: usually mount doesn't create a directory*/
			if (NULL == (dir_node = vfs_add_path (params->dir, NULL))) {
				return -ENODEV;/*device not found*/
			}
			dir_node->properties = DIRECTORY_NODE_TYPE;
		}

		/* If dev_node created, but not attached to the filesystem driver */
		if (NULL == (dev_fd = (tmpfs_file_description_t *) dev_node->fd)) {
			if((NULL == (dev_fd = pool_alloc(&tmpfs_file_pool))) ||
					(NULL == (dev_fd->fs = pool_alloc(&tmpfs_fs_pool)))) {
				return -ENOMEM;
			}
			dev_node->fd = dev_fd;
			dev_fd->fs->dev_id = dev_node->dev_id;
			dev_node->file_info = (void *) &tmpfs_fop;
		}

		strcpy((char *) dev_fd->fs->root_name, params->dir);

		if(NULL == (fd = pool_alloc(&tmpfs_file_pool))) {
			return -ENOMEM;
		}

		fd->fs = dev_fd->fs;
		dir_node->fs_type = &tmpfs_drv;
		dir_node->fd = (void *) fd;

		return 0;//tmpfs_mount_files(dir_node);
}
static tmpfs_file_description_t *tmpfs_create_file(void *fs) {
	tmpfs_file_description_t *fd;

	if(NULL == (fd = pool_alloc(&tmpfs_file_pool))) {
		return NULL;
	}

	fd->fs = fs;
	fd->fi.index = index_alloc(&tmpfs_file_idx, INDEX_ALLOC_MIN);
	fd->fi.start_block = fd->fi.index * fd->fs->vi.block_per_file;
	fd->fi.block = fd->fi.start_block;
	fd->fi.filelen = fd->fi.pointer = 0;

	return fd;
}

static int tmpfs_create(void *params) {
	file_create_param_t *param;
	tmpfs_file_description_t *fd, *parents_fd;
	node_t *node, *parents_node;
	int node_quantity;

	param = (file_create_param_t *) params;

	node = (node_t *)param->node;
	parents_node = (node_t *)param->parents_node;
	parents_fd = (tmpfs_file_description_t *) parents_node->fd;

	if (DIRECTORY_NODE_TYPE == (node->properties & DIRECTORY_NODE_TYPE)) {
		node_quantity = 3; /* need create . and .. directory */
	}
	else {
		node_quantity = 1;
	}

	for (int count = 0; count < node_quantity; count ++) {
		if(0 < count) {
			if(1 == count) {
				strcat(param->path, "/.");
			}
			else if(2 == count) {
				strcat(param->path, ".");
			}
			if(NULL == (node = vfs_add_path (param->path, NULL))) {
				return -ENOMEM;
			}
		}

		node->fs_type = &tmpfs_drv;
		node->dev_id = parents_node->dev_id;
		/* don't need create node for directory */
		node->fd = parents_node->fd;

		if((0 >= count) & (DIRECTORY_NODE_TYPE !=
				(node->properties & DIRECTORY_NODE_TYPE))) {
			if(NULL == (fd = tmpfs_create_file(parents_fd->fs))) {
				return -ENOMEM;
			}
			node->fd = (void *) fd;
		}
	}
	/* cut /.. from end of PATH, if need */
	if (1 < node_quantity) {
		param->path[strlen(param->path) - 3] = '\0';
	}

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
	tmpfs_fileinfo_t *fileinfo;
	uint32_t curr_offset;

	curr_offset = offset;

	desc = (struct file_desc *) file;
	fd = (tmpfs_file_description_t *)desc->node->fd;
	fileinfo = &fd->fi;

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		curr_offset += fileinfo->pointer;
		break;
	case SEEK_END:
		curr_offset = fileinfo->filelen + offset;
		break;
	default:
		return -1;
	}

	if (curr_offset > fileinfo->filelen) {
		curr_offset = fileinfo->filelen;
	}
	fileinfo->pointer = curr_offset;
	return 0;
}

static void *tmpfs_fopen(struct file_desc *desc, const char *mode) {
	node_t *nod;
	//char path [MAX_LENGTH_PATH_NAME];
	tmpfs_file_description_t *fd;

	nod = desc->node;
	fd = (tmpfs_file_description_t *)nod->fd;

	if ('r' == *mode) {
		fd->fi.mode = O_RDONLY;
	}
	else if ('w' == *mode) {
		fd->fi.mode = O_WRONLY;
	}
	else {
		fd->fi.mode = O_RDONLY;
	}
	fd->fi.pointer = 0;

	return desc;
}

static int tmpfs_fclose(struct file_desc *desc) {
	return 0;
}

static int tmpfs_read_sector(void *fdsc, char *buffer,
		uint32_t count, uint32_t sector) {
	tmpfs_file_description_t *fd;

	fd = (tmpfs_file_description_t *) fdsc;

	if(0 > block_dev_read(fd->fs->dev_id, (char *) buffer,
			count * fd->fs->vi.block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static int tmpfs_write_sector(void *fdsc, char *buffer,
		uint32_t count, uint32_t sector) {
	tmpfs_file_description_t *fd;

	fd = (tmpfs_file_description_t *) fdsc;

	if(0 > block_dev_write(fd->fs->dev_id, (char *) buffer,
			count * fd->fs->vi.block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static size_t tmpfs_fread(void *buf, size_t size, size_t count, void *file) {
	struct file_desc *desc;
	tmpfs_file_description_t *fd;
	tmpfs_fileinfo_t *fileinfo;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;

	desc = (struct file_desc *) file;
	fd = (tmpfs_file_description_t *)desc->node->fd;
	fileinfo = &fd->fi;

	len = size * count;

	/* Don't try to read past EOF */
	if (len > fileinfo->filelen - fileinfo->pointer) {
		len = fileinfo->filelen - fileinfo->pointer;
	}

	end_pointer = fileinfo->pointer + len;
	bytecount = 0;

	while(len) {
		blk = fileinfo->pointer / fd->fs->vi.block_size;
		/* check if block over the file */
		if(blk >= fd->fs->vi.block_per_file) {
			bytecount = 0;
			break;
		}
		else {
			blk += fd->fi.start_block;
		}
		/* check if block over the filesystem */
		if(blk >= fd->fs->vi.numblocks) {
			bytecount = 0;
			break;
		}
		/* set pointer in scratch buffer */
		current = fileinfo->pointer % fd->fs->vi.block_size;

		/* set the counter how many bytes read from block */
		if(end_pointer - fileinfo->pointer > fd->fs->vi.block_size) {
			if(current) {
				cnt = fd->fs->vi.block_size - current;
			}
			else {
				cnt = fd->fs->vi.block_size;
			}
		}
		else {
			cnt = end_pointer - fileinfo->pointer;
		}

		/* one 4096-bytes block read operation */
		if(1 != tmpfs_read_sector(fd, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		/* get data from block */
		memcpy (buf, sector_buff + current, cnt);

		bytecount += cnt;
		/* shift the pointer */
		fileinfo->pointer += cnt;
		if(end_pointer >= fileinfo->pointer) {
			break;
		}
	}
	return bytecount;
}


static size_t tmpfs_fwrite(const void *buf, size_t size,
	size_t count, void *file) {
	struct file_desc *desc;
	tmpfs_file_description_t *fd;
	tmpfs_fileinfo_t *fileinfo;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;

	desc = (struct file_desc *) file;
	fd = (tmpfs_file_description_t *)desc->node->fd;
	fileinfo = &fd->fi;
	bytecount = 0;

	/* Don't allow writes to a file that's open as readonly */
	if (!(fileinfo->mode & O_WRONLY)) {
		return 0;
	}

	len = size * count;
	end_pointer = fileinfo->pointer + len;

	while(1) {
		blk = fileinfo->pointer / fd->fs->vi.block_size;
		/* check if block over the file */
		if(blk >= fd->fs->vi.block_per_file) {
			bytecount = 0;
			break;
		}
		else {
			blk += fd->fi.start_block;
		}
		/* set pointer in scratch buffer */
		current = fileinfo->pointer % fd->fs->vi.block_size;

		/* set the counter how many bytes written in block */
		if(end_pointer - fileinfo->pointer > fd->fs->vi.block_size) {
			if(current) {
				cnt = fd->fs->vi.block_size - current;
			}
			else {
				cnt = fd->fs->vi.block_size;
			}
		}
		else {
			cnt = end_pointer - fileinfo->pointer;
		}

		/* one 4096-bytes block read operation */
		if(1 != tmpfs_read_sector(fd, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		/* set new data in block */
		memcpy (sector_buff + current, buf, cnt);

		/* write one block to device */
		if(1 != tmpfs_write_sector(fd, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		bytecount += cnt;
		/* shift the pointer */
		fileinfo->pointer += cnt;
		if(end_pointer >= fileinfo->pointer) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (fileinfo->filelen < fileinfo->pointer) {
		fileinfo->filelen = fileinfo->pointer;
	}

	return bytecount;
}

static int tmpfs_ioctl(void *file, int request, va_list args) {
	return 0;
}

static fsop_desc_t tmpfs_fsop = { tmpfs_init, tmpfs_format, tmpfs_mount,
		tmpfs_create, tmpfs_delete};

static file_operations_t tmpfs_fop = { tmpfs_fopen, tmpfs_fclose, tmpfs_fread,
		tmpfs_fwrite, tmpfs_fseek, tmpfs_ioctl, NULL };

static fs_drv_t tmpfs_drv = {
	.name = TMPFS_NAME,
	.file_op = &tmpfs_fop,
	.fsop = &tmpfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(tmpfs_drv);

