/**
 * @file
 * @brief
 *
 * @date 05.07.2020
 * @author KOUAM Ledoux
 */
#include <embox/test.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

const int aa_test = 0xFFF ;

EMBOX_TEST_SUITE("aligned_alloc test");

TEST_CASE("TEST WITH BAD VALUES") {
    test_assert_equal(aligned_alloc(2, 75), NULL);
}
TEST_CASE("TEST WITH BAD VALUES") {
    test_assert_equal(aligned_alloc(0, 0), NULL);
}
TEST_CASE("TEST WITH GOOD VALUES") {
    test_assert_not_equal(aligned_alloc(2, 64), NULL);
}
TEST_CASE("tEST WITH LARGE SIZE") {
    test_assert_not_equal(aligned_alloc(64, 0xF00), NULL);
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION") {
    void *mem[aa_test];
    uint32_t i = 0;
    for (;i < aa_test;i++) {
        mem[i] = aligned_alloc(i, i*0xF00);
    }
    for (i = 0;i < aa_test;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0;i < aa_test;i++) {
        free(mem[i]);
    }
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION WITH LONG SIZE") {
    void *mem[aa_test];
    uint32_t i = 0;
    for (;i < aa_test;i++) {
        mem[i] = aligned_alloc(i, i*0xF00);
    }
    for (i = 0;i < aa_test;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0;i < aa_test;i++) {
        free(mem[i]);
    }
}
