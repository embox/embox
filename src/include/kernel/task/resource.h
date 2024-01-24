/**
 * @file
 *
 * @date Nov 13, 2013
 * @author: Anton Bondarev
 */

#ifndef KERNEL_TASK_RESOURCE_H_
#define KERNEL_TASK_RESOURCE_H_

#include <stddef.h>
#include <lib/libds/array.h>

struct task;
struct thread;

struct task_resource_desc {
	void (*init)(const struct task *task, void *resource_space);
	int (*inherit)(const struct task *task,
			const struct task *parent);
	void (*deinit)(const struct task *task);
	size_t resource_size; /* to be used in on-stack allocation */
	size_t *resource_offset;
	int (*exec)(const struct task *task, const char *path, char *const argv[]);
};

#define TASK_RESOURCE_DEF(desc, type) \
	typeof(type) __kernel_task_resource_storage_##desc \
			__attribute__((aligned(sizeof(void *)), \
						section(".bss.embox.ktask.resource"))); \
	static const struct task_resource_desc desc; \
	ARRAY_SPREAD_DECLARE(const struct task_resource_desc *const, \
			task_resource_desc_array); \
	ARRAY_SPREAD_ADD(task_resource_desc_array, &desc)

#define TASK_RESOURCE_DECLARE(desc_modifies, desc_name, resource_type, ...) \
	static size_t __ ## desc_name ## _offset; \
	desc_modifies const struct task_resource_desc desc_name = { \
		__VA_ARGS__ \
		.resource_size = sizeof(resource_type), \
		.resource_offset = &__ ## desc_name ## _offset, \
	}; \
	TASK_RESOURCE_DEF(desc_name, resource_type)

extern char _ktask_resource_start, _ktask_resource_end;
#define TASK_RESOURCE_SIZE \
	((size_t)(&_ktask_resource_end - &_ktask_resource_start))

extern void task_resource_init(const struct task *task);
extern int task_resource_inherit(const struct task *task,
		const struct task *parent);
extern void task_resource_deinit(const struct task *task);
extern void task_resource_exec(const struct task *task, const char *path, char *const argv[]);

ARRAY_SPREAD_DECLARE(const struct task_resource_desc *const,
		task_resource_desc_array);

#define task_resource_foreach(item) \
	array_spread_foreach(item, task_resource_desc_array)

#include <assert.h>
#include <kernel/task.h>
static inline void *task_resource(const struct task *task,
		const struct task_resource_desc *rdesc) {
	assert(task != NULL);
	return (void *) task->resources + *rdesc->resource_offset;
}

static inline void *task_self_resource(const struct task_resource_desc *rdesc) {
	return task_resource(task_self(), rdesc);
}

#endif /* KERNEL_TASK_RESOURCE_H_ */
