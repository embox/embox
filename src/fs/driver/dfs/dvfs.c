/* @author Denis Deryugin
 * @date 11 Mar 2014
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
#include <util/bitmap.h>

/* Now hardcoded to use DumbFS, need to detect FS and mountpoints */
extern struct super_block *dfs_sb(void);
extern struct file_operations dfs_fops;
extern struct inode_operations dfs_iops;

int dvfs_open(const char *path, struct file *desc) {
	struct inode *inode = dfs_iops.lookup(path, NULL);

	if (!inode) {
		struct dentry d_new = { .d_sb = dfs_sb(), };
		strcpy(d_new.name, path);
		inode = dfs_iops.create(&d_new, &d_new, 0);
	}

	return dfs_fops.open(inode, desc);
}

int dvfs_write(struct file *desc, char *buf, int count) {
	return dfs_fops.write(desc, buf, count);
}

int dvfs_read(struct file *desc, char *buf, int count) {
	return dfs_fops.read(desc, buf, count);
}
