/**
 * @file
 * @brief Test unit for stdlib/bsearch.
 *
 * @date Nov 19, 2011
 * @author Avdyukhin Dmitry
 */

#include <embox/test.h>
#include <stdlib.h>
#include <stdio.h>

EMBOX_TEST_SUITE("stdlib/qsort test");

/**
 * Compare pointers to integer numbers
 */
static int int_comp(const void *fst, const void *snd) {
	return *((int *)fst) - *((int *)snd);
}

/* Test array of numbers. */
static const int a[] = {0, 1, 2, 2, 4, 5, 10, 10, 20};
/* Count of numbers in array. */
static const int cnt = 9;

/* Check if number wasn't found. */
static void not_found(void *res) {
	test_assert(res == NULL);
}

/**
 * Check if number was found
 *    and result pointer corresponds to expected index ans.
 */
static void found(void * res, size_t ans) {
	test_assert((res - (void *)a) / sizeof(int) == ans);
}

static void * find(int num) {
	return bsearch((int *)&num, a, cnt, sizeof(int), int_comp);
}

TEST_CASE("Test that not-existing too small item won't be found") {
	not_found(find(-10));
}

TEST_CASE("Test that not-existing too large item won't be found") {
	not_found(find(100));
}

TEST_CASE("Test that not-existing medium item won't be found") {
	not_found(find(3));
}

TEST_CASE("Test that existing unique item will be found") {
	found(find(5), 5);
}

TEST_CASE("Test that existing last unique item will be found") {
	found(find(20), 8);
}

TEST_CASE("Test that existing not-unique item will be found. "
			"Result must be the rightmost one") {
	found(find(2), 3);
}
