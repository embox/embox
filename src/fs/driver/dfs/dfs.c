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
#include <framework/mod/options.h>
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
	return flash_read(dfs_flashdev, offset, buff, len);
}

static inline int _write(unsigned long offset, const void *buff, size_t len) {
	int i;
	for (i = 0; i < len; i++)
		bitmap_clear_bit(dfs_free_pages, i + offset);
	return flash_write(dfs_flashdev, offset, buff, len);
}

static inline int _copy(unsigned long to, unsigned long from, size_t len) {
	return flash_copy(dfs_flashdev, to, from, len);
}

static inline int _blkcpy(unsigned int to, unsigned long from) {
	_erase(to);
	return _copy(to * NAND_BLOCK_SIZE, from * NAND_BLOCK_SIZE, NAND_BLOCK_SIZE);
}

static int dfs_write_raw(int pos, void *buff, size_t size) {
	int start_bk = pos / NAND_BLOCK_SIZE;
	int last_bk = (pos + size) / NAND_BLOCK_SIZE;
	struct dfs_sb_info *sbi = dfs_sb()->sb_data;
	int buff_bk = sbi->buff_bk;
	int bk;

	pos %= NAND_BLOCK_SIZE;

	_erase(buff_bk);
	_copy(buff_bk * NAND_BLOCK_SIZE, start_bk * NAND_BLOCK_SIZE, pos);

	if (start_bk == last_bk) {
		_write(buff_bk * NAND_BLOCK_SIZE + pos, buff, size);
		pos += size;
	} else {
		_write(buff_bk * NAND_BLOCK_SIZE + pos, buff, NAND_BLOCK_SIZE - pos);
		_blkcpy(start_bk, buff_bk);
		buff += NAND_BLOCK_SIZE - pos;
		pos = (pos + size) % NAND_BLOCK_SIZE;

		for (bk = start_bk + 1; bk < last_bk; bk++) {
			_erase(bk);
			_write(bk * NAND_BLOCK_SIZE, buff, NAND_BLOCK_SIZE);
			buff += NAND_BLOCK_SIZE;
		}

		_erase(buff_bk);
		_write(buff_bk * NAND_BLOCK_SIZE, buff, pos);
	}

	uint8_t tmpbuff[256];

	_read(buff_bk * NAND_BLOCK_SIZE +pos, tmpbuff, 256);
	_read(last_bk * NAND_BLOCK_SIZE + pos, tmpbuff, 256);
	_copy(buff_bk * NAND_BLOCK_SIZE + pos, last_bk * NAND_BLOCK_SIZE + pos, NAND_BLOCK_SIZE - pos);

	_read(buff_bk * NAND_BLOCK_SIZE, tmpbuff, 256);

	_blkcpy(last_bk, buff_bk);
	_read(0, tmpbuff, 256);

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
	dfs_flashdev = new_dev;
	return 0;
}

struct flash_dev *dfs_get_dev(void) {
	return dfs_flashdev;
}

/*---------------------------------*\
 	File System Interface
\*---------------------------------*/
/*
struct dfs_desc *dfs_open(const char *path) {
	struct dfs_desc *fd = malloc(sizeof(struct dfs_desc));
	struct dfs_inode *node = malloc(sizeof(struct dfs_inode));

	if (!fd || !node) {
		errno = ENOMEM;
		return NULL;
	}

	if (0 > dfs_read_inode(inode_from_path(path), node)) {
		errno = ENOENT;
		return NULL;
	}

	fd->pos = 0;
	fd->node = node;
	fd->len = node->len * NAND_PAGE_SIZE;

	return fd;
}

int dfs_write(struct dfs_desc *fd, void *buf, size_t size) {
	int pos = pos_from_page(fd->node->page_start) + fd->pos;

	dfs_write_raw(pos, buf, size);

	fd->pos += size;

	return 0;
}

int dfs_read(struct dfs_desc *fd, void *buf, size_t size) {
	int pos = fd->pos + pos_from_page(fd->node->page_start);

	_read(pos, buf, size);
	fd->pos += size;

	return 0;
}

*/

static int dfs_read_sb_info(struct dfs_sb_info *sbi) {
	_read(0, sbi, sizeof(struct dfs_sb_info));
	return !(sbi->magic[0] == DFS_MAGIC_0 && sbi->magic[1] == DFS_MAGIC_1);
};

static int dfs_write_sb_info(struct dfs_sb_info *sbi) {
	dfs_write_raw(0, sbi, sizeof(struct dfs_sb_info));
	return 0;
}

static int dfs_read_dirent(int n, struct dfs_dir_entry *dtr) {
	uint32_t offt = _capacity(sizeof(struct dfs_sb_info)) +
	                n * _capacity(sizeof(struct dfs_dir_entry));

	_read(offt, dtr, sizeof(struct dfs_dir_entry));

	return 0;
}

static int dfs_write_dirent(int n, struct dfs_dir_entry *dtr) {
	uint32_t offt = _capacity(sizeof(struct dfs_sb_info)) +
	                n * _capacity(sizeof(struct dfs_dir_entry));

	dfs_write_raw(offt, dtr, sizeof(struct dfs_dir_entry));
	return 0;
};

int ino_from_path(const char *path) {
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

static struct inode *dfs_alloc_inode(struct super_block *sb) {
	struct inode *inode = malloc(sizeof(struct inode));

	if (inode)
		*inode = (struct inode) {
			.i_no = -1,
			.i_sb = sb,
			.i_ops = &dfs_iops,
		};

	return inode;
};

static int dfs_destroy_inode(struct inode *inode) {
	free(inode);
	return 0;
};

struct super_block_operations dfs_sbops = {
	.alloc_inode = dfs_alloc_inode,
	.destroy_inode = dfs_destroy_inode,
	.write_inode = NULL,
};

static struct inode *dfs_icreate(struct dentry *d_new,
                                 struct dentry *d_dir, int mode) {
	struct super_block *sb = d_dir->d_sb;
	assert(sb && sb->sb_ops && sb->sb_ops->alloc_inode);
	struct inode *i_new = sb->sb_ops->alloc_inode(sb);
	struct dfs_sb_info sbi;
	struct dfs_dir_entry dirent;

	dfs_read_sb_info(&sbi);

	if (sbi.inode_count > sbi.max_inode_count)
		return NULL;

	dirent.pos_start = sbi.free_space;
	dirent.len = 136; /* XXX */
	strcpy(dirent.name, d_new->name);
	dfs_write_dirent(sbi.inode_count, &dirent);

	*i_new = (struct inode) {
		.i_no      = sbi.inode_count,
		.start_pos = dirent.pos_start,
		.length    = dirent.len,
		.i_sb      = sb,
		.i_ops     = &dfs_iops,
	};

	sbi.inode_count++;
	sbi.free_space += dirent.len;
	dfs_write_sb_info(&sbi);

	memset(&dirent, 0xA5, sizeof(dirent));
	dfs_read_dirent(0, &dirent);

	return d_new->d_inode = i_new;
};

static int dfs_itruncate(struct inode *inode, size_t new_len) {
	int max_l = ((struct dfs_sb_info *)inode->i_sb->sb_data)->max_file_size;

	if (new_len < 0 || new_len > max_l)
		return -1;

	inode->length = new_len;

	return 0;
};

static int dfs_ipathname(struct inode *inode, char *buf) {
	strcpy(buf, inode->i_dentry->name);
	return 0;
};

static struct inode *dfs_ilookup(char const *path, struct dentry const *dir) {
	struct dfs_sb_info sbi;
	struct dfs_dir_entry dirent;
	struct inode *inode;

	dfs_read_sb_info(&sbi);

	inode = dfs_sb()->sb_ops->alloc_inode(dfs_sb());

	inode->i_no = ino_from_path(path);

	if (inode->i_no < 0) {
		dfs_sb()->sb_ops->destroy_inode(inode);
		return NULL;
	}

	dfs_read_dirent(inode->i_no, &dirent);

	inode->start_pos = dirent.pos_start;
	inode->length    = dirent.len;

	return inode;
};

struct inode_operations dfs_iops = {
	.create = dfs_icreate,
	.lookup = dfs_ilookup,
	.mkdir = NULL,
	.rmdir = NULL,
	.truncate = dfs_itruncate,
	.pathname = dfs_ipathname
};

static int dfs_open(struct inode *node, struct file *desc) {
	struct dfs_dir_entry dirent;

	if (!desc || !node) {
		return ENOENT;
	}

	dfs_read_dirent(node->i_no, &dirent);

	*desc = (struct file) {
		.pos = 0,
		.f_inode = node,
		.f_ops = &dfs_fops,
	};

	return 0;
}

static int dfs_close(struct file *desc) {
	if (desc)
		free(desc);
	return 0;
}

static size_t dfs_write(struct file *desc, void *buf, size_t size) {
	int pos = pos_from_page(desc->f_inode->start_pos) + desc->pos;
	int l   = min(size, desc->f_inode->length - pos);

	assert(l >= 0);

	dfs_write_raw(pos, buf, l);
	desc->pos += l;

	return l;
}

size_t dfs_read(struct file *desc, void *buf, size_t size) {
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

static struct dumb_fs_driver dfs_d_fs_driver = {
	.name = "DumbFS",
};

struct super_block *dfs_sb(void) {
	static struct super_block sb = {
		.fs_drv = &dfs_d_fs_driver,
		.root = NULL,
		.inode_list = NULL,
		.sb_ops = &dfs_sbops,
		.sb_data = NULL,
	};

	if (!sb.sb_data) {
		sb.sb_data = malloc(sizeof(struct dfs_sb_info));
	}
	if (!sb.bdev)
		sb.bdev = dfs_flashdev->bdev;

	return &sb;
}

extern struct flash_dev stm32_flash;
int dfs_init(void) {
	dfs_set_dev(&stm32_flash);
	dfs_format();

	dfs_read_sb_info(dfs_sb()->sb_data);

	return 0;
};


