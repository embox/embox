/**
 * @file
 * @brief Implementation of hashtable data structure
 *
 * @date 30.09.11
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 */

#include <util/list.h>
#include <stdlib.h>
#include <mem/objalloc.h>
#include <util/hashtable.h>

struct hashtable_element {
	struct list_link lnk;
	void *key;
	void *value;
};

struct hashtable_entry {
	struct list list;
	size_t cnt;
};

struct hashtable {
	struct hashtable_entry *table;
	get_hash_ft get_hash_key;
	size_t table_size;
	size_t key_size;
	size_t value_size;
};

#define CONFIG_HASHTABLES_QUANTITY     0x10
#define CONFIG_HASHTABLE_ELEM_QUNTITY  0x10

OBJALLOC_DEF(ht_pool, struct hashtable, CONFIG_HASHTABLES_QUANTITY);
OBJALLOC_DEF(ht_elem_pool, struct hashtable_entry,
		CONFIG_HASHTABLES_QUANTITY * CONFIG_HASHTABLE_ELEM_QUNTITY);


struct hashtable *hashtable_create(size_t table_size, size_t key_size,
		size_t value_size, get_hash_ft get_hash) {
	struct hashtable *ht;

	if (NULL == (ht = objalloc(&ht_pool))) {
		return NULL;
	}

	if (NULL ==	(ht->table = malloc(table_size * sizeof(struct hashtable_entry)))) {
		objfree(&ht_pool, ht);
		return NULL;
	}
	ht->get_hash_key = get_hash;
	ht->table_size = table_size;
	ht->key_size = key_size;
	ht->value_size = value_size;

	return ht;
}

void hashtable_put(struct hashtable *ht, void *key, void *value) {
	size_t idx;
	struct hashtable_element *htel;

	assert(ht);

	if (NULL == (htel = objalloc(&ht_elem_pool))) {
		return;
	}

	idx = ht->get_hash_key(key) % ht->key_size;

	list_add_first_link(&htel->lnk, &ht->table[idx].list);
	ht->table[idx].cnt ++;
}

void *hashtable_get(struct hashtable *ht, void* key) {
	return NULL;
}

void hashtable_del(struct hashtable *ht, void *key, void *value) {
}

void hashtable_destroy(struct hashtable *ht) {
	free(ht->table);
	objfree(&ht_pool, ht);
}
