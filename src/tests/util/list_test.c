/**
 * @file
 * @brief Tests methods of util/list.
 *
 * @date 12.03.11
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <string.h>
#include <util/list.h>

EMBOX_TEST_SUITE("util/list test");

TEST_SETUP(setup);

struct element {
	int some_stuff;
	struct list_link lnk;
};

struct element x, y;
struct list l;

TEST_CASE("list_link_element should cast link member out to its container") {
	struct list_link *link = &x.lnk;
	test_assert_equal(list_link_element(link, struct element, lnk), &x);
}

TEST_CASE("list_init should return its argument") {
	struct list l;
	test_assert_equal(list_init(&l), &l);
}

TEST_CASE("list_link_init should return its argument") {
	struct element e;
	test_assert_equal(list_link_init(&e.lnk), &e.lnk);
}

TEST_CASE("list_init should have the same effect as static initializer") {
	struct list l = LIST_INIT(&l);
	char buff[sizeof(l)];
	memcpy(buff, &l, sizeof(l));
	memset(&l, 0xA5, sizeof(l)); /* poison. */
	test_assert_zero(memcmp(buff, list_init(&l), sizeof(l)));
}

TEST_CASE("list_link_init should have the same effect as static initializer") {
	struct element e = { .lnk = LIST_LINK_INIT(&e.lnk) };
	char buff[sizeof(e.lnk)];
	memcpy(buff, &e.lnk, sizeof(e.lnk));
	memset(&e.lnk, 0xA5, sizeof(e.lnk)); /* poison. */
	test_assert_zero(memcmp(buff, list_link_init(&e.lnk), sizeof(e.lnk)));
}

TEST_CASE("list_empty should return true for just created list") {
	test_assert_true(list_empty(&l));
}

TEST_CASE("list_first should return null for empty list") {
	test_assert_null(list_first(&l, struct element, lnk));
}

TEST_CASE("list_last should return null for empty list") {
	test_assert_null(list_last(&l, struct element, lnk));
}

TEST_CASE("list_first_link should return null for empty list") {
	test_assert_null(list_first_link(&l));
}

TEST_CASE("list_last_link should return null for empty list") {
	test_assert_null(list_last_link(&l));
}

TEST_CASE("list_add_first should make the list non empty") {
	list_add_first(&x, &l, lnk);
	test_assert_false(list_empty(&l));
}

TEST_CASE("list_add_last should make the list non empty") {
	list_add_last(&x, &l, lnk);
	test_assert_false(list_empty(&l));
}

TEST_CASE("list_add_first link should make the list non empty") {
	list_add_first_link(&x.lnk, &l);
	test_assert_false(list_empty(&l));
}

TEST_CASE("list_add_last_link should make the list non empty") {
	list_add_last_link(&x.lnk, &l);
	test_assert_false(list_empty(&l));
}

TEST_CASE("list_first_link and list_last_link on a single element list "
		"constructed using list_add_first should return the element's link") {
	list_add_first(&x, &l, lnk);
	test_assert_equal(list_first_link(&l), &x.lnk);
	test_assert_equal(list_last_link(&l), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link on a single element list "
		"constructed using list_add_last should return the element's link") {
	list_add_last(&x, &l, lnk);
	test_assert_equal(list_first_link(&l), &x.lnk);
	test_assert_equal(list_last_link(&l), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link should return a new and an old "
		"element accordingly after adding a new one with list_add_first") {
	test_assert_not_equal(&x, &y);
	list_add_first(&x, &l, lnk);
	list_add_first(&y, &l, lnk);
	test_assert_equal(list_first_link(&l), &y.lnk);
	test_assert_equal(list_last_link(&l), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link should return a new and an old "
		"element accordingly after adding a new one with list_add_first") {
	test_assert_not_equal(&x, &y);
	list_add_last(&x, &l, lnk);
	list_add_last(&y, &l, lnk);
	test_assert_equal(list_first_link(&l), &x.lnk);
	test_assert_equal(list_last_link(&l), &y.lnk);
}

TEST_CASE("list_remove on a single element list should make the list empty "
		"and element initialized again") {
	char buff[sizeof(x.lnk)];
	memcpy(buff, &x.lnk, sizeof(x.lnk));
	list_add_first(&x, &l, lnk);
	list_remove(&x, lnk);
	test_assert_true(list_empty(&l));
	test_assert_zero(memcmp(buff, &x.lnk, sizeof(x.lnk)));
}


static int setup(void) {
	list_init(&l);
	list_link_init(&x.lnk);
	list_link_init(&y.lnk);
	return 0;
}
