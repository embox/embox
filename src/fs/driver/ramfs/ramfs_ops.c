#include <string.h>

#include <drivers/block_dev.h>
#include <fs/file_desc.h>
#include <mem/phymem.h> /* PAGE_SIZE() */
#include <util/math.h>

#include "ramfs.h"

static char sector_buff[PAGE_SIZE()];/* TODO */

size_t ramfs_read(struct file_desc *desc, void *buf, size_t size) {
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

		assert(sizeof(sector_buff) == fsi->block_size);
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

size_t ramfs_write(struct file_desc *desc, void *buf, size_t size) {
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

	return bytecount;
}
