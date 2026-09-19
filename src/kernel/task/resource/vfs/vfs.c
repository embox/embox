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

TASK_RESOURCE_DEF(task_vfs_desc, struct task_vfs);

static void task_vfs_init(const struct task *task, void *task_vfs) {
	struct task_vfs *fs;

	fs = task_vfs;
	fs->pwd = dvfs_root();
	/* A working directory holds its dentry, like an open descriptor, so
	 * neither removal nor reclaim can free it under the task. */
	if (fs->pwd) {
		dentry_ref_inc(fs->pwd);
	}
}

static int task_vfs_inherit(const struct task *task,
    const struct task *parent) {
	struct task_vfs *fs_self;
	struct task_vfs *fs_parent;

	fs_self = task_resource_vfs(task);
	fs_parent = task_resource_vfs(parent);

	assert(fs_self);
	assert(fs_parent);

	if (fs_self->pwd) {
		dentry_ref_dec(fs_self->pwd);
	}
	fs_self->pwd = fs_parent->pwd;
	if (fs_self->pwd) {
		dentry_ref_inc(fs_self->pwd);
	}

	return 0;
}

static void task_vfs_deinit(const struct task *task) {
	struct task_vfs *fs;

	fs = task_resource_vfs(task);
	if (fs->pwd) {
		dentry_ref_dec(fs->pwd);
		fs->pwd = NULL;
	}
}

static size_t task_vfs_offset;

static const struct task_resource_desc task_vfs_desc = {
    .init = task_vfs_init,
    .inherit = task_vfs_inherit,
    .deinit = task_vfs_deinit,
    .resource_size = sizeof(struct task_vfs),
    .resource_offset = &task_vfs_offset,
};

struct task_vfs *task_resource_vfs(const struct task *task) {
	assert(task != NULL);

	return (void *)task->resources + task_vfs_offset;
}
