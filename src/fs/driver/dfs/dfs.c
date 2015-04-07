/* @author Denis Deryugin
 * @date 26 Dec 2014
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <embox/unit.h>
#include <fs/dfs.h>
#include <fs/dvfs.h>
#include <framework/mod/options.h>
#include <util/array.h>
#include <util/bitmap.h>
#include <util/math.h>

static struct flash_dev *dfs_flashdev;

EMBOX_UNIT_INIT(dfs_init);

#define DFS_MAGIC_0 0x0D
#define DFS_MAGIC_1 0xF5

#define NAND_PAGE_SIZE 8
#define NAND_BLOCK_SIZE (dfs_flashdev->block_info.block_size)
#define NAND_PAGES_PER_BLOCK (NAND_BLOCK_SIZE / NAND_PAGE_SIZE)
#define NAND_PAGES_MAX (1024 * 128 / 8 * 3)

BITMAP_DECL(dfs_free_pages, NAND_PAGES_MAX);

extern struct super_block *dfs_sb(void);

/* Converting */
static inline int page_capacity(int bytes) {
	return (bytes + NAND_PAGE_SIZE - 1) / NAND_PAGE_SIZE;
}
static inline int _capacity(int bytes) { return NAND_PAGE_SIZE * page_capacity(bytes); }

static inline int pos_from_page(int pg) { return pg * NAND_PAGE_SIZE; }
static inline int page_from_pos(int pos) { return pos / NAND_PAGE_SIZE; }
static inline int pos_from_block(int block) { return block * NAND_BLOCK_SIZE; }
static inline int block_from_pos(int pos) { return pos / NAND_BLOCK_SIZE; }

static inline int _erase(unsigned int block) {
	int i;
	for (i = 0; i < NAND_PAGES_PER_BLOCK; i++)
		bitmap_set_bit(dfs_free_pages, i + block * NAND_BLOCK_SIZE / NAND_PAGE_SIZE);
	return flash_erase(dfs_flashdev, block);
}

static inline int _read(unsigned long offset, void *buff, size_t len) {
	assert(buff);
	return flash_read(dfs_flashdev, offset, buff, len);
}

static inline int _write(unsigned long offset, const void *buff, size_t len) {
	assert(buff);
	int i;
	char b[NAND_PAGE_SIZE] __attribute__ ((aligned(4)));
	int head = offset & 0x7;

	if (head) {
		offset -= head;
		_read(offset, b, NAND_PAGE_SIZE);
		memcpy(b + head, buff, NAND_PAGE_SIZE - head);
		flash_write(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		buff   += NAND_PAGE_SIZE;
		offset += NAND_PAGE_SIZE;

		if (len > NAND_PAGE_SIZE - head)
			len -= NAND_PAGE_SIZE - head;
		else
			len = 0;
	}

	if (len < 0)
		return 0;

	for (i = 0; len >= NAND_PAGE_SIZE; i++) {
		memcpy(b, buff, NAND_PAGE_SIZE);
		flash_write(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
		offset += NAND_PAGE_SIZE;
		buff   += NAND_PAGE_SIZE;
		len    -= NAND_PAGE_SIZE;
	}

	if (len) {
		_read(offset, b, NAND_PAGE_SIZE);
		memcpy(b, buff, len);
		flash_write(dfs_flashdev, offset, b, NAND_PAGE_SIZE);
	}

	return 0;
}

static inline int _copy(unsigned long to, unsigned long from, size_t len) {
	return _write(to, (void*) (dfs_flashdev->start + from), len);
}

static inline int _blkcpy(unsigned int to, unsigned long from) {
	_erase(to);
	return _copy(to * NAND_BLOCK_SIZE, from * NAND_BLOCK_SIZE, NAND_BLOCK_SIZE);
}

static int dfs_write_raw(int pos, void *buff, size_t size) {
	assert(buff);
	int start_bk = pos / NAND_BLOCK_SIZE;
	int last_bk = (pos + size) / NAND_BLOCK_SIZE;
	struct dfs_sb_info *sbi = dfs_sb()->sb_data;
	int buff_bk = sbi->buff_bk;
	int bk;
	int err;

	pos %= NAND_BLOCK_SIZE;

	_erase(buff_bk);
	_copy(buff_bk * NAND_BLOCK_SIZE, start_bk * NAND_BLOCK_SIZE, pos);

	if (start_bk == last_bk) {
		if ((err = _write(buff_bk * NAND_BLOCK_SIZE + pos, buff, size)))
			return err;
		pos += size;
	} else {
		_write(buff_bk * NAND_BLOCK_SIZE + pos, buff, NAND_BLOCK_SIZE - pos);
		_blkcpy(start_bk, buff_bk);
		buff += NAND_BLOCK_SIZE - pos;
		pos = (pos + size) % NAND_BLOCK_SIZE;

		for (bk = start_bk + 1; bk < last_bk; bk++) {
			_erase(bk);
			if ((err = _write(bk * NAND_BLOCK_SIZE, buff, NAND_BLOCK_SIZE)))
				return err;
			buff += NAND_BLOCK_SIZE;
		}

		_erase(buff_bk);
		_write(buff_bk * NAND_BLOCK_SIZE, buff, pos);
	}

	_copy(buff_bk * NAND_BLOCK_SIZE + pos, last_bk * NAND_BLOCK_SIZE + pos, NAND_BLOCK_SIZE - pos);
	_blkcpy(last_bk, buff_bk);

	return 0;
}

int dfs_format(void) {
	struct dfs_sb_info *sbi = dfs_sb()->sb_data;

	if (!dfs_flashdev) {
		return -ENOENT;
	}

	/* Empty FS */
	*sbi = (struct dfs_sb_info) {
		.magic = {DFS_MAGIC_0, DFS_MAGIC_1},
		.inode_count = 0,
		.max_inode_count = DFS_INODES_MAX,
		.buff_bk = 2,
		.max_file_size = 136,
		.free_space = _capacity(sizeof(struct dfs_sb_info)) +
		              DFS_INODES_MAX * _capacity(sizeof(struct dfs_dir_entry)),
	};

	dfs_write_raw(0, sbi, sizeof(struct dfs_sb_info));

	return 0;
}

int dfs_set_dev(struct flash_dev *new_dev) {
	assert(new_dev);
	dfs_flashdev = new_dev;
	return 0;
}

struct flash_dev *dfs_get_dev(void) {
	return dfs_flashdev;
}

/*---------------------------------*\
 	File System Interface
\*---------------------------------*/

static enum { EMPTY, DIRTY, ACTUAL } dfs_sb_status = EMPTY;

static int dfs_read_sb_info(struct dfs_sb_info *sbi) {
	assert(sbi);
	if (dfs_sb_status == EMPTY)
		_read(0, sbi, sizeof(struct dfs_sb_info));
	dfs_sb_status = ACTUAL;
	if (!(sbi->magic[0] == DFS_MAGIC_0 && sbi->magic[1] == DFS_MAGIC_1))
		dfs_format();

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
	assert(dtr);
	uint32_t offt = _capacity(sizeof(struct dfs_sb_info)) +
	                n * _capacity(sizeof(struct dfs_dir_entry));

	_read(offt, dtr, sizeof(struct dfs_dir_entry));

	return 0;
}

static int dfs_write_dirent(int n, struct dfs_dir_entry *dtr) {
	assert(dtr);
	uint32_t offt = _capacity(sizeof(struct dfs_sb_info)) +
	                n * _capacity(sizeof(struct dfs_dir_entry));

	dfs_write_raw(offt, dtr, sizeof(struct dfs_dir_entry));
	return 0;
}

int ino_from_path(const char *path) {
	assert(path);
	struct dfs_dir_entry dirent;

	for (int i = 0; i < DFS_INODES_MAX; i++)
		if (!dfs_read_dirent(i, &dirent) && strcmp(path, dirent.name) == 0)
			return i;

	return -1;
}

/***************
 New-VFS-related
 ***************/
extern struct inode_operations dfs_iops;

struct super_block_operations dfs_sbops = {
	.alloc_inode   = NULL,
	.destroy_inode = NULL,
	.write_inode   = NULL,
};

static struct inode *dfs_icreate(struct dentry *d_new,
                                 struct dentry *d_dir, int mode) {
	assert(d_dir);
	struct super_block *sb = d_dir->d_sb;
	struct inode *i_new = dvfs_alloc_inode(sb);
	struct dfs_sb_info *sbi = sb->sb_data;
	struct dfs_dir_entry dirent;

	assert(sb);
	if (i_new == NULL)
		return NULL;

	dfs_read_sb_info(sbi);

	if (sbi->inode_count > sbi->max_inode_count)
		return NULL;

	dirent.pos_start = sbi->free_space;
	dirent.len = 136; /* XXX */
	strcpy(dirent.name, d_new->name);
	dfs_write_dirent(sbi->inode_count, &dirent);

	*i_new = (struct inode) {
		.i_no      = sbi->inode_count,
		.start_pos = dirent.pos_start,
		.length    = dirent.len,
		.i_sb      = sb,
		.i_ops     = &dfs_iops,
	};

	sbi->inode_count++;
	sbi->free_space += dirent.len;

	dfs_sb_status = DIRTY;
	dfs_write_sb_info(sbi);

	memset(&dirent, 0xA5, sizeof(dirent));
	dfs_read_dirent(0, &dirent);

	return d_new->d_inode = i_new;
}

static int dfs_itruncate(struct inode *inode, size_t new_len) {
	assert(inode);
	int max_l = ((struct dfs_sb_info *)inode->i_sb->sb_data)->max_file_size;

	if (new_len < 0 || new_len > max_l)
		return -1;

	inode->length = new_len;

	return 0;
}

static struct inode *dfs_ilookup(char const *path, struct dentry const *dir) {
	struct dfs_dir_entry dirent;
	struct inode *inode;

	assert(path);
	assert(dir);

	inode = dvfs_alloc_inode(dfs_sb());

	if (!inode)
		return NULL;

	inode->i_no = ino_from_path(path);

	if (inode->i_no < 0) {
		dvfs_destroy_inode(inode);
		return NULL;
	}

	dfs_read_dirent(inode->i_no, &dirent);

	inode->start_pos = dirent.pos_start;
	inode->length    = dirent.len;

	return inode;
}

static struct inode *dfs_iterate(struct inode *inode, struct dir_ctx *ctx) {
	struct dfs_dir_entry dirent;
	struct inode *ent;
	//assert(inode);
	assert(ctx);

	if (ctx->pos >= ((struct dfs_sb_info *) dfs_sb()->sb_data)->inode_count)
		return NULL;

	dfs_read_dirent(ctx->pos, &dirent);

	ent = dvfs_alloc_inode(dfs_sb());

	if (!ent)
		return NULL;

	*ent = (struct inode) {
		.i_no      = ctx->pos,
		.start_pos = dirent.pos_start,
		.length    = dirent.len,
		.i_sb      = dfs_sb(),
		.i_ops     = &dfs_iops,
	};

	return ent;
}

static int dfs_pathname(struct inode *inode, char *buf) {
	struct dfs_dir_entry dirent;

	assert(inode);

	dfs_read_dirent(inode->i_no, &dirent);
	strcpy(buf, dirent.name);

	return 0;
}

struct inode_operations dfs_iops = {
	.create   = dfs_icreate,
	.lookup   = dfs_ilookup,
	.mkdir    = NULL,
	.rmdir    = NULL,
	.iterate  = dfs_iterate,
	.truncate = dfs_itruncate,
	.pathname = dfs_pathname,
};

static int dfs_open(struct inode *node, struct file *desc) {
	//struct dfs_dir_entry dirent;

	if (!desc || !node) {
		return ENOENT;
	}

	//dfs_read_dirent(node->i_no, &dirent);

	desc->f_ops = &dfs_fops;

	return 0;
}

static int dfs_close(struct file *desc) {
	/* if (desc)
		free(desc); */
	return 0;
}

static size_t dfs_write(struct file *desc, void *buf, size_t size) {
	assert(desc);
	assert(desc->f_inode);
	assert(buf);
	int pos = pos_from_page(desc->f_inode->start_pos) + desc->pos;
	int l   = min(size, desc->f_inode->length - pos);

	assert(l >= 0);

	dfs_write_raw(pos, buf, l);
	desc->pos += l;

	return l;
}

size_t dfs_read(struct file *desc, void *buf, size_t size) {
	assert(desc);
	assert(desc->f_inode);
	assert(buf);

	int pos = pos_from_page(desc->f_inode->start_pos) + desc->pos;
	int l   = min(size, desc->f_inode->length - pos);

	assert(l >= 0);

	_read(pos, buf, l);
	desc->pos += l;

	return l;
}

struct file_operations dfs_fops = {
	.open = dfs_open,
	.close = dfs_close,
	.write = dfs_write,
	.read = dfs_read,
	.ioctl = NULL,
};

static struct super_block *dfs_alloc_sb(char *dev) {
	return dfs_sb();
}

static struct dumb_fs_driver dfs_dumb_driver = {
	.name = "DumbFS",
	.alloc_sb = &dfs_alloc_sb,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &dfs_dumb_driver);

static struct dfs_sb_info dfs_info;
struct super_block *dfs_sb(void) {
	static struct super_block sb = {
		.fs_drv     = &dfs_dumb_driver,
		.root       = NULL,
		.inode_list = NULL,
		.sb_ops     = &dfs_sbops,
		.sb_iops    = &dfs_iops,
		.sb_fops    = &dfs_fops,
		.sb_data    = &dfs_info,
	};

	if (!sb.bdev)
		sb.bdev = dfs_flashdev->bdev;

	return &sb;
}

extern struct flash_dev stm32_flash;
int dfs_init(void) {
	dfs_set_dev(&stm32_flash);

	dfs_read_sb_info(dfs_sb()->sb_data);

	return 0;
};

