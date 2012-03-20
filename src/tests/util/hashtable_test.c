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
#include <util/hashtable.h>

EMBOX_TEST_SUITE("util/hashtable test");

struct ht_element {
	int number;
};

static size_t get_hash(void *key) {
	char *key_str = key;
	return (size_t) key_str[0];
}

static int cmp_keys(void *key1,void *key2) {
	return strcmp(key1, key2);
}


TEST_CASE("Add single element to hashtable") {
	struct hashtable *ht;
	struct ht_element el = { 1 };
	struct ht_element *ht_value;

	ht = hashtable_create(0x10, get_hash, cmp_keys);
	hashtable_put(ht, "first", &el);
	ht_value = (struct ht_element *) hashtable_get(ht, "first");
	hashtable_destroy(ht);

	test_assert_equal(ht_value, &el);
}

TEST_CASE("Add three elements to hashtable") {
	struct hashtable *ht;
	struct ht_element el[3] = { {1}, {2}, {3}};

	struct ht_element *ht_value[3];
	const char *key[3] = {"first", "secnd", "third" };

	ht = hashtable_create(0x30, get_hash, cmp_keys);
	for(int i=0; i<3; i++)	{
		hashtable_put(ht, (void *)key[i], &el[i]);
	}

	for(int i=0; i<3; i++)	{
		if(NULL != hashtable_get(ht, (void *)key[i])){
			ht_value[i] = (struct ht_element *) hashtable_get(ht, (void *)key[i]);
			printf ("%d\n",(int) ht_value[i]->number);
		}
		else printf ("NULL");
	}

	hashtable_destroy(ht);

	for(int i=0; i<3; i++)	{
		for(int j=0; j<3; j++)	{
			if (i == j){
				test_assert_equal(ht_value[i], &el[j]);
			}
			else{
				test_assert_not_equal(ht_value[i], &el[j]);
			}
		}
	}
}
