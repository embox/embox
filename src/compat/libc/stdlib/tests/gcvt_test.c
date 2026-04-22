#include <stdlib.h>
#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("gcvt() tests");

TEST_CASE("gcvt with positive float") {
	char buf[32];
	gcvt(3.14, 3, buf);
	test_assert(strcmp(buf, "3.14") == 0);
}

TEST_CASE("gcvt with negative float") {
	char buf[32];
	gcvt(-3.14, 3, buf);
	test_assert(strcmp(buf, "-3.14") == 0);
}

TEST_CASE("gcvt with zero") {
	char buf[32];
	gcvt(0.0, 3, buf);
	test_assert(strcmp(buf, "0.000") == 0);
}

TEST_CASE("gcvt with integer-like value") {
	char buf[32];
	gcvt(100.0, 3, buf);
	test_assert(strcmp(buf, "100") == 0);
}

TEST_CASE("gcvt with trailing zero") {
	char buf[32];
	gcvt(1.5, 3, buf);
	test_assert(strcmp(buf, "1.50") == 0);
}

TEST_CASE("gcvt with NULL buf returns NULL") {
	test_assert(gcvt(1.5, 3, NULL) == NULL);
}
