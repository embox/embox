/**
 * @file
 * @brief Header file for VFS task resource
 * @date 28 Mar 2015
 * @author Denis Deryugin
 */

#ifndef KERNEL_TASK_RESOURCE_VFS_
#define KERNEL_TASK_RESOURCE_VFS_

#include <kernel/task.h>

struct dentry;

struct task_vfs {
	struct dentry *pwd;
};

extern struct task_vfs *task_resource_vfs(const struct task *task);

#define task_self_resource_vfs() task_resource_vfs(task_self())

#endif /* KERNEL_TASK_RESOURCE_VFS_ */
