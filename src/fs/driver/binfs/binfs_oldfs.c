/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.05.2014
 */
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <embox/cmd.h>

//#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/fs_driver.h>
#include <fs/super_block.h>
#include <fs/dir_context.h>
#include <fs/file_desc.h>

#define BINFS_NAME "binfs"
#if 0
static int binfs_mount(struct super_block *sb, struct inode *dest) {
	return 0;
}
#endif

extern int binfs_destroy_inode(struct inode *inode) ;

extern struct file_operations binfs_fops;

extern struct inode_operations binfs_iops;

extern int binfs_fill_sb(struct super_block *sb, const char *source);

struct super_block_operations binfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = binfs_destroy_inode,
};

#if 0
static struct fsop_desc binfs_fsop = {
	//.mount = binfs_mount,
};
#endif
static struct fs_driver binfs_driver = {
	.name = BINFS_NAME,
	.fill_sb   = binfs_fill_sb,
	//.fsop = &binfs_fsop,

};

DECLARE_FILE_SYSTEM_DRIVER(binfs_driver);

FILE_SYSTEM_AUTOMOUNT("/bin", binfs_driver);

