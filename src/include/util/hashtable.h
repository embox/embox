/**
 * @file
 * @brief interface to hashtable data structure
 *
 * @date 30.09.11
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 */

#ifndef UTIL_HASHTABLE_H_
#define UTIL_HASHTABLE_H_

#include <util/list.h>

/**
 * A link which has to be embedded into each element of the hashtable.
 *
 * Usage:
 * @code
 *  struct my_element {
 *  	...
 *  	struct hashtable_link my_link;
 *  	...
 *  };
 * @endcode
 *
 */
struct hashtable_link {
	struct list_link link;
};

/** Hash function delegate. */
typedef size_t (*hash_t) (const struct hashtable_link *);

/** Compare function for hashtable. */
typedef int (*hashtable_comparator_t) (const struct hashtable_link *,
		const struct hashtable_link *);
/** Dispose function for hashtable link. */
typedef int (*hashtable_dispose_t) (struct hashtable_link *);

/* hashtable structure */
struct hashtable {
	/** Dynamic array contains lists of data */
	struct list *data;
	/** Count of elements at hashtable */
	int count;
	/** Index of element at GoodSizes[] array */
	int index_of_size;
};

/**
 * Initialization of hashtable
 * @param index_of_size - index at good_sizes array
 * @param key_size - size of key type
 * @param value_size - size of value type
 * @return hashtable
 */
extern struct hashtable *hashtable_init(struct hashtable *hash_tab, size_t size);

/**
 * Getter for info about count of elements into hashtable
 * @param hash_tab - hashtable into which will be element
 */
extern size_t hashtable_count(struct hashtable *hash_tab);

/**
 * Insert element into hashtable
 * @param hash_tab - hashtable into which will be inserted element
 * @param key - identifier of data
 * @param hash - hash function
 * @param compare - compare function
 */
extern void hashtable_insert(struct hashtable *hash_tab,
		struct hashtable_link *key, hash_t hash, hashtable_comparator_t compare);

/**
 * Delete element from hashtable
 * @param hash_tab - hashtable from which will be deleted element
 * @param key - identifier of data in element
 * @param hash - hash function
 * @param compare - compare function
 */
extern void hashtable_remove(struct hashtable *hash_tab,
		struct hashtable_link *key, hash_t hash, hashtable_comparator_t compare);

/**
 * Copy of hashtable
 * @param from - source hashtable
 * @param to - result hashtable
 * @param hash - hash function
 * @param compare - compare function
 */
extern struct hashtable *hashtable_copy(struct hashtable *from,
		struct hashtable *to, hash_t hash, hashtable_comparator_t compare);

/**
 * Delete all elements from hashtable. Call dispose function for each element.
 * @param hash_tab - cleared hashtable
 * @param dispose - function, applyed to each deleted element
 */
extern void hashtable_clear(struct hashtable *hash_tab, hashtable_dispose_t dispose);

/**
 * Delete all elements from hashtable. Call dispose function for each element.
 * Clear memory from hashtable.
 * @param hash_tab - cleared hashtable
 * @param dispose - function, applyed to each deleted element
 */
extern void hashtable_destoy(struct hashtable *hash_tab, hashtable_dispose_t dispose);

/**
 * Search of data in hashtable by key
 * @param hash_tab - hashtable at which will be found element
 * @param key - identifier for data of element
 * @param hash - hash function
 * @param compare - compare function
 */
extern struct hashtable_link *hashtable_search(struct hashtable *hash_tab,
		const struct hashtable_link *key,
		hash_t hash, hashtable_comparator_t compare);

/**
 * Iterator for all elements of hashtable
 * @element - pointer to current element of hashtable
 * @hash_tab - pointer to hashtable
 */
#define hashtable_foreach(element, hash_tab) \
	for (size_t i = 0; i < hash_tab->index_of_size; i++) \
		list_foreach(element, hash_tab->data[i], lnk)

#endif /* UTIL_HASHTABLE_H_ */
