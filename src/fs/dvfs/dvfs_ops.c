/* @author Denis Deryugin
 * @date 27 Mar 2014
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <fs/dvfs.h>
#include <fs/dfs.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/bitmap.h>

POOL_DEF(file_pool, struct file, 4); /* XXX */

struct file *dvfs_alloc_file(void) {
	return pool_alloc(&file_pool);
}

int dvfs_destroy_file(struct file *desc) {
	pool_free(&file_pool, desc);
	return 0;
}

/* Default FS-nondependent operations */
struct inode *dvfs_default_alloc_inode(struct super_block *sb) {
	struct inode *inode = malloc(sizeof(struct inode));

	if (inode)
		*inode = (struct inode) {
			.i_no = -1,
			.i_sb = sb,
			.i_ops = sb->sb_iops,
		};

	return inode;
}

int dvfs_default_destroy_inode(struct inode *inode) {
	free(inode);
	return 0;
}

int dvfs_default_pathname(struct inode *inode, char *buf) {
	assert(inode);
	if (inode->i_dentry)
		strcpy(buf, inode->i_dentry->name);
	else
		strcpy(buf, "empty");

	return 0;
}
