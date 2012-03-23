/**
 * @file
 *
 * @brief
 *
 * @date 23.03.2012
 * @author Anton Bondarev
 */


#include <embox/test.h>
#include <util/dlist.h>

EMBOX_TEST_SUITE("util/dlist test");


static DLIST_DEFINE(static_dlist);
//static struct dlist_head dynamic_dlist; /* require initialization */

struct element {
	int data;
	struct dlist_head lnk;
};

static struct element element[0x10];

TEST_CASE("Add a single element to a dlist") {
	test_assert_not_zero(dlist_empty(&static_dlist));
	dlist_add_next(&element[0].lnk, &static_dlist);
	test_assert_zero(dlist_empty(&static_dlist));
	dlist_del(&element[0].lnk);
	test_assert_not_zero(dlist_empty(&static_dlist));
}

TEST_CASE("Add two elements to a dlist"
		" first added by dlist_add_next and second with dlist_add_prev") {
	test_assert_not_zero(dlist_empty(&static_dlist));
	dlist_add_next(&element[0].lnk, &static_dlist);
	dlist_add_prev(&element[1].lnk, &static_dlist);

	test_assert_equal(static_dlist.next, &element[0].lnk);
	test_assert_equal(&static_dlist, element[0].lnk.prev);

	test_assert_zero(dlist_empty(&static_dlist));

	dlist_del(&element[0].lnk);
	dlist_del(&element[1].lnk);
	test_assert_not_zero(dlist_empty(&static_dlist));
}
