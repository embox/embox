/**
 * @file
 * @brief This file is derived from Embox test template.
 *
 * @date 12.03.11
 * @author Eldar Abusalimov
 */

#include <embox/test.h>

#include <string.h>
#include <util/list.h>
#include <util/array.h>
#include <test/tools.h>

EMBOX_TEST(run);

struct element {
	int some_stuff;
	struct list_link m_link;
};

typedef int (*test_list_fn)(void);

static int test_list_link_element_should_cast_link_member_out_to_its_container(
		void) {
	struct element e;
	struct list_link *link = &e.m_link;
	return &e == list_link_element(link, struct element, m_link);
}

static int test_list_init_should_return_its_argument(void) {
	struct list l;
	return &l == list_init(&l);
}

static int test_list_link_init_should_return_its_argument(void) {
	struct element e;
	return &e.m_link == list_link_init(&e.m_link);
}

static int test_list_init_should_do_the_same_as_static_initializer(void) {
	struct list l = LIST_INIT(&l);
	char buff[sizeof(l)];
	memcpy(buff, &l, sizeof(l));
	memset(&l, 0xA5, sizeof(l)); /* poison. */
	return !memcmp(buff, list_init(&l), sizeof(l));
}

static int test_list_link_init_should_do_the_same_as_static_initializer(void) {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	char buff[sizeof(e.m_link)];
	memcpy(buff, &e.m_link, sizeof(e.m_link));
	memset(&e.m_link, 0xA5, sizeof(e.m_link)); /* poison. */
	return !memcmp(buff, list_link_init(&e.m_link), sizeof(e.m_link));
}

static int test_list_empty_should_return_true_for_just_created_list(void) {
	struct list l = LIST_INIT(&l);
	return list_empty(&l);
}

static int test_list_first_should_return_null_for_empty_list(void) {
	struct list l = LIST_INIT(&l);
	return list_first(&l, struct element, m_link) == NULL;
}

static int test_list_last_should_return_null_for_empty_list(void) {
	struct list l = LIST_INIT(&l);
	return list_last(&l, struct element, m_link) == NULL;
}

static int test_list_first_link_should_return_null_for_empty_list(void) {
	struct list l = LIST_INIT(&l);
	return list_first_link(&l) == NULL;
}

static int test_list_last_link_should_return_null_for_empty_list(void) {
	struct list l = LIST_INIT(&l);
	return list_last_link(&l) == NULL;
}

static int test_list_add_first_should_make_the_list_non_empty(void) {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	struct list l = LIST_INIT(&l);
	list_add_first(&e, &l, m_link);
	return !list_empty(&l);
}

static int test_list_add_last_should_make_the_list_non_empty(void) {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	struct list l = LIST_INIT(&l);
	list_add_last(&e, &l, m_link);
	return !list_empty(&l);
}

static int test_list_add_first_link_should_make_the_list_non_empty(void) {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	struct list l = LIST_INIT(&l);
	list_add_first_link(&e.m_link, &l);
	return !list_empty(&l);
}

static int test_list_add_last_link_should_make_the_list_non_empty(void) {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	struct list l = LIST_INIT(&l);
	list_add_last_link(&e.m_link, &l);
	return !list_empty(&l);
}

static int test_list_first_on_a_single_element_list_should_return_the_element(
		void) {
	struct element e = { .m_link = LIST_LINK_INIT(&e.m_link) };
	struct list l = LIST_INIT(&l);
	list_add_first(&e, &l, m_link);
	return list_first(&l, struct element, m_link);
}

struct test_entry {
	const char *name;
	test_list_fn fn;
};

#define TEST_ENTRY(test) { \
		.name = #test, \
		.fn = (test), \
	}

/* TODO generalize such approach. -- Eldar */
static const struct test_entry tests[] = {
	TEST_ENTRY(
		test_list_link_element_should_cast_link_member_out_to_its_container),
	TEST_ENTRY(test_list_init_should_return_its_argument),
	TEST_ENTRY(test_list_link_init_should_return_its_argument),
	TEST_ENTRY(test_list_init_should_do_the_same_as_static_initializer),
	TEST_ENTRY(test_list_link_init_should_do_the_same_as_static_initializer),
	TEST_ENTRY(test_list_empty_should_return_true_for_just_created_list),
	TEST_ENTRY(test_list_first_should_return_null_for_empty_list),
	TEST_ENTRY(test_list_last_should_return_null_for_empty_list),
	TEST_ENTRY(test_list_first_link_should_return_null_for_empty_list),
	TEST_ENTRY(test_list_last_link_should_return_null_for_empty_list),
//	TEST_ENTRY(test_list_add_first_should_make_the_list_non_empty),
//	TEST_ENTRY(test_list_add_last_should_make_the_list_non_empty),
//	TEST_ENTRY(test_list_add_first_link_should_make_the_list_non_empty),
//	TEST_ENTRY(test_list_add_last_link_should_make_the_list_non_empty),
//	TEST_ENTRY(
//		test_list_first_on_a_single_element_list_should_return_the_element),
};

static int run(void) {
	const struct test_entry *test;

	array_static_foreach_ptr(test, tests) {
		if (!test->fn()) {
			return test_fail(test->name);
		}
	}

	return test_pass();
}
