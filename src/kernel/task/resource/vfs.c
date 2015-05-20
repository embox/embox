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

TASK_RESOURCE_DEF(task_vfs, struct task_vfs);

static void task_vfs_init(const struct task *task, void *task_vfs) {
	struct task_vfs *fs = task_vfs;
	memset(fs, 0, sizeof(struct task_vfs));

	fs->root = fs->pwd = dvfs_root();
}

static void task_vfs_deinit(const struct task *task) {
	/* TODO destroy file descriptors */
}

static size_t task_vfs_offset;

static const struct task_resource_desc task_vfs = {
	.init   = task_vfs_init,
	.deinit = task_vfs_deinit,
	.resource_size = sizeof(struct task_vfs),
	.resource_offset = &task_vfs_offset
};

struct task_vfs *task_resource_vfs(const struct task *task) {
	return (void*)task->resources + task_vfs_offset;
}

