/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.09.2013
 */

#include <fs/vfs.h>
#include <fs/inode.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Test for various vfs operations");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

static struct inode *test_root;
static struct inode *a, *b, *c, *d, *e;

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

	test_root = inode_alloc(NULL);
	a = inode_alloc(NULL);
	b = inode_alloc(NULL);
	c = inode_alloc(NULL);
	d = inode_alloc(NULL);
	e = inode_alloc(NULL);

	if (!a || !b || !c || !d || !e) {
		return -ENOMEM;
	}

	/* `test_node` has not existed for a long time -- every one of
	 * these six named it, so this suite has not compiled since whatever
	 * rename took it away. The names are the ones the tree below is built
	 * from, in the order it builds them. */
	inode_name_set(test_root, "R");
	inode_name_set(a, "A");
	inode_name_set(b, "B");
	inode_name_set(c, "C");
	inode_name_set(d, "D");
	inode_name_set(e, "E");

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

/* This used to ignore `node` and call vfs_lookup(), which resolves against
 * the RUNNING SYSTEM'S root -- so every case here compared a node of the
 * detached tree the suite builds against something out of the real filesystem,
 * and the first assertion failed. vfs_subtree_lookup() is the one that answers
 * the question this suite is asking: resolve a path relative to a given node. */
static struct inode *vfs_lookup_nr(struct inode *node, const char *str_path) {
	return vfs_subtree_lookup(node, str_path);
}

TEST_CASE("vfs_lookup_nr should find all nodes") {

	test_assert_equal(test_root, vfs_lookup_nr(test_root, "/"));
	test_assert_equal(test_root, vfs_lookup_nr(test_root, "////"));

	test_assert_equal(a, vfs_lookup_nr(test_root, "/A"));
	test_assert_equal(b, vfs_lookup_nr(test_root, "/B"));
	test_assert_equal(c, vfs_lookup_nr(test_root, "/B/C"));
	test_assert_equal(d, vfs_lookup_nr(test_root, "/B/D"));
	test_assert_equal(e, vfs_lookup_nr(test_root, "/B/D/E"));

	test_assert_equal(NULL, vfs_lookup_nr(test_root, "/a"));
	test_assert_equal(NULL, vfs_lookup_nr(test_root, "foo"));
	test_assert_equal(NULL, vfs_lookup_nr(test_root, "/B/D/foo"));

	/* FIXME: rewrite vfs */
	/* some wired ones, but it's so */

	test_assert_equal(test_root, vfs_lookup_nr(test_root, ""));
	test_assert_equal(a, vfs_lookup_nr(test_root, "A"));
}

#if 0 /* FIXME */
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
#endif

TEST_CASE("vfs_lookup_nr should treat . dir entry correct") {

	test_assert_equal(test_root, vfs_lookup_nr(test_root, "/"));
	test_assert_equal(test_root, vfs_lookup_nr(test_root, "/./"));
	test_assert_equal(test_root, vfs_lookup_nr(test_root, "/./."));
	test_assert_equal(test_root, vfs_lookup_nr(test_root, "/././././"));
	test_assert_equal(test_root, vfs_lookup_nr(test_root, "/././././."));

	test_assert_equal(a, vfs_lookup_nr(test_root, "/A/./"));
	test_assert_equal(a, vfs_lookup_nr(test_root, "/A/."));
}
