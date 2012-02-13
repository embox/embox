/**
 * @file
 * @brief implementation of hashtable data structure
 *
 * @date 30.09.11
 * @author Dmitry Zubarevich
 * @author Avdyukhin Dmitry
 */

#include <util/hashtable.h>
#include <util/list.h>
#include <mem/kmalloc.h>

/** Standard sizes of hashtables */
static int good_sizes[] = {
	17, 31, 53, 97, 193, 389,
	769, 1543, 3079, 6151,
	12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869,
	3145739, 6291469, 12582917, 25165843,
	50331653, 100663319, 201326611, 402653189,
	805306457, 1610612741
};

/** */
static const int density1 = 2, density2 = 3;
/** */
static const int min_index = 0, max_index = 27;

struct hashtable *hashtable_init(struct hashtable *hash_tab, size_t size) {
	int length;
	for (int i = 0; ; ++i) {
		if (good_sizes[i] >= size) {
			hash_tab->index_of_size = i;
			break;
		}
	}
	length = good_sizes[hash_tab->index_of_size];
	hash_tab->data = kmalloc(sizeof(struct list *) * length);
	for(int i = 0; i < length; i++) {
		list_init(&hash_tab->data[i]);
	}
	hash_tab->count = 0;
	return hash_tab;
}

size_t hashtable_count(struct hashtable *hash_tab) {
	return hash_tab->count;
}

struct hashtable *hashtable_copy(struct hashtable *hash_tab,
		struct hashtable *new_hash_tab,
		hash_t hash, hashtable_comparator_t compare) {
#if 0
	int length = good_sizes[hash_tab->index_of_size];
	hashtable_link *element;
	struct list *new_curret_list;
	int index;
	for(int i = 0; i < length; i++) {
		struct list *current_list = hash_tab->data[i];
		list_foreach(element, current_list, lnk) {
			index = new_hash_tab->hash_key(element->key) %
					good_sizes[new_hash_tab->index_of_size];
			new_curret_list = new_hash_tab->data[index];
			list_add_last_link(&(element->lnk), new_curret_list);
		}
	}
	return new_hash_tab;
#endif
	return NULL;
}

#if 0
static void hashtable_resize(struct hashtable *hash_tab, int new_size_index) {
	struct hashtable* new_hash_tab =
		hashtable_create(new_hash_tab, new_size_index,
			hash_tab->hash_key, hash_tab->compare_keys);
	new_hash_tab = hashtable_copy(hash_tab, new_hash_tab);
	kfree(hash_tab->data);
	hash_tab = new_hash_tab;
}
#endif

void hashtable_insert(struct hashtable *hash_tab,
		struct hashtable_link *key,
		hash_t hash, hashtable_comparator_t compare) {
#if 0
	int size_index = hash_tab->index_of_size;
	int index = hash_tab->hash_key(key) %
			good_sizes[size_index];
	ht_element *element;

	if (hashtable_search(hash_tab, key) == NULL)
		return;
	element->key = key;
	element->value = value;
	list_add_last_link(&(element->lnk), hash_tab->data[index]);
	hash_tab->count++;
	if (hash_tab->count * density2 >= good_sizes[size_index] * density1 &&
			size_index < max_index)
		hashtable_resize(hash_tab, hash_tab->index_of_size + 1);
#endif
}

struct hashtable_link *hashtable_search(struct hashtable *hash_tab,
		const struct hashtable_link *key,
		hash_t hash, hashtable_comparator_t compare) {
#if 0
	struct list *current_list = hash_tab->data[hash_tab->hash_key(key) %
	        good_sizes[hash_tab->index_of_size]];
	ht_element *element;
	list_foreach(element, current_list, lnk) {
		if (!hash_tab->compare_keys(element->key, key)) {
			return element->value;
		}
	}
#endif
	return NULL;
}
#if 0
static int list_remove(struct list *list, key_t *key,
		struct hashtable *hash_tab) {
	ht_element *element;
	list_foreach(element, list, lnk) {
		if (!hash_tab->compare_keys(element->key, key)) {
			list_unlink_element(element, lnk);
			return 1;
		}
	}
	return 0;
}
#endif
void hashtable_remove(struct hashtable *hash_tab,
		struct hashtable_link *key,
		hash_t hash, hashtable_comparator_t compare) {
#if 0
	int size_index = hash_tab->index_of_size;
	int index = hash_tab->hash_key(key) %
			good_sizes[size_index];
	struct list* list = hash_tab->data[index];
	hash_tab->count -= list_remove(list, key, hash_tab);
	if (hash_tab->count * density2 < good_sizes[size_index] * density1 &&
			size_index > min_index)
		hashtable_resize(hash_tab, hash_tab->index_of_size - 1);
#endif
}

#if 0
static void destroy_list(struct list *list) {
	while (!list_is_empty(list)) {
		list_remove_last_link(list);
	}
}
#endif

void hashtable_destroy(struct hashtable *hash_tab, hashtable_dispose_t dispose) {
}

void hashtable_clear(struct hashtable *hash_tab, hashtable_dispose_t dispose) {
#if 0
	int n = good_sizes[hash_tab->index_of_size];
	for(int i = 0; i < n; i++)	{
		destroy_list(hash_tab->data[i]);
	}
	kfree(hash_tab->data);
#endif
}

