/**
 * @brief
 *
 * @date Mar 28 2015
 * @author Denis Deryugin
 */
#include <assert.h>
#include <stddef.h>

#include <fs/dentry.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/vfs.h>

#include "vfs.h"

TASK_RESOURCE_DEF(task_vfs_desc, struct task_vfs);

static void task_vfs_init(const struct task *task, void *task_vfs) {
	struct task_vfs *fs;

	fs = task_vfs;
	fs->pwd = dvfs_root();
	fs->root = dvfs_root();
}

static int task_vfs_inherit(const struct task *task,
    const struct task *parent) {
	struct task_vfs *fs_self;
	struct task_vfs *fs_parent;

	fs_self = task_resource_vfs(task);
	fs_parent = task_resource_vfs(parent);

	assert(fs_self);
	assert(fs_parent);

	fs_self->pwd = fs_parent->pwd;
	fs_self->root = fs_parent->root;

	return 0;
}

static size_t task_vfs_offset;

static const struct task_resource_desc task_vfs_desc = {
    .init = task_vfs_init,
    .inherit = task_vfs_inherit,
    .resource_size = sizeof(struct task_vfs),
    .resource_offset = &task_vfs_offset,
};

struct task_vfs *task_resource_vfs(const struct task *task) {
	assert(task != NULL);

	return (void *)task->resources + task_vfs_offset;
}
