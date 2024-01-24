/**
 * @file
 *
 * @brief Tests for dlist API
 *
 * @details These tests examine correct work dlist implementation. They checks
 *         static and stack allocations both for items and list. Also it uses
 *         dynamic initialization by function #dlist_init and #dlist_head_init
 *         and static initialization by macro #DLIST_DEFINE and #DLIST_INIT.
 *         As a data these tests use integer value as element id.
 *         There are some tests which examine situation when an element has two
 *         item head and can be added to two different lists.
 *
 * @date 23.03.2012
 * @author Anton Bondarev
 */
#include <embox/test.h>
#include <lib/libds/dlist.h>

EMBOX_TEST_SUITE("util/dlist test");

/* Static define and initialize a list. It can be use immediate without using
 * #dlist_init
 */
static DLIST_DEFINE(static_dlist);
/* Just reserved memory for head. It requires initialization in runtime
 * #dlist_init if we want as list entry*/
static struct dlist_head dynamic_dlist;

/*
 * A structure data which we will link in double linked lists. It contains two
 * item head, due to each element can be added in two different lists
 */
struct element {
	int data;                      /**< element data */
	struct dlist_head lnk;         /**< item head for first list */
	struct dlist_head another_lnk; /**< item head for second list */
};

/* Item pool for list. Each item head required initialization before using. It's
 * initialized by #list_head_init */
static struct element element[0x10];

TEST_CASE("Add a single element to a dlist") {
	struct dlist_head *dlist = &static_dlist; /*Initialization is not required*/

	test_assert_not_zero(dlist_empty(dlist));
	dlist_add_next(dlist_head_init(&element[0].lnk), dlist);
	test_assert_zero(dlist_empty(dlist));
	dlist_del(&element[0].lnk);
	test_assert_not_zero(dlist_empty(dlist));
}

TEST_CASE("Add two elements to a dlist"
		" first added by dlist_add_next and second with dlist_add_prev") {
	struct dlist_head *dlist = &static_dlist; /*initialization is not required*/

	test_assert_not_zero(dlist_empty(dlist));
	dlist_add_next(dlist_head_init(&element[0].lnk), dlist);
	dlist_add_prev(dlist_head_init(&element[1].lnk), dlist);

	test_assert_equal(dlist->next, &element[0].lnk);
	test_assert_equal(dlist, element[0].lnk.prev);

	test_assert_zero(dlist_empty(dlist));

	dlist_del(&element[0].lnk);
	dlist_del(&element[1].lnk);
	test_assert_not_zero(dlist_empty(dlist));
}

TEST_CASE("Create a list in runtime and add a single element") {
	struct dlist_head *dlist = dlist_init(&dynamic_dlist); /*initialize*/

	test_assert_not_zero(dlist_empty(dlist));
	dlist_add_next(dlist_head_init(&element[0].lnk), dlist);
	test_assert_zero(dlist_empty(dlist));
	dlist_del(&element[0].lnk);
	test_assert_not_zero(dlist_empty(dlist));
}
