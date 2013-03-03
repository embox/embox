/**
 * @file
 *
 * @brief 'hashtable' interface with a list resolve collision
 *
 * @details
 *
 * @date 30.09.11
 *
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 * @author Anton Bondarev
 */

#ifndef UTIL_HASHTABLE_H_
#define UTIL_HASHTABLE_H_

#include <stddef.h>

/**
 * hash-table handler declaration. You do not allow to use field of this
 * structure outside of hash-table implementation library
 */
struct hashtable;

/** Hash function type definition */
typedef size_t (*get_hash_ft)(void *key);

/** Compare item function type definition. */
typedef int (*ht_cmp_ft)(void *key1, void *key2);

/**
 * Create hashtable with specified size and special hash-table functions
 * (#get_hash_ft calculating hash index and #ht_cmp_ft comparing items). This
 * function creates array for lists with element pointer. The Lists use for
 * resolving hash index collision. Return value is the created hash-table
 * handler it must be use during other operation with hash-table
 *
 * @param table_size - array size for hash table
 * @param get_hash - handler for calculating item hash index
 * @param cmp - handler for comparing two difference items
 *
 * @return hashtable structure pointer
 */
extern struct hashtable *hashtable_create(size_t table_size,
		get_hash_ft get_hash, ht_cmp_ft cmp);

/**
 * Delete all elements from hash-table and free hash-table structure memory
 *
 * @param hash_tab - hash-table handler from #hashtable_create
 */
extern void hashtable_destroy(struct hashtable *ht);

/**
 * Insert element into the hash-table. Add element into the hash-table with
 * according key
 *
 * @param hash_tab - hash-table handler which you want to be the item inserted
 * @param key - key identifier of inserted element
 * @param value - inserted element
 *
 * @return error code
 */
extern int hashtable_put(struct hashtable *ht, void *key, void *value);

/**
 * Search element in the hash-table by key. It returns first appropriate object
 *
 * @param hash_tab - hash-table at which will be found the element
 * @param key - identifier of the element
 * @param hash - hash function
 */
extern void *hashtable_get(struct hashtable *ht, void* key);

/**
 * Delete element from the hash-table
 *
 * @param hash_tab - hash-table from which will be deleted the element
 * @param key - key identifier of the element
 *
 * @return error code
 */
extern int hashtable_del(struct hashtable *ht, void *key);

/**
 * Get initial pointer to key from the hash-table
 *
 * @param hash_tab - hash-table on which we will iterate
 *
 * @return pointer to first key or NULL if no more entries in hash-table
 */
extern void *hashtable_get_key_first(struct hashtable *ht);

/**
 * Get next pointer to key from the hash-table
 *
 * @param hash_tab - hash-table on which we iterate
 * @param prev_key - pointer to previous key
 *
 * @return pointer to next key or NULL if no more entries in hash-table
 */
extern void *hashtable_get_key_next(struct hashtable *ht, void *prev_key);

#endif /* UTIL_HASHTABLE_H_ */
