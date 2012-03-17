/**
 * @file
 * @brief Test unit for util/hashtable.
 *
 * @date Dec 11, 2011
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>

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
}
