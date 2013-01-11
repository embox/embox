/**
 * @file
 * @brief Test for printf
 *
 * @date 28.11.12
 * @author Ilia Vaprol
 */

#include <stdio.h>
#include <string.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("stdio/printf test");

#define HOPE_EQUAL(str, buff, format, ...) \
	sprintf(buff, format, __VA_ARGS__);	   \
	test_assert_str_equal(buff, str)

#define BUFF1_SZ 32

TEST_CASE("Test of specifier with type integer") {
	char buff1[BUFF1_SZ];

	/* test zero */
	HOPE_EQUAL("0", &buff1[0], "%d", 0);

	/* signed/unsigned char */
	HOPE_EQUAL("120", &buff1[0], "%hd", 120);
	HOPE_EQUAL("-121", &buff1[0], "%hd", -121);
	HOPE_EQUAL("250", &buff1[0], "%hu", 250);

	/* signed/unsigned short int */
	HOPE_EQUAL("32760", &buff1[0], "%hhd", 32760);
	HOPE_EQUAL("-32761", &buff1[0], "%hhd", -32761);
	HOPE_EQUAL("65530", &buff1[0], "%hhu", 65530);

	/* signed/unsigned int */
	HOPE_EQUAL("2147483640", &buff1[0], "%d", 2147483640);
	HOPE_EQUAL("-2147483641", &buff1[0], "%d", -2147483641);
	HOPE_EQUAL("4294967290", &buff1[0], "%u", 4294967290U);

	/* signed/unsigned long int */
	HOPE_EQUAL("2147483640", &buff1[0], "%ld", 2147483640L);
	HOPE_EQUAL("-2147483641", &buff1[0], "%ld", -2147483641L);
	HOPE_EQUAL("4294967290", &buff1[0], "%lu", 4294967290UL);

	/* signed/unsigned long long int */
	HOPE_EQUAL("9223372036854775800", &buff1[0], "%lld", 9223372036854775800LL);
	HOPE_EQUAL("-9223372036854775801", &buff1[0], "%lld", -9223372036854775801LL);
	HOPE_EQUAL("18446744073709551610", &buff1[0], "%llu", 18446744073709551610ULL);
}

#define BUFF2_SZ 32

TEST_CASE("Test of specifier with type string") {
	char buff2[BUFF2_SZ], *null;

	null = NULL;

	HOPE_EQUAL("c", &buff2[0], "%c", 'c');
;
	HOPE_EQUAL("hello!", &buff2[0], "%s!", "hello");
	HOPE_EQUAL("(null) - is NULL", &buff2[0], "%s - is NULL", null);
}

#define BUFF3_SZ 32

TEST_CASE("Test of specifier with type pointer") {
	char buff3[BUFF3_SZ], *answer;
	void *ptr;

	ptr = (void *)0x89ab;
	answer = sizeof ptr == 4 ? "0x000089ab" : "0x00000000000089ab"; /* :) */

	HOPE_EQUAL(answer, &buff3[0], "%p", ptr);
}

#define BUFF4_SZ 32

TEST_CASE("Test print with width, precision and alignement") {
	char buff4[BUFF4_SZ];

	HOPE_EQUAL("  0123", &buff4[0], "%6.4i", 123);
	HOPE_EQUAL("0123  ", &buff4[0], "%-6.4d", 123);
	HOPE_EQUAL("0000123", &buff4[0], "%07d", 123);
	HOPE_EQUAL("000123", &buff4[0], "%.6d", 123);
	HOPE_EQUAL(" 1f  01F", &buff4[0], "%*x %4.3X", 3, 0x1f, 0x1f);
	HOPE_EQUAL("0x10 0x01 016", &buff4[0], "%#x %#04x %#o", 0x10, 1, 016);
	HOPE_EQUAL(" abc", &buff4[0], "%4.*s", 3, "abcdef");
}

#define BUFF5_SZ 32

TEST_CASE("Test print with sign specifiers") {
	char buff5[BUFF5_SZ];

	HOPE_EQUAL("+23 -23", &buff5[0], "%+d %+d", 23, -23);
	HOPE_EQUAL(" 23 -23", &buff5[0], "% d % d", 23, -23);
}

#define BUFF6_SZ 32

TEST_CASE("Test print % symbol") {
	char buff6[BUFF6_SZ];

	HOPE_EQUAL("% %", &buff6[0], "%% %c", '%');
}

#define BUFF7_SZ 32

TEST_CASE("Test of %n specifier") {
	char buff7[BUFF7_SZ], *answer;
	int size;

	answer = " 1'm not stupid   ";

	HOPE_EQUAL(answer, &buff7[0], "% 1.1d'%-*cnot %-9.6s%n", 1, 2, 'm', "stupid!!", &size);
	test_assert_equal(strlen(answer), size);
}

#define BUFF8_SZ 32

TEST_CASE("Test of snprintf with zero size") {
	char buff8[BUFF8_SZ], random_char;

	random_char = 17;

	buff8[0] = random_char;
	test_assert_equal(3, snprintf(&buff8[0], 0, "012"));
	test_assert_equal(buff8[0], random_char);
}

#define BUFF9_SZ 32

TEST_CASE("Test of snprintf with small size") {
	char buff9[BUFF9_SZ], *answer;
	int size;

	answer = "01234";

	test_assert_equal(10, snprintf(&buff9[0], 6, "0123456789%n", &size));
	test_assert_equal(size, 10);
	test_assert_str_equal(answer, &buff9[0]);
}

TEST_CASE("Test for float-pointing") {
	printf("%.1f\n", 1.983183456);
}
