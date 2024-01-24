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

#include <lib/libds/dlist.h>
#include <lib/libds/hashtable.h>

void hashtable_destroy(struct hashtable *ht) {
	int i;
	struct hashtable_item *htel;

	assert(ht);

	for (i = 0; i < ht->table_size; i++) {
		if (ht->table[i].cnt == 0) {
			continue;
		}
		dlist_foreach_entry(htel, &ht->table[i].list, lnk) {
			/* TODO: */
		}
	}
}
