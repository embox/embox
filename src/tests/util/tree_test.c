/**
 * @file
 * @brief Test unit for util/tree.
 *
 * @date 12.03.11
 * @author Eldar Abusalimov
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <util/tree.h>

EMBOX_TEST_SUITE("util/tree test");

struct tree tree;
struct tree_link link[3];
int comp(struct tree_link* first, struct tree_link* second) {
	return first - second;
}

TEST_CASE("Any test for tree") {
	test_emit('-');
	tree_init(&tree, comp);
	tree_min_link(&tree);
	tree_add_link(&tree, link+1);
	tree_add_link(&tree, link+0);
	tree_add_link(&tree, link+2);
	test_assert_null(0);
}



