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

static struct flash_dev *dfs_flashdev;

#define DFS_MAGIC_0 0x0D
#define DFS_MAGIC_1 0xF5

#define NAND_PAGE_SIZE         OPTION_GET(NUMBER, page_size)
#define NAND_BLOCK_SIZE        OPTION_GET(NUMBER, block_size)
#define MIN_FILE_SZ            OPTION_GET(NUMBER, minimum_file_size)
#define USE_RAM_AS_CACHE       OPTION_GET(BOOLEAN, use_ram_as_cache)
#define USE_RAM_SECTION        OPTION_GET(BOOLEAN, use_ram_section)

extern struct super_block *dfs_sb(void);
static int dfs_write_dirent(int n, struct dfs_dir_entry *dtr);

#define DFS_DENTRY_OFFSET(N) \
	((sizeof(struct dfs_sb_info)) + N * (sizeof(struct dfs_dir_entry)))

#if USE_RAM_SECTION
#define CACHE_SECTION      __attribute__((section(".dfs_cache_section")))
#else
#define CACHE_SECTION
#endif

#if USE_RAM_AS_CACHE
static uint8_t cache_block_buffer[NAND_BLOCK_SIZE]
								  CACHE_SECTION  __attribute__ ((aligned(NAND_PAGE_SIZE)));
#endif

static inline int dfs_erase_flash(unsigned int block) {
	return flash_erase(dfs_flashdev, block);
}

static inline int dfs_read_flash(unsigned long offset, void *buff, size_t len) {
	int i;
	char b[NAND_PAGE_SIZE] __attribute__ ((aligned(NAND_PAGE_SIZE)));
	int head;

	assert(buff);

	head = offset % NAND_PAGE_SIZE;

	if (head) {
		size_t head_cnt = min(len, NAND_PAGE_SIZE - head);

		offset -= head;
		flash_read(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		memcpy(buff, b + head, head_cnt);

		if (len <= head_cnt) {
			return 0;
		}

		buff   += head_cnt;
		offset += NAND_PAGE_SIZE;
		len    -= head_cnt;
	}

	for (i = 0; len >= NAND_PAGE_SIZE; i++) {
		flash_read(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		memcpy(buff, b, NAND_PAGE_SIZE);

		offset += NAND_PAGE_SIZE;
		buff   += NAND_PAGE_SIZE;
		len    -= NAND_PAGE_SIZE;
	}

	if (len > 0) {
		flash_read(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		memcpy(buff, b, len);
	}

	return 0;
}

/* @brief Write non-aligned raw data to \b erased NAND flash
 * @param offset Start position on disk
 * @param buff   Source of the data
 * @param len    Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
static inline int dfs_write_flash(unsigned long offset, const void *buff, size_t len) {
	int i;
	char b[NAND_PAGE_SIZE] __attribute__ ((aligned(NAND_PAGE_SIZE)));
	int head;

	assert(buff);

	head = offset % NAND_PAGE_SIZE;

	if (head) {
		size_t head_write_cnt = min(len, NAND_PAGE_SIZE - head);

		offset -= head;
		flash_read(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		memcpy(b + head, buff, head_write_cnt);
		flash_write(dfs_flashdev, offset, b, NAND_PAGE_SIZE);

		if (len <= head_write_cnt) {
			return 0;
		}

		buff   += head_write_cnt;
		offset += NAND_PAGE_SIZE;
		len    -= head_write_cnt;
	}

	for (i = 0; len >= NAND_PAGE_SIZE; i++) {
		memcpy(b, buff, NAND_PAGE_SIZE);
		flash_write(dfs_flashdev, offset, b, NAND_PAGE_SIZE);

		offset += NAND_PAGE_SIZE;
		buff   += NAND_PAGE_SIZE;
		len    -= NAND_PAGE_SIZE;
	}

	if (len > 0) {
		flash_read(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		memcpy(b, buff, len);
		flash_write(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
	}

	return 0;
}

static inline int dfs_copy_flash(unsigned long to, unsigned long from, int len) {
	char b[NAND_PAGE_SIZE] __attribute__ ((aligned(NAND_PAGE_SIZE)));

	while (len > 0) {
		int tmp_len;

		tmp_len = min(len, sizeof(b));

		if (0 > dfs_read_flash(from, b, tmp_len)) {
			return -1;
		}
		if (0 > dfs_write_flash(to, b, tmp_len)) {
			return -1;
		}

		len -= tmp_len;
		to += tmp_len;
		from += tmp_len;
	}

	return 0;
}

static inline int dfs_blkcpy_flash(unsigned int to, unsigned long from) {
	dfs_erase_flash(to);
	return dfs_copy_flash(to * NAND_BLOCK_SIZE, from * NAND_BLOCK_SIZE, NAND_BLOCK_SIZE);
}

#if USE_RAM_AS_CACHE
static int dfs_cache_erase(uint32_t addr) {
	return 0;
}

static int dfs_cache(uint32_t to, uint32_t from, int len) {
	char b[NAND_PAGE_SIZE] __attribute__ ((aligned(NAND_PAGE_SIZE)));

	while (len > 0) {
		int tmp_len;

		tmp_len = min(len, sizeof(b));

		if (0 > dfs_read_flash(from, b, tmp_len)) {
			return -1;
		}
		memcpy((void *)((uintptr_t)to), b, tmp_len);

		len -= tmp_len;
		to += tmp_len;
		from += tmp_len;
	}

	return 0;
}

static inline int dfs_cache_write(uint32_t offset, const void *buff, size_t len) {
	memcpy((void *)((uintptr_t)offset), buff, len);
	return 0;
}

static inline int dfs_cache_restore(uint32_t to, uint32_t from) {
	dfs_erase_flash(to);
	return dfs_write_flash(to * NAND_BLOCK_SIZE, (void *)((uintptr_t)from), NAND_BLOCK_SIZE);
}
#define CACHE_OFFSET                  ((uintptr_t)cache_block_buffer)
#else
#define dfs_cache_erase(block)         dfs_erase_flash(block)
#define dfs_cache(to, from, len)       dfs_copy_flash(to,from,len)
#define dfs_cache_write(off,buf, len)  dfs_write_flash(off,buf, len)
#define dfs_cache_restore(to, from)    dfs_blkcpy_flash(to,from)
#define CACHE_OFFSET                  (buff_bk * NAND_BLOCK_SIZE)
#endif
/* @brief Write non-aligned raw data to \b non-erased NAND flash
 * @param pos  Start position on disk
 * @param buff Source of the data
 * @param size Length of the data in bytes
 *
 * @returns Bytes written or negative error code
 */
static int dfs_write_raw(int pos, void *buff, size_t size) {
	int start_bk = pos / NAND_BLOCK_SIZE;
	int last_bk = (pos + size) / NAND_BLOCK_SIZE;
	struct dfs_sb_info *sbi;
	uint32_t buff_bk;
	int bk;
	int err;

	assert(buff);

	sbi = dfs_sb()->sb_data;
	buff_bk = sbi->buff_bk;
	pos %= NAND_BLOCK_SIZE;

	err = 0;

	dfs_cache_erase(buff_bk);
	dfs_cache(CACHE_OFFSET, start_bk * NAND_BLOCK_SIZE, pos);

	if (start_bk == last_bk) {
		if ((err = dfs_cache_write(CACHE_OFFSET + pos, buff, size))) {
			return err;
		}
		pos += size;
	} else {
		dfs_write_flash(CACHE_OFFSET + pos, buff, NAND_BLOCK_SIZE - pos);
		dfs_blkcpy_flash(start_bk, buff_bk);
		buff += NAND_BLOCK_SIZE - pos;
		pos = (pos + size) % NAND_BLOCK_SIZE;

		for (bk = start_bk + 1; bk < last_bk; bk++) {
			dfs_erase_flash(bk);
			if ((err = dfs_write_flash(bk * NAND_BLOCK_SIZE, buff, NAND_BLOCK_SIZE))) {
				return err;
			}
			buff += NAND_BLOCK_SIZE;
		}

		dfs_erase_flash(buff_bk);
		dfs_write_flash(CACHE_OFFSET, buff, pos);
	}

	dfs_cache(CACHE_OFFSET + pos, last_bk * NAND_BLOCK_SIZE + pos, NAND_BLOCK_SIZE - pos);
	dfs_cache_restore(last_bk, buff_bk);

	return 0;
}

static int dfs_format(void) {
	struct dfs_sb_info *sbi;
	struct dfs_dir_entry root;
	uint8_t write_buf[sizeof(struct dfs_sb_info) + sizeof(struct dfs_dir_entry)];
	int i, j, k;
	int err;

	if (!dfs_flashdev) {
		return -ENOENT;
	}


	for (j = 0, k = 0; j < dfs_flashdev->num_block_infos; j++) {
		for (i = 0; i < dfs_flashdev->block_info[j].blocks; i++) {
			if ((err = dfs_erase_flash(k))) {
				return err;
			}
			k++;
			i++;
		}
	}

	sbi = dfs_sb()->sb_data;
	/* Empty FS */
	*sbi = (struct dfs_sb_info) {
		.magic = {DFS_MAGIC_0, DFS_MAGIC_1},
		.inode_count = 0,
		.max_inode_count = DFS_INODES_MAX + 1, /* + root folder with i_no 0 */
		.max_len = MIN_FILE_SZ,
		/* Set buffer block to the last one */
#if USE_RAM_AS_CACHE
		.buff_bk = ((uintptr_t)cache_block_buffer),
#else
		.buff_bk = dfs_flashdev->block_info[0].blocks - 1,
#endif
		.free_space = DFS_DENTRY_OFFSET(DFS_INODES_MAX),
	};

	/* Configure root directory */
	sbi->inode_count++;

	strcpy((char *) root.name, "/");
	root.pos_start = sbi->free_space;
	root.len       = DFS_INODES_MAX;
	root.flags     = S_IFDIR;

	memcpy(write_buf, sbi, sizeof(struct dfs_sb_info));
	memcpy(&write_buf[sizeof(struct dfs_sb_info)], &root, sizeof(struct dfs_dir_entry));

	dfs_write_raw(0, write_buf, sizeof(write_buf));

	return 0;
}

static inline int dfs_set_dev(struct flash_dev *new_dev) {
	assert(new_dev);
	dfs_flashdev = new_dev;
	return 0;
}

static inline struct flash_dev *dfs_get_dev(void) {
	return dfs_flashdev;
}

/*---------------------------------*\
 	File System Interface
\*---------------------------------*/

static enum { EMPTY, DIRTY, ACTUAL } dfs_sb_status = EMPTY;

static int dfs_read_sb_info(struct dfs_sb_info *sbi) {
	assert(sbi);
	if (dfs_sb_status == EMPTY) {
		dfs_read_flash(0, sbi, sizeof(struct dfs_sb_info));
	}
	dfs_sb_status = ACTUAL;
	if (!(sbi->magic[0] == DFS_MAGIC_0 && sbi->magic[1] == DFS_MAGIC_1)) {
		dfs_format();
	}

	return 0;
}

static int dfs_write_sb_info(struct dfs_sb_info *sbi) {
	assert(sbi);
	if (dfs_sb_status == DIRTY) {
		dfs_write_raw(0, sbi, sizeof(struct dfs_sb_info));
		dfs_sb_status = ACTUAL;
	}
	return 0;
}

static int dfs_read_dirent(int n, struct dfs_dir_entry *dtr) {
	uint32_t offt = DFS_DENTRY_OFFSET(n);

	assert(dtr);

	dfs_read_flash(offt, dtr, sizeof(struct dfs_dir_entry));

	if (dtr->name[0] == '\0') {
		return -ENOENT;
	}

	return 0;
}

static int dfs_write_dirent(int n, struct dfs_dir_entry *dtr) {
	uint32_t offt = DFS_DENTRY_OFFSET(n);

	assert(dtr);

	dfs_write_raw(offt, dtr, sizeof(struct dfs_dir_entry));
	return 0;
}

static int ino_from_path(const char *path) {
	struct dfs_dir_entry dirent;
	int i;

	assert(path);

	for (i = 0; i < DFS_INODES_MAX; i++) {
		if (!dfs_read_dirent(i, &dirent) &&
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

	dfs_read_sb_info(sbi);

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

	dfs_write_dirent(sbi->inode_count, &dirent);

	*i_new = (struct inode) {
		.i_no      = sbi->inode_count,
		.i_data    = (void *) dirent.pos_start,
		.length    = 0,
		.i_sb      = sb,
		.i_ops     = &dfs_iops,
	};

	sbi->inode_count++;
	sbi->free_space += MIN_FILE_SZ;

	dfs_sb_status = DIRTY;
	dfs_write_sb_info(sbi);

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
	struct dfs_sb_info *sbi;
	struct dfs_dir_entry entry;

	assert(inode);

	if (new_len < 0) {
		return -1;
	}

	sbi = dfs_sb()->sb_data;

	if (new_len > sbi->max_len) {
		return -1;
	}

	dfs_read_dirent(inode->i_no, &entry);
	if (new_len == inode->length) {
		/* No need to write changes on drive */
		return 0;
	}
	entry.len = new_len;
	dfs_write_dirent(inode->i_no, &entry);

	inode->length = new_len;

	return 0;
}

static struct inode *dfs_ilookup(char const *path, struct inode const *dir) {
	struct dfs_dir_entry dirent;
	struct inode *inode;

	assert(path);
	assert(dir);

	inode = dvfs_alloc_inode(dfs_sb());

	if (!inode) {
		return NULL;
	}

	inode->i_no = ino_from_path(path);

	if (inode->i_no < 0) {
		dvfs_destroy_inode(inode);
		return NULL;
	}

	dfs_read_dirent(inode->i_no, &dirent);

	inode->i_data = (void *) (uintptr_t) dirent.pos_start;
	inode->length = dirent.len;
	inode->i_mode = dirent.flags;

	return inode;
}

static int dfs_iterate(struct inode *next, char *name_buf,
		struct inode *parent, struct dir_ctx *ctx) {
	struct dfs_dir_entry dirent;
	int i, dir_pos;

	assert(ctx);
	assert(next);
	assert(parent);
	assert(name_buf);

	dir_pos = (int) ctx->fs_ctx;
	if (dir_pos == 0) {
		dir_pos++; /*skip root dir */
	}

	for (i = dir_pos; i < parent->length; i++) {
		const uint32_t empty_dirent = 0xFFFFFFFF;

		dfs_read_dirent(i, &dirent);
		if (memcmp(&dirent, &empty_dirent, sizeof(empty_dirent))) {
			*next = (struct inode) {
				.i_no   = i,
				.i_data = (void *) (uintptr_t) dirent.pos_start,
				.length = dirent.len,
				.i_sb   = dfs_sb(),
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

	if (flags & DVFS_NAME) {
		dfs_read_dirent(inode->i_no, &dirent);
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
	struct dfs_sb_info *sbi;

	assert(desc);
	assert(desc->f_inode);
	assert(buf);

	sbi = dfs_sb()->sb_data;

	pos = ((uintptr_t) desc->f_inode->i_data) + desc->pos;
	l = min(size, sbi->max_len - desc->pos);

	if (l <= 0) {
		return -1;
	}

	dfs_write_raw(pos, buf, l);

	return l;
}

static size_t dfs_read(struct file_desc *desc, void *buf, size_t size) {
	int pos;
	int l;

	assert(desc);
	assert(desc->f_inode);
	assert(buf);

	pos = ((uintptr_t) desc->f_inode->i_data) + desc->pos;
	l = min(size, file_get_size(desc) - desc->pos);

	if (l < 0) {
		return -1;
	}

	dfs_read_flash(pos, buf, l);

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
static struct super_block *dfs_super;
static const struct fs_driver dfs_dumb_driver;

struct super_block *dfs_sb(void) {
	return dfs_super;
}

static int dfs_fill_sb(struct super_block *sb, const char *source) {
	struct dfs_dir_entry dtr;

	dfs_super = sb;
	sb->fs_drv     = &dfs_dumb_driver;
	sb->sb_ops     = &dfs_sbops;
	sb->sb_iops    = &dfs_iops;
	sb->sb_fops    = &dfs_fops;
	sb->sb_data    = &dfs_info;
	sb->bdev       = bdev_by_path(source);

	if (!sb->bdev) {
		sb->bdev = dfs_flashdev->bdev;
	}

	dfs_set_dev(flash_by_id(0));
	dfs_read_sb_info(dfs_sb()->sb_data);

	dfs_read_dirent(0, &dtr);

	sb->sb_root->i_no      = 0;
	sb->sb_root->length    = dtr.len;
	sb->sb_root->i_data    = (void *) ((uintptr_t) dtr.pos_start);

	return 0;
}

static const struct fs_driver dfs_dumb_driver = {
	.name      = "DumbFS",
	.fill_sb   = &dfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(dfs_dumb_driver);
