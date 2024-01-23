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
#include <stddef.h>

#include <util/dlist.h>
#include <util/hashtable.h>

void *hashtable_get(struct hashtable *ht, void *key) {
	size_t idx;
	struct hashtable_item *htel;

	assert(ht);

	idx = ht->get_hash_key(key) % ht->table_size;
	if (!ht->table[idx].cnt) {
		return NULL;
	}

	dlist_foreach_entry(htel, &ht->table[idx].list, lnk) {
		if (0 == ht->cmp(key, htel->key)) {
			return htel->value;
		}
	}

	return NULL;
}
