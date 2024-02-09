/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.09.2013
 */

#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/mount.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("Test for various vfs operations");

TEST_SETUP_SUITE(setup_suite);
TEST_TEARDOWN_SUITE(teardown_suite);

static struct inode *test_node;
/* Builds the tree under `test_root':
 *
 *   test
 *     |
 *    ch1
 *   /   \
 *  ch4   ch2
 *         |
 *        ch3
 *
 * (test)->(r1)
 *          |
 *          |
 *         (ch1)->(r2)->(r4)
 *                 |     |
 *                 |     |
 *                 |    (ch4)
 *                 |
 *                (ch2)->(r3)
 *                        |
 *                        |
 *                       (ch3)
 *
 */

static int test_mt_path_init(const char *strpath, const char *name) {
	struct mount_descriptor *mdesc;
	struct inode *node, *root_node;
	struct path path;
	struct mount_descriptor *mnt_desc;

	if (vfs_lookup(strpath, &path)) {
		return -ENOENT;
	}

	node = inode_alloc(name);
	assert(node);
	root_node = inode_alloc("/");
	assert(root_node);

	vfs_add_leaf(node, root_node);

	mnt_desc = mount_desc_by_inode(path.node);

	mdesc = mount_table_add(&path, mnt_desc, root_node, "");
	assert(mdesc);

	return 0;
}

static int test_mt_path_fini(const char *strpath) {
	struct path path;
	struct inode *root_node;

	if (vfs_lookup(strpath, &path)) {
		return -ENOENT;
	}

	root_node = path.mnt_desc->mnt_root;
	mount_table_del(path.mnt_desc);

	vfs_del_leaf(path.node);

	inode_free(root_node);

	return 0;
}

static int setup_suite(void) {
	struct path root;

	vfs_get_root_path(&root);
	test_node = inode_alloc("test");

	vfs_add_leaf(test_node, root.node);

	test_mt_path_init("/test", "ch1");
	test_mt_path_init("/test/ch1", "ch2");
	test_mt_path_init("/test/ch1/ch2", "ch3");
	test_mt_path_init("/test/ch1", "ch4");

	return 0;
}

static int teardown_suite(void) {

	test_mt_path_fini("/test/ch1/ch4");
	test_mt_path_fini("/test/ch1/ch2/ch3");
	test_mt_path_fini("/test/ch1/ch2");
	test_mt_path_fini("/test/ch1");

	vfs_del_leaf(test_node);

	return 0;
}

TEST_CASE("new mounts should hide old content") {
	struct path path;

	test_assert_zero(vfs_lookup("test", &path));

	test_assert_zero(vfs_lookup("test/ch1", &path));
	test_assert_zero(vfs_lookup("test/ch1/ch4", &path));
	test_assert_not_zero(vfs_lookup("test/ch1/ch2/ch3", &path));
	test_assert_not_zero(vfs_lookup("test/ch1/ch2", &path));
}

TEST_CASE("mount_table_get_child should find `test' mount_desc") {
	struct path path;

	test_assert_zero(vfs_lookup("test", &path));

	test_assert_not_null(mount_table_get_child(path.mnt_desc, path.node));
}

TEST_CASE("umount should expose old content") {
	struct path path;

	test_assert_not_zero(vfs_lookup("/test/ch1/ch2/ch3", &path));

	test_assert_zero(test_mt_path_fini("/test/ch1/ch4"));

	test_assert_zero(vfs_lookup("/test/ch1/ch2/ch3", &path));

	/* reverting changes back */
	test_assert_zero(test_mt_path_init("/test/ch1", "ch4"));
	test_assert_not_zero(vfs_lookup("/test/ch1/ch2/ch3", &path));
}

TEST_CASE("mount_table_del() and mount_table_get_child() correctness") {
	struct path path, test_path, c1_path, c2_path;

	test_assert_zero(vfs_lookup("/test", &test_path));
	test_assert_not_null(mount_table_get_child(test_path.mnt_desc, test_path.node));

	test_assert_zero(vfs_lookup("/test/ch1", &c1_path));

	test_assert_zero(vfs_lookup("/test/ch1/ch4", &c2_path));
	test_assert_not_zero(vfs_lookup("/test/ch1/ch2", &path));

	mount_table_del(c2_path.mnt_desc);
	test_assert_zero(vfs_lookup("/test/ch1/ch2/ch3", &path));

	mount_table_del(path.mnt_desc);
	test_assert_not_zero(vfs_lookup("test/ch1/ch2/ch3", &path));
	test_assert_zero(vfs_lookup("/test/ch1/ch2", &path));

	mount_table_del(path.mnt_desc);
	test_assert_not_zero(vfs_lookup("/test/ch1/ch2", &path));
	test_assert_zero(vfs_lookup("/test/ch1", &path));

	mount_table_del(path.mnt_desc);
	test_assert_not_zero(vfs_lookup("/test/ch1", &path));
}
