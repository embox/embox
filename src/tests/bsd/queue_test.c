/**
 * @file
 *
 * @date March 7, 2020
 * @author: Subhojit Ghorai
 */

#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("test Singly Linked List Functions of BSD queue.h");
struct test_type {
	int				data;
	SLIST_ENTRY(test_type)		test_field;
};
TEST_CASE("Testing BSD queue functions: ") {
	SLIST_HEAD(test_head, test_type);
	struct test_head test_head =       
	SLIST_HEAD_INITIALIZER(test_head);                                           
	struct test_type *test_data;
	int head_data;

	test_data = (struct test_type *)malloc(sizeof(*test_data));
	memset(test_data, 1, sizeof(*test_data));

	SLIST_INSERT_HEAD(&test_head, test_data, test_field);
	test_assert_false(SLIST_EMPTY(&test_head));

	head_data = test_head.slh_first->data;
	SLIST_REMOVE(&test_head, test_data, test_type, test_field);

	test_assert_equal(test_data->data, head_data);	
	test_assert_true(SLIST_EMPTY(&test_head));	
	free(test_data);
}