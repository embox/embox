/**
 * @file
 *
 * @brief
 *
 * @date 09.11.2013
 * @author Anton Bondarev
 */
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <kernel/task/resource/idesc.h>
#include <kernel/task.h>

#include <kernel/task/resource/idesc_table.h>
#include <lib/libds/array.h>
#include <lib/libds/indexator.h>

int idesc_index_valid(int idx) {
	return (idx >=0) && (idx < MODOPS_IDESC_TABLE_SIZE);
}

int idesc_table_add(struct idesc_table *t, struct idesc *idesc, int cloexec) {
	int idx;

	assert(t);
	assert(idesc);

	idx = index_alloc(&t->indexator, INDEX_MIN);
	if (idx == INDEX_NONE) {
		return -EMFILE;
	}

	idesc->idesc_count++;

	if (cloexec) {
		idesc_cloexec_set(idesc);
	}

	t->idesc_table[idx] = idesc;

	return idx;
}

int idesc_table_lock(struct idesc_table *t, struct idesc *idesc, int idx, int cloexec) {
	assert(t);
	assert(idesc);
	assert(idesc_index_valid(idx));
	assert(!index_locked(&t->indexator, idx));

	index_lock(&t->indexator, idx);

	idesc->idesc_count++;

	if (cloexec) {
		idesc_cloexec_set(idesc);
	}

	t->idesc_table[idx] = idesc;

	return idx;
}

int idesc_table_locked(struct idesc_table *t, int idx) {
	assert(t);
	assert(idesc_index_valid(idx));

	return index_locked(&t->indexator, idx);
}

void idesc_table_del(struct idesc_table *t, int idx) {
	struct idesc *idesc;

	assert(t);
	assert(idesc_index_valid(idx));

	idesc = idesc_table_get(t, idx);
	assert(idesc);
	assert(idesc->idesc_ops && idesc->idesc_ops->close);

	if (!(--idesc->idesc_count)) {
		idesc->idesc_ops->close(idesc);
	}

	index_free(&t->indexator, idx);

	t->idesc_table[idx] = NULL;
}

struct idesc *idesc_table_get(struct idesc_table *t, int idx) {
	struct idesc *idesc;

	assert(t);
	assert(idesc_index_valid(idx));

	idesc = t->idesc_table[idx];

	return idesc_cloexec_clear(idesc);
}


void idesc_table_init(struct idesc_table *t) {
	assert(t);
	memset(t->idesc_table, 0, sizeof t->idesc_table);
	index_init(&t->indexator, 0, ARRAY_SIZE(t->idesc_table),
			t->index_buffer);
}

void idesc_table_finit(struct idesc_table *t) {
	int i;

	assert(t);

	for(i = 0; i < ARRAY_SIZE(t->idesc_table); i++) {
		if (t->idesc_table[i]) {
			assert(idesc_table_get(t, i));
			idesc_table_del(t, i);
		}
	}
}

/* Alloc idesc at the specified position. */
static int idesc_table_idx_copy(struct idesc_table *t, int idx,
	struct idesc *idesc, int cloexec) {
	assert(t);
	assert(idesc);

	if (index_try_lock(&t->indexator, idx) != 1) {
		return -1;
	}

	idesc->idesc_count++;

	if (cloexec) {
		idesc_cloexec_set(idesc);
	}
	t->idesc_table[idx] = idesc;

	return idx;
}

int idesc_table_fork(struct idesc_table *t, struct idesc_table *parent_table) {
	int i;
	struct idesc *idesc;

	assert(t);
	assert(parent_table);

	/* idesc_table_init(t); -- not required (called after idesc_table_init) */

	for (i = 0; i < ARRAY_SIZE(t->idesc_table); i++) {
		if (parent_table->idesc_table[i]) {
			idesc = idesc_table_get(parent_table, i);
			assert(idesc);
			if (!idesc_is_cloexeced(parent_table->idesc_table[i])) {
				int ret = idesc_table_idx_copy(t, i, idesc, 0);
				if (ret < 0) {
					return ret;
				}
			}
		}
	}

	return 0;
}

int idesc_table_exec(struct idesc_table *t) {
	//TODO this for exec() only
	return 0;
}
