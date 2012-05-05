/**
 * @file
 *
 * @brief
 *
 * @date 25.11.2011
 * @author Anton Bondarev
 */
#include <embox/test.h>
#include <stdlib.h>
#include <mem/page.h>

EMBOX_TEST_SUITE("heap allocation test");

struct small_struct {
	int data;
};

struct big_struct {
	int data[0x10];
};

struct unaligned_struct {
	char data;
};

TEST_CASE("Allocates small object") {
	void *obj = malloc(sizeof(struct small_struct));
	test_assert_not_null(obj);
	free(obj);
}

TEST_CASE("Allocates big object") {
	void *obj = malloc(sizeof(struct big_struct));
	test_assert_not_null(obj);
	free(obj);
}

#define MAX_MALLOC_SIZE (CONFIG_HEAP_SIZE /2 - 8) //XXX based on current impl
#define BIG_MALLOC_SIZE (1024 * 1024)

#if BIG_MALLOC_SIZE + 100 <= MAX_MALLOC_SIZE
/* + align */

TEST_CASE("Allocate really big size with align, fill 0's, free, allocate again") {
		void *big = memalign(8,BIG_MALLOC_SIZE);
		test_assert_not_null(big);
		memset(big, 0, BIG_MALLOC_SIZE); // comment this out, and everything is OK
		free(big);

		big = malloc(BIG_MALLOC_SIZE);
		test_assert_not_null(big);
		free(big);
}
#endif


TEST_CASE("Allocates unaligned object") {
	void *obj = malloc(sizeof(struct unaligned_struct));
	test_assert_not_null(obj);
	free(obj);
}

TEST_CASE("Allocates several objects with different size") {
	void *obj;

	obj = malloc(sizeof(struct unaligned_struct));
	test_assert_not_null(obj);
	free(obj);

	obj = malloc(sizeof(struct big_struct));
	test_assert_not_null(obj);
	free(obj);

	obj = malloc(sizeof(struct small_struct));
	test_assert_not_null(obj);
	free(obj);
}

TEST_CASE("Allocates several objects with different size and fill them than free them") {
	struct small_struct *small;
	struct unaligned_struct *unaligned;
	struct big_struct *big;
	int i;

	unaligned = memalign(8, sizeof(struct unaligned_struct));
	test_assert_not_null(unaligned);

	big = memalign(8, sizeof(struct big_struct));
	test_assert_not_null(big);

	small = malloc(sizeof(struct small_struct));
	test_assert_not_null(small);


	unaligned->data = 0x55;
	small->data = 0x12345678;
	for(i = 0; i < ARRAY_SIZE(big->data); i ++) {
		big->data[i] = 0x87654321;
	}

	test_assert_equal(unaligned->data, 0x55);
	test_assert_equal(small->data, 0x12345678);
	for(i = 0; i < ARRAY_SIZE(big->data); i ++) {
		test_assert_equal(big->data[i], 0x87654321);
	}

	free(big);
	free(small);
	free(unaligned);
}

