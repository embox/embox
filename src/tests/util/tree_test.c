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

static struct tree_link links[3];

TEST_CASE("Any test for tree") {
	struct tree_link *link;
	int i;
	for (i = 0; i < 3; i++) {
		tree_link_init(links + i);
	}
	tree_add_link(links+1, links+0);
	tree_add_link(links+1, links+2);
	tree_foreach_link(link, links+1) {
		/* printf("%d ", (int)link); */
	}
}

