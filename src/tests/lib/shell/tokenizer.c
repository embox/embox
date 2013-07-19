/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    16.07.2013
 */

#include <cmd/cmdline.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("lib/shell/tokenizer test");

TEST_CASE("double quotes should be considered right") {
	char *argv[10];

	test_assert_equal(3, cmdline_tokenize("test      \"test    test\"     test", argv));

	test_assert_zero(strcmp(argv[0], "test"));
	test_assert_zero(strcmp(argv[1], "test    test"));
	test_assert_zero(strcmp(argv[2], "test"));
}

TEST_CASE("empty tokens shouldn't go to argc") {
	char *argv[10];

	test_assert_equal(1, cmdline_tokenize("test    ", argv));

	test_assert_zero(strcmp(argv[0], "test"));
}
