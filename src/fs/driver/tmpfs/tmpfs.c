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
#include <limits.h>

#include <util/array.h>
#include <util/indexator.h>

#include <embox/unit.h>

#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>

#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>
#include <fs/tmpfs.h>

#include <util/err.h>


/* tmpfs filesystem description pool */
POOL_DEF(tmpfs_fs_pool, struct tmpfs_fs_info, OPTION_GET(NUMBER,tmpfs_descriptor_quantity));

/* tmpfs file description pool */
POOL_DEF(tmpfs_file_pool, struct tmpfs_file_info, OPTION_GET(NUMBER,inode_quantity));

INDEX_DEF(tmpfs_file_idx,0,OPTION_GET(NUMBER,inode_quantity));

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,tmpfs_file_size)
#define FILESYSTEM_SIZE OPTION_GET(NUMBER,tmpfs_filesystem_size)

#define TMPFS_NAME "tmpfs"
#define TMPFS_DEV  "/dev/ram#"
#define TMPFS_DIR  "/tmp"

static char sector_buff[PAGE_SIZE()];/* TODO */
static char tmpfs_dev[] = TMPFS_DEV;

static int tmpfs_format(void *path);
static int tmpfs_mount(void *dev, void *dir);

static int tmpfs_init(void * par) {
	struct path dir_path;
	struct node *dev_node;
	int res;
	struct ramdisk *ramdisk;

	if (!par) {
		return 0;
	}

	/*TODO */

	if (0 != vfs_lookup(TMPFS_DIR, &dir_path)) {
		return -ENOENT;
	}

	ramdisk = ramdisk_create(tmpfs_dev, FILESYSTEM_SIZE * PAGE_SIZE());
	if (0 != (res = err(ramdisk))) {
		return res;
	}

	dev_node = ramdisk->bdev->dev_vfs_info;
	if (!dev_node) {
		return -1;
	}

	/* format filesystem */
	if (0 != (res = tmpfs_format((void *) dev_node))) {
		return res;
	}

	/* mount filesystem */
	return tmpfs_mount(dev_node, dir_path.node);
}

static int tmp_ramdisk_fs_init(void) {
	return tmpfs_init(tmpfs_dev);
}

EMBOX_UNIT_INIT(tmp_ramdisk_fs_init); /*TODO*/


static struct idesc *tmpfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    tmpfs_close(struct file_desc *desc);
static size_t tmpfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t tmpfs_write(struct file_desc *desc, void *buf, size_t size);

static struct file_operations tmpfs_fop = {
	.open = tmpfs_open,
	.close = tmpfs_close,
	.read = tmpfs_read,
	.write = tmpfs_write,
};

/*
 * file_operation
 */

static struct idesc *tmpfs_open(struct node *node, struct file_desc *desc, int flags) {
	return &desc->idesc;
}

static int tmpfs_close(struct file_desc *desc) {
	return 0;
}

static int tmpfs_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct tmpfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_read_buffered(nas->fs->bdev, buffer,
			count * fsi->block_size, sector * fsi->block_size)) {
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

	if(0 > block_dev_write_buffered(nas->fs->bdev, buffer,
			count * fsi->block_size, sector * fsi->block_size)) {
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

	len = size;

	/* Don't try to read past EOF */
	if (len > nas->fi->ni.size - desc->cursor) {
		len = nas->fi->ni.size - desc->cursor;
	}

	end_pointer = desc->cursor + len;
	bytecount = 0;
	start_block = fi->index * fsi->block_per_file;

	while(len) {
		blk = desc->cursor / fsi->block_size;
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
		current = desc->cursor % fsi->block_size;

		/* set the counter how many bytes read from block */
		if(end_pointer - desc->cursor > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			}
			else {
				cnt = fsi->block_size;
			}
		}
		else {
			cnt = end_pointer - desc->cursor;
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
		desc->cursor += cnt;
		if(end_pointer >= desc->cursor) {
			break;
		}
	}

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

	len = size;
	end_pointer = desc->cursor + len;
	start_block = fi->index * fsi->block_per_file;

	while(1) {
		if(0 == fsi->block_size) {
			break;
		}
		blk = desc->cursor / fsi->block_size;
		/* check if block over the file */
		if(blk >= fsi->block_per_file) {
			bytecount = 0;
			break;
		}
		else {
			blk += start_block;
		}
		/* calculate pointer in scratch buffer */
		current = desc->cursor % fsi->block_size;

		/* set the counter how many bytes written in block */
		if(end_pointer - desc->cursor > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			}
			else {
				cnt = fsi->block_size;
			}
		}
		else {
			cnt = end_pointer - desc->cursor;
			/* over the block ? */
			if((current + cnt) > fsi->block_size) {
				cnt -= (current + cnt) % fsi->block_size;
			}
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
		desc->cursor += cnt;
		if(end_pointer <= desc->cursor) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (nas->fi->ni.size < desc->cursor) {
		nas->fi->ni.size = desc->cursor;
	}

	return bytecount;
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
	struct stat *buffer;

	desc = (struct file_desc *) file;
	fi = (tmpfs_file_info_t *)desc->node->fi;
	buffer = (struct stat *) buff;

	if (buffer) {
			memset(buffer, 0, sizeof(struct stat));

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
static int tmpfs_truncate(struct node *node, off_t length);

static struct fsop_desc tmpfs_fsop = {
	.init = tmpfs_init,
	.format = tmpfs_format,
	.mount = tmpfs_mount,
	.create_node = tmpfs_create,
	.delete_node = tmpfs_delete,

	.truncate = tmpfs_truncate,
};

static struct fs_driver tmpfs_driver = {
	.name = TMPFS_NAME,
	.file_op = &tmpfs_fop,
	.fsop = &tmpfs_fsop,
};

static tmpfs_file_info_t *tmpfs_create_file(struct nas *nas) {
	tmpfs_file_info_t *fi;
	size_t fi_index;

	fi = pool_alloc(&tmpfs_file_pool);
	if (!fi) {
		return NULL;
	}

	fi_index = index_alloc(&tmpfs_file_idx, INDEX_MIN);
	if (fi_index == INDEX_NONE) {
		pool_free(&tmpfs_file_pool, fi);
		return NULL;
	}

	fi->index = fi_index;
	nas->fi->ni.size = 0;

	return fi;
}

static int tmpfs_create(struct node *parent_node, struct node *node) {
	struct nas *nas;

	nas = node->nas;

	if (!node_is_directory(node)) {
		if (!(nas->fi->privdata = tmpfs_create_file(nas))) {
			return -ENOMEM;
		}
	}

	nas->fs = parent_node->nas->fs;

	return 0;
}

static int tmpfs_delete(struct node *node) {
	struct tmpfs_file_info *fi;
	struct nas *nas;

	nas = node->nas;
	fi = nas->fi->privdata;

	if (!node_is_directory(node)) {
		index_free(&tmpfs_file_idx, fi->index);
		pool_free(&tmpfs_file_pool, fi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int tmpfs_truncate(struct node *node, off_t length) {
	struct nas *nas = node->nas;

	if (length > MAX_FILE_SIZE * PAGE_SIZE()) {
		return -EFBIG;
	}

	nas->fi->ni.size = length;

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

	if (NULL == (dir_nas->fs = filesystem_create("tmpfs"))) {
		return -ENOMEM;
	}
	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&tmpfs_fs_pool))) {
		filesystem_free(dir_nas->fs);
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct tmpfs_fs_info));
	dir_nas->fs->fsi = fsi;

	fsi->block_per_file = MAX_FILE_SIZE;
	fsi->block_size = PAGE_SIZE();
	fsi->numblocks = block_dev(dev_fi->privdata)->size / PAGE_SIZE();

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&tmpfs_file_pool))) {
		return -ENOMEM;
	}
	memset(fi, 0, sizeof(struct tmpfs_file_info));
	fi->index = fi->mode = 0;
	dir_nas->fi->privdata = (void *) fi;

	return 0;
}

DECLARE_FILE_SYSTEM_DRIVER(tmpfs_driver);

