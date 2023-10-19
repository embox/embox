/* @file
 * @brief  DumbFS driver
 * @author Denis Deryugin
 * @date   26 Dec 2014
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>

#include <fs/dvfs.h>
#include <drivers/flash/flash.h>

#include <util/array.h>
#include <util/bitmap.h>
#include <util/math.h>

#include <fs/dfs.h>

#include <framework/mod/options.h>

#define DFS_MAGIC_0 0x0D
#define DFS_MAGIC_1 0xF5


#define MIN_FILE_SZ            OPTION_GET(NUMBER, minimum_file_size)


#define DFS_DENTRY_OFFSET(N) \
	((sizeof(struct dfs_sb_info)) + N * (sizeof(struct dfs_dir_entry)))

extern int dfs_write_buffered(struct flash_dev *flashdev,
					int pos, void *buff, size_t size);
extern uintptr_t flash_cache_addr(struct flash_dev *flashdev);

int dfs_format(struct block_dev *bdev, void *priv) {
	uint8_t write_buf[sizeof(struct dfs_sb_info) + sizeof(struct dfs_dir_entry)];
	struct dfs_sb_info *sbi;
	struct dfs_dir_entry *root;
	struct flash_dev *fdev;
	int i, j, k;
	int err;

	fdev = flash_by_bdev(bdev);

	k = 0;
	for (j = 0; j < fdev->num_block_infos; j++) {
		for (i = 0; i < fdev->block_info[j].blocks; i++) {
			if ((err = flash_erase(fdev, k))) {
				return err;
			}
			k++;
		}
	}

	sbi  = (struct dfs_sb_info *)&write_buf[0];
	root = (struct dfs_dir_entry *)&write_buf[sizeof(struct dfs_sb_info)];
	
	/* Empty FS */
	*sbi = (struct dfs_sb_info) {
		.magic = {DFS_MAGIC_0, DFS_MAGIC_1},
		.inode_count = 1, /* Configure root directory */
		.max_inode_count = DFS_INODES_MAX + 1, /* + root folder with i_no 0 */
		.max_len = MIN_FILE_SZ,
		/* Set buffer block to the last one */
		//.buff_bk = flash_cache_addr(fdev),
		.free_space = DFS_DENTRY_OFFSET(DFS_INODES_MAX),
	};

	strcpy((char *) root->name, "/");
	root->pos_start = sbi->free_space;
	root->len       = DFS_INODES_MAX;
	root->flags     = S_IFDIR;

	flash_write_aligned(fdev, 0, write_buf, sizeof(write_buf));

	return 0;
}

/*---------------------------------*\
 	File System Interface
\*---------------------------------*/

static int dfs_read_sb_info(struct super_block *sb, struct dfs_sb_info *sbi) {
	struct flash_dev *fdev;

	assert(sb);
	assert(sbi);

	fdev = flash_by_bdev(sb->bdev);

	flash_read_aligned(fdev, 0, sbi, sizeof(struct dfs_sb_info));

	return 0;
}

static int dfs_write_sb_info(struct super_block *sb, struct dfs_sb_info *sbi) {
	struct flash_dev *fdev;

	assert(sb);
	assert(sbi);

	fdev = flash_by_bdev(sb->bdev);
	
	dfs_write_buffered(fdev, 0, sbi, sizeof(struct dfs_sb_info));

	return 0;
}

static int dfs_read_dirent(struct super_block *sb, int n, struct dfs_dir_entry *dtr) {
	struct flash_dev *fdev;
	uint32_t offt = DFS_DENTRY_OFFSET(n);

	assert(dtr);

	fdev = flash_by_bdev(sb->bdev);

	flash_read_aligned(fdev, offt, dtr, sizeof(struct dfs_dir_entry));

	if (dtr->name[0] == '\0') {
		return -ENOENT;
	}

	return 0;
}

static int dfs_write_dirent(struct super_block *sb, int n, struct dfs_dir_entry *dtr) {
	uint32_t offt = DFS_DENTRY_OFFSET(n);
	struct flash_dev *fdev;

	assert(dtr);

	fdev = flash_by_bdev(sb->bdev);

	dfs_write_buffered(fdev, offt, dtr, sizeof(struct dfs_dir_entry));
	return 0;
}

static int ino_from_path(struct super_block *sb, const char *path) {
	struct dfs_dir_entry dirent;
	int i;

	assert(path);

	for (i = 0; i < DFS_INODES_MAX; i++) {
		if (!dfs_read_dirent(sb, i, &dirent) &&
				strcmp(path, (char *) dirent.name) == 0) {
			return i;
		}
	}

	return -1;
}

/***************
 New-VFS-related
 ***************/
static struct inode_operations dfs_iops;

static struct super_block_operations dfs_sbops = {
	.open_idesc = dvfs_file_open_idesc,
};

static int dfs_icreate(struct inode *i_new, struct inode *i_dir, int mode) {
	struct super_block *sb = i_dir->i_sb;
	struct dfs_sb_info *sbi = sb->sb_data;
	struct dfs_dir_entry dirent;

	assert(sb);
	assert(i_dir);

	if (i_new == NULL) {
		return -1;
	}

	dfs_read_sb_info(sb, sbi);

	if (sbi->inode_count > sbi->max_inode_count) {
		return -ENOMEM;
	}

	memset(&dirent, 0, sizeof(dirent));
	dirent = (struct dfs_dir_entry) {
		.pos_start = sbi->free_space,
		.len       = 0,
		.flags     = i_new->i_mode & S_IFMT,
	};

	strcpy((char *) dirent.name, inode_name(i_new));

	dfs_write_dirent(sb, sbi->inode_count, &dirent);

	*i_new = (struct inode) {
		.i_no      = sbi->inode_count,
		.i_data    = (void *) dirent.pos_start,
		.length    = 0,
		.i_sb      = sb,
		.i_ops     = &dfs_iops,
	};

	sbi->inode_count++;
	sbi->free_space += MIN_FILE_SZ;

	dfs_write_sb_info(sb, sbi);

	return 0;
}

/**
 * @brief Change size of file
 * @note In this FS we can only increase size
 *
 * @param inode
 * @param new_len
 *
 * @return Negative error number or 0 if succeed
 */
static int dfs_itruncate(struct inode *inode, off_t new_len) {
	struct super_block *sb;
	struct dfs_sb_info *sbi;
	struct dfs_dir_entry entry;

	assert(inode);

	if (new_len < 0) {
		return -1;
	}

	sb = inode->i_sb;
	sbi =sb->sb_data;

	if (new_len > sbi->max_len) {
		return -1;
	}

	dfs_read_dirent(sb, inode->i_no, &entry);
	if (new_len == inode->length) {
		/* No need to write changes on drive */
		return 0;
	}
	entry.len = new_len;
	dfs_write_dirent(sb, inode->i_no, &entry);

	inode->length = new_len;

	return 0;
}

static struct inode *dfs_ilookup(char const *path, struct inode const *dir) {
	struct dfs_dir_entry dirent;
	struct inode *inode;
	struct super_block *sb;

	assert(path);
	assert(dir);

	sb = dir->i_sb;

	inode = dvfs_alloc_inode(sb);

	if (!inode) {
		return NULL;
	}

	inode->i_no = ino_from_path(sb, path);

	if (inode->i_no < 0) {
		dvfs_destroy_inode(inode);
		return NULL;
	}

	dfs_read_dirent(sb, inode->i_no, &dirent);

	inode->i_data = (void *) (uintptr_t) dirent.pos_start;
	inode->length = dirent.len;
	inode->i_mode = dirent.flags;

	return inode;
}

static int dfs_iterate(struct inode *next, char *name_buf,
		struct inode *parent, struct dir_ctx *ctx) {
	struct super_block *sb;
	struct dfs_dir_entry dirent;
	int i, dir_pos;

	assert(ctx);
	assert(next);
	assert(parent);
	assert(name_buf);

	sb = parent->i_sb;

	dir_pos = (int) ctx->fs_ctx;
	if (dir_pos == 0) {
		dir_pos++; /*skip root dir */
	}

	for (i = dir_pos; i < parent->length; i++) {
		const uint32_t empty_dirent = 0xFFFFFFFF;

		dfs_read_dirent(sb, i, &dirent);
		if (memcmp(&dirent, &empty_dirent, sizeof(empty_dirent))) {
			*next = (struct inode) {
				.i_no   = i,
				.i_data = (void *) (uintptr_t) dirent.pos_start,
				.length = dirent.len,
				.i_sb   = sb,
				.i_ops  = &dfs_iops,
				.i_mode = dirent.flags,
			};
			ctx->fs_ctx = (void*) (i + 1);

			strncpy(name_buf, (char *) dirent.name, NAME_MAX);
			return 0;
		}
	}

	/* End of directory */
	return -1;
}

static int dfs_pathname(struct inode *inode, char *buf, int flags) {
	struct dfs_dir_entry dirent;

	assert(inode);
	dfs_read_dirent(inode->i_sb, inode->i_no, &dirent);

	if (flags & DVFS_NAME) {
		strcpy(buf, (char *) dirent.name);
	} else {
		*buf = '/';
		strcpy(buf + 1, (char *) dirent.name);
	}

	return 0;
}

static struct inode_operations dfs_iops = {
	.create   = dfs_icreate,
	.lookup   = dfs_ilookup,
	.mkdir    = NULL,
	.rmdir    = NULL,
	.iterate  = dfs_iterate,
	.truncate = dfs_itruncate,
	.pathname = dfs_pathname,
};

static struct file_operations dfs_fops;

static struct idesc *dfs_open(struct inode *node, struct idesc *desc, int __oflag) {
	struct file_desc *fdesc;

	if (!desc || !node) {
		SET_ERRNO(ENOENT);
		return NULL;
	}

	fdesc = (struct file_desc*)desc;
	fdesc->f_ops = &dfs_fops;

	return desc;
}

static int dfs_close(struct file_desc *desc) {
	return 0;
}

static size_t dfs_write(struct file_desc *desc, void *buf, size_t size) {
	int pos;
	int l;
	struct super_block *sb;
	struct dfs_sb_info *sbi;
	struct flash_dev *fdev;

	assert(desc);
	assert(desc->f_inode);
	assert(buf);

	sb = desc->f_inode->i_sb;
	sbi = sb->sb_data;
	fdev = flash_by_bdev(sb->bdev);

	pos = ((uintptr_t) desc->f_inode->i_data) + desc->pos;
	l = min(size, sbi->max_len - desc->pos);

	if (l <= 0) {
		return -1;
	}

	dfs_write_buffered(fdev, pos, buf, l);

	return l;
}

static size_t dfs_read(struct file_desc *desc, void *buf, size_t size) {
	int pos;
	int l;
	struct super_block *sb;
	struct flash_dev *fdev;

	assert(desc);
	assert(desc->f_inode);
	assert(buf);

	sb = desc->f_inode->i_sb;
	fdev = flash_by_bdev(sb->bdev);

	pos = ((uintptr_t) desc->f_inode->i_data) + desc->pos;
	l = min(size, file_get_size(desc) - desc->pos);

	if (l < 0) {
		return -1;
	}

	flash_read_aligned(fdev, pos, buf, l);

	return l;
}

static struct file_operations dfs_fops = {
	.open = dfs_open,
	.close = dfs_close,
	.write = dfs_write,
	.read = dfs_read,
	.ioctl = NULL,
};

static struct dfs_sb_info dfs_info;

static int dfs_fill_sb(struct super_block *sb, const char *source) {
	struct dfs_dir_entry dtr;
	struct dfs_sb_info *sbi;
	struct flash_dev *fdev;

	sb->sb_ops     = &dfs_sbops;
	sb->sb_iops    = &dfs_iops;
	sb->sb_fops    = &dfs_fops;
	sb->sb_data    = &dfs_info;
	sb->bdev       = bdev_by_path(source);

	sbi = sb->sb_data;

	fdev = flash_by_bdev(sb->bdev);

	/* TODO move to flash driver */
	fdev->fld_cache = flash_cache_addr(fdev);

	dfs_read_sb_info(sb, sbi);

	if (!(sbi->magic[0] == DFS_MAGIC_0 && sbi->magic[1] == DFS_MAGIC_1)) {
#if OPTION_GET(NUMBER, format_during_fill_sb)
		dfs_format(sb->bdev, NULL);
		dfs_read_sb_info(sb, sbi);
#else
		return -EINVAL;
#endif /* OPTION_GET(NUMBER, format_during_fill_sb) */
	}

	dfs_read_dirent(sb, 0, &dtr);

	sb->sb_root->i_no      = 0;
	sb->sb_root->length    = dtr.len;
	sb->sb_root->i_data    = (void *) ((uintptr_t) dtr.pos_start);

	return 0;
}

static const struct fs_driver dfs_dumb_driver = {
	.name      = "DumbFS",
	.fill_sb   = &dfs_fill_sb,
	.format    = dfs_format,
};

DECLARE_FILE_SYSTEM_DRIVER(dfs_dumb_driver);
