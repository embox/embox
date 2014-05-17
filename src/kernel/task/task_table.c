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

int task_table_add(struct task *tsk) {
	assert(tsk != NULL);
	return util_idx_table_add(task_table, tsk);
}

struct task * task_table_get(int tid) {
	assert(tid >= 0);
	return util_idx_table_get(task_table, tid);
}

void task_table_del(int tid) {
	assert(tid >= 0);
	util_idx_table_del(task_table, tid);
}

int task_table_has_space(void) {
	return 0 <= util_idx_table_next_alloc(task_table);
}

int task_table_get_first(int since) {
	assert(since >= 0);
	return util_idx_table_next_mark(task_table, since, 1);
}

static int task_table_module_init(void) {
	UTIL_IDX_TABLE_INIT(task_table, MODOPS_TASK_TABLE_SIZE);
	return 0;
}
