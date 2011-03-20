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

struct element {
	int some_stuff;
	struct list_link m_link;
};

TEST_CASE("list_link_element should cast link member out to its container") {
	struct element e;
	struct list_link *link = &e.m_link;
	test_assert_equal(list_link_element(link, struct element, m_link), &e);
}

TEST_CASE("list_init should return its argument") {
	struct list l;
	test_assert_equal(list_init(&l), &l);
}

TEST_CASE("list_link_init should return its argument") {
	struct element e;
	test_assert_equal(list_link_init(&e.m_link), &e.m_link);
}

TEST_CASE("list_init should have the same effect as static initializer") {
	struct list l = LIST_INIT(&l);
	char buff[sizeof(l)];
	memcpy(buff, &l, sizeof(l));
	memset(&l, 0xA5, sizeof(l)); /* poison. */
	test_assert_zero(memcmp(buff, list_init(&l), sizeof(l)));
}

TEST_CASE("list_link_init should have the same effect as static initializer") {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	char buff[sizeof(e.m_link)];
	memcpy(buff, &e.m_link, sizeof(e.m_link));
	memset(&e.m_link, 0xA5, sizeof(e.m_link)); /* poison. */
	test_assert_zero(memcmp(buff, list_link_init(&e.m_link), sizeof(e.m_link)));
}

TEST_CASE("list_empty should return true for just created list") {
	struct list l = LIST_INIT(&l);
	test_assert_true(list_empty(&l));
}

TEST_CASE("list_first should return null for empty list") {
	struct list l = LIST_INIT(&l);
	test_assert_null(list_first(&l, struct element, m_link));
}

TEST_CASE("list_last should return null for empty list") {
	struct list l = LIST_INIT(&l);
	test_assert_null(list_last(&l, struct element, m_link));
}

TEST_CASE("list_first_link should return null for empty list") {
	struct list l = LIST_INIT(&l);
	test_assert_null(list_first_link(&l));
}

TEST_CASE("list_last_link should return null for empty list") {
	struct list l = LIST_INIT(&l);
	test_assert_null(list_last_link(&l));
}

#if 0
TEST_CASE("test_list_add_first_should_make_the_list_non_empty")
{
	struct element e = {.m_link = LIST_LINK_INIT(&e.m_link)};
	struct list l = LIST_INIT(&l);
	list_add_first(&e, &l, m_link);
	return list_empty(&l);
}

TEST_CASE("test_list_add_last_should_make_the_list_non_empty")
{
	struct element e = {.m_link = LIST_LINK_INIT(&e.m_link)};
	struct list l = LIST_INIT(&l);
	list_add_last(&e, &l, m_link);
	return !list_empty(&l);
}

TEST_CASE("test_list_add_first_link_should_make_the_list_non_empty")
{
	struct element e = {.m_link = LIST_LINK_INIT(&e.m_link)};
	struct list l = LIST_INIT(&l);
	list_add_first_link(&e.m_link, &l);
	return !list_empty(&l);
}

TEST_CASE("test_list_add_last_link_should_make_the_list_non_empty")
{
	struct element e = {.m_link = LIST_LINK_INIT(&e.m_link)};
	struct list l = LIST_INIT(&l);
	list_add_last_link(&e.m_link, &l);
	return !list_empty(&l);
}

TEST_CASE("test_list_first_on_a_single_element_list_should_return_the_element")
{
	struct element e = {.m_link = LIST_LINK_INIT(&e.m_link)};
	struct list l = LIST_INIT(&l);
	list_add_first(&e, &l, m_link);
	return list_first(&l, struct element, m_link);
}
#endif

