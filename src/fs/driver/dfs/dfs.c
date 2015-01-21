/* @author Denis Deryugin
 * @date 26 Dec 2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <fs/dfs.h>
#include <framework/mod/options.h>

struct flash_dev *dfs_flashdev;
struct dfs_superblock dfs_sb;

#define NAND_PAGE_SIZE 8
#define NAND_BLOCK_SIZE (dfs_flashdev->block_info.block_size)
#define NAND_PAGES_PER_BLOCK (NAND_BLOCK_SIZE / NAND_PAGE_SIZE)

/* XXX Hardcode */
char *file_name[] = { "set", };
int file_len[] = { 136, };

/* Converting */
static inline int page_capacity(int bytes) {
	return (bytes + NAND_PAGE_SIZE - 1) / NAND_PAGE_SIZE;
}

static inline int pos_from_page(int pg) { return pg * NAND_PAGE_SIZE; }
static inline int page_from_pos(int pos) { return pos / NAND_PAGE_SIZE; }
static inline int pos_from_block(int block) { return block * NAND_BLOCK_SIZE; }
static inline int block_from_pos(int pos) { return pos / NAND_BLOCK_SIZE; }

static inline int _erase(unsigned int block) {
	return flash_erase(dfs_flashdev, block);
}

static inline int _read(unsigned long offset, void *buff, size_t len) {
	return flash_read(dfs_flashdev, offset, buff, len);
}

static inline int _write(unsigned long offset, const void *buff, size_t len) {
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
	int buff_bk = dfs_sb.buff_bk;
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

	_copy(buff_bk * NAND_BLOCK_SIZE + pos, last_bk * NAND_BLOCK_SIZE + pos, NAND_BLOCK_SIZE - pos);
	_blkcpy(last_bk, buff_bk);
	return 0;
}

int dfs_read_superblock(void) {
	return _read(0, (void *) &dfs_sb, sizeof(dfs_sb));
}

static int dfs_write_superblock(void) {
	return dfs_write_raw(0, (void *) &dfs_sb, sizeof(dfs_sb));
}

int dfs_read_inode(int n, struct dfs_inode *node) {
	if (n >= dfs_sb.inode_count)
		return -1;

	_read(sizeof(struct dfs_superblock) +
			n * sizeof(struct dfs_inode),
			node, sizeof(struct dfs_inode));
	return 0;
}

int dfs_init(void) {
	int node_pt, file_pt;
	struct dfs_inode node;
	printf("Writing DFS image...\n");

	dfs_sb.sb_size = sizeof(dfs_sb);
	dfs_sb.max_inode_count = DFS_INODES_MAX;
	/* XXX Hardcode */
	dfs_sb.inode_count = 1;
	dfs_sb.buff_bk = 2;

	dfs_write_superblock();

	node_pt = page_capacity(sizeof(dfs_sb));
	file_pt = node_pt + dfs_sb.max_inode_count * page_capacity(sizeof(struct dfs_inode));
	for (int i = 0; i < dfs_sb.inode_count; i++) {
		printf("\tAdding file: %s, %d bytes\n", file_name[i], file_len[i]);

		node.num = i;
		node.page_start = file_pt;
		node.len = page_capacity(file_len[i]);;
		strcpy(node.name, file_name[i]);

		dfs_write_raw(pos_from_page(node_pt), &node, sizeof(node));
		node_pt += page_capacity(sizeof(node));
		file_pt += page_capacity(page_capacity(file_len[i]));
	}

	return 0;
}

static int inode_from_path(const char *path) {
	struct dfs_inode node;

	for (int i = 0; i < DFS_INODES_MAX; i++)
		if (!dfs_read_inode(i, &node) && strcmp(path, node.name) == 0)
			return i;

	return -1;
}

/*---------------------------------*\
 	File System Interface
\*---------------------------------*/

struct dfs_desc *dfs_open(const char *path) {
	struct dfs_desc *fd = malloc(sizeof(struct dfs_desc));
	struct dfs_inode *node = malloc(sizeof(struct dfs_inode));

	dfs_read_inode(inode_from_path(path), node);

	fd->pos = 0;
	fd->node = node;
	fd->len = node->len * NAND_PAGE_SIZE;

	return fd;
}

int dfs_close(struct dfs_desc *fd) {
	if (fd) {
		if (fd->node)
			free(fd->node);
		free(fd);
	}
	return 0;
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

int dfs_stat(struct dfs_desc *fd, struct stat *buf) {
	memset(buf, 0, sizeof(struct stat));

	buf->st_mode = S_IFREG | 0666;
	buf->st_nlink = 1;
	buf->st_size = fd->len;

	return 0;
}

