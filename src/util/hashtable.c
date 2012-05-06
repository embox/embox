/**
 * @file
 *
 * @brief An implementation of 'hashtable' interface
 *
 * @date 30.09.11
 *
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 * @author Anton Bondarev
 */
#include <stdlib.h>
#include <mem/objalloc.h>


#include <util/array.h>
#include <util/list.h>

#include <util/hashtable.h>

#include <embox/unit.h>

//#include <module/embox/util/hashtable.h>

#define CONFIG_HASHTABLES_QUANTITY     OPTION_GET(NUMBER,hashtables_quantity)
#define CONFIG_HASHTABLE_ELEM_QUNTITY  OPTION_GET(NUMBER,item_quntity)

struct hashtable_element {
	struct list_link lnk;
	void *key;
	void *value;
};

struct hashtable_entry {
	struct list list;
	size_t cnt;
};

/**
 * handler of hash-table.
 * It contains size of hash-table array, compare element function, calculation
 * hash index function and array of the table entry. table entry is the list of
 *  the element with equivalent hash index.
 */
struct hashtable {
	struct hashtable_entry *table; /**< array of the tables entry */
	get_hash_ft get_hash_key; /**< handler of the calculation index function */
	ht_cmp_ft cmp; /** < handler of the compare elements function */
	size_t table_size; /** size of the array of the table entry */
};



OBJALLOC_DEF(ht_pool, struct hashtable, CONFIG_HASHTABLES_QUANTITY);
OBJALLOC_DEF(ht_elem_pool, struct hashtable_element,
		CONFIG_HASHTABLES_QUANTITY * CONFIG_HASHTABLE_ELEM_QUNTITY);


struct hashtable *hashtable_create(size_t table_size, get_hash_ft get_hash, ht_cmp_ft cmp) {
	struct hashtable *ht;
	int i;

	if (NULL == (ht = objalloc(&ht_pool))) {
		return NULL;
	}

	if (NULL ==	(ht->table = malloc(table_size * sizeof(struct hashtable_entry)))) {
		objfree(&ht_pool, ht);
		return NULL;
	}
	for(i = 0; i < table_size; i ++) {
		list_init(&ht->table[i].list);
	}
	ht->get_hash_key = get_hash;
	ht->table_size = table_size;
	ht->cmp = cmp;

	return ht;
}

void hashtable_put(struct hashtable *ht, void *key, void *value) {
	size_t idx;
	struct hashtable_element *htel;

	assert(ht);

	if (NULL == (htel = objalloc(&ht_elem_pool))) {
		return;
	}
	htel->key = key;
	htel->value = value;

	idx = ht->get_hash_key(key) % ht->table_size;

	list_add_first_link(&htel->lnk, &ht->table[idx].list);
	ht->table[idx].cnt ++;
}

void *hashtable_get(struct hashtable *ht, void* key) {
	size_t idx;
	struct hashtable_element *htel;

	assert(ht);

	idx = ht->get_hash_key(key) % ht->table_size;
	list_foreach(htel, &ht->table[idx].list, lnk) {
		if(0 == ht->cmp(key, htel->key)) {
			return htel->value;
		}
	}

	return NULL;
}

void hashtable_del(struct hashtable *ht, void *key, void *value) {
	size_t idx;
	struct hashtable_element *htel;

	assert(ht);

	idx = ht->get_hash_key(key) % ht->table_size;
	list_foreach(htel, &ht->table[idx].list, lnk) {
		if(0 == ht->cmp(key, htel->key)) {
			list_unlink_link(&htel->lnk);
			objfree(&ht_elem_pool, htel);
			return;
		}
	}
}

void hashtable_destroy(struct hashtable *ht) {
	int i;
	struct hashtable_element *htel;

	assert(ht);

	for(i = 0; i < ARRAY_SIZE(ht->table); i ++) {
		list_foreach(htel, &ht->table[i].list, lnk) {
			list_unlink_link(&htel->lnk);
			objfree(&ht_elem_pool, htel);
		}

	}
	free(ht->table);
	objfree(&ht_pool, ht);
}
