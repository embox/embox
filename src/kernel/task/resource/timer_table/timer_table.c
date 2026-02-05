/**
 * @file
 *
 * @date 5.02.2026
 * @author: Anton Bondarev
 */

#include <stddef.h>
#include <assert.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/thread/sync/mutex.h>

#include <lib/libds/indexator.h>

#include <module/embox/kernel/task/resource/timer_table.h>

#include <framework/mod/options.h>

#define MODOPS_TASK_TIMER_MAX   OPTION_GET(NUMBER, task_timer_max)

struct timer_table {
	struct task_resource_timer_desc desc[MODOPS_TASK_TIMER_MAX];
	struct indexator indexator;
	index_data_t index_buffer[INDEX_DATA_LEN(MODOPS_TASK_TIMER_MAX)];
	struct mutex mutex;	
};

TASK_RESOURCE_DEF(task_timer_table_desc, struct timer_table);

static void task_timer_table_init(const struct task *task,
									void *timer_table_space) {
	struct timer_table *t;

	assert(timer_table_space == task_resource_timer_table(task));

	t = timer_table_space;

	index_init(&t->indexator, 0, MODOPS_TASK_TIMER_MAX, t->index_buffer);
	mutex_init(&t->mutex);
}

static size_t task_timer_table_offset;

static const struct task_resource_desc task_timer_table_desc = {
	.init = task_timer_table_init,
	.resource_size = sizeof(struct timer_table),
	.resource_offset = &task_timer_table_offset
};

struct timer_table *task_resource_timer_table(const struct task *task) {
	assert(task != NULL);

	return (void *)task->resources + task_timer_table_offset;
}

/********************************/ 
int task_resource_timer_table_alloc_index(const struct task *task) {
	struct timer_table *t;
	int res;
	size_t idx;

	t = task_resource_timer_table(task);

	mutex_lock(&t->mutex);
	{
		idx = index_find(&t->indexator, INDEX_MIN);
		if (idx == INDEX_NONE) {
			res = -EBUSY;
		} else {
			index_lock(&t->indexator, idx);
			res = (int) idx;
		}
	}
	mutex_unlock(&t->mutex);

	return res;
}

int task_resource_timer_table_free_index(const struct task *task, int idx) {
	struct timer_table *t;

	t = task_resource_timer_table(task);

	mutex_lock(&t->mutex);
	{
		index_unlock(&t->indexator, idx);
	}
	mutex_unlock(&t->mutex);

	return 0;
}

int task_resource_timer_table_add(const struct task *task, int idx,
										struct task_resource_timer_desc *desc) {
	struct timer_table *t;

	t = task_resource_timer_table(task);

	t->desc[idx] = *desc;

	return 0;
}

int task_resource_timer_table_del(const struct task *task, int idx) {

	return 0;
}

struct task_resource_timer_desc *task_resource_timer_table_get(
											const struct task *task, int idx) {
	struct timer_table *t;

	t = task_resource_timer_table(task);

	return &t->desc[idx];
}