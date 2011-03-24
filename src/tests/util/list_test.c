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

struct element x, y, z;
struct list m, n;

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
	test_assert_true(list_empty(&m));
}

TEST_CASE("list_alone_link should return true for just initialized link") {
	test_assert_true(list_alone_link(&x.lnk));
}

TEST_CASE("list_alone should return true for just initialized element") {
	test_assert_true(list_alone(&x, lnk));
}

TEST_CASE("list_first should return null for empty list") {
	test_assert_null(list_first(&m, struct element, lnk));
}

TEST_CASE("list_last should return null for empty list") {
	test_assert_null(list_last(&m, struct element, lnk));
}

TEST_CASE("list_first_link should return null for empty list") {
	test_assert_null(list_first_link(&m));
}

TEST_CASE("list_last_link should return null for empty list") {
	test_assert_null(list_last_link(&m));
}

TEST_CASE("list_add_first should make the list non empty "
		"and the element not alone") {
	list_add_first(&x, &m, lnk);

	test_assert_false(list_empty(&m));
	test_assert_false(list_alone(&x, lnk));
}

TEST_CASE("list_add_last should make the list non empty "
		"and the element not alone") {
	list_add_last(&x, &m, lnk);

	test_assert_false(list_empty(&m));
	test_assert_false(list_alone(&x, lnk));
}

TEST_CASE("list_add_first_link should make the list non empty "
		"and the element's link not alone") {
	list_add_first_link(&x.lnk, &m);

	test_assert_false(list_empty(&m));
	test_assert_false(list_alone_link(&x.lnk));
}

TEST_CASE("list_add_last_link should make the list non empty "
		"and the element's link not alone") {
	list_add_last_link(&x.lnk, &m);

	test_assert_false(list_empty(&m));
	test_assert_false(list_alone_link(&x.lnk));
}

TEST_CASE("list_first_link and list_last_link on a single element list "
		"constructed using list_add_first should return the element's link") {
	list_add_first(&x, &m, lnk);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link on a single element list "
		"constructed using list_add_last should return the element's link") {
	list_add_last(&x, &m, lnk);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link should return a new and an old "
		"element accordingly after adding a new one with list_add_first") {
	test_assert_not_equal(&x, &y);

	list_add_first(&x, &m, lnk);
	list_add_first(&y, &m, lnk);

	test_assert_equal(list_first_link(&m), &y.lnk);
	test_assert_equal(list_last_link(&m), &x.lnk);
}

TEST_CASE("list_first_link and list_last_link should return a new and an old "
		"element accordingly after adding a new one with list_add_last") {
	test_assert_not_equal(&x, &y);

	list_add_last(&x, &m, lnk);
	list_add_last(&y, &m, lnk);

	test_assert_equal(list_first_link(&m), &x.lnk);
	test_assert_equal(list_last_link(&m), &y.lnk);
}

TEST_CASE("list_remove on a single element list should make the list empty "
		"and element alone again") {
	list_add_first(&x, &m, lnk);
	list_remove(&x, lnk);

	test_assert_true(list_empty(&m));
	test_assert_true(list_alone(&x, lnk));
}

TEST_CASE("single list_remove and subsequent list_add_first to another list "
		"should make the first list empty but an element not alone") {
	test_assert_not_equal(&m, &n);

	list_add_first(&x, &m, lnk);
	list_remove(&x, lnk);
	list_add_first(&x, &n, lnk);

	test_assert_true(list_empty(&m));
	test_assert_false(list_empty(&n));
	test_assert_false(list_alone(&x, lnk));
}

TEST_CASE("multiple list_remove and subsequent list_add_first to another list "
		"should make the first list empty but elements not alone") {
	list_add_last(&x, &m, lnk);
	list_add_last(&y, &m, lnk);
	list_add_last(&z, &m, lnk);
	list_remove(&x, lnk);
	list_remove(&z, lnk);
	list_remove(&y, lnk);
	list_add_first(&x, &n, lnk);
	list_add_first(&y, &n, lnk);
	list_add_first(&z, &n, lnk);

	test_assert_true(list_empty(&m));
	test_assert_false(list_empty(&n));
	test_assert_false(list_alone(&x, lnk));
	test_assert_false(list_alone(&y, lnk));
	test_assert_false(list_alone(&z, lnk));

	test_assert_equal(list_last_link(&n), &x.lnk);
	test_assert_equal(list_first_link(&n), &z.lnk);
}

TEST_CASE("list_insert_before on a single element list should make "
		"a new element the first one in the list") {
	list_add_first(&x, &m, lnk);
	list_insert_before(&y, &x, lnk);

	test_assert_equal(list_last_link(&m), &x.lnk);
	test_assert_equal(list_first_link(&m), &y.lnk);
}

static int setup(void) {
	list_init(&m);
	list_init(&n);
	list_link_init(&x.lnk);
	list_link_init(&y.lnk);
	list_link_init(&z.lnk);
	return 0;
}
