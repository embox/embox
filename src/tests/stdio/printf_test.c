/**
 * @file
 * @brief Test for printf
 *
 * @date 28.11.12
 * @author Ilia Vaprol
 */

#include <alloca.h>
#include <embox/test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

EMBOX_TEST_SUITE("stdio/printf test");

#define TEST_STR_FMT(answer, format, ...)          \
	do {                                           \
		size_t buff_sz = strlen(answer) + 2;       \
		char *buff = (char *)alloca(buff_sz);      \
		const char *fmt = format;                  \
		snprintf(buff, buff_sz, fmt, __VA_ARGS__); \
		test_assert_str_equal(answer, buff);       \
	} while(0)


TEST_CASE("Test of specifier with type integer") {
	/* test zero */
	TEST_STR_FMT("0", "%d", 0);

	/* signed/unsigned char */
	TEST_STR_FMT("120", "%hhd", 120);
	TEST_STR_FMT("-121","%hhd", -121);
	TEST_STR_FMT("250", "%hhu", 250);

	/* signed/unsigned short int */
	TEST_STR_FMT("32760", "%hd", 32760);
	TEST_STR_FMT("-32761", "%hd", -32761);
	TEST_STR_FMT("65530", "%hu", 65530);

	/* signed/unsigned int */
	TEST_STR_FMT("2147483640", "%d", 2147483640);
	TEST_STR_FMT("-2147483641", "%d", -2147483641);
	TEST_STR_FMT("4294967290", "%u", 4294967290U);

	/* signed/unsigned long int */
	TEST_STR_FMT("2147483640", "%ld", 2147483640L);
	TEST_STR_FMT("-2147483641","%ld", -2147483641L);
	TEST_STR_FMT("4294967290", "%lu", 4294967290UL);

	/* signed/unsigned long long int */
	TEST_STR_FMT("9223372036854775800", "%lld", 9223372036854775800LL);
	TEST_STR_FMT("-9223372036854775801", "%lld", -9223372036854775801LL);
	TEST_STR_FMT("18446744073709551610", "%llu", 18446744073709551610ULL);
}

TEST_CASE("Test of specifier with type string") {
	TEST_STR_FMT("c", "%c", 'c');

	TEST_STR_FMT("hello!", "%s!", "hello");
	TEST_STR_FMT("(null) - is NULL", "%s - is NULL", NULL);

	TEST_STR_FMT("", "%.0s", NULL);
	TEST_STR_FMT("(", "%.1s", NULL);
	TEST_STR_FMT("(n", "%.2s", NULL);
	TEST_STR_FMT("(nu", "%.3s", NULL);
	TEST_STR_FMT("(nul", "%.4s", NULL);
	TEST_STR_FMT("(null", "%.5s", NULL);
	TEST_STR_FMT("(null)", "%.6s", NULL);
	TEST_STR_FMT("(null)", "%.7s", NULL);
}

TEST_CASE("Test of specifier with type pointer") {
	char *answer;
	void *ptr;

	ptr = (void *)0x89ab;
	answer = sizeof ptr == 4 ? "0x000089ab" : "0x00000000000089ab"; /* :) */

	TEST_STR_FMT(answer, "%p", ptr);
}

TEST_CASE("Test print with width, precision and alignement") {
	TEST_STR_FMT("  0123", "%6.4i", 123);
	TEST_STR_FMT("0123  ", "%-6.4d", 123);
	TEST_STR_FMT("0000123", "%07d", 123);
	TEST_STR_FMT("000123", "%.6d", 123);
	TEST_STR_FMT(" 1f  01F", "%*x %4.3X", 3, 0x1f, 0x1f);
	TEST_STR_FMT("0x10 0x01 016", "%#x %#04x %#o", 0x10, 1, 016);
	TEST_STR_FMT(" abc", "%4.*s", 3, "abcdef");
}

TEST_CASE("Test print with sign specifiers") {
	TEST_STR_FMT("+23 -23", "%+d %+d", 23, -23);
	TEST_STR_FMT(" 23 -23", "% .0f % d", 23.0, -23);
}

TEST_CASE("Test print % symbol") {
	TEST_STR_FMT("% %", "%% %c", '%');
}

TEST_CASE("Test of %n specifier") {
	char *answer;
	int size;

	answer = " 1'm not stupid   ";

	TEST_STR_FMT(answer, "% 1.1d'%-*cnot %-9.6s%n", 1, 2, 'm', "stupid!!", &size);
	test_assert_equal(strlen(answer), size);
}

TEST_CASE("Test of snprintf with truncated output") {
	char random_char, backup_char;
	char dest[4];

	backup_char = random_char = 17;

	test_assert_equal(3, snprintf(&random_char, 0, "012"));
	test_assert_equal(random_char, backup_char);

	test_assert_equal(4, snprintf(dest, 4, "1234"));
	test_assert_str_equal(dest, "123");

	test_assert_equal(8, snprintf(dest, 2, "12345678"));
	test_assert_str_equal(dest, "1");
}

TEST_CASE("Test of printing with mistake in format") {
	TEST_STR_FMT("%r+3.1u 10", "%r+3.1u %d", 10);
	TEST_STR_FMT(" 3 %r+.1u", " %d %r+.1u", 3);
	TEST_STR_FMT("%< u\n", "%< u\n","");
	TEST_STR_FMT("%a", "%30.3%a","");
}

#if 0
/* Float is disabled until the reasons (r11423) */
TEST_CASE("Test of specifier with type float") {
	TEST_STR_FMT("2.00", "%.2f", 1.997);
	TEST_STR_FMT("2.0", "%.1f", 1.983183456);
	TEST_STR_FMT("   1.98", "%7.2Lf", 1.983183456L);
	TEST_STR_FMT("001.9832", "%08.4f", 1.983183456);
	TEST_STR_FMT("2.   ", "%#-5.f", 1.983183456);
	TEST_STR_FMT("1.983184", "%f", 1.983183556);
	TEST_STR_FMT("17 ", "%.0f ", 17.4);
	TEST_STR_FMT("17. ", "%#.0f ", 17.4);
	TEST_STR_FMT("17.400000", "%f", 17.4);
	TEST_STR_FMT("0.000000e+00", "%e", 0.0);
	TEST_STR_FMT("0.e+00", "%#.e", 0.0);
	TEST_STR_FMT("3e-01", "%.e", 0.291);
	TEST_STR_FMT("2e-01", "%.e", 0.241);
	TEST_STR_FMT(" 1.000e+01", "% .3e", 9.9999);
	TEST_STR_FMT("1.000000e+100", "%e", 10e+99);
	TEST_STR_FMT("1.234560e+02", "%e", 123.456);
	TEST_STR_FMT("+008.2346E-03", "%+013.4E", 0.00823456);
}
#endif
