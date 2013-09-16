/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.09.2013
 */

#include <fs/vfs.h>
#include <fs/node.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Test for various vfs operations");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

static struct node *test_root;
static struct node *a, *b, *c, *d, *e;

/* Builds the tree under `test_root':
 *
 * R
 * |->A
 * |->B
 *    |->C
 *    |->D
 *    	 |->E
 */
static int setup_suite(void) {

	test_root = node_alloc("R", 0);
	a = node_alloc("A", 0);
	b = node_alloc("B", 0);
	c = node_alloc("C", 0);
	d = node_alloc("D", 0);
	e = node_alloc("E", 0);

	if (!a || !b || !c || !d || !e) {
		return -ENOMEM;
	}

	vfs_add_leaf(a, test_root);
	vfs_add_leaf(b, test_root);
	vfs_add_leaf(c, b);
	vfs_add_leaf(d, b);
	vfs_add_leaf(e, d);

	return 0;
}

static int teardown_suite(void) {

	if (!a || !b || !c || !d || !e) {
		return -EINVAL;
	}

	vfs_del_leaf(e);
	vfs_del_leaf(d);
	vfs_del_leaf(c);
	vfs_del_leaf(b);
	vfs_del_leaf(a);
	vfs_del_leaf(test_root);

	return 0;
}

TEST_CASE("Node parent should be same as created") {

	test_assert_equal(test_root, node_parent(a));
	test_assert_equal(test_root, node_parent(b));
	test_assert_equal(b, node_parent(c));
	test_assert_equal(b, node_parent(d));
	test_assert_equal(d, node_parent(e));
}

TEST_CASE("vfs_lookup should find all nodes") {

	test_assert_equal(test_root, vfs_lookup(test_root, "/"));
	test_assert_equal(test_root, vfs_lookup(test_root, "////"));

	test_assert_equal(a, vfs_lookup(test_root, "/A"));
	test_assert_equal(b, vfs_lookup(test_root, "/B"));
	test_assert_equal(c, vfs_lookup(test_root, "/B/C"));
	test_assert_equal(d, vfs_lookup(test_root, "/B/D"));
	test_assert_equal(e, vfs_lookup(test_root, "/B/D/E"));

	test_assert_equal(NULL, vfs_lookup(test_root, "/a"));
	test_assert_equal(NULL, vfs_lookup(test_root, "foo"));
	test_assert_equal(NULL, vfs_lookup(test_root, "/B/D/foo"));

	/* FIXME: rewrite vfs */
	/* some wired ones, but it's so */

	test_assert_equal(test_root, vfs_lookup(test_root, ""));
	test_assert_equal(a, vfs_lookup(test_root, "A"));
}

static char test_path[PATH_MAX];

TEST_CASE("vfs_get_path_till_root should generate all paths correctly") {

	test_assert_zero(vfs_get_path_till_root(test_root, test_root,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/", test_path));

	test_assert_zero(vfs_get_path_till_root(a, test_root,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/A", test_path));

	test_assert_zero(vfs_get_path_till_root(b, test_root,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/B", test_path));

	test_assert_zero(vfs_get_path_till_root(c, test_root,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/B/C", test_path));

	test_assert_zero(vfs_get_path_till_root(d, test_root,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/B/D", test_path));

	test_assert_zero(vfs_get_path_till_root(e, test_root,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/B/D/E", test_path));

	test_assert_zero(vfs_get_path_till_root(e, b,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/D/E", test_path));

	test_assert_equal(1, vfs_get_path_till_root(e, a,
			       	test_path, PATH_MAX));
	test_assert_zero(strcmp("/R/B/D/E", test_path));
}
