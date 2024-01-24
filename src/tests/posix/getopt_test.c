/**
 * @file
 * @brief Tests for standard 'getopt' calling functionality
 *
 * @date 15.03.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <embox/test.h>
#include <lib/libds/array.h>
#include <unistd.h>
#include <util/getopt.h>

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

struct opt_snapshot {
	int code;
	int index;
	int option;
	int error;
	char *argument;
};

static struct opt_snapshot * opt_snap_lookup(int code,
		struct opt_snapshot snaps[], size_t snaps_sz) {
	size_t i;

	for (i = 0; i < snaps_sz; ++i)
		if (snaps[i].code == code)
			return &snaps[i];

	return NULL;
}

static void getopt_test(int argc, char *argv[], const char *opts,
		struct opt_snapshot snaps[], size_t snaps_sz) {
	int ret;
	struct opt_snapshot *snap;

	getopt_init();
	do {
		ret = getopt(argc, argv, opts);

		snap = opt_snap_lookup(ret, snaps, snaps_sz);
		test_assert_not_null(snap);

		test_assert_equal(snap->index, optind);
		test_assert_equal(snap->option, optopt);
		test_assert_equal(snap->error, opterr);
		if (optarg == NULL) {
			test_assert_null(snap->argument);
		} else {
			test_assert_str_equal(snap->argument, optarg);
		}
	} while (ret != -1);
}

TEST_CASE("Test opt_xxx_ valiables after command"
		" without options and arguments") {
	char *argv[] = { "prog" };
	struct opt_snapshot snaps[] = {
		{ -1, 1, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" without options with many argument") {
	char *argv[] = { "prog", "x", "y", "z" };
	struct opt_snapshot snaps[] = {
		{ -1, 1, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with one option and without arguments") {
	char *argv[] = { "prog", "-a" };
	struct opt_snapshot snaps[] = {
		{ 'a', 2, 0, 1, NULL },
		{ -1, 2, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with one option and many arguments") {
	char *argv[] = { "prog", "-a", "x", "y", "z" };
	struct opt_snapshot snaps[] = {
		{ 'a', 2, 0, 1, NULL },
		{ -1, 2, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with many single options and many arguments") {
	char *argv[] = { "prog", "-ade", "-j", "x", "y", "z" };
	struct opt_snapshot snaps[] = {
		{ 'a', 1, 0, 1, NULL },
		{ 'd', 1, 0, 1, NULL },
		{ 'e', 2, 0, 1, NULL },
		{ 'j', 3, 0, 1, NULL },
		{ -1, 3, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with many options with argument and without arguments") {
	char *argv[] = { "prog", "-abb_opt", "-c", "c_opt", "-def",
			"f_opt", "-j" };
	struct opt_snapshot snaps[] = {
		{ 'a', 1, 0, 1, NULL },
		{ 'b', 2, 0, 1, "b_opt" },
		{ 'c', 4, 0, 1, "c_opt" },
		{ 'd', 4, 0, 1, NULL },
		{ 'e', 4, 0, 1, NULL },
		{ 'f', 6, 0, 1, "f_opt" },
		{ 'j', 7, 0, 1, NULL },
		{ -1, 7, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with many options with argument and with arguments") {
	char *argv[] = { "prog", "-abb_opt", "-c", "c_opt", "-def",
			"f_opt", "-j", "x", "y" };
	struct opt_snapshot snaps[] = {
		{ 'a', 1, 0, 1, NULL },
		{ 'b', 2, 0, 1, "b_opt" },
		{ 'c', 4, 0, 1, "c_opt" },
		{ 'd', 4, 0, 1, NULL },
		{ 'e', 4, 0, 1, NULL },
		{ 'f', 6, 0, 1, "f_opt" },
		{ 'j', 7, 0, 1, NULL },
		{ -1, 7, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with many options with argument and with arguments (shuffle)") {
	char *argv[] = { "prog", "-abb_opt", "x", "-c", "c_opt", "-def",
			"f_opt", "y", "-j", "z" };
	struct opt_snapshot snaps[] = {
		{ 'a', 1, 0, 1, NULL },
		{ 'b', 2, 0, 1, "b_opt" },
		{ 'c', 5, 0, 1, "c_opt" },
		{ 'd', 5, 0, 1, NULL },
		{ 'e', 5, 0, 1, NULL },
		{ 'f', 7, 0, 1, "f_opt" },
		{ 'j', 9, 0, 1, NULL },
		{ -1, 7, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command "
		" with single option and argument without spaces between them") {
	char *argv[] = { "prog", "-ax", "-bb_opt", "y" };
	struct opt_snapshot snaps[] = {
		{ 'a', 1, 0, 1, NULL },
		{ '?', 2, 120, 1, NULL },
		{ 'b', 3, 120, 1, "b_opt" },
		{ -1, 3, 120, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command with separator") {
	char *argv[] = { "prog", "-a", "-bb_opt", "--", "-f", "x" };
	struct opt_snapshot snaps[] = {
		{ 'a', 2, 0, 1, NULL },
		{ 'b', 3, 0, 1, "b_opt" },
		{ -1, 4, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}

TEST_CASE("Test opt_xxx_ valiables after command with separator instead option") {
	char *argv[] = { "prog", "-a", "-bb_opt", "-f", "--", "x" };
	struct opt_snapshot snaps[] = {
		{ 'a', 2, 0, 1, NULL },
		{ 'b', 3, 0, 1, "b_opt" },
		{ 'f', 5, 0, 1, "--" },
		{ -1, 5, 0, 1, NULL }
	};

	getopt_test(ARRAY_SIZE(argv), argv, "ab:c:def:j",
			&snaps[0], ARRAY_SIZE(snaps));
}
