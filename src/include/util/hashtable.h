/**
 * @file
 * @brief interface to hashtable data structure
 *
 * @date 30.09.11
 * @author Dmitry Zubarevich
 */

#ifndef UTIL_HASHTABLE_H_
#define UTIL_HASHTABLE_H_

#include <util/list.h>

/* type of key for data */
typedef char key_t;
/* type of data in hashtable */
typedef char data_t;

typedef struct pool pool_t;

/* standard sizes of hashtables */
int good_sizes[] = {
53, 97, 193, 389,
769, 1543, 3079, 6151,
12289, 24593, 49157, 98317,
196613, 393241, 786433, 1572869,
3145739, 6291469, 12582917, 25165843,
50331653, 100663319, 201326611, 402653189,
805306457, 1610612741
};

const double density = 0.6666667;

const int max_index = 25;
const int min_index = 0;

#define HASHTABLE_COUNT 0x10

/* Element of list */
typedef struct ht_element {
	key_t* key;
	data_t* value;
	struct list_link lnk;
} ht_element;

/* hash function delegate */
typedef int (*hash_key) (key_t *);
/* compare function delegate */
typedef int (*compare_keys) (key_t *, key_t *);

/* hashtable structure */
typedef struct hashtable {
	struct list **data;                  /* dynamic array contains lists of data */
	int count;           		  /* count of elements at hashtable */
	int index_of_size;   		  /* index of element at GoodSizes[] array */
	hash_key hash_key;            /* hash function */
	compare_keys compare_keys;    /* key compare function */
} hashtable;

/**
 * creation of hashtable
 * @param index_of_size - index at good_sizes array
 * @param hash - hash function
 * @param compare - compdre function
 * @param key_size - size of key type
 * @param value_size - size of value type
 * @return hashtable
 */
extern hashtable *hashtable_create(hashtable *hash_tab, int index_of_size,
		hash_key hash, compare_keys compare);

/**
 * Insert element into hashtable
 * @param hash_tab - hashtable into which will be inserted element
 * @param key - identifier of data
 * @param value - useful information
 * */
extern void hashtable_insert (hashtable *hash_tab, key_t *key, data_t *value);

/**
 * Delete element from hashtable
 * @param HashTable - hashtable from which will be deleted element
 * @param key - identifier of data in element
 * */
extern void hashtable_remove (hashtable *hash_tab, key_t *key);


/**
 * Getter for info about count of elements into hashtable
 * @param HashTable - hashtable into which will be element
 * */
extern int hashtable_count (hashtable *hash_tab);

/**
 * Copy of hashtable
 * @param from - source hashtable
 * @param to - result hashtable
 * */
extern hashtable *hashtable_copy(hashtable *from, hashtable *to);

/**
 * free memory from hashtable
 * @param HashTable - finallysed hashtable
 * */
extern void hashtable_destroy(hashtable *hash_tab);

/**
 * Search of data in hashtable by key
 * @param HashTable - hashtable at which will be found element
 * @param key - identifier for data of element
 * */
data_t *hashtable_search(hashtable *hash_tab, key_t *key);

/**
 * Iterator for all elements of hashtable
 * @element - pointer to current element of hashtable
 * @hash_tab - pointer to hashtable
 */
#define hashtable_foreach(element, hash_tab) 					\
	for (int i = 0; i < hash_tab->index_of_size; i++)   		\
		list_foreach(element, hash_tab->data[i], lnk)  \

#endif /* UTIL_HASHTABLE_H_ */
