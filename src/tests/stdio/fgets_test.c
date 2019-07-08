/**
 * @file
 * @brief
 *
 * @author  Nastya Nizharadze
 * @date    08.07.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("stdio/fgets test");

/* File's content is
* qwer\n
* asdf\n
* \n
* zxcv
*/
#define TEST_FILE_NAME       "/test_fgets.txt"
#define TEST_FILE_NAME_EMPTY "/test_fgets_empty.txt"

#define ERROR_MESSAGE(str) "cannot read: " str ": No such file\n"
#define STR_SIZE_TEST (6 * sizeof(char))

TEST_CASE("fgets should read lines with \\n at the end") {
	FILE *fp;
	char line[STR_SIZE_TEST];
	if (NULL == (fp = fopen(TEST_FILE_NAME, "r"))) {
		test_fail(ERROR_MESSAGE(TEST_FILE_NAME));
	}

	fgets(line, STR_SIZE_TEST, fp);
	test_assert_str_equal(line, "qwer\n");

	fgets(line, STR_SIZE_TEST, fp);
	test_assert_str_equal(line, "asdf\n");

	fgets(line, STR_SIZE_TEST, fp);
	test_assert_str_equal(line, "\n");

	fgets(line, STR_SIZE_TEST, fp);
	test_assert_str_equal(line, "zxcv");

	fclose(fp);
}

TEST_CASE("fgets should return NULL with empty file") {
	FILE *fp;
	char line[STR_SIZE_TEST];
	if (NULL == (fp = fopen(TEST_FILE_NAME_EMPTY, "r"))) {
		test_fail(ERROR_MESSAGE(TEST_FILE_NAME_EMPTY));
	}
	test_assert_null(fgets(line, STR_SIZE_TEST, fp));
	
	fclose(fp);
}
