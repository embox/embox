/**
 * @file
 * @brief
 *
 * @date Dec 15, 2020
 * @author:Evgeny Svirin
 */

#include <embox/test.h>
#include <sys/stat.h> 
#include <unistd.h> 

EMBOX_TEST_SUITE("Test for mkdir");

#define MKDIR_TEST_MOUNTPOINT "/mnt"
#define MKDIR_TEST_CREAT_MODE 0777

TEST_CASE("Make one directory") {
	static const char directory [] = MKDIR_TEST_MOUNTPOINT "/mkdir_test_dir";

	test_assert_zero(mkdir(directory, MKDIR_TEST_CREAT_MODE));

	test_assert_zero(rmdir(directory));
}

TEST_CASE("Make 3 directories") {
	static const char directory1 [] = MKDIR_TEST_MOUNTPOINT "/mkdir_test_dir1";
	static const char directory2 [] = MKDIR_TEST_MOUNTPOINT "/mkdir_test_dir1/mkdir_test_dir2";
	static const char directory3 [] = MKDIR_TEST_MOUNTPOINT "/mkdir_test_dir1/mkdir_test_dir3";

	test_assert_zero(mkdir(directory1, MKDIR_TEST_CREAT_MODE));

	test_assert_zero(mkdir(directory2, MKDIR_TEST_CREAT_MODE));

	test_assert_zero(mkdir(directory3, MKDIR_TEST_CREAT_MODE));

	test_assert_zero(rmdir(directory3));

	test_assert_zero(rmdir(directory2));

	test_assert_zero(rmdir(directory1));
}

TEST_CASE("Make same directory two times") {
	static const char directory4 [] = MKDIR_TEST_MOUNTPOINT "/mkdir_test_dir4";

	test_assert_zero(mkdir(directory4, MKDIR_TEST_CREAT_MODE));

	test_assert_not_equal(0 , mkdir(directory4, MKDIR_TEST_CREAT_MODE));

	test_assert_zero(rmdir(directory4));
}

TEST_CASE("Wrong order make") {
	static const char directory6 [] = MKDIR_TEST_MOUNTPOINT "/mkdir_test_dir5/mkdir_test_dir6";

	test_assert_not_equal(0, mkdir(directory6, MKDIR_TEST_CREAT_MODE));
}
