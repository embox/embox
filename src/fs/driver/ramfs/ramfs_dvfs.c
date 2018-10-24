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

#include <fs/dvfs.h>

#include <util/array.h>
#include <util/indexator.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h> /* PAGE_SIZE() */

#include <util/math.h>
#include <util/err.h>

#include <embox/unit.h>
#include <drivers/block_dev.h>

/* define sizes in 4096 blocks */
#define MAX_FILE_SIZE   OPTION_GET(NUMBER, ramfs_file_size)
#define RAMFS_FILES     OPTION_GET(NUMBER, inode_quantity)
#define FILESYSTEM_SIZE (MAX_FILE_SIZE * RAM_FILES)

#define RAMFS_NAME_LEN	32

typedef struct ramfs_fs_info {
	uint32_t numblocks;			/* number of block in volume */
	uint32_t block_size;		/* size of block */
	uint32_t block_per_file;	/* max number of blocks filesize*/
} ramfs_fs_info_t;

typedef struct ramfs_file_info {
	int     index;		        /* number of file in FS*/
	int     mode;				/* mode in which this file was opened */
	uint32_t pointer;			/* current (BYTE) pointer */
	char    name[RAMFS_NAME_LEN];
	struct inode *inode;
	ramfs_fs_info_t *fsi;
} ramfs_file_info_t;

/* ramfs filesystem description pool */
POOL_DEF(ramfs_fs_pool, struct ramfs_fs_info, OPTION_GET(NUMBER,ramfs_descriptor_quantity));

struct ramfs_file_info ramfs_files[RAMFS_FILES];

INDEX_DEF(ramfs_file_idx, 0, RAMFS_FILES);

#define RAMFS_DIR  "/"

static char sector_buff[PAGE_SIZE()];/* TODO */

static int ramfs_close(struct file *desc) {
	return 0;
}

static size_t ramfs_read(struct file *desc, void *buf, size_t size) {
	struct ramfs_fs_info *fsi;
	void *pbuf, *ebuf;
	struct block_dev *bdev;
	ramfs_file_info_t *fi;

	assert(desc);
	assert(desc->f_inode);
	assert(desc->f_inode->i_sb);

	fsi = desc->f_inode->i_sb->sb_data;
	assert(fsi);

	fi = desc->f_inode->i_data;
	assert(fi);

	bdev = desc->f_inode->i_sb->bdev;
	assert(bdev);

	pbuf = buf;
	ebuf = buf + min(fi->inode->length - desc->pos, size);
	while (pbuf < ebuf) {
		assert(fsi->block_size != 0);

		blkno_t blk = desc->pos / fsi->block_size;
		int offset = desc->pos % fsi->block_size;
		int read_n;

		assert(blk < fsi->block_per_file);
		assert(blk < fsi->numblocks);

		assert(sizeof(sector_buff) == fsi->block_size);
		if (0 > block_dev_read(bdev, sector_buff,
					fsi->block_size, blk)) {
			break;
		}

		read_n = min(fsi->block_size - offset, ebuf - pbuf);
		memcpy (pbuf, sector_buff + offset, read_n);

		pbuf += read_n;
	}

	return pbuf - buf;
}

static size_t ramfs_write(struct file *desc, void *buf, size_t size) {
	ramfs_file_info_t *fi;
	size_t len;
	size_t current, cnt;
	uint32_t end_pointer;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;
	struct ramfs_fs_info *fsi;
	struct block_dev *bdev;

	assert(desc);
	assert(desc->f_inode);
	assert(desc->f_inode->i_sb);

	fi = desc->f_inode->i_data;
	assert(fi);

	fsi = desc->f_inode->i_sb->sb_data;
	assert(fsi);

	bdev = desc->f_inode->i_sb->bdev;
	assert(bdev);

	bytecount = 0;

	fi->pointer = desc->pos;
	len = size;
	end_pointer = fi->pointer + len;
	start_block = fi->index * fsi->block_per_file;

	while (1) {
		assert(fsi->block_size > 0);
		blk = fi->pointer / fsi->block_size;
		/* check if block over the file */
		if (blk >= fsi->block_per_file) {
			bytecount = 0;
			break;
		}

		blk += start_block;

		/* calculate pointer in scratch buffer */
		current = fi->pointer % fsi->block_size;

		/* set the counter how many bytes written in block */
		if (end_pointer - fi->pointer > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			} else {
				cnt = fsi->block_size;
			}
		} else {
			cnt = end_pointer - fi->pointer;
			/* over the block ? */
			if((current + cnt) > fsi->block_size) {
				cnt -= (current + cnt) % fsi->block_size;
			}
		}

		/* one 4096-bytes block read operation */
		if(0 > block_dev_read(bdev, sector_buff, fsi->block_size, blk)) {
			bytecount = 0;
			break;
		}

		/* set new data in block */
		memcpy (sector_buff + current, buf, cnt);

		/* write one block to device */
		if(0 > block_dev_write(bdev, sector_buff, fsi->block_size, blk)) {
			bytecount = 0;
			break;
		}
		bytecount += cnt;
		buf = (uint8_t*) buf + cnt;
		/* shift the pointer */
		fi->pointer += cnt;
		if(end_pointer <= fi->pointer) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (desc->f_inode->length < fi->pointer) {
		desc->f_inode->length = fi->pointer;
	}

	desc->pos = fi->pointer;
	return bytecount;
}

static int ramfs_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	struct ramfs_fs_info *fsi;
	int cur_id;

	assert(ctx);
	assert(next);
	assert(parent);
	assert(parent->i_sb);

	cur_id = (int) ctx->fs_ctx;
	fsi = parent->i_sb->sb_data;

	while (cur_id < RAMFS_FILES) {
		if (ramfs_files[cur_id].fsi != fsi) {
			cur_id++;
			continue;
		}

		next->i_data = &ramfs_files[cur_id];
		next->i_no = cur_id;
		ctx->fs_ctx = (void *) (cur_id + 1);
		return 0;
	}

	ctx->fs_ctx = NULL;
	return -1;
}

static int ramfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	ramfs_file_info_t *fi;
	size_t fi_index;

	assert(i_new);
	assert(i_new->i_dentry);
	assert(i_new->i_dentry->name);
	assert(i_dir);
	assert(i_dir->i_sb);
	assert(i_dir->i_sb->sb_data);

	fi_index = index_alloc(&ramfs_file_idx, INDEX_MIN);
	if (fi_index == INDEX_NONE) {
		return -ENOMEM;
	}

	fi = &ramfs_files[fi_index];
	memset(fi, 0, sizeof(*fi));

	strncpy(fi->name, i_new->i_dentry->name, sizeof(fi->name) - 1);

	fi->index = fi_index;
	fi->pointer = 0;
	fi->inode = i_new;
	fi->fsi = i_dir->i_sb->sb_data;

	i_new->i_data = fi;
	i_new->i_no = fi->index;
	return 0;
}

static int ramfs_truncate(struct inode *inode, size_t len) {
	ramfs_file_info_t *fi;

	assert(inode);

	if (len > MAX_FILE_SIZE) {
		return -EFBIG;
	}

	fi = inode->i_data;
	assert(fi);
	assert(fi->inode);
	fi->inode->length = len;

	return 0;
}

static struct inode *ramfs_ilookup(char const *name, struct dentry const *dir) {
	struct inode *node;
	struct super_block *sb;

	assert(dir);
	assert(dir->d_inode);
	assert(dir->d_inode->i_sb);
	assert(dir->d_inode->i_sb->sb_data);

	sb = dir->d_inode->i_sb;

	for (int i = 0; i < RAMFS_FILES; i++) {
		if (ramfs_files[i].fsi != sb->sb_data) {
			continue;
		}

		if (strcmp(name, ramfs_files[i].name)) {
			continue;
		}

		if (NULL == (node = dvfs_alloc_inode(sb))) {
			return NULL;
		}

		node->i_data = &ramfs_files[i];
		node->i_no = ramfs_files[i].index;

		return node;
	}

	return NULL;
}

static int ramfs_remove(struct inode *inode) {
	ramfs_file_info_t *fi;

	assert(inode);

	fi = inode->i_data;
	assert(fi);

	memset(fi, 0, sizeof(*fi));

	return 0;
}

/* Declaration of operations */
struct inode_operations ramfs_iops = {
	.create   = ramfs_create,
	.lookup   = ramfs_ilookup,
	.remove   = ramfs_remove,
	.iterate  = ramfs_iterate,
	//.pathname = ramfs_pathname,
	.truncate = ramfs_truncate,
};

struct file_operations ramfs_fops = {
	.close = ramfs_close,
	.write = ramfs_write,
	.read = ramfs_read,
};

static int ramfs_destroy_inode(struct inode *inode) {
	ramfs_file_info_t *fi;

	assert(inode);

	fi = inode->i_data;
	if (fi) {
		index_free(&ramfs_file_idx, fi->index);
	}

	return 0;
}

struct super_block_operations ramfs_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ramfs_destroy_inode,
};

static int ramfs_fill_sb(struct super_block *sb, struct file *bdev_file) {
	struct ramfs_fs_info *fsi;

	assert(sb);
	assert(bdev_file);
	assert(bdev_file->f_inode);
	assert(bdev_file->f_inode->i_data);

	if (NULL == (fsi = pool_alloc(&ramfs_fs_pool))) {
		return -ENOMEM;
	}

	memset(fsi, 0, sizeof(struct ramfs_fs_info));
	fsi->block_per_file = MAX_FILE_SIZE / PAGE_SIZE();
	fsi->block_size = PAGE_SIZE();
	fsi->numblocks = block_dev(bdev_file->f_inode->i_data)->size / PAGE_SIZE();

	sb->sb_data = fsi;
	sb->sb_iops = &ramfs_iops;
	sb->sb_fops = &ramfs_fops;
	sb->sb_ops  = &ramfs_sbops;

	return 0;
}

static int ramfs_mount_end(struct super_block *sb) {
	return 0;
}

static int ramfs_format(void *dev, void *priv) {
	return 0;
}

static const struct dumb_fs_driver ramfs_dumb_driver = {
	.name      = "ramfs",
	.fill_sb   = ramfs_fill_sb,
	.mount_end = ramfs_mount_end,
	.format    = ramfs_format,
};

ARRAY_SPREAD_DECLARE(const struct dumb_fs_driver *const, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &ramfs_dumb_driver);
