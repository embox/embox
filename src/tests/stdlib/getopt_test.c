/**
 * @file
 *
 * @brief Tests for standard 'getopt' calling functionality
 *
 * @date 15.03.2012
 * @author Anton Bondarev
 */
#include <embox/test.h>
#include <getopt.h>

EMBOX_TEST_SUITE("stdlib/getopt test");

TEST_CASE("We pass to getopt function both string arguments as \"-h\" "
		"we expected that getopt find the 'h' option and return it") {
	int opt;
	char *argv[3];

	getopt_init();

	argv[0] = "test_getenv";
	argv[1] = "-h";
	argv[2] = NULL;

	opt = getopt(2, argv, "h");
	test_assert_equal(opt, 'h');
}

TEST_CASE("We pass to getopt string  \"h\" as argv and \"a\" as ops "
		"we expected that getopt return '?' because 'h' is a missing option"
		" argument ") {
	int opt;
	char *argv[3];

	getopt_init();

	argv[0] = "test_getenv";
	argv[1] = "-h";
	argv[2] = NULL;

	opt = getopt(2, argv, "a");
	test_assert_equal(opt, '?');
}

TEST_CASE("We pass to getopt string  \"-i 1\" as argv and \"i:\" as ops "
		"it means that we pass option with the next parameter '1' "
		"we expect that getopt returns 'i' option and puts \"1\" string to"
		" optarg") {
	int opt;
	char *argv[4];

	getopt_init();

	argv[0] = "test_getenv";
	argv[1] = "-i";
	argv[2] = "1";
	argv[3] = NULL;

	opt = getopt(3, argv, "i:");

	test_assert_equal(opt, 'i');
	test_assert_equal(optarg[0], '1');
}
