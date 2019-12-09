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
#include <mem/misc/pool.h>
#include <mem/phymem.h> /* PAGE_SIZE() */
#include <mem/page.h>

#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/file_system.h>
#include <fs/file_operation.h>
#include <fs/path.h>

#include <util/math.h>
#include <util/err.h>

#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include "ramfs.h"

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE   OPTION_GET(NUMBER, ramfs_file_size)
#define RAMFS_FILES     OPTION_GET(NUMBER, inode_quantity)
#define FILESYSTEM_SIZE (MAX_FILE_SIZE * RAMFS_FILES)

/* ramfs filesystem description pool */
POOL_DEF(ramfs_fs_pool, struct ramfs_fs_info, OPTION_GET(NUMBER,ramfs_descriptor_quantity));

/* ramfs file description pool */
POOL_DEF(ramfs_file_pool, struct ramfs_file_info, RAMFS_FILES);

INDEX_DEF(ramfs_file_idx, 0, RAMFS_FILES);

static char sector_buff[PAGE_SIZE()];/* TODO */
static char ramfs_dev[] = RAMFS_DEV;

static int ramfs_format(struct block_dev *bdev, void *priv);
static int ramfs_mount(void *dev, void *dir);

static int ramfs_init(void * par) {
	struct path dir_node;
	struct node *dev_node;
	int res;
	struct ramdisk *ramdisk;

	if (!par) {
		return 0;
	}

	/*TODO */

	vfs_lookup(RAMFS_DIR, &dir_node);

	if (dir_node.node == NULL) {
		return -ENOENT;
	}

	if (err(ramdisk = ramdisk_create(ramfs_dev, FILESYSTEM_SIZE))) {
		return err(ramdisk);
	}

	dev_node = ramdisk->bdev->dev_vfs_info;
	assert(dev_node != NULL);

	/* format filesystem */
	res = ramfs_format(dev_node->nas->fi->privdata, NULL);
	if (res != 0) {
		return res;
	}

	/* mount filesystem */
	return ramfs_mount(dev_node, dir_node.node);
}

static int ramfs_ramdisk_fs_init(void) {
	return ramfs_init(ramfs_dev);
>>>>>>> 3d11a6b0c... fs/drivers: Make `format()` function VFS-agnostic:src/fs/driver/ramfs/ramfs.c
}

static struct idesc *ramfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct nas *nas;
	struct ramfs_file_info *fi;

	nas = node->nas;
	fi = (struct ramfs_file_info *)nas->fi->privdata;

	fi->pointer = file_get_pos(desc);;

	return &desc->idesc;
}

static int ramfs_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ramfs_fs_info *fsi;
	size_t blksize;
	int blkno, sectorsize;

	fsi = nas->fs->fsi;

	blksize = block_dev_block_size(nas->fs->bdev);
	sectorsize = fsi->block_size;
	assert(blksize > 0 && sectorsize > 0);
	blkno = sector * (sectorsize / blksize);

	if(0 > block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, blkno)) {
		return -1;
	}
	else {
		return count;
	}
}

static int ramfs_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ramfs_fs_info *fsi;
	size_t blksize;
	int blkno, sectorsize;

	fsi = nas->fs->fsi;

	blksize = block_dev_block_size(nas->fs->bdev);
	sectorsize = fsi->block_size;
	assert(blksize > 0 && sectorsize > 0);
	blkno = sector * (sectorsize / blksize);

	if(0 > block_dev_write(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, blkno)) {
		return -1;
	}
	else {
		return count;
	}
}

static size_t ramfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct ramfs_fs_info *fsi;
	void *pbuf, *ebuf;
	struct block_dev *bdev;
	struct ramfs_file_info *fi;
	off_t pos;

	assert(desc);

	fsi = desc->node->nas->fs->fsi;
	assert(fsi);

	fi = desc->node->nas->fi->privdata;
	assert(fi);

	bdev = desc->node->nas->fs->bdev;
	assert(bdev);

	pos = file_get_pos(desc);

	pbuf = buf;
	ebuf = buf + min(file_get_size(desc) - pos, size);

	while (pbuf < ebuf) {
		blkno_t blk = pos / fsi->block_size;
		int offset = pos % fsi->block_size;
		int read_n;

		assert (blk < fsi->block_per_file);
		assert (blk < fsi->numblocks);

		assert(sizeof(sector_buff) == fsi->block_size);
		if(1 != ramfs_read_sector(desc->node->nas, sector_buff, 1, blk)) {
			break;
		}

		read_n = min(fsi->block_size - offset, ebuf - pbuf);
		memcpy (pbuf, sector_buff + offset, read_n);

		pos += read_n;
		pbuf += read_n;
	}

	return pbuf - buf;
}

static size_t ramfs_write(struct file_desc *desc, void *buf, size_t size) {
	struct ramfs_file_info *fi;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;
	struct nas *nas;
	struct ramfs_fs_info *fsi;
	off_t pos;

	pos = file_get_pos(desc);

	nas = desc->node->nas;
	fi = (struct ramfs_file_info *)nas->fi->privdata;
	fsi = nas->fs->fsi;

	bytecount = 0;

	fi->pointer = pos;
	len = size;
	end_pointer = fi->pointer + len;
	start_block = fi->index * fsi->block_per_file;

	while(1) {
		if(0 == fsi->block_size) {
			break;
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
			/* over the block ? */
			if((current + cnt) > fsi->block_size) {
				cnt -= (current + cnt) % fsi->block_size;
			}
		}

		/* one 4096-bytes block read operation */
		if(1 != ramfs_read_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		/* set new data in block */
		memcpy (sector_buff + current, buf, cnt);

		/* write one block to device */
		if(1 != ramfs_write_sector(nas, sector_buff, 1, blk)) {
			bytecount = 0;
			break;
		}
		bytecount += cnt;
		buf = (void*) (((uint8_t*) buf) + cnt);
		/* shift the pointer */
		fi->pointer += cnt;
		if(end_pointer <= fi->pointer) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (file_get_size(desc) < fi->pointer) {
		file_set_size(desc, fi->pointer);
	}

	return bytecount;
}

static struct file_operations ramfs_fop = {
	.open = ramfs_open,
	.close = ramfs_close,
	.read = ramfs_read,
	.write = ramfs_write,
};

static struct ramfs_file_info *ramfs_create_file(struct nas *nas) {
	struct ramfs_file_info *fi;
	size_t fi_index;

	fi = pool_alloc(&ramfs_file_pool);
	if (!fi) {
		return NULL;
	}

	fi_index = index_alloc(&ramfs_file_idx, INDEX_MIN);
	if (fi_index == INDEX_NONE) {
		pool_free(&ramfs_file_pool, fi);
		return NULL;
	}

	fi->index = fi_index;
	nas->fi->ni.size = fi->pointer = 0;

	return fi;
}

static int ramfs_create(struct node *parent_node, struct node *node) {
	struct nas *nas;

	nas = node->nas;

	if (!node_is_directory(node)) {
		if (!(nas->fi->privdata = ramfs_create_file(nas))) {
			return -ENOMEM;
		}
	}

	nas->fs = parent_node->nas->fs;

	return 0;
}

static int ramfs_delete(struct node *node) {
	struct ramfs_file_info *fi;
	struct nas *nas;

	nas = node->nas;
	fi = nas->fi->privdata;

	if (!node_is_directory(node)) {
		index_free(&ramfs_file_idx, fi->index);
		pool_free(&ramfs_file_pool, fi);
	}

	vfs_del_leaf(node);

	return 0;
}

static int ramfs_truncate(struct node *node, off_t length) {
	struct nas *nas = node->nas;

	if (length > MAX_FILE_SIZE) {
		return -EFBIG;
	}

	nas->fi->ni.size = length;

	return 0;
}

static int ramfs_format(struct block_dev *bdev, void *priv) {
	if (NULL == bdev) {
		return -ENODEV;
	}

	if (MAX_FILE_SIZE > bdev->size) {
		return -ENOSPC;
	}

	return 0;
}

static int ramfs_mount(void *dev, void *dir) {
	struct node *dir_node, *dev_node;
	struct nas *dir_nas, *dev_nas;
	struct ramfs_file_info *fi;
	struct ramfs_fs_info *fsi;
	struct node_fi *dev_fi;

	dev_node = dev;
	dev_nas = dev_node->nas;
	dir_node = dir;
	dir_nas = dir_node->nas;

	if (NULL == (dev_fi = dev_nas->fi)) {
		return -ENODEV;
	}

	if (NULL == (dir_nas->fs = filesystem_create("ramfs"))) {
		return -ENOMEM;
	}
	dir_nas->fs->bdev = dev_fi->privdata;

	/* allocate this fs info */
	if(NULL == (fsi = pool_alloc(&ramfs_fs_pool))) {
		filesystem_free(dir_nas->fs);
		return -ENOMEM;
	}
	memset(fsi, 0, sizeof(struct ramfs_fs_info));
	dir_nas->fs->fsi = fsi;

	fsi->block_per_file = MAX_FILE_SIZE / PAGE_SIZE();
	fsi->block_size = PAGE_SIZE();
	fsi->numblocks = block_dev(dev_fi->privdata)->size / PAGE_SIZE();

	/* allocate this directory info */
	if(NULL == (fi = pool_alloc(&ramfs_file_pool))) {
		return -ENOMEM;
	}
	memset(fi, 0, sizeof(struct ramfs_file_info));
	fi->index = fi->mode = 0;
	fi->pointer = 0;
	dir_nas->fi->privdata = (void *) fi;

	return 0;
}

static int ramfs_init(void * par);

static struct fsop_desc ramfs_fsop = {
	.format = ramfs_format,
	.mount = ramfs_mount,
	.create_node = ramfs_create,
	.delete_node = ramfs_delete,

	.truncate = ramfs_truncate,
};

static struct fs_driver ramfs_driver = {
	.name = "ramfs",
	.file_op = &ramfs_fop,
	.fsop = &ramfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_driver);

/****************/
EMBOX_UNIT_INIT(ramfs_ramdisk_fs_init); /*TODO*/

#define RAMFS_DEV  "/dev/ram#"
static char ramfs_dev[] = RAMFS_DEV;

static int ramfs_init(void * par) {
	struct path dir_node;
	struct node *dev_node;
	int res;
	struct ramdisk *ramdisk;

	if (!par) {
		return 0;
	}

	/*TODO */

	vfs_lookup(RAMFS_DIR, &dir_node);

	if (dir_node.node == NULL) {
		return -ENOENT;
	}

	if (err(ramdisk = ramdisk_create(ramfs_dev, FILESYSTEM_SIZE))) {
		return err(ramdisk);
	}

	dev_node = ramdisk->bdev->dev_vfs_info;
	assert(dev_node != NULL);

	/* format filesystem */
	res = ramfs_format(dev_node);
	if (res != 0) {
		return res;
	}

	/* mount filesystem */
	return ramfs_mount(dev_node, dir_node.node);
}

static int ramfs_ramdisk_fs_init(void) {
	return ramfs_init(ramfs_dev);
}
