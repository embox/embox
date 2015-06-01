/**
 * @file
 * @brief Header file for VFS task resource
 * @date 28 Mar 2015
 * @author Denis Deryugin
 */

#ifndef FILE_TABLE_H_
#define FILE_TABLE_H

#include <fs/dvfs.h>
#include <kernel/task.h>

struct task_vfs {
	struct dentry *root;
	struct dentry *pwd;
};

extern struct task_vfs *task_resource_vfs(const struct task *task);

extern struct task* task_self(void);
static inline struct task_vfs *task_fs(void) {
	return task_resource_vfs(task_self());
}

#endif /* FILE_TABLE_H_ */
