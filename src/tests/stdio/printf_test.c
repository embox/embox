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
	sprintf(buff, format, __VA_ARGS__);    \
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
	HOPE_EQUAL("-9223372036854775800", &buff1[0], "%lld", -9223372036854775800LL);
	HOPE_EQUAL("18446744073709551610", &buff1[0], "%llu", 18446744073709551610ULL);
}

#define BUFF2_SZ 32

TEST_CASE("Test of specifier with type string") {
	char buff2[BUFF2_SZ], *null;

	null = NULL;

	HOPE_EQUAL("c", &buff2[0], "%c", 'c');
;
	HOPE_EQUAL("hello!", &buff2[0], "%s!", "hello");
#if 0 /* LOL */
	/* THIS IS SEGMENTATION FAULT: GCC Compiler will replace
	 * call of sprintf(<buff>, "%s", <str>) to call of strcpy(<buff>, <str>)..
	 * this is so meanly */
	HOPE_EQUAL("(null)", &buff2[0], "%s", null);
#else
	{
	int (*spf)(char *out, const char *format, ...) = sprintf;
	spf(&buff2[0], "%s", null);
	test_assert_str_equal("(null)", &buff2[0]);
	}
#endif
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
