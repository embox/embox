/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <embox/unit.h>
#include <kernel/task/task_table.h>
#include <util/idx_table.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(task_table_module_init);

#define MODOPS_TASK_TABLE_SIZE OPTION_GET(NUMBER, task_table_size)

UTIL_IDX_TABLE_DEF(struct task *, task_table, MODOPS_TASK_TABLE_SIZE);

/*
 * tasks are started from 1, util table is from 0;
 * doing 1+ translation to output, and 1- for input
 */
int task_table_add(struct task *tsk) {
	assert(tsk != NULL);
	return 1 + util_idx_table_add(task_table, tsk);
}

struct task * task_table_get(int tid) {
	assert(tid >= 0);
	return util_idx_table_get(task_table, tid - 1);
}

void task_table_del(int tid) {
	assert(tid > 0);
	util_idx_table_del(task_table, tid - 1);
}

int task_table_has_space(void) {
	return 0 <= util_idx_table_next_alloc(task_table);
}

int task_table_get_first(int since) {
	/* since should be decremented. i.e. task_foreach calls
 	 * __func__, it returns first task. Then task_foreach
	 * gets returned task id, increments it and calls __func__.
	 * Without decrement it will skip task following next returned one.
	 */
	assert(since >= 0);
	return 1 + util_idx_table_next_mark(task_table, since - 1, 1);
}

static int task_table_module_init(void) {
	UTIL_IDX_TABLE_INIT(task_table, MODOPS_TASK_TABLE_SIZE);
	return 0;
}
