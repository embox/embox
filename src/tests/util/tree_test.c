/**
 * @file
 * @brief Test unit for util/tree.
 *
 * @date Oct 8, 2011
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <util/tree.h>

EMBOX_TEST_SUITE("util/tree test");

struct tree_link links[3];

TEST_CASE("Any test for tree") {
	test_emit('-');
	tree_link_init(links + 1);
	tree_add_link(links+1, links+0);
	tree_add_link(links+1, links+2);
}

