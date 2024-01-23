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

#include <util/dlist.h>
#include <util/hashtable.h>

void *hashtable_get_key_first(struct hashtable *ht) {
	struct hashtable_item *htel;

	assert(ht);

	if (dlist_empty(&ht->all)) {
		return NULL;
	}

	htel = dlist_first_entry(&ht->all, struct hashtable_item, general_lnk);

	return &htel->key;
}
