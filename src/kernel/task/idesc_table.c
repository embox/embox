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

#include <fs/idesc.h>
#include <kernel/task.h>

#include <kernel/task/idesc_table.h>
#include <util/indexator.h>



#define idesc_cloexec_set(idesc) \
	idesc = (struct idesc *)(((uintptr_t)idesc) | 0x1)

#define idesc_cloexec_clear(idesc) \
	idesc = (struct idesc *)((uintptr_t)t->idesc_table[idx] & ~0x1)

#define idesc_is_cloexec(idesc)  ((uintptr_t)idesc & 0x1)

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

int idesc_table_del(struct idesc_table *t, int idx) {
	struct idesc *idesc;

	assert(t);
	assert(idesc_index_valid(idx));

	idesc = idesc_table_get_desc(t, idx);
	assert(idesc);
	//assert(idesc->idesc_ops && idesc->idesc_ops->close);

	if (!(idesc->idesc_count--)) {
		//idesc->idesc_ops->close(idesc);
	}

	index_free(&t->indexator, idx);

	t->idesc_table[idx] = NULL;

	return 0;
}

struct idesc *idesc_table_get_desc(struct idesc_table *t, int idx) {
	struct idesc *idesc;

	assert(t);
	assert(idesc_index_valid(idx));

	idesc = t->idesc_table[idx];

	return idesc_cloexec_clear(idesc);
}


int idesc_table_init(struct idesc_table *t) {
	assert(t);
	index_init(&t->indexator, 0, IDESC_QUANTITY, t->index_buffer);

	return 0;
}

int idesc_table_finit(struct idesc_table *t) {
	int i;
	struct idesc *idesc;

	assert(t);

	for(i = 0; i < IDESC_QUANTITY; i++) {
		if (t->idesc_table[i]) {
			idesc = idesc_table_get_desc(t, i);
			assert(idesc);
			idesc_table_del(t, i);
		}
	}

	return 0;
}

int idesc_table_fork(struct idesc_table *t, struct idesc_table *parent_table) {
	int i;
	struct idesc *idesc;

	assert(t);

	idesc_table_init(t);

	if (parent_table == NULL) {
		return 0;
	}

	for (i = 0; i < IDESC_QUANTITY; i++) {
		if (parent_table->idesc_table[i]) {
			idesc = idesc_table_get_desc(parent_table, i);
			assert(idesc);
			if (!idesc_is_cloexec(t->idesc_table[i])) {
				idesc_table_add(t, idesc, 0);
			}
		}
	}

	return 0;
}

int idesc_table_exec(struct idesc_table *t) {
	//TODO this for exec() only
	return 0;
}
