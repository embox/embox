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
#include <unistd.h>
#include <fcntl.h>

#include <fs/fs_drv.h>
#include <fs/vfs.h>
#include <fs/tmpfs.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <drivers/ramdisk.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>

/* tmpfs filesystem description pool */
POOL_DEF(tmpfs_fs_pool, struct tmpfs_fs_info, OPTION_GET(NUMBER,tmpfs_descriptor_quantity));

/* tmpfs file description pool */
POOL_DEF(tmpfs_file_pool, struct tmpfs_file_info, OPTION_GET(NUMBER,tmpfs_inode_quantity));

INDEX_DEF(tmpfs_file_idx,0,OPTION_GET(NUMBER,tmpfs_inode_quantity));

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,tmpfs_file_size)
#define FILESYSTEM_SIZE OPTION_GET(NUMBER,tmpfs_filesystem_size)

#define TMPFS_NAME "tmpfs"
#define TMPFS_DEV  "/dev/ram0"
#define TMPFS_DIR  "/tmp"

static char sector_buff[PAGE_SIZE()];/* TODO */

static int tmpfs_format(void *path);
static int tmpfs_mount(void *dev, void *dir);

static int tmpfs_init(void * par) {
	struct node *dev_node, *dir_node;

	if(NULL == par) {
		return 0;
	}

	if (0 != ramdisk_create(TMPFS_DEV, FILESYSTEM_SIZE * PAGE_SIZE())) {
		return -1;
	}

	if((NULL == (dev_node = vfs_find_node(TMPFS_DEV, NULL))) ||
	  (NULL == (dir_node = vfs_add_path(TMPFS_DIR, NULL)))) {
		return -1;
	}
	dir_node->type = NODE_TYPE_DIRECTORY;

	/* format filesystem */
	if(0 != tmpfs_format((void *)dev_node)) {
		return -1;
	}

	/* mount filesystem */
	return tmpfs_mount(dev_node, dir_node);
}

static int tmp_ramdisk_fs_init(void) {
	return tmpfs_init(TMPFS_DEV);
}
EMBOX_UNIT_INIT(tmp_ramdisk_fs_init); /*TODO*/



static int    tmpfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    tmpfs_close(struct file_desc *desc);
static size_t tmpfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t tmpfs_write(struct file_desc *desc, void *buf, size_t size);
static int    tmpfs_ioctl(struct file_desc *desc, int request, va_list args);

static struct kfile_operations tmpfs_fop = {
		tmpfs_open,
		tmpfs_close,
		tmpfs_read,
		tmpfs_write,
		tmpfs_ioctl
};

/*
 * file_operation
 */


static int tmpfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct nas *nas;
	//char path [MAX_LENGTH_PATH_NAME];
	tmpfs_file_info_t *fi;

	nas = node->nas;
	fi = (tmpfs_file_info_t *)nas->fi->privdata;

	fi->mode = flags;
	if (O_WRONLY == fi->mode) {
		nas->fi->ni.size = 0;
	}

	fi->pointer = desc->cursor;

	return 0;
}

static int tmpfs_close(struct file_desc *desc) {
	return 0;
}

static int tmpfs_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct tmpfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static int tmpfs_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct tmpfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_write(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static size_t tmpfs_read(struct file_desc *desc, void *buf, size_t size) {
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;		/* start of file */
	struct nas *nas;
	struct tmpfs_fs_info *fsi;
	struct tmpfs_file_info *fi;

	nas = desc->node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	fi->pointer = desc->cursor;

	len = size;

	/* Don't try to read past EOF */
	if (len > nas->fi->ni.size - fi->pointer) {
		len = nas->fi->ni.size - fi->pointer;
	}

	end_pointer = fi->pointer + len;
	bytecount = 0;
	start_block = fi->index * fsi->block_per_file;

	while(len) {
		blk = fi->pointer / fsi->block_size;
		/* check if block over the file */
		if(blk >= fsi->block_per_file) {
			bytecount = 0;
			break;
		}
		else {
			blk += start_block;
		}
		/* check if block over the filesystem */
		if(blk >= fsi->numblocks) {
			bytecount = 0;
			break;
		}
		/* calculate pointer in scratch buffer */
		current = fi->pointer % fsi->block_size;

		/* set the counter how many bytes read from block */
		if(end_pointer - fi->pointer > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			}
			else {
				cnt = fsi->block_size;
			}
		}
		else {
			cnt = end_pointer - fi->pointer;
		}

		/* one 4096-bytes block read operation */
		if(1 != tmpfs_read_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		/* get data from block */
		memcpy (buf, sector_buff + current, cnt);

		bytecount += cnt;
		/* shift the pointer */
		fi->pointer += cnt;
		if(end_pointer >= fi->pointer) {
			break;
		}
	}

	desc->cursor = fi->pointer;
	return bytecount;
}


static size_t tmpfs_write(struct file_desc *desc, void *buf, size_t size) {
	tmpfs_file_info_t *fi;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;
	struct nas *nas;
	struct tmpfs_fs_info *fsi;

	nas = desc->node->nas;
	fi = (tmpfs_file_info_t *)nas->fi->privdata;
	fsi = nas->fs->fsi;

	bytecount = 0;

	/* Don't allow writes to a file that's open as readonly */
	if (!(fi->mode & O_WRONLY) && !(fi->mode & O_APPEND)) {
		return 0;
	}

	fi->pointer = desc->cursor;
	len = size;
	end_pointer = fi->pointer + len;
	start_block = fi->index * fsi->block_per_file;

	while(1) {
		if(0 == fsi->block_size) {
			return 0;
		}
		blk = fi->pointer / fsi->block_size;
		/* check if block over the file */
		if(blk >= fsi->block_per_file) {
			bytecount = 0;
			break;
		}
		else {
			blk += start_block;
		}
		/* calculate pointer in scratch buffer */
		current = fi->pointer % fsi->block_size;

		/* set the counter how many bytes written in block */
		if(end_pointer - fi->pointer > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			}
			else {
				cnt = fsi->block_size;
			}
		}
		else {
			cnt = end_pointer - fi->pointer;
		}

		/* one 4096-bytes block read operation */
		if(1 != tmpfs_read_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		/* set new data in block */
		memcpy (sector_buff + current, buf, cnt);

		/* write one block to device */
		if(1 != tmpfs_write_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		bytecount += cnt;
		/* shift the pointer */
		fi->pointer += cnt;
		if(end_pointer >= fi->pointer) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (nas->fi->ni.size < fi->pointer) {
		nas->fi->ni.size = fi->pointer;
	}

	desc->cursor = fi->pointer;
	return bytecount;
}

static int tmpfs_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}

/*
static int tmpfs_seek(void *file, long offset, int whence);
static int tmpfs_stat(void *file, void *buff);

static int tmpfs_fseek(void *file, long offset, int whence) {
	struct file_desc *desc;
	tmpfs_file_info_t *fi;
	uint32_t curr_offset;

	curr_offset = offset;

	desc = (struct file_desc *) file;
	fi = (tmpfs_file_info_t *)desc->node->fi;

	switch (whence) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		curr_offset += fi->pointer;
		break;
	case SEEK_END:
		curr_offset = fi->filelen + offset;
		break;
	default:
		return -1;
	}

	if (curr_offset > fi->filelen) {
		curr_offset = fi->filelen;
	}
	fi->pointer = curr_offset;
	return 0;
}
static int tmpfs_stat(void *file, void *buff) {
	struct file_desc *desc;
	tmpfs_file_info_t *fi;
	stat_t *buffer;

	desc = (struct file_desc *) file;
	fi = (tmpfs_file_info_t *)desc->node->fi;
	buffer = (stat_t *) buff;

	if (buffer) {
			memset(buffer, 0, sizeof(stat_t));

			buffer->st_mode = fi->mode;
			buffer->st_ino = fi->index;
			buffer->st_nlink = 1;
			buffer->st_dev = *(int *) fsi->bdev;
			buffer->st_atime = buffer->st_mtime = buffer->st_ctime = 0;
			buffer->st_size = fi->filelen;
			buffer->st_blksize = fsi->block_size;
			buffer->st_blocks = fsi->numblocks;
		}

	return fi->filelen;
}
*/


static int tmpfs_init(void * par);
static int tmpfs_format(void *path);
static int tmpfs_mount(void *dev, void *dir);
static int tmpfs_create(struct node *parent_node, struct node *node);
static int tmpfs_delete(struct node *node);

static fsop_desc_t tmpfs_fsop = {
		tmpfs_init,
		tmpfs_format,
		tmpfs_mount,
		tmpfs_create,
		tmpfs_delete
};

static fs_drv_t tmpfs_drv = {
	.name = TMPFS_NAME,
	.file_op = &tmpfs_fop,
	.fsop = &tmpfs_fsop
};

static tmpfs_file_info_t *tmpfs_create_file(struct nas *nas) {
	tmpfs_file_info_t *fi;

	if(NULL == (fi = pool_alloc(&tmpfs_file_pool))) {
		return NULL;
	}

	fi->index = index_alloc(&tmpfs_file_idx, INDEX_ALLOC_MIN);
	nas->fi->ni.size = fi->pointer = 0;

	return fi;
}

static int tmpfs_create(struct node *parent_node, struct node *node) {
	struct nas *nas, *parents_nas;
	int node_quantity;
	char path[MAX_LENGTH_PATH_NAME];

	parents_nas = parent_node->nas;

	if (node_is_directory(node)) {
		node_quantity = 3; /* need create . and .. directory */
	}
	else {
		node_quantity = 1;
	}
	vfs_get_path_by_node(node, path);

	for (int count = 0; count < node_quantity; count ++) {
		if(0 < count) {
			if(1 == count) {
				strcat(path, "/.");
			}
			else if(2 == count) {
				strcat(path, ".");
			}
			if(NULL == (node = vfs_add_path (path, NULL))) {
				return -ENOMEM;
			}
		}

		nas = node->nas;
		nas->fs = parents_nas->fs;
		/* don't need create fi for directory - take root node fi */
		nas->fi->privdata = parents_nas->fi->privdata;

		if((0 >= count) & (!node_is_directory(node))) {
			if(NULL == (nas->fi->privdata = tmpfs_create_file(nas))) {
				return -ENOMEM;
			}
		}
	}

	return 0;
}

static int tmpfs_delete(struct node *node) {
	struct tmpfs_file_info *fi;
	struct tmpfs_fs_info *fsi;
	node_t *pointnod;
	struct nas *nas;
	char path [MAX_LENGTH_PATH_NAME];

	nas = node->nas;
	fi = nas->fi->privdata;
	fsi = nas->fs->fsi;

	vfs_get_path_by_node(node, path);

	/* need delete "." and ".." node for directory */
	if (node_is_directory(node)) {

		strcat(path, "/.");
		pointnod = vfs_find_node(path, NULL);
		vfs_del_leaf(pointnod);

		strcat(path, ".");
		pointnod = vfs_find_node(path, NULL);
		vfs_del_leaf(pointnod);

		path[strlen(path) - 3] = '\0';
	}

	if (node_is_directory(node)) {
		index_free(&tmpfs_file_idx, fi->index);
		pool_free(&tmpfs_file_pool, fi);
	}

	/* root node - have fi, but haven't index*/
	if(0 == strcmp((const char *) path, (const char *) fsi->mntto)){
		pool_free(&tmpfs_fs_pool, fsi);
		pool_free(&tmpfs_file_pool, fi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int tmpfs_format(void *dev) {
	node_t *dev_node;
	struct nas *dev_nas;
	struct node_fi *dev_fi;

	if (NULL == (dev_node = dev)) {
		return -ENODEV;/*device not found*/
	}

	if(!node_is_block_dev(dev_node)) {
		return -ENODEV;
	}
	dev_nas = dev_node->nas;
	dev_fi = dev_nas->fi;

	if(MAX_FILE_SIZE > block_dev(dev_fi->privdata)->size / PAGE_SIZE()) {
		return -ENOSPC;
	}
	return 0;
}

static int tmpfs_mount(void *dev, void *dir) {
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct tmpfs_file_info *fi;
	struct tmpfs_fs_info *fsi;
	struct node_fi *dev_fi;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		return -ENODEV;
	}

	if (NULL == (dir_nas->fs = alloc_filesystem("tmpfs"))) {
		return -ENOMEM;
	}
	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&tmpfs_fs_pool))) {
		free_filesystem(dir_nas->fs);
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct tmpfs_fs_info));
	dir_nas->fs->fsi = fsi;
	vfs_get_path_by_node(dir_node, fsi->mntto);
	vfs_get_path_by_node(dev_node, fsi->mntfrom);
	fsi->block_per_file = MAX_FILE_SIZE;
	fsi->block_size = PAGE_SIZE();
	fsi->numblocks = block_dev(dev_fi->privdata)->size / PAGE_SIZE();

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&tmpfs_file_pool))) {
		return -ENOMEM;
	}
	memset(fi, 0, sizeof(struct tmpfs_file_info));
	fi->index = fi->mode = 0;
	fi->pointer = 0;
	dir_nas->fi->privdata = (void *) fi;

	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(tmpfs_drv);

