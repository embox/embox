/**
 * @file
 * @brief Tests methods of util/slist.
 *
 * @date 17.04.11
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <string.h>
#include <lib/libds/array.h>

#include <lib/libds/slist.h>

EMBOX_TEST_SUITE("util/slist test");

TEST_SETUP(setup);

struct element {
	int some_stuff;
	struct slist_link lnk;
};

static struct element x, y, z;
static struct slist m;

static int setup(void) {
	slist_init(&m);
	slist_link_init(&x.lnk);
	slist_link_init(&y.lnk);
	slist_link_init(&z.lnk);
	return 0;
}

TEST_CASE("slist_element should cast link member out to its container") {
	struct slist_link *link = &x.lnk;
	test_assert_equal(slist_element(link, struct element, lnk), &x);
}

TEST_CASE("slist_init should return its argument") {
	struct slist l;
	test_assert_equal(slist_init(&l), &l);
}

TEST_CASE("slist_link_init should return its argument") {
	struct element e;
	test_assert_equal(slist_link_init(&e.lnk), &e.lnk);
}

TEST_CASE("slist_init should have the same effect as static initializer") {
	struct slist l = SLIST_INIT(&l);
	char buff[sizeof(l)];
	memcpy(buff, &l, sizeof(l));
	memset(&l, 0xA5, sizeof(l)); /* poison. */

	test_assert_mem_equal(buff, slist_init(&l), sizeof(l));
}

TEST_CASE("slist_link_init should have the same effect as static initializer") {
	struct element e = { .lnk = SLIST_LINK_INIT(&e.lnk), .some_stuff = 42, };
	char buff[sizeof(e.lnk)];
	memcpy(buff, &e.lnk, sizeof(e.lnk));
	memset(&e.lnk, 0xA5, sizeof(e.lnk)); /* poison. */

	test_assert_mem_equal(buff, slist_link_init(&e.lnk), sizeof(e.lnk));
}

TEST_CASE("slist_empty should return true for just created list") {
	test_assert_true(slist_empty(&m));
}

TEST_CASE("slist_alone_link should return true for just initialized link") {
	test_assert_true(slist_alone_link(&x.lnk));
}

TEST_CASE("slist_alone_element should return true for just initialized element") {
	test_assert_true(slist_alone_element(&x, lnk));
}

TEST_CASE("slist_first should return null for empty list") {
	test_assert_null(slist_first_element(&m, struct element, lnk));
}

TEST_CASE("slist_first_link should return null for empty list") {
	test_assert_null(slist_first_link(&m));
}

TEST_CASE("slist_add_first_element should make the list non empty "
		"and the element not alone") {
	slist_add_first_element(&x, &m, lnk);

	test_assert_false(slist_empty(&m));
	test_assert_false(slist_alone_element(&x, lnk));
}

TEST_CASE("slist_add_first_link should make the list non empty "
		"and the element's link not alone") {
	slist_add_first_link(&x.lnk, &m);

	test_assert_false(slist_empty(&m));
	test_assert_false(slist_alone_link(&x.lnk));
}

TEST_CASE("slist_first_link on a single element list "
		"constructed using slist_add_first_element should return the element's link") {
	slist_add_first_element(&x, &m, lnk);

	test_assert_equal(slist_first_link(&m), &x.lnk);
}

TEST_CASE("slist_first_link should return an "
		"element which has been added with the last slist_add_first_element call") {
	test_assert_not_equal(&x, &y);

	slist_add_first_element(&x, &m, lnk);
	slist_add_first_element(&y, &m, lnk);

	test_assert_equal(slist_first_link(&m), &y.lnk);
}

TEST_CASE("slist_remove_first_element should return null for empty list") {
	test_assert_null(slist_remove_first_element(&m, struct element, lnk));
}

TEST_CASE("slist_remove_first_element on a single element list should return the "
		"element and make the list empty and element alone again") {
	slist_add_first_element(&x, &m, lnk);

	test_assert_equal(slist_remove_first_element(&m, struct element, lnk), &x);

	test_assert_true(slist_empty(&m));
	test_assert_true(slist_alone_element(&x, lnk));
}

TEST_CASE("slist_insert_after_element on a single element list should make "
		"a new element the last one in the list") {
	slist_add_first_element(&x, &m, lnk);

	slist_insert_after_element(&y, &x, lnk);

	test_assert_equal(slist_first_link(&m), &x.lnk);
}

TEST_CASE("slist_insert_after_element: inserting a new element after the first one "
		"in a list of two elements should insert a new one between them") {
	slist_add_first_element(&z, &m, lnk);
	slist_add_first_element(&x, &m, lnk);

	slist_insert_after_element(&y, &x, lnk);

	test_assert_equal(slist_remove_first_link(&m), &x.lnk);
	test_assert_equal(slist_remove_first_link(&m), &y.lnk);

	test_assert_equal(slist_first_link(&m), &z.lnk);
}
