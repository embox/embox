/**
 * @file
 * @brief implementation of hashtable data structure
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
	size_t table_size;
	size_t key_size;
	size_t value_size;
};

#define CONFIG_HASHTABLES_QUANTITY 0x10

OBJALLOC_DEF(ht_pool, struct hashtable, CONFIG_HASHTABLES_QUANTITY);

struct hashtable *hashtable_create(size_t table_size, size_t key_size, size_t value_size) {
	struct hashtable *ht;
	if(NULL == (ht = objalloc(&ht_pool))) {
		return NULL;
	}


	if( NULL == (ht->table = malloc(table_size * sizeof(struct hashtable_entry)))) {
		objfree(&ht_pool, ht);
	}

	return ht;
}

void hashtable_put(struct hashtable *ht, void *key, void *value) {
}

void *hashtable_get(struct hashtable *ht, void* key) {
	return NULL;
}

void hashtable_del(struct hashtable *ht, void *key, void *value) {
}

void hashtable_destroy(struct hashtable *ht) {
	objfree(&ht_pool, ht);
}
