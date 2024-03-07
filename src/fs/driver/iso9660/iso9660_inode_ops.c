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
#include <fs/super_block.h>

#include <fs/iso9660.h>


extern int cdfs_isonum_711(unsigned char *p);
extern int cdfs_isonum_733(unsigned char *p);

extern int cdfs_alloc_inode_priv(struct inode* node);
extern int cdfs_destroy_inode(struct inode *inode);
extern int cdfs_fill_node(struct inode* node, char *name, struct cdfs_fs_info *cdfs, iso_directory_record_t *rec);
extern int cdfs_find_dir(struct cdfs_fs_info *cdfs, char *name, int len);

extern struct inode *cdfs_ilookup(struct inode *node, char const *name, struct inode const *dir);

static int cdfs_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx) {
	int n;
	struct cdfs_fs_info *fsi;
	struct block_dev_cache *cache;
	int blk;
	char *p;
	iso_directory_record_t *rec;
	int left;
	int reclen;
	int idx = 0;
	char *dir_name;

	fsi = parent->i_sb->sb_data;

	if (0 == (int) (intptr_t) dir_ctx->fs_ctx) {
		dir_ctx->fs_ctx = (void*)(intptr_t)2;
	}
	if (parent == parent->i_sb->sb_root) {
		dir_name = "";
	} else {
		dir_name = inode_name(parent);
	}
	n = cdfs_find_dir(fsi, dir_name, strlen(dir_name));

	/* The first two directory records are . (current) and .. (parent) */
	blk = fsi->path_table[n]->extent;
	cache = block_dev_cached_read(fsi->bdev, blk++);
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
			cache = block_dev_cached_read(fsi->bdev, blk++);
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
				int res;

				res = cdfs_alloc_inode_priv(next);
				if (res) {
					return -1;
				}

				res = cdfs_fill_node(next, next_name, fsi, rec);
				if (res) {
					cdfs_destroy_inode(next);
					return -1;
				}

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
	.ino_lookup  = cdfs_ilookup,
};
