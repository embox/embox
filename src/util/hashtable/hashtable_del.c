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
#include <string.h>

#include <util/dlist.h>
#include <util/hashtable.h>

struct hashtable_item *hashtable_del(struct hashtable *ht, void *key) {
	size_t idx;
	struct hashtable_item *htel;

	assert(ht);

	idx = ht->get_hash_key(key) % ht->table_size;
	dlist_foreach_entry(htel, &ht->table[idx].list, lnk) {
		if (0 == ht->cmp(key, htel->key)) {
			dlist_del_init(&htel->lnk);
			dlist_del_init(&htel->general_lnk);
			ht->table[idx].cnt--;
			return htel;
		}
	}

	return NULL;
}
