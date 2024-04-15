/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <errno.h>
#include <embox/unit.h>
#include <kernel/task/task_table.h>
#include <lib/libds/bitmap.h>
#include <framework/mod/options.h>

EMBOX_UNIT_INIT(task_table_module_init);

#define TABLE_SZ OPTION_GET(NUMBER, task_table_size)

static struct task *task_table[TABLE_SZ];
static unsigned long task_table_bm[BITMAP_SIZE(TABLE_SZ)];

/*
 * tasks are started from 1, table from 0;
 * doing 1+ translation to output, and 1- for input
 */
#define TID2IDX(tid) (tid - 1)
#define IDX2TID(idx) (idx + 1)

#define IDXCHK(idx) (idx >= 0 && idx < TABLE_SZ)

#define BMFOUND(idx, size) (idx < size)

int task_table_add(struct task *tsk) {
	int idx = bitmap_find_zero_bit(task_table_bm, TABLE_SZ, 0);
	assert(BMFOUND(idx, TABLE_SZ));
	task_table[idx] = tsk;
	bitmap_set_bit(task_table_bm, idx);
	return IDX2TID(idx);
}

struct task *task_table_get(int tid) {
	int idx = TID2IDX(tid);
	assert(IDXCHK(idx));
	return bitmap_test_bit(task_table_bm, idx) ? task_table[idx] : NULL;
}

void task_table_del(int tid) {
	int idx = TID2IDX(tid);
	assert(IDXCHK(idx));
	bitmap_clear_bit(task_table_bm, idx);
}

int task_table_has_space(void) {
	int idx = bitmap_find_zero_bit(task_table_bm, TABLE_SZ, 0);
	return BMFOUND(idx, TABLE_SZ);
}

int task_table_get_first(int since) {
	int idx = TID2IDX(since);
	int set_idx = bitmap_find_bit(task_table_bm, TABLE_SZ, idx);
	return BMFOUND(set_idx, TABLE_SZ) ? IDX2TID(set_idx) : -ENOENT;
}

static int task_table_module_init(void) {
	bitmap_clear_all(task_table_bm, TABLE_SZ);
	return 0;
}
