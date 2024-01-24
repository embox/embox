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

struct hashtable_item *hashtable_item_init(struct hashtable_item *ht_item,
    void *key, void *value) {
	assert(ht_item);

	ht_item->key = key;
	ht_item->value = value;

	dlist_head_init(&ht_item->lnk);

	return ht_item;
}
