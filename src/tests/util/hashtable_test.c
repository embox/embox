/**
 * @file
 * @brief Test unit for util/hashtable.
 *
 * @date Dec 11, 2011
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <lib/libds/array.h>
#include <lib/libds/hashtable.h>

#include <mem/misc/pool.h>

EMBOX_TEST_SUITE("util/hashtable test");

struct ht_element {
	int number;
};

static struct ht_element el[3] = { {1}, {2}, {3}};
static const char *key[3] = {"first", "secnd", "third" };

POOL_DEF(ht_test_item_pool, struct hashtable_item, 3);

static size_t get_hash(void *key) {
	char *key_str = key;
	return (size_t) key_str[0];
}

static int cmp_keys(void *key1,void *key2) {
	return strcmp(key1, key2);
}


TEST_CASE("Add single element to hashtable") {
	HASHTABLE_DECL(ht,0x10);
	struct hashtable_item *ht_item;
	struct ht_element *ht_value;

	ht = hashtable_init(ht, 0x10, get_hash, cmp_keys);

	ht_item = pool_alloc(&ht_test_item_pool);
	ht_item = hashtable_item_init(ht_item, (void *)key[0], &el[0]);
	hashtable_put(ht, ht_item);

	ht_value = (struct ht_element *) hashtable_get(ht, (void *)key[0]);
	hashtable_destroy(ht);

	pool_free(&ht_test_item_pool, ht_item);

	test_assert_equal(ht_value, &el[0]);
}

TEST_CASE("Add three elements to hashtable") {
	HASHTABLE_DECL(ht,0x30);
	struct hashtable_item *ht_item;
	struct ht_element *ht_value[3];
	int i, j;

	ht = hashtable_init(ht, 0x30, get_hash, cmp_keys);

	for(i = 0; i < ARRAY_SIZE(el); i++)	{
		ht_item = pool_alloc(&ht_test_item_pool);
		ht_item = hashtable_item_init(ht_item, (void *)key[i], &el[i]);
		hashtable_put(ht, ht_item);
	}

	for(i = 0; i < ARRAY_SIZE(ht_value); i++)	{
		ht_value[i] = (struct ht_element *) hashtable_get(ht, (void *)key[i]);
		test_assert_not_null(ht_value[i]);
	}

	for(i = 0; i < ARRAY_SIZE(el); i++)	{
		ht_item = hashtable_del(ht, (void *)key[i]);
		pool_free(&ht_test_item_pool, ht_item);
	}

	hashtable_destroy(ht);



	for(i = 0; i < ARRAY_SIZE(el); i++)	{
		for(j = 0; j < ARRAY_SIZE(ht_value); j++)	{
			if (i == j){
				test_assert_equal(ht_value[i], &el[j]);
			}
			else{
				test_assert_not_equal(ht_value[i], &el[j]);
			}
		}
	}
}

TEST_CASE("Add tree elements and comparer there on each iteration") {
	int i;
	char **key_iter;
	struct hashtable_item *ht_item;
	HASHTABLE_DECL(ht, 0x1);

	ht = hashtable_init(ht, 0x1, get_hash, cmp_keys);

	for (i = 0; i < ARRAY_SIZE(el); i++) {
		ht_item = pool_alloc(&ht_test_item_pool);
		ht_item = hashtable_item_init(ht_item, (void *)key[i], &el[i]);
		hashtable_put(ht, ht_item);
	}

	for (key_iter = hashtable_get_key_first(ht), i = 0;
			key_iter != NULL;
			key_iter = hashtable_get_key_next(ht, key_iter), ++i) {
		test_assert_zero(strcmp(*key_iter, key[i]));
	}

	for(i = 0; i < ARRAY_SIZE(el); i++)	{
		ht_item = hashtable_del(ht, (void *)key[i]);
		pool_free(&ht_test_item_pool, ht_item);
	}


	hashtable_destroy(ht);
}
