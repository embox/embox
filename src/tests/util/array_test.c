/**
 * @file
 * @brief Tests array utilities.
 *
 * @date 14.03.11
 * @author Nikolay Korotky
 */

#include <embox/test.h>

#include <string.h>
#include <util/array.h>

EMBOX_TEST_SUITE("util/array test");

/* Spread array unaligned head regression. */

/* Pointer size struct. */
struct unaligned {
	void *foo;
};

/* Define a spread array of one pointer size element.
 * Notice the array name. */
ARRAY_SPREAD_DEF(const static struct unaligned, spread_0_unaligned);
ARRAY_SPREAD_ADD(spread_0_unaligned, { });

/* Struct of eight pointers. */
struct aligned {
	void *bar[8];
};

/* Define an array of bigger elements assuming that compiler will decide to
 * align them on bigger boundary.
 * Because of the name this array will be placed in .rodata just after the
 * previous one making a room between its last element and the head of this
 * array. */
ARRAY_SPREAD_DEF(const static struct aligned, spread_1_aligned);
ARRAY_SPREAD_ADD_NAMED(spread_1_aligned, spread_aligned_element, {});

TEST_CASE("Pointers to a spread array and to the first array element should "
		"be equal") {
	volatile unsigned int head = (unsigned int) spread_1_aligned;
	volatile unsigned int element = (unsigned int) spread_aligned_element;

	test_assert_equal(head, element);
}
