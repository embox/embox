/**
 * @file
 * @brief This file is derived from Embox test template.
 *
 * @date
 * @author
 */

#include <embox/test.h>

#include <string.h>
#include <test/tools.h>
#include <util/list.h>

EMBOX_TEST(run);

struct element {
	int some_stuff;
	struct list_link m_link;
};

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

#if 0
static int test_list_empty_should_return_false_for_non_empty_list(void) {
	struct list l = LIST_INIT(&l);
//	list_add_first(t->element, t_list, m_link);
	return !list_empty(&l);
}
#endif

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	TEST_ASSERT(
			test_list_link_element_should_cast_link_member_out_to_its_container());
	TEST_ASSERT(test_list_init_should_return_its_argument());
	TEST_ASSERT(test_list_link_init_should_return_its_argument());
	TEST_ASSERT(test_list_init_should_do_the_same_as_static_initializer());
	TEST_ASSERT(test_list_link_init_should_do_the_same_as_static_initializer());
	TEST_ASSERT(test_list_empty_should_return_true_for_just_created_list());
	TEST_ASSERT(test_list_first_should_return_null_for_empty_list());
	TEST_ASSERT(test_list_last_should_return_null_for_empty_list());
	TEST_ASSERT(test_list_first_link_should_return_null_for_empty_list());
	TEST_ASSERT(test_list_last_link_should_return_null_for_empty_list());
//	TEST_ASSERT(test_list_empty_should_return_false_for_non_empty_list());

	return result;
}
