/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#ifndef KERNEL_TASK_TASK_NO_TABLE_H_
#define KERNEL_TASK_TASK_NO_TABLE_H_

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>


static inline int task_table_add(struct task *tsk) {
	assert(tsk == task_kernel_task());
	return 0;
}

static inline struct task * task_table_get(int tid) {
	if (tid != 1) {
		return NULL;
	}
	return task_kernel_task();
}

static inline void task_table_del(int tid) {
	assert(tid > 0);
}

static inline int task_table_has_space(void) {
	return 0;
}

static inline int task_table_get_first(int since) {
	assert(since >= 0);
	return since == 1 ? 1 : -ENOENT;
}

#endif /* KERNEL_TASK_TASK_NO_TABLE_H_ */
