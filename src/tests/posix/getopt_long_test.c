/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    17.07.2014
 */

#include <getopt.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("getopt_long test suite");

TEST_SETUP(test_getopt_long_case_setup);

static int test_getopt_long_case_setup(void) {
	optind = 1;
	return 0;
}

TEST_CASE("getopt_long should take NULL longindex") {
	char *const argv[] = {"", "--long1", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long1", no_argument, 0, 1 },
		{ },
	};

	test_assert_equal(1, getopt_long(argc, argv, "", long_opts, NULL));
}

TEST_CASE("getopt_long should process every argument one time exactly") {
	char *const argv[] = {"", "--long1", "--long2", "--long1", "--long1", "--long4", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long1", no_argument, 0, 0 },
		{ "long2", no_argument, 0, 1 },
		{ "long3", no_argument, 0, 2 },
		{ },
	};
	int opt;
	int seen[2] = {0, 0};

	while (-1 != (opt = getopt_long(argc, argv, "", long_opts, NULL))) {
		test_assert(opt == 0 || opt == 1);
		seen[opt]++;
	}

	test_assert_equal(3, seen[0]);
	test_assert_equal(1, seen[1]);
	test_assert_equal(5, optind);
}

TEST_CASE("getopt_long should fill longindex if provided") {
	char *const argv[] = {"", "--long1", "--long2", "--long1", "--long1", "--long4", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long1", no_argument, 0, 10 },
		{ "long2", no_argument, 0, 11 },
		{ "long3", no_argument, 0, 12 },
		{ },
	};
	int longindex, opt;

	while (-1 != (opt = getopt_long(argc, argv, "", long_opts, &longindex))) {
		test_assert_equal(longindex, opt - 10);
	}
	test_assert_equal(5, optind);
}

TEST_CASE("getopt_long should set optind to next arg") {
	char *const argv[] = {"", "--long1", "--long2", "arg1", "arg2", "arg3", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long1", no_argument,       0, 1 },
		{ "long2", no_argument,       0, 2 },
		{ },
	};
	int opt;

	while (-1 != (opt = getopt_long(argc, argv, "", long_opts, NULL))) {
		test_assert(opt == 1 || opt == 2);

		if (opt == 1) {
			test_assert_equal(2, optind);
		} else {
			test_assert_equal(3, optind);
		}
	}

	test_assert_equal(3, optind);
}

TEST_CASE("getopt_long should write to flag ptr, if provided") {
	char *const argv[] = {"", "--long1", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	int flag = 0;
	struct option long_opts[] = {
		{ "long1", no_argument, &flag, 1 },
		{ },
	};

	test_assert_equal(0, getopt_long(argc, argv, "", long_opts, NULL));
	test_assert_equal(1, flag);
	test_assert_equal(2, optind);
}

TEST_CASE("getopt_long should get mandatory arg") {
	char *const argv[] = {"", "--long2", "optarg", "--long3", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long2", required_argument, 0, 1 },
		{ },
	};

	test_assert_equal(1, getopt_long(argc, argv, "", long_opts, NULL));
	test_assert_str_equal("optarg", optarg);
	test_assert_equal(3, optind);
}

TEST_CASE("getopt_long shouldn't get empty arg") {
	char *const argv[] = {"", "--long2", "optarg", "--long3", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long2", no_argument, 0, 1 },
		{ },
	};

	test_assert_equal(1, getopt_long(argc, argv, "", long_opts, NULL));
	test_assert_equal(NULL, optarg);
	test_assert_equal(2, optind);
}

TEST_CASE("getopt_long should treat options with argument separated with =") {
	char *const argv[] = {"", "--long1=optarg1", "--long2=optarg2", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long1", required_argument, 0, 1 },
		{ "long2", no_argument,       0, 2 },
		{ },
	};
	int opt;

	while (-1 != (opt = getopt_long(argc, argv, "", long_opts, NULL))) {
		test_assert_equal(opt, 1);
		test_assert_str_equal("optarg1", optarg);
	}
	test_assert_equal(2, optind);
}

TEST_CASE("getopt_long should treat optional argument right") {
	char *const argv[] = {"", "--long1", "--long2=optarg", NULL};
	const int argc = ARRAY_SIZE(argv) - 1;
	struct option long_opts[] = {
		{ "long1", optional_argument, 0, 1 },
		{ "long2", optional_argument, 0, 2 },
		{ },
	};
	int opt;

	while (-1 != (opt = getopt_long(argc, argv, "", long_opts, NULL))) {

		switch(opt) {
		case 1:
			test_assert_null(optarg);
			break;
		case 2:
			test_assert_str_equal("optarg", optarg);
			break;
		default:
			test_fail();
		}
	}
	test_assert_equal(3, optind);
}
