/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <kernel/task.h>
#include <stddef.h>

static inline void task_table_init(void) {
}

static inline int task_table_add(struct task *tsk) {
	assert(tsk == task_kernel_task());
	return 0;
}

static inline struct task * task_table_get(int tid) {
	assert(tid >= 0);
	return tid == 0 ? task_kernel_task() : NULL;
}

static inline void task_table_del(int tid) {
	assert(tid > 0);
}

static inline int task_table_has_space(void) {
	return 0;
}

static inline int task_table_get_first(int since) {
	assert(since >= 0);
	return since;
}
