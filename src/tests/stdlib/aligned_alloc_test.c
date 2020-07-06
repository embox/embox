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

const int _const_size_ = 0xFFF ;

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
    int *mem[_const_size_] = {NULL};
    uint32_t i = 0;
    for (i;i < _const_size_;i++) {
        mem[i] = (int*)aligned_alloc(i, i*0xF00);
    }
    for (i = 0;i < _const_size_;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0;i < _const_size_;i++) {
        free(mem[i]);
    }
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION WITH LONG SIZE") {
    int *mem[_const_size_] = {NULL};
    uint32_t i = 0;
    for (i = 0;i < _const_size_;i++) {
        mem[i] = (int*)aligned_alloc(i, i*0xF00);
    }
    for (i = 0;i < _const_size_;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0;i < _const_size_;i++){
        free(mem[i]);
    }
}