/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.06.2012
 */

#include <kernel/task/task_table.h>

#include <util/idx_table.h>
#include <embox/unit.h>
#include <module/embox/kernel/task/multi.h>

EMBOX_UNIT_INIT(task_table_unit_init);

#define TASK_QUANT OPTION_MODULE_GET(embox__kernel__task__multi, NUMBER, tasks_quantity)

UTIL_IDX_TABLE_DEF(struct task *, task_table, TASK_QUANT);

int task_table_add(struct task *task) {
	return util_idx_table_add((util_idx_table_t *) &task_table, task);
}

struct task *task_table_get(int n) {
	return (struct task *) util_idx_table_get((util_idx_table_t *) &task_table, n);
}

void task_table_del(int n) {
	util_idx_table_del((util_idx_table_t *) &task_table, n);
}

int task_table_has_space(void) {
	return (util_idx_table_next_alloc((util_idx_table_t *) &task_table) >= 0);
}

static int task_table_unit_init(void) {
	UTIL_IDX_TABLE_INIT(&task_table, TASK_QUANT);
	return 0;
}
