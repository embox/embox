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
#include <util/member.h>

void *hashtable_get_key_next(struct hashtable *ht, void *prev_key) {
	struct hashtable_item *htel;

	assert(ht);

	if (dlist_empty(&ht->all)) {
		return NULL;
	}

	htel = member_cast_out(prev_key, struct hashtable_item, key);
	if (dlist_last(&ht->all) == &htel->general_lnk) {
		return NULL;
	}

	htel = dlist_first_entry(&htel->general_lnk, struct hashtable_item,
	    general_lnk);

	return &htel->key;
}
