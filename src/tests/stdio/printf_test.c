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

#define BUFF_SZ 65

#define __val_d(x)   x
#define __val_u(x)   x##U
#define __val_ld(x)  x##L
#define __val_lu(x)  x##UL
#define __val_lld(x) x##LL
#define __val_llu(x) x##ULL
#define __str(x)     #x

#define TEST_VAL_TO_STR(buff, prefix, val)          \
	sprintf(buff, "%"#prefix, __val_##prefix(val)); \
	test_assert_str_equal(buff, __str(val))

EMBOX_TEST_SUITE("stdio/printf test");

TEST_CASE("Test of print long numbers") {
	char buff[BUFF_SZ];

	/* signed int */
	TEST_VAL_TO_STR(&buff[0], d, 32760);
	TEST_VAL_TO_STR(&buff[0], d, -32761);

	/* unsigned int */
	TEST_VAL_TO_STR(&buff[0], u, 65530);

	/* long signed int */
	TEST_VAL_TO_STR(&buff[0], ld, 2147483640);
	TEST_VAL_TO_STR(&buff[0], ld, -2147483641);

	/* long unsigned int */
	TEST_VAL_TO_STR(&buff[0], lu, 4294967290);

	/* long long signed int */
	TEST_VAL_TO_STR(&buff[0], lld, 9223372036854775800);
	TEST_VAL_TO_STR(&buff[0], lld, -9223372036854775801);

	/* long long unsigned int */
	TEST_VAL_TO_STR(&buff[0], llu, 18446744073709551610);
}
