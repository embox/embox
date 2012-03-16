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

TEST_CASE("Add single element to hashtable") {
	struct hashtable *ht;
	struct ht_element el = {1};
	struct ht_element *ht_value;

	ht = hashtable_create(20, 0x10, sizeof(struct ht_element));

	hashtable_put(ht, "first", &el);
	el.number = 0; /* hashed value have not to change */
	ht_value = (struct ht_element *)hashtable_get(ht, "first");

	hashtable_destroy(ht);
}
