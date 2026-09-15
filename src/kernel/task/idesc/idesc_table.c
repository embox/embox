/**
 * @file
 * @brief
 *
 * @date 09.11.2013
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hal/ipl.h>
#include <kernel/spinlock.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>
#include <lib/libds/array.h>
#include <lib/libds/indexator.h>
#include <util/atomic_rmw.h>
#include <util/binalign.h>

/* One lock over every descriptor table's slot array and index allocator.
 * Interrupts are masked rather than preemption disabled, because the sections
 * are a bitmap walk and masking makes them non-preemptible for free -- so
 * nothing here touches the critical count. */
static spinlock_t idesc_tables_lock = SPIN_STATIC_UNLOCKED;

/* The image's RAM window, straight from src/arch/aarch64/embox.lds.S. */

/* Callers that already hold the lock. idesc_cloexec_clear() assigns to its
 * argument, hence the local. */
static inline struct idesc *idesc_table_get_locked(struct idesc_table *t,
    int idx) {
	struct idesc *idesc = t->idesc_table[idx];

	return idesc_cloexec_clear(idesc);
}

int idesc_index_valid(int idx) {
	return (idx >= 0) && (idx < MODOPS_IDESC_TABLE_SIZE);
}

int idesc_table_add(struct idesc_table *t, struct idesc *idesc, int cloexec) {
	int idx;
	ipl_t ipl;

	assert(t);
	assert(idesc);

	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);

	idx = index_alloc(&t->indexator, INDEX_MIN);
	if (idx == INDEX_NONE) {
		__spin_unlock(&idesc_tables_lock);
		ipl_restore(ipl);
		return -EMFILE;
	}

	atomic_rmw_add_fetch(&idesc->idesc_usage_count, 1, __ATOMIC_RELAXED);

	if (cloexec) {
		idesc_cloexec_set(idesc);
	}

	t->idesc_table[idx] = idesc;

	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);

	return idx;
}

int idesc_table_lock(struct idesc_table *t, struct idesc *idesc, int idx,
    int cloexec) {
	ipl_t ipl;

	assert(t);
	assert(idesc);
	assert(idesc_index_valid(idx));
	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);

	assert(!index_locked(&t->indexator, idx));

	index_lock(&t->indexator, idx);

	atomic_rmw_add_fetch(&idesc->idesc_usage_count, 1, __ATOMIC_RELAXED);

	if (cloexec) {
		idesc_cloexec_set(idesc);
	}

	t->idesc_table[idx] = idesc;

	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);

	return idx;
}

int idesc_table_locked(struct idesc_table *t, int idx) {
	ipl_t ipl;
	int locked;

	assert(t);
	assert(idesc_index_valid(idx));

	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);
	locked = index_locked(&t->indexator, idx);
	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);

	return locked;
}

void idesc_table_del(struct idesc_table *t, int idx) {
	struct idesc *idesc;
	ipl_t ipl;
	int last;

	assert(t);
	assert(idesc_index_valid(idx));

	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);

	idesc = idesc_table_get_locked(t, idx);
	assert(idesc);
	assert(idesc->idesc_ops && idesc->idesc_ops->close);

	last = !atomic_rmw_sub_fetch(&idesc->idesc_usage_count, 1, __ATOMIC_ACQ_REL);

	index_free(&t->indexator, idx);
	t->idesc_table[idx] = NULL;

	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);

	/* Outside the lock: close() reaches the filesystem and blocks. The slot
	 * is already gone, so nobody can find this descriptor through the table
	 * while it is being closed. */
	if (last) {
		idesc->idesc_ops->close(idesc);
	}
}

struct idesc *idesc_table_get(struct idesc_table *t, int idx) {
	struct idesc *idesc;
	ipl_t ipl;

	assert(t);
	assert(idesc_index_valid(idx));

	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);
	idesc = idesc_table_get_locked(t, idx);
	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);

	/* A slot holds NULL or a descriptor, and a descriptor contains pointers,
	 * so it is pointer-aligned. sizeof(void *), not 8: on a 32-bit target a
	 * descriptor is 4-aligned and an 8-byte demand fires on a healthy kernel.
	 *
	 * Only alignment, not a range check: descriptors do not all live inside
	 * [_ram_base, _ram_base + _ram_size) (x86/test/net, usermode86), and a
	 * check a correct kernel fails is worse than no check.
	 *
	 * The neighbouring slots are printed with it: they separate one clobbered
	 * entry from a table pointer that was wrong to begin with. */
	assertf(idesc == NULL
	            || binalign_check_bound((uintptr_t)idesc, sizeof(void *)),
	    "descriptor table %p slot %d holds %p; slots 0..3 = %p %p %p %p", t,
	    idx, idesc, t->idesc_table[0], t->idesc_table[1], t->idesc_table[2],
	    t->idesc_table[3]);

	return idesc;
}

void idesc_table_init(struct idesc_table *t) {
	ipl_t ipl;

	assert(t);

	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);
	memset(t->idesc_table, 0, sizeof t->idesc_table);
	index_init(&t->indexator, 0, ARRAY_SIZE(t->idesc_table), t->index_buffer);
	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);
}

void idesc_table_finit(struct idesc_table *t) {
	int i;

	assert(t);

	for (i = 0; i < ARRAY_SIZE(t->idesc_table); i++) {
		if (t->idesc_table[i]) {
			assert(idesc_table_get(t, i));
			idesc_table_del(t, i);
		}
	}
}

/* Alloc idesc at the specified position. */
static int idesc_table_idx_copy(struct idesc_table *t, int idx,
    struct idesc *idesc, int cloexec) {
	ipl_t ipl;

	assert(t);
	assert(idesc);

	ipl = ipl_save();
	__spin_lock(&idesc_tables_lock);

	if (index_try_lock(&t->indexator, idx) != 1) {
		__spin_unlock(&idesc_tables_lock);
		ipl_restore(ipl);
		return -1;
	}

	atomic_rmw_add_fetch(&idesc->idesc_usage_count, 1, __ATOMIC_RELAXED);

	if (cloexec) {
		idesc_cloexec_set(idesc);
	}
	t->idesc_table[idx] = idesc;

	__spin_unlock(&idesc_tables_lock);
	ipl_restore(ipl);

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
