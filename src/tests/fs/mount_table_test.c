/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.09.2013
 */

#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/mount.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Test for various vfs operations");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

static struct node *r1, *ch1, *r2, *ch2, *r3, *ch3, *r4, *ch4, *test;
/* Builds the tree under `test_root':
 *
 *   test
 *     |
 *    ch1
 *   /   \
 *  ch4   ch2
 *         |
 *        ch3
 */

#include <util/list.h>
static int setup_suite(void) {
	struct path test_path, path1, path2, path3, path4;

	r1 = node_alloc("/", 1);
	ch1 = node_alloc("ch1", 3);
	r2 = node_alloc("/", 1);
	ch2 = node_alloc("ch2", 3);
	r3 = node_alloc("/", 1);
	ch3 = node_alloc("ch3", 3);
	r4 = node_alloc("/", 1);
	ch4 = node_alloc("ch4", 3);
	test = node_alloc("test", 4);

	if (!r1 || !ch1 || !r2 || !ch2 || !r3 || ! ch3 || !r4 || !ch4) {
		return -ENOMEM;
	}

	vfs_add_leaf(ch1, r1);
	vfs_add_leaf(ch2, r2);
	vfs_add_leaf(ch3, r3);
	vfs_add_leaf(ch4, r4);
	vfs_add_leaf(test, vfs_get_root());

	test_path.mnt_desc = mount_table();
	test_path.node = test;
	path1.node = ch1;
	assert(path1.mnt_desc = mount_table_add(&test_path, r1, "", ""));

	path2.node = ch2;
	assert(path2.mnt_desc = mount_table_add(&path1, r2, "", ""));

	path3.node = ch3;
	assert(path3.mnt_desc = mount_table_add(&path2, r3, "", ""));

	path4.node = ch4;
	assert(path4.mnt_desc = mount_table_add(&path1, r4, "", ""));

	return 0;
}

static int teardown_suite(void) {

	if (!r1 || !ch1 || !r2 || !ch2 || !r3 || ! ch3 || !r4 || !ch4) {
		return -EINVAL;
	}

	vfs_del_leaf(r1);
	vfs_del_leaf(r2);
	vfs_del_leaf(r3);
	vfs_del_leaf(r4);
	vfs_del_leaf(ch1);
	vfs_del_leaf(ch2);
	vfs_del_leaf(ch3);
	vfs_del_leaf(ch4);
	vfs_del_leaf(test);
	return 0;
}

TEST_CASE("mount_table_del() and mount_table_get_child() correctness") {
	struct path root, path, test_path, c1_path, c2_path;

	vfs_get_root_path(&root);
	test_assert_zero(vfs_lookup(&root, "test", &test_path));
	test_assert_not_null(mount_table_get_child(test_path.mnt_desc, test_path.node));

	test_assert_zero(vfs_lookup(&root, "test/ch1", &c1_path));

	test_assert_zero(vfs_lookup(&root, "test/ch1/ch4", &c2_path));
	test_assert_not_zero(vfs_lookup(&root, "test/ch1/ch2", &path));

	mount_table_del(c2_path.mnt_desc);
	test_assert_zero(vfs_lookup(&root, "test/ch1/ch2/ch3", &path));

	mount_table_del(path.mnt_desc);
	test_assert_not_zero(vfs_lookup(&root, "test/ch1/ch2/ch3", &path));
	test_assert_zero(vfs_lookup(&root, "test/ch1/ch2", &path));

	mount_table_del(path.mnt_desc);
	test_assert_not_zero(vfs_lookup(&root, "test/ch1/ch2", &path));
	test_assert_zero(vfs_lookup(&root, "test/ch1", &path));

	mount_table_del(path.mnt_desc);
	test_assert_not_zero(vfs_lookup(&root, "test/ch1", &path));
}
