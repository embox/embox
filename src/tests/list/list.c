/**
 * @file
 * @brief This file is derived from Embox test template.
 *
 * @date
 * @author
 */

#include <embox/test.h>

#include <test/tools.h>
#include <util/list.h>

EMBOX_TEST(run);

struct element {
	struct list_link m_link;
};

static int test_list_init_should_return_its_argument(void) {
	struct list l;
	return &l == list_init(&l);
}

static int test_list_link_init_should_return_its_argument(void) {
	struct element e;
	return &e.m_link == list_link_init(&e.m_link);
}

static int test_list_empty_should_return_true_for_just_created_list(void) {
	struct list l = LIST_INIT(&l);
	return list_empty(&l);
}

static int test_list_empty_should_return_false_for_non_empty_list(void) {
	struct list l = LIST_INIT(&l);
//	list_add_first(t->element, t_list, m_link);
	return !list_empty(&l);
}

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;

	TEST_ASSERT(test_list_init_should_return_its_argument());
	TEST_ASSERT(test_list_link_init_should_return_its_argument());
	TEST_ASSERT(test_list_empty_should_return_true_for_just_created_list());
//	TEST_ASSERT(test_list_empty_should_return_false_for_non_empty_list());

	return result;
}
