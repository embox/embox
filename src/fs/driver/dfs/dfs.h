/* @author Denis Deryugin
 * @date 26 Dec 2014
 *
 * NAND-based Dumb File System (DFS).
 */

#ifndef _DFS_H_
#define _DFS_H_

#include <drivers/block_dev/flash/flash_dev.h>
#include <fs/dvfs.h>

#define DFS_INODES_MAX 4
#define DFS_NAME_LEN 16

/* Non-VFS declarations */
struct dfs_sb_info {
	char magic[2];
	int  inode_count;
	int  max_inode_count;
	int  buff_bk;         /* For buffer-based writing */
	int  max_file_size;
	int  free_space;
};

struct dfs_dir_entry {
	char name[DFS_NAME_LEN];
	int  pos_start;
	int  len;
};

extern int dfs_mount(void);
extern int dfs_set_dev(struct flash_dev *new_dev);
extern struct flash_dev *dfs_get_dev(void);
extern int dfs_read_superblock(void);

/* VFS-related declarations */

extern struct super_block_operations dfs_sbops;
extern struct inode_operations dfs_iops;
extern struct file_operations dfs_fops;

#endif

