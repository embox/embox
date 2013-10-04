/**
 * @file
 *
 * @date Oct 3, 2013
 * @author: Anton Bondarev
 */
#include <util/indexator.h>
#include <kernel/task.h>
#include <kernel/task/thread_key_table.h>


void thread_key_table_create(struct task *task) {
	struct thread_key_table *kt;

	kt = &task->key_table;

	index_init(&kt->indexator, 0, THREAD_KEYS_QUANTITY, kt->ind_buff);
}
