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
#include <errno.h>

#include <util/array.h>
#include <util/list.h>

#include <util/hashtable.h>

#include <embox/unit.h>

//#include <module/embox/util/hashtable.h>

#define CONFIG_HASHTABLES_QUANTITY     OPTION_GET(NUMBER,hashtables_quantity)
#define CONFIG_HASHTABLE_ELEM_QUNTITY  OPTION_GET(NUMBER,item_quntity)

struct hashtable_element {
	struct list_link lnk;
	struct list_link general_lnk;
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
	struct list all;
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
	list_init(&ht->all);

	return ht;
}

int hashtable_put(struct hashtable *ht, void *key, void *value) {
	size_t idx;
	struct hashtable_element *htel;

	assert(ht);

	if (NULL == (htel = objalloc(&ht_elem_pool))) {
		return -ENOMEM;
	}
	htel->key = key;
	htel->value = value;

	idx = ht->get_hash_key(key) % ht->table_size;

	list_add_first_link(&htel->lnk, &ht->table[idx].list);
	ht->table[idx].cnt ++;

	list_add_last_link(&htel->general_lnk, &ht->all);

	return ENOERR;
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

int hashtable_del(struct hashtable *ht, void *key) {
	size_t idx;
	struct hashtable_element *htel;

	assert(ht);

	idx = ht->get_hash_key(key) % ht->table_size;
	list_foreach(htel, &ht->table[idx].list, lnk) {
		if(0 == ht->cmp(key, htel->key)) {
			list_unlink_link(&htel->lnk);
			list_unlink_link(&htel->general_lnk);
			objfree(&ht_elem_pool, htel);
			return ENOERR;
		}
	}

	return -ENOENT;
}

void hashtable_destroy(struct hashtable *ht) {
	int i;
	struct hashtable_element *htel;

	assert(ht);

	for(i = 0; i < ARRAY_SIZE(ht->table); i ++) {
		list_foreach(htel, &ht->table[i].list, lnk) {
//			list_unlink_link(&htel->lnk); // no matter
			objfree(&ht_elem_pool, htel);
		}

	}
	free(ht->table);
	objfree(&ht_pool, ht);
}

void *hashtable_get_key_first(struct hashtable *ht) {
	struct hashtable_element *htel;

	assert(ht);

	if (list_is_empty(&ht->all)) {
		return NULL;
	}

	htel = list_element(list_first_link(&ht->all), struct hashtable_element, general_lnk);
	return &htel->key;
}

void *hashtable_get_key_next(struct hashtable *ht, void *prev_key) {
	struct hashtable_element *htel;

	assert(ht);

	if (list_is_empty(&ht->all)) {
		return NULL;
	}

	htel = member_cast_out(prev_key, struct hashtable_element, key);
	if (list_last_link(&ht->all) == &htel->general_lnk) {
		return NULL;
	}

	htel = list_element(htel->general_lnk.next, struct hashtable_element, general_lnk);
	return &htel->key;
}
