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

TEST_CASE("Add two elements to hashtable") {
	struct hashtable *ht;
	struct ht_element el_1 = { 1 };
	struct ht_element el_2 = { 2 };
	struct ht_element *ht_value_1;
	struct ht_element *ht_value_2;

	ht = hashtable_create(0x20, get_hash, cmp_keys);
	hashtable_put(ht, "first", &el_1);
	hashtable_put(ht, "secnd", &el_2);

	ht_value_1 = (struct ht_element *) hashtable_get(ht, "first");
	ht_value_2 = (struct ht_element *) hashtable_get(ht, "secnd");
	hashtable_destroy(ht);

	//test_assert_equal(ht_value_1, &el_1);
	test_assert_equal(ht_value_2, &el_2);

	test_assert_not_equal(ht_value_1, &el_2);
	test_assert_not_equal(ht_value_2, &el_1);

	//while (1);
}

TEST_CASE("Add three elements to hashtable") {
	struct hashtable *ht;
	struct ht_element el[3] = { {1}, {2}, {3}};
	//struct ht_element el_2 = { 2 };
	//struct ht_element el_3 = { 3 };

	struct ht_element *ht_value_1;
	struct ht_element *ht_value_2;
	struct ht_element *ht_value_3;

	ht = hashtable_create(0x30, get_hash, cmp_keys);

	hashtable_put(ht, "first", &el[0]);
	hashtable_put(ht, "secnd", &el[1]);
	hashtable_put(ht, "third", &el[2]);

	ht_value_1 = (struct ht_element *) hashtable_get(ht, "first");
	ht_value_2 = (struct ht_element *) hashtable_get(ht, "secnd");
	ht_value_3 = (struct ht_element *) hashtable_get(ht, "third");

	printf ("%d\n",(int) ht_value_1->number);
	printf ("%d\n",(int) ht_value_2->number);
	printf ("%d\n",(int) ht_value_3->number);

	hashtable_destroy(ht);

	//test_assert_equal(ht_value_1, &el_1);
	test_assert_equal(ht_value_2, &el[1]);
	test_assert_equal(ht_value_3, &el[2]);

	test_assert_not_equal(ht_value_1, &el[1]);
	test_assert_not_equal(ht_value_1, &el[2]);
	test_assert_not_equal(ht_value_2, &el[0]);
	test_assert_not_equal(ht_value_2, &el[2]);
	test_assert_not_equal(ht_value_3, &el[0]);
	test_assert_not_equal(ht_value_3, &el[1]);

	//while (1);
}
