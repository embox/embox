/**
 * @file
 * @brief implementation of hashtable data structure
 *
 * @date 30.09.11
 * @author Dmitry Zubarevich
 */

#include <util/hashtable.h>
#include <util/list.h>
#include <mem/kmalloc.h>

hashtable *hashtable_create(hashtable *hash_tab, int index_of_size,
		hash_key hash, compare_keys compare) {
	int length = good_sizes[index_of_size];
	struct list **array;
	array = kmalloc(sizeof(struct list *) * length);
				for(int i = 0; i < length; i++)
					array[i] = list_init(array[i]);
	hash_tab->index_of_size = index_of_size;
	hash_tab->count = 0;
	hash_tab->data = array;
	hash_tab->hash_key = hash;
	hash_tab->compare_keys = compare;
	return hash_tab;
}

int hashtable_count (hashtable *hash_tab) {
	return hash_tab->count;
}

hashtable *hashtable_copy(hashtable *hash_tab, hashtable *new_hash_tab) {
	int length = good_sizes[hash_tab->index_of_size];
	ht_element *element;
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
}

static void hashtable_resize(hashtable *hash_tab, int new_size_index) {
	hashtable* new_hash_tab = hashtable_create(new_hash_tab, new_size_index,
			hash_tab->hash_key, hash_tab->compare_keys);
	new_hash_tab = hashtable_copy(hash_tab, new_hash_tab);
	kfree(hash_tab->data);
	hash_tab = new_hash_tab;
}

void hashtable_insert(hashtable *hash_tab, key_t *key, data_t *value) {
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
	if ((double)(hash_tab->count)/good_sizes[size_index] >= density &&
			size_index < max_index)
		hashtable_resize(hash_tab, hash_tab->index_of_size + 1);
}

data_t *hashtable_search(hashtable *hash_tab, key_t *key) {
	struct list *current_list = hash_tab->data[hash_tab->hash_key(key) %
	        good_sizes[hash_tab->index_of_size]];
	ht_element *element;
	list_foreach(element, current_list, lnk) {
		if (!hash_tab->compare_keys(element->key, key)) {
			return element->value;
		}
	}
	return NULL;
}

static int list_remove(struct list *list, key_t *key, hashtable *hash_tab) {
	ht_element *element;
	list_foreach(element, list, lnk) {
		if (!hash_tab->compare_keys(element->key, key)) {
			list_unlink_element(element, lnk);
			return 1;
		}
	}
	return 0;
}

void hashtable_remove (hashtable *hash_tab, key_t *key) {
	int size_index = hash_tab->index_of_size;
	int index = hash_tab->hash_key(key) %
			good_sizes[size_index];
	struct list* list = hash_tab->data[index];
	hash_tab->count -= list_remove(list, key, hash_tab);
	if ((double)(hash_tab->count)/good_sizes[size_index] < density &&
			size_index > min_index)
		hashtable_resize(hash_tab, hash_tab->index_of_size - 1);
}

static void destroy_list(struct list *list) {
	while (!list_is_empty(list)) {
		list_remove_last_link(list);
	}
}

void hashtable_destroy(hashtable *hash_tab) {
	int n = good_sizes[hash_tab->index_of_size];
	for(int i = 0; i < n; i++)	{
		destroy_list(hash_tab->data[i]);
	}
	kfree(hash_tab->data);
}
