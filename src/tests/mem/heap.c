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
	void *obj = malloc(sizeof(struct small_struct));
	test_assert_not_null(obj);
	free(obj);
}

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

	unaligned = malloc(sizeof(struct unaligned_struct));
	test_assert_not_null(unaligned);

	big = malloc(sizeof(struct big_struct));
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
