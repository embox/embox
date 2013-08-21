/**
 * @file
 * @brief Test for __udivdi3, __divdi3
 *
 * @date 04.12.12
 * @author Ilia Vaprol
 */

#include <stdint.h>
#include <limits.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("gcc/division with remainder tests");

/**
 * Routines for unsigned division with remainder
 */
extern uint32_t __udivsi3(uint32_t num, uint32_t den);
extern uint32_t __umodsi3(uint32_t num, uint32_t den);
extern uint64_t __udivdi3(uint64_t num, uint64_t den);
extern uint64_t __umoddi3(uint64_t num, uint64_t den);

/**
 * Routines for signed division with remainder
 */
extern int32_t __divsi3(int32_t num, int32_t den);
extern int32_t __modsi3(int32_t num, int32_t den);
extern int64_t __divdi3(int64_t num, int64_t den);
extern int64_t __moddi3(int64_t num, int64_t den);

extern int __mulsi3(int a, int b);
/**
 * Some constants
 */
#define def_i32p (+2147483640)
#define def_i32n (-2147483641)
#define def_u32  4294967290U
#define def_i64p (+9223372036854775800LL)
#define def_i64n (-9223372036854775801LL)
#define def_u64  18446744073709551610ULL
#define base     10

TEST_CASE("Test for 32-bit unsigned division") {
	uint32_t var_u32;

	var_u32 = def_u32;

	test_assert_true((def_u32 / base) == (var_u32 / base));
	test_assert_true((def_u32 / base) == __udivsi3(def_u32, base));
	test_assert_true((var_u32 / base) == __udivsi3(var_u32, base));
}

TEST_CASE("Test for 32-bit unsigned modulo") {
	uint32_t var_u32;

	var_u32 = def_u32;

	test_assert_true((def_u32 % base) < base);
	test_assert_true((var_u32 % base) < base);
	test_assert_true((def_u32 % base) == __umodsi3(def_u32, base));
	test_assert_true((var_u32 % base) == __umodsi3(var_u32, base));
}

TEST_CASE("Test for 64-bit unsigned division") {
	uint64_t var_u64;

	var_u64 = def_u64;

	test_assert_true((def_u64 / base) == (var_u64 / base));
	test_assert_true((def_u64 / base) == __udivdi3(def_u64, base));
	test_assert_true((var_u64 / base) == __udivdi3(var_u64, base));
}

TEST_CASE("Test for 64-bit unsigned modulo") {
	uint64_t var_u64;

	var_u64 = def_u64;

	test_assert_true((def_u64 % base) < base);
	test_assert_true((var_u64 % base) < base);
	test_assert_true((def_u64 % base) == __umoddi3(def_u64, base));
	test_assert_true((var_u64 % base) == __umoddi3(var_u64, base));
}

TEST_CASE("Test for 32-bit signed division") {
	int32_t var_i32p, var_i32n;

	var_i32p = def_i32p;
	var_i32n = def_i32n;

	test_assert_true((def_i32p / base) == (var_i32p / base));
	test_assert_true((def_i32p / base) == __divsi3(def_i32p, base));
	test_assert_true((var_i32p / base) == __divsi3(var_i32p, base));

	test_assert_true((def_i32n / base) == (var_i32n / base));
	test_assert_true((def_i32n / base) == __divsi3(def_i32n, base));
	test_assert_true((var_i32n / base) == __divsi3(var_i32n, base));
}

TEST_CASE("Test for 32-bit signed modulo") {
	int32_t var_i32p, var_i32n;

	var_i32p = def_i32p;
	var_i32n = def_i32n;

	test_assert_true((def_i32p % base) < base);
	test_assert_true((var_i32p % base) < base);
	test_assert_true((def_i32p % base) == __modsi3(def_i32p, base));
	test_assert_true((var_i32p % base) == __modsi3(var_i32p, base));

	test_assert_true((def_i32n % base) < base);
	test_assert_true((var_i32n % base) < base);
	test_assert_true((def_i32n % base) == __modsi3(def_i32n, base));
	test_assert_true((var_i32n % base) == __modsi3(var_i32n, base));
}

TEST_CASE("Test for 64-bit signed division") {
	int64_t var_i64p, var_i64n;

	var_i64p = def_i64p;
	var_i64n = def_i64n;

	test_assert_true((def_i64p / base) == (var_i64p / base));
	test_assert_true((def_i64p / base) == __divdi3(def_i64p, base));
	test_assert_true((var_i64p / base) == __divdi3(var_i64p, base));

	test_assert_true((def_i64n / base) == (var_i64n / base));
	test_assert_true((def_i64n / base) == __divdi3(def_i64n, base));
	test_assert_true((var_i64n / base) == __divdi3(var_i64n, base));
}

TEST_CASE("Test for 64-bit signed modulo") {
	int64_t var_i64p, var_i64n;

	var_i64p = def_i64p;
	var_i64n = def_i64n;

	test_assert_true((def_i64p % base) < base);
	test_assert_true((var_i64p % base) < base);
	test_assert_true((def_i64p % base) == __moddi3(def_i64p, base));
	test_assert_true((var_i64p % base) == __moddi3(var_i64p, base));

	test_assert_true((def_i64n % base) < base);
	test_assert_true((var_i64n % base) < base);
	test_assert_true((def_i64n % base) == __moddi3(def_i64n, base));
	test_assert_true((var_i64n % base) == __moddi3(var_i64n, base));
}

TEST_CASE("__modsi should be equal for int multiplication") {

	test_assert(1 		== __mulsi3(-1, 	-1));
	test_assert(-2147483647 == __mulsi3(-1, 	INT_MAX));
	test_assert(-2147483647 == __mulsi3(INT_MAX, 	-1));
	test_assert(1 		== __mulsi3(INT_MAX, 	INT_MAX));
	test_assert(-1 		== __mulsi3(-1, 	1));
	test_assert(-2147483648 == __mulsi3(-1, 	INT_MIN));
	test_assert(2147483647 	== __mulsi3(INT_MAX, 	1));
	test_assert(-2147483648 == __mulsi3(INT_MAX, 	INT_MIN));
	test_assert(-1 		== __mulsi3(1, 		-1));
	test_assert(-2147483648 == __mulsi3(1, 		INT_MIN));
	test_assert(-2147483648 == __mulsi3(INT_MIN, 	1));
	test_assert(-2147483648 == __mulsi3(INT_MIN, 	INT_MAX));
	test_assert(1 		== __mulsi3(1, 		1));
	test_assert(2147483647  == __mulsi3(1, 		INT_MAX));
	test_assert(-2147483648 == __mulsi3(INT_MIN, 	-1));
	test_assert(0 		== __mulsi3(INT_MIN, 	INT_MIN));
}
