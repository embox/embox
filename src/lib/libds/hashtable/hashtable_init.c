/**
 * @brief An implementation of 'hashtable' interface
 *
 * @date 30.09.11
 *
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 * @author Anton Bondarev
 */

#include <string.h>

#include <lib/libds/dlist.h>
#include <lib/libds/hashtable.h>

struct hashtable *hashtable_init(struct hashtable *ht, unsigned int table_size,
    ht_hash_ft get_hash, ht_cmp_ft cmp) {
	/* ht must have following structure:
	 * first of all place struct hashtable and then place
	 * array struct hashtable_item[size]
	 */
	ht->table = (struct hashtable_entry *)(ht + 1);

	memset(ht->table, 0, table_size * sizeof(struct hashtable_entry));

	ht->get_hash_key = get_hash;
	ht->table_size = table_size;
	ht->cmp = cmp;
	dlist_init(&ht->all);

	return ht;
}
