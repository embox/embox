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

TASK_RESOURCE_DECLARE(static,
                      task_vfs,
                      struct task_vfs,
                      .init   = task_vfs_init,);

static void task_vfs_init(const struct task *task, void *task_vfs) {
	struct task_vfs *fs = task_vfs;
	memset(fs, 0, sizeof(struct task_vfs));

	fs->root = fs->pwd = dvfs_root();
}

struct task_vfs *task_resource_vfs(const struct task *task) {
	return task_resource(task, &task_vfs);
}
