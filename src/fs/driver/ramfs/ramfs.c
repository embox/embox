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

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/ramfs.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/path.h>

#include <util/math.h>
#include <util/err.h>

#include <embox/unit.h>
#include <embox/block_dev.h>
#include <drivers/ramdisk.h>

/* ramfs filesystem description pool */
POOL_DEF(ramfs_fs_pool, struct ramfs_fs_info, OPTION_GET(NUMBER,ramfs_descriptor_quantity));

/* ramfs file description pool */
POOL_DEF(ramfs_file_pool, struct ramfs_file_info, OPTION_GET(NUMBER,inode_quantity));

INDEX_DEF(ramfs_file_idx,0,OPTION_GET(NUMBER,inode_quantity));

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE OPTION_GET(NUMBER,ramfs_file_size)
#define FILESYSTEM_SIZE OPTION_GET(NUMBER,ramfs_filesystem_size)


#define RAMFS_DEV  "/dev/ram#"
#define RAMFS_DIR  "/"

static char sector_buff[PAGE_SIZE()];/* TODO */
static char ramfs_dev[] = RAMFS_DEV;

static int ramfs_format(void *path);
static int ramfs_mount(void *dev, void *dir);

static int ramfs_init(void * par) {
	struct path dir_node;
	struct node *dev_node;
	int res;
	ramdisk_t *ramdisk;

	if (!par) {
		return 0;
	}

	/*TODO */

	vfs_lookup(RAMFS_DIR, &dir_node);

	if (dir_node.node == NULL) {
		return -ENOENT;
	}

	if (err(ramdisk = ramdisk_create(ramfs_dev,
					FILESYSTEM_SIZE * PAGE_SIZE()))) {
		return err(ramdisk);
	}

	dev_node = ramdisk->bdev->dev_node;
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

EMBOX_UNIT_INIT(ramfs_ramdisk_fs_init); /*TODO*/


static int    ramfs_open(struct node *node, struct file_desc *file_desc, int flags);
static int    ramfs_close(struct file_desc *desc);
static size_t ramfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ramfs_write(struct file_desc *desc, void *buf, size_t size);
static int    ramfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations ramfs_fop = {
	.open = ramfs_open,
	.close = ramfs_close,
	.read = ramfs_read,
	.write = ramfs_write,
	.ioctl = ramfs_ioctl,
};

/*
 * file_operation
 */

static int ramfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct nas *nas;
	ramfs_file_info_t *fi;

	nas = node->nas;
	fi = (ramfs_file_info_t *)nas->fi->privdata;

	fi->pointer = desc->cursor;

	return 0;
}

static int ramfs_close(struct file_desc *desc) {
	return 0;
}

static int ramfs_read_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ramfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_read(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static int ramfs_write_sector(struct nas *nas, char *buffer,
		uint32_t count, uint32_t sector) {
	struct ramfs_fs_info *fsi;

	fsi = nas->fs->fsi;

	if(0 > block_dev_write(nas->fs->bdev, (char *) buffer,
			count * fsi->block_size, sector)) {
		return -1;
	}
	else {
		return count;
	}
}

static size_t ramfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct nas *nas = desc->node->nas;
	struct ramfs_fs_info *fsi = nas->fs->fsi;
	void *pbuf, *ebuf;

	pbuf = buf;
	ebuf = buf + min(nas->fi->ni.size - desc->cursor, size);
	while (pbuf - ebuf > 0) {
		blkno_t blk = desc->cursor / fsi->block_size;
		int offset = desc->cursor % fsi->block_size;
		int read_n;

		assert (blk < fsi->block_per_file);
		assert (blk < fsi->numblocks);

		assert(sizeof(sector_buff) == fsi->block_size);
		if(1 != ramfs_read_sector(nas, sector_buff, 1, blk)) {
			break;
		}

		read_n = min(fsi->block_size - offset, pbuf - ebuf);
		memcpy (buf, sector_buff + offset, read_n);

		desc->cursor += read_n;
		pbuf += read_n;
	}

	return pbuf - buf;
}

static size_t ramfs_write(struct file_desc *desc, void *buf, size_t size) {
	ramfs_file_info_t *fi;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;
	struct nas *nas;
	struct ramfs_fs_info *fsi;

	nas = desc->node->nas;
	fi = (ramfs_file_info_t *)nas->fi->privdata;
	fsi = nas->fs->fsi;

	bytecount = 0;

	fi->pointer = desc->cursor;
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
		/* shift the pointer */
		fi->pointer += cnt;
		if(end_pointer <= fi->pointer) {
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

static int ramfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

/*
static int ramfs_seek(void *file, long offset, int whence);
static int ramfs_stat(void *file, void *buff);

static int ramfs_fseek(void *file, long offset, int whence) {
	struct file_desc *desc;
	ramfs_file_info_t *fi;
	uint32_t curr_offset;

	curr_offset = offset;

	desc = (struct file_desc *) file;
	fi = (ramfs_file_info_t *)desc->node->fi;

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
static int ramfs_stat(void *file, void *buff) {
	struct file_desc *desc;
	ramfs_file_info_t *fi;
	stat_t *buffer;

	desc = (struct file_desc *) file;
	fi = (ramfs_file_info_t *)desc->node->fi;
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




static ramfs_file_info_t *ramfs_create_file(struct nas *nas) {
	ramfs_file_info_t *fi;
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

static int ramfs_format(void *dev) {
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

	fsi->block_per_file = MAX_FILE_SIZE;
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


static struct fsop_desc ramfs_fsop = {
	.init = ramfs_init,
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

