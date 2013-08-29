/**
 * @file
 *
 * @date Aug 29, 2013
 * @author: Anton Bondarev
 */

#include <embox/test.h>
#include <fcntl.h>
#include <errno.h>

EMBOX_TEST_SUITE("test suit for fopen()");

#define NOT_EXISTED_FILE "/dev/fffff12"

TEST_CASE("open not existed file for reading") {
	test_assert_null(fopen(NOT_EXISTED_FILE,"r"));
}

TEST_CASE("open not existed file for writing") {
	test_assert_null(fopen(NOT_EXISTED_FILE,"w"));
}
