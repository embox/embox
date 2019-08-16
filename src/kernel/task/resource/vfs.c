/**
 * @filoe
 * @brief
 *
 * @date Mar 28 2015
 * @author Denis Deryugin
 */

#include <fs/dvfs.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/vfs.h>

static void task_vfs_init(const struct task *task, void *task_vfs);
static int task_vfs_inherit(const struct task *task, const struct task *parent);

TASK_RESOURCE_DECLARE(static,
                      task_vfs,
                      struct task_vfs,
                      .init    = task_vfs_init,
                      .inherit = task_vfs_inherit,);

static void task_vfs_init(const struct task *task, void *task_vfs) {
	struct task_vfs *fs = task_vfs;
	memset(fs, 0, sizeof(struct task_vfs));

	fs->root = fs->pwd = dvfs_root();
}

static int task_vfs_inherit(const struct task *task,
		const struct task *parent) {
	struct task_vfs *fs_self = task_resource_vfs(task);
	struct task_vfs *fs_parent = task_resource_vfs(parent);

	assert(fs_self);
	assert(fs_parent);

	fs_self->pwd  = fs_parent->pwd;
	fs_self->root = fs_parent->root;

	return 0;
}

struct task_vfs *task_resource_vfs(const struct task *task) {
	return task_resource(task, &task_vfs);
}
