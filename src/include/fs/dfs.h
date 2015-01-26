/* @author Denis Deryugin
 * @date 26 Dec 2014
 *
 * NAND-based Dumb File System (DFS).
 */

#ifndef _DFS_H_
#define _DFS_H_

#include <drivers/flash/flash_dev.h>

#define DFS_INODES_MAX 1
#define DFS_NAME_MAX_LENGTH 16

struct dfs_inode {
	int num;
	int page_start;
	int len;
	char name[DFS_NAME_MAX_LENGTH];
};

struct dfs_superblock {
	int sb_size; /* size of this structure */
	int inode_count;
	int max_inode_count;
	int buff_bk; /* For buffer-based writing */
};

struct dfs_desc {
	int pos;
	size_t len;
	struct dfs_inode *node;
};

extern int dfs_init(void);
extern int dfs_mount(void);
extern int dfs_set_dev(struct flash_dev *new_dev);
extern struct flash_dev *dfs_get_dev(void);

/* Inode interface */
int dfs_read_inode(int n, struct dfs_inode*);
void dfs_write_inode(int n);
void dfs_delete_inode(int n);

/* Directory interface */
void dfs_link(); /* NIY */
void dfs_unlink(); /* NIY */
void dfs_mkdir(); /* NIY */
void dfs_rmdir(); /* NIY */
extern int dfs_rename(struct dfs_desc *fd, const char *name);

/* File interface */
extern struct dfs_desc *dfs_open(const char *path);
extern int dfs_close(struct dfs_desc *fd);
extern int dfs_write(struct dfs_desc *fd, void *buf, size_t size);
extern int dfs_read(struct dfs_desc *fd, void *buf, size_t size);
extern int dfs_create(struct dfs_inode *parent, struct dfs_inode *new_node);
extern int dfs_stat(struct dfs_desc *fd, struct stat *buf);
extern void dfs_lseek(); /* NIY */
extern void dfs_mmap(); /* NIY */

extern int dfs_read_superblock(void);

#endif
