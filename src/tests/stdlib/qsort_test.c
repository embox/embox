/**
 * @file
 * @brief Test unit for stdlib/qsort.
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

/**
 * Check that elements are sorted in ascending order.
 * Also check that all numbers from 0 to cnt-1 exists.
 * So, we can be sure that elements weren't lost.
 */
static void check(int *a, int cnt) {
	bool was[16];
	for (int i = 0; i < cnt-1; i++) {
		test_assert(a[i] <= a[i+1]);
	}
	for (int i = 0; i < cnt; i++) {
		was[i] = false;
	}
	for (int i = 0; i < cnt; i++) {
		was[a[i]] = true;
	}
	for (int i = 0; i < cnt; i++) {
		test_assert(was[i]);
	}
}

TEST_CASE("Sort array of zero elements") {
	int a[1];
	a[0] = 10;
	qsort(a, 0, sizeof(int), int_comp);
	test_assert(a[0] == 10);
}

TEST_CASE("Sort array of one element") {
	int a[1];
	a[0] = 0;
	qsort(a, 1, sizeof(int), int_comp);
	check(a, 1);
}

TEST_CASE("Sort array of 2 elements") {
	int a[2];
	a[0] = 1;
	a[1] = 0;
	qsort(a, 2, sizeof(int), int_comp);
	check(a, 2);
}

TEST_CASE("Sort array of 3 elements") {
	int a[3];
	a[0] = 2;
	a[1] = 1;
	a[2] = 0;
	qsort(a, 3, sizeof(int), int_comp);
	check(a, 3);
}

TEST_CASE("Sort array of 16 elements") {
	int a[16];
	for (int i = 0; i < 16; i++) {
		a[i] = 15 - i;
	}
	qsort(a, 16, sizeof(int), int_comp);
	check(a, 16);
}

TEST_CASE("Sort array of equal elements") {
	int a[10];
	for (int i = 0; i < 10; i++) {
		a[i] = 2;
	}
	qsort(a, 10, sizeof(int), int_comp);
	for (int i = 0; i < 10; i++) {
		test_assert(a[i] == 2);
	}
}

TEST_CASE("Sort array with equal elements") {
	int a[6];
	for (int i = 0; i < 6; i++) {
		a[i] = (5-i) / 2;
	}
	qsort(a, 6, sizeof(int), int_comp);
	test_assert(a[0] == 0 && a[1] == 0 && a[2] == 1
			&& a[3] == 1 && a[4] == 2 && a[5] == 2);
}
