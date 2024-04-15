/**
 * @brief An implementation of 'hashtable' interface
 *
 * @date 30.09.11
 *
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>

#include <lib/libds/dlist.h>
#include <lib/libds/hashtable.h>

int hashtable_put(struct hashtable *ht, struct hashtable_item *ht_item) {
	size_t idx;

	assert(ht);
	assert(ht_item);

	idx = ht->get_hash_key(ht_item->key) % ht->table_size;
	if (0 == ht->table[idx].cnt) {
		dlist_init(&ht->table[idx].list);
	}

	ht->table[idx].cnt++;
	dlist_add_next(&ht_item->lnk, &ht->table[idx].list);

	dlist_add_prev(dlist_head_init(&ht_item->general_lnk), &ht->all);

	return ENOERR;
}
