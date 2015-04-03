/**
 * @file
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <fs/dvfs.h>
#include <kernel/task.h>
#include <kernel/task/resource/file_table.h>

int close(int fd) {
	assert(fd >= 0 && fd < FILE_TABLE_SZ);
	struct file *f = task_fs()->file[fd];
	if (f != NULL) {
		if (f->f_inode)
			dvfs_destroy_inode(f->f_inode);
		if (f->f_dentry)
			dvfs_destroy_dentry(f->f_dentry);
		dvfs_destroy_file(f);
		task_fs()->file[fd] = NULL;
	}
	return 0;
}
