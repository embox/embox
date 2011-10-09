/**
 * @file
 * @brief Test unit for util/set.
 *
 * @date Oct 8, 2001
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdio.h>
#include <string.h>
#include <util/set.h>

EMBOX_TEST_SUITE("util/set test");

struct set set;
struct set_link link[3];
int comp(struct set_link* first, struct set_link* second) {
	return first - second;
}

TEST_CASE("Any test for set") {
	test_emit('-');
	set_init(&set);
	set_min_link(&set);
	set_add_link(&set, link+1, comp);
	set_add_link(&set, link+0, comp);
	set_add_link(&set, link+2, comp);
}

