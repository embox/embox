#include <iconv.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("lib/iconv_test");

TEST_CASE("test from UTF-8 to KOI8-R") {
	iconv_t cd;
	size_t utf_size = 4;
	char* utf = "Тест";
	char* in = utf;
	char actual[5];
	char* out = actual;
	cd = iconv_open("KOI8-R", "UTF-8");
	test_assert_false(cd == (iconv_t)(-1));
	size_t t = sizeof(actual);
	for (int i = 0; i < sizeof(actual) / sizeof(char); i++) {
		actual[i] = 0;
	}
	iconv(cd, &in, &utf_size, &out, &t);
	int expected[] = { 244, 197, 211, 212 };
	for (int i = 0; i < utf_size; i++) {
		test_assert_equal(actual[i], expected[i]);
	}
}

TEST_CASE("test from UTF-8 to UTF-8") {
	iconv_t cd;
	size_t utf_size = 4;
	char* utf = "Тест";
	char* in = utf;
	char actual[5];
	char* out = actual;
	cd = iconv_open("UTF-8", "UTF-8");
	test_assert_false(cd == (iconv_t)(-1));
	size_t t = sizeof(actual);
	for (int i = 0; i < sizeof(actual) / sizeof(char); i++) {
		actual[i] = 0;
	}
	iconv(cd, &in, &utf_size, &out, &t);
	for (int i = 0; i < utf_size; i++) {
		test_assert_equal(actual[i], utf[i]);
	}

}
