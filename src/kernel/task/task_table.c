/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/task_table.h>
#include <util/idx_table.h>
#include <framework/mod/options.h>

#define MODOPS_TASK_TABLE_SIZE OPTION_GET(NUMBER, task_table_size)

UTIL_IDX_TABLE_DEF(struct task *, task_table, MODOPS_TASK_TABLE_SIZE);

void task_table_init(void) {
	UTIL_IDX_TABLE_INIT(&task_table, MODOPS_TASK_TABLE_SIZE);
}

int task_table_add(struct task *tsk) {
	int tid;

	tid = util_idx_table_add((util_idx_table_t *) &task_table, tsk);
	if (tid >= 0) {
		tsk->tid = tid;
	}

	return tid;
}

struct task * task_table_get(int tid) {
	assert(tid >= 0);
	return (struct task *) util_idx_table_get((util_idx_table_t *) &task_table, tid);
}

void task_table_del(int tid) {
	assert(tid >= 0);
	util_idx_table_del((util_idx_table_t *) &task_table, tid);
}

int task_table_has_space(void) {
	return (util_idx_table_next_alloc((util_idx_table_t *) &task_table) >= 0);
}

int task_table_get_first(int since) {
	return util_idx_table_next_mark((util_idx_table_t *) &task_table, since, 1);
}
