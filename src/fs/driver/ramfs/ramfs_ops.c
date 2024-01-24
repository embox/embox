#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>

#include <drivers/block_dev.h>

#include <fs/file_desc.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/mount.h>
#include <fs/super_block.h>
#include <fs/dir_context.h>

#include <mem/misc/pool.h>
#include <mem/page.h>

#include <lib/libds/indexator.h>
#include <util/math.h>

#include "ramfs.h"

struct ramfs_file_info ramfs_files[RAMFS_FILES];

INDEX_DEF(ramfs_file_idx, 0, RAMFS_FILES);

static char sector_buff[RAMFS_BUFFER_SIZE];

static size_t ramfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct ramfs_file_info *fi;
	struct ramfs_fs_info *fsi;
	struct block_dev *bdev;
	void *pbuf, *ebuf;
	off_t pos;

	assert(desc);

	fi = file_get_inode_data(desc);
	assert(fi);

	fsi = fi->fsi;
	assert(fsi);

	bdev = fsi->bdev;
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

		blk += fi->index * fsi->block_per_file;

		assert(sizeof(sector_buff) >= fsi->block_size);
		if (0 > block_dev_read(bdev, sector_buff,
					fsi->block_size, blk)) {
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
	uint32_t end_pos;
	blkno_t blk;
	uint32_t bytecount;
	uint32_t start_block;
	struct ramfs_fs_info *fsi;
	struct block_dev *bdev;
	off_t pos;

	fi = file_get_inode_data(desc);
	fsi = fi->fsi;
	bdev = fsi->bdev;

	pos = file_get_pos(desc);

	bytecount = 0;

	len = size;
	end_pos = pos + len;
	start_block = fi->index * fsi->block_per_file;

	while(1) {
		if (0 == fsi->block_size) {
			break;
		}

		blk = pos / fsi->block_size;
		/* check if block over the file */
		if(blk >= fsi->block_per_file) {
			bytecount = 0;
			break;
		} else {
			blk += start_block;
		}
		/* calculate pointer in scratch buffer */
		current = pos % fsi->block_size;

		/* set the counter how many bytes written in block */
		if(end_pos - pos > fsi->block_size) {
			if(current) {
				cnt = fsi->block_size - current;
			} else {
				cnt = fsi->block_size;
			}
		} else {
			cnt = end_pos - pos;
			/* over the block ? */
			if((current + cnt) > fsi->block_size) {
				cnt -= (current + cnt) % fsi->block_size;
			}
		}

		/* one block read operation */
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
		buf = (void*) (((uint8_t*) buf) + cnt);
		/* shift the pointer */
		pos += cnt;
		if (end_pos <= pos) {
			break;
		}
	}
	/* if we write over the last EOF, set new filelen */
	if (file_get_size(desc) < pos) {
		file_set_size(desc, pos);
	}

	fi->length = file_get_size(desc);

	return bytecount;
}

/* ramfs filesystem description pool */
POOL_DEF(ramfs_fs_pool, struct ramfs_fs_info, RAMFS_DESCRIPTORS);

extern struct inode_operations ramfs_iops;
extern struct super_block_operations ramfs_sbops;

int ramfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	struct ramfs_fs_info *fsi;
	int cur_id;

	assert(ctx);
	assert(next);
	assert(parent);
	assert(parent->i_sb);

	cur_id = (int) (uintptr_t)ctx->fs_ctx;
	fsi = parent->i_sb->sb_data;

	while (cur_id < RAMFS_FILES) {
		if (ramfs_files[cur_id].fsi != fsi) {
			cur_id++;
			continue;
		}

		inode_priv_set(next, &ramfs_files[cur_id]);
		next->i_no = cur_id;
		next->length = ramfs_files[cur_id].length;
		next->i_mode = ramfs_files[cur_id].mode & (S_IFMT | S_IRWXA);

		ctx->fs_ctx = (void *) (uintptr_t)(cur_id + 1);
		strncpy(name, (char *) ramfs_files[cur_id].name, NAME_MAX);

		return 0;
	}

	ctx->fs_ctx = NULL;
	return -1;
}

int ramfs_fill_sb(struct super_block *sb, const char *source) {
	struct ramfs_fs_info *fsi;
	struct block_dev *bdev;

	assert(sb);

	bdev = bdev_by_path(source);
	if (NULL == bdev) {
		return -ENODEV;
	}

	if (NULL == (fsi = pool_alloc(&ramfs_fs_pool))) {
		return -ENOMEM;
	}

	assert(bdev->block_size <= sizeof(sector_buff));

	memset(fsi, 0, sizeof(struct ramfs_fs_info));
	fsi->block_per_file = MAX_FILE_SIZE / bdev->block_size;
	fsi->block_size = bdev->block_size;
	fsi->numblocks = bdev->size / bdev->block_size;
	fsi->bdev = bdev;

	sb->sb_data = fsi;
	sb->sb_iops = &ramfs_iops;
	sb->sb_fops = &ramfs_fops;
	sb->sb_ops  = &ramfs_sbops;
	sb->bdev    = bdev;

	return 0;
}

struct file_operations ramfs_fops = {
	.write = ramfs_write,
	.read = ramfs_read,
};

int ramfs_format(struct block_dev *bdev, void *priv) {
	if (NULL == bdev) {
		return -ENODEV;
	}

	if (MAX_FILE_SIZE > bdev->size) {
		return -ENOSPC;
	}

	return 0;
}

struct ramfs_file_info *ramfs_file_alloc(struct inode *node) {
	struct ramfs_file_info *fi;
	size_t fi_index;

	fi_index = index_alloc(&ramfs_file_idx, INDEX_MIN);
	if (fi_index == INDEX_NONE) {
		return NULL;
	}

	fi = &ramfs_files[fi_index];
	memset(fi, 0, sizeof(*fi));

	fi->index = fi_index;
	fi->fsi   = node->i_sb->sb_data;
	fi->inode = node;

	inode_size_set(node, 0);
	inode_priv_set(node, fi);

	return fi;
}

static int ramfs_file_free(struct ramfs_file_info *fi) {
	index_free(&ramfs_file_idx, fi->index);
	memset(fi, 0, sizeof(*fi));

	return 0;
}

int ramfs_delete(struct inode *node) {
	struct ramfs_file_info *fi;

	fi = inode_priv(node);

	return ramfs_file_free(fi);
}

int ramfs_truncate(struct inode *node, off_t length) {
	assert(node);

	if (length > MAX_FILE_SIZE) {
		return -EFBIG;
	}

	inode_size_set(node, length);

	return 0;
}

int ramfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	struct ramfs_file_info *fi;

	if (S_ISREG(i_new->i_mode)) {
		assert(i_new);

		fi = ramfs_file_alloc(i_new);
		if (NULL == fi) {
			return -ENOMEM;
		}
		fi->mode = i_new->i_mode;
		strncpy(fi->name, inode_name(i_new), sizeof(fi->name) - 1);

		i_new->i_no = fi->index;
	}

	return 0;
}

int ramfs_destroy_inode(struct inode *inode) {
	assert(inode);
	return 0;
}

