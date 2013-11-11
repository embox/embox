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

#include <kernel/task/idesc_table.h>
#include <util/indexator.h>

#define idesc_index_valid(idx) ((idx >=0) && (idx < TASKS_RES_QUANTITY))

#define idesc_cloexec_set(idesc) \

#define idesc_cloexec_clear(idesc)

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
		idesc = (struct idesc *)(((uintptr_t)idesc) | 0x1);
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
		idesc = (struct idesc *)(((uintptr_t)idesc) | 0x1);
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

	idesc = idesc_table_get(t, idx);
	assert(idesc);
	assert(idesc->idesc_ops && idesc->idesc_ops->close);

	if (!(idesc->idesc_count--)) {
		//idesc->idesc_ops->close(idesc);
	}

	index_free(&t->indexator, idx);

	t->idesc_table[idx] = NULL;

	return 0;
}

struct idesc *idesc_table_get(struct idesc_table *t, int idx) {
	struct idesc *idesc;

	assert(t);
	assert(idesc_index_valid(idx));

	idesc = (struct idesc *)((uintptr_t)t->idesc_table[idx] & ~0x1);

	return idesc;
}

int idesc_table_init(struct idesc_table *t, struct idesc_table *parent_table) {

	return 0;
}
