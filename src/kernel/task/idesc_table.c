/**
 * @file
 *
 * @brief
 *
 * @date 09.11.2013
 * @author Anton Bondarev
 */


union idesc_table_entry {
	unsigned int flags;
	struct idesc *idesc;
};

struct idesc_table {
	union idesc_table_entry idesc_entry[TASKS_RES_QUANTITY];
	struct indexator indexator;
};

int idesc_table_add(struct idesc *idesc, int cloexec) {
	struct task task;
	struct idesc_table *idesc_table;
	int idx;

	task = task_self();
	idesc_table = &task->idesc_table;

	idx = index_alloc(&idesc_table->indexator, INDEX_MIN);

	idesc_table->idesc_entry[idx] = idesc;

	return idx;
}

int idesc_table_del(int idx) {
	struct task task;
	struct idesc_table *idesc_table;

	task = task_self();
	idesc_table = &task->idesc_table;

	idesc_table->idesc_entry[idx] = NULL;

	index_free(&idesc_table->indexator, idx);

	return 0;
}

struct idesc *idesc_table_get(int idx) {
	struct task task;
	struct idesc_table *idesc_table;

	task = task_self();
	idesc_table = &task->idesc_table;

	return idesc_table->idesc_entry[idx];
}

int idesc_table_init(struct idesc_table *idesc_table) {
	return 0;
}
