/* @author Denis Deryugin
 * @date 26 Dec 2014
 *
 * NAND-based Dumb File System (DFS).
 */

#ifndef _DFS_H_
#define _DFS_H_

#include <drivers/flash/flash.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <stdint.h>

#define DFS_INODES_MAX \
	OPTION_MODULE_GET(embox__fs__driver__dfs, NUMBER, inode_count)
#define DFS_NAME_LEN \
	OPTION_MODULE_GET(embox__fs__driver__dfs, NUMBER, max_name_len)

#define DFS_POS_MASK     0x00FF
#define DFS_DIR_MASK     0xFF00
#define DFS_DIRENT_EMPTY 0xFF

/* Non-VFS declarations */
struct dfs_sb_info {
	uint8_t magic[2];
	uint32_t inode_count;
	uint32_t max_inode_count;
	uint32_t buff_bk;         /* For buffer-based writing */
	uint32_t free_space;
};

struct dfs_dir_entry {
	uint8_t name[DFS_NAME_LEN];
	uint32_t pos_start;
	uint32_t len;
	uint32_t flags;
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

