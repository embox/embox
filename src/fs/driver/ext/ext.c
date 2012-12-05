/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
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
#include <fs/ext.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
#include <drivers/ramdisk.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>

/* ext filesystem description pool */
POOL_DEF(ext_fs_pool, struct ext_fs_info, OPTION_GET(NUMBER,ext_descriptor_quantity));

/* ext file description pool */
POOL_DEF(ext_file_pool, struct ext_file_info, OPTION_GET(NUMBER,ext_inode_quantity));

INDEX_DEF(ext_file_idx,0,OPTION_GET(NUMBER,ext_inode_quantity));

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,ext_file_size)
#define FILESYSTEM_SIZE OPTION_GET(NUMBER,ext_filesystem_size)

#define EXT_NAME "ext2"
#define EXT_DEV  "/dev/ram0"
#define EXT_DIR  "/tmp"

static char sector_buff[PAGE_SIZE()];/* TODO */

static int ext_format(void *path);
static int ext_mount(void *dev, void *dir);

static int ext_init(void * par) {

	return 0;
}

static int    ext_open(struct node *node, struct file_desc *file_desc, int flags);
static int    ext_close(struct file_desc *desc);
static size_t ext_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext_write(struct file_desc *desc, void *buf, size_t size);
static int    ext_ioctl(struct file_desc *desc, int request, va_list args);

static struct kfile_operations ext_fop = {
		ext_open,
		ext_close,
		ext_read,
		ext_write,
		ext_ioctl
};

/*
 * file_operation
 */


static int ext_open(struct node *node, struct file_desc *desc, int flags) {

	return 0;
}

static int ext_close(struct file_desc *desc) {
	return 0;
}

static int ext_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ext_fs_info *fsi;
	fsi = nas->fs->fsi;

	return block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector);
}

static int ext_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ext_fs_info *fsi;
	fsi = nas->fs->fsi;

	return block_dev_write(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector);
}

static size_t ext_read(struct file_desc *desc, void *buf, size_t size) {
	struct nas *nas;

	nas = desc->node->nas;

	return ext_read_sector(nas, sector_buff, 1, 0);
}


static size_t ext_write(struct file_desc *desc, void *buf, size_t size) {
	struct nas *nas;

	nas = desc->node->nas;

	return ext_write_sector(nas, sector_buff, 1, 0);
}

static int ext_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}

static int ext_init(void * par);
static int ext_format(void *path);
static int ext_mount(void *dev, void *dir);
static int ext_create(struct node *parent_node, struct node *node);
static int ext_delete(struct node *node);

static fsop_desc_t ext_fsop = {
		ext_init,
		ext_format,
		ext_mount,
		ext_create,
		ext_delete
};

static fs_drv_t ext_drv = {
	.name = EXT_NAME,
	.file_op = &ext_fop,
	.fsop = &ext_fsop
};

static ext_file_info_t *ext_create_file(struct nas *nas) {
	ext_file_info_t *fi;

	if(NULL == (fi = pool_alloc(&ext_file_pool))) {
		return NULL;
	}

	fi->index = index_alloc(&ext_file_idx, INDEX_ALLOC_MIN);
	nas->fi->ni.size = fi->pointer = 0;

	return fi;
}

static int ext_create(struct node *parent_node, struct node *node) {
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
			if(NULL == (nas->fi->privdata = ext_create_file(nas))) {
				return -ENOMEM;
			}
		}
	}

	return 0;
}

static int ext_delete(struct node *node) {
	struct ext_file_info *fi;
	struct ext_fs_info *fsi;
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
		index_free(&ext_file_idx, fi->index);
		pool_free(&ext_file_pool, fi);
	}

	/* root node - have fi, but haven't index*/
	if(0 == strcmp((const char *) path, (const char *) fsi->mntto)){
		pool_free(&ext_fs_pool, fsi);
		pool_free(&ext_file_pool, fi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int ext_format(void *dev) {
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

static int ext_mount(void *dev, void *dir) {
	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(ext_drv);




