/**
 * @file
 * @brief
 *
 * @date 20.08.2012
 * @author Andrey Gazukin
 */

#include <string.h>

#include <fs/inode.h>

#include <drivers/block_dev.h>

#include <fs/fs_driver.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>

#include <fs/iso9660.h>


extern int cdfs_isonum_711(unsigned char *p);
extern int cdfs_isonum_733(unsigned char *p);

extern int cdfs_fill_node(struct inode* node, char *name, cdfs_t *cdfs, iso_directory_record_t *rec);
extern int cdfs_find_dir(cdfs_t *cdfs, char *name, int len);

static int cdfs_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx) {
	int n;
	cdfs_t *cdfs;
	struct cdfs_fs_info *fsi;
	struct block_dev_cache *cache;
	int blk;
	char *p;
	iso_directory_record_t *rec;
	int left;
	int reclen;
	int idx = 0;

	fsi = parent->i_sb->sb_data;
	cdfs = fsi->data;

	if (0 == (int) (intptr_t) dir_ctx->fs_ctx) {
		dir_ctx->fs_ctx = (void*)(intptr_t)2;
	}

	n = cdfs_find_dir(cdfs, inode_name(parent), strlen(inode_name(parent)));

	/* The first two directory records are . (current) and .. (parent) */
	blk = cdfs->path_table[n]->extent;
	cache = block_dev_cached_read(cdfs->bdev, blk++);
	if (!cache) {
		return -1;
	}

	/* Get length of directory from the first record */
	p = cache->data;
	rec = (iso_directory_record_t *) p;
	left = cdfs_isonum_733(rec->size);

	/* Find named entry in directory */
	while (left > 0) {
		/*
		 * Read next block if all records in current block has been read
		 * Directory records never cross block boundaries
		 */
		if (p >= cache->data + CDFS_BLOCKSIZE) {
			if (p > cache->data + CDFS_BLOCKSIZE) {
				return -1;
			}
			cache = block_dev_cached_read(cdfs->bdev, blk++);
			if (!cache) {
				return -1;
			}
			p = cache->data;
		}

		/* Check for match */
		rec = (iso_directory_record_t *) p;
		reclen = cdfs_isonum_711(rec->length);

		if (reclen > 0) {

			if (idx++ < (int)(uintptr_t)dir_ctx->fs_ctx) {
			} else {
				cdfs_fill_node(next, next_name, cdfs, rec);
				dir_ctx->fs_ctx = (void *)(uintptr_t)idx;
				return 0;
			}
			/* Skip to next record */
			p += reclen;
			left -= reclen;
		}
		else {
			/* Skip to next block */
			left -= (cache->data + CDFS_BLOCKSIZE) - p;
			p = cache->data + CDFS_BLOCKSIZE;
		}
	}
	return -1;
}

struct inode_operations cdfs_iops = {
	.ino_iterate = cdfs_iterate,
};
