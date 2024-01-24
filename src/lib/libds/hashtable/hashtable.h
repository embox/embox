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
 * @author Dmitry Avdyukhin
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

struct hashtable_item;


/** Hash function type definition */
typedef size_t (*ht_hash_ft)(void *key);

/** Compare item function type definition. */
typedef int (*ht_cmp_ft)(void *key1, void *key2);

/**
 * Initialize a buffer as hashtable with specified size and special hash-table functions
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
extern struct hashtable *hashtable_init(struct hashtable *ht,
		unsigned int table_size, ht_hash_ft get_hash, ht_cmp_ft cmp);

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
extern int hashtable_put(struct hashtable *ht, struct hashtable_item *ht_item);

extern struct hashtable_item *hashtable_item_init(
		struct hashtable_item *ht_item,  void *key, void *value);

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
extern struct hashtable_item *hashtable_del(struct hashtable *ht, void *key);

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


#include <lib/libds/dlist.h>

struct hashtable_item {
	struct dlist_head lnk;
	struct dlist_head general_lnk;
	void *key;
	void *value;
};

struct hashtable_entry {
	struct dlist_head list;
	unsigned int cnt;
};

/**
 * handler of hash-table.
 * It contains size of hash-table array, compare element function, calculation
 * hash index function and array of the table entry. table entry is the list of
 *  the element with equivalent hash index.
 */
struct hashtable {
	struct hashtable_entry *table; /**< array of the tables entry */
	ht_hash_ft get_hash_key; /**< handler of the calculation index function */
	ht_cmp_ft cmp; /** < handler of the compare elements function */
	unsigned int table_size; /** size of the array of the table entry */
	struct dlist_head all;
};

#define HASHTABLE_BUFFER_SIZE(size) \
	(size * sizeof(struct hashtable_entry))

#define HASHTABLE_BUFFER_DEF(buff_name,buff_size) \
	void *buff_name[(HASHTABLE_BUFFER_SIZE(buff_size) / sizeof(void *)) + 1] = {0}

#define HASHTABLE_DEF(name,size,hash_fn,cmp_fn)       \
		static HASHTABLE_BUFFER_DEF(name##_buff, size); \
		static struct hashtable name = {                \
				(struct hashtable_entry *)name##_buff,  \
				hash_fn,                                \
				cmp_fn,                                 \
				size, \
				DLIST_INIT(name.all),                    \
		}


#define HASHTABLE_SIZE(size) \
	(HASHTABLE_BUFFER_SIZE(size) + sizeof(struct hashtable))

#define HASHTABLE_DECL(name, size) \
	HASHTABLE_BUFFER_DEF(name##_buff,HASHTABLE_SIZE(size)); \
	struct hashtable *name = (struct hashtable *)name##_buff;

#endif /* UTIL_HASHTABLE_H_ */
