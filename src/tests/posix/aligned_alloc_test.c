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

const uint64_t  const_size 0xFFF ;


EMBOX_TEST_SUITE("aligned_alloc test");

TEST_CASE("TEST WITH BAD VALUE") {
    test_assert_equal(aligned_alloc(0, 0), NULL) ;
}
TEST_CASE("TEST WITH BAD VALUES") {
    test_assert_equal(aligned_alloc(2, 75), NULL) ;
}
TEST_CASE("TEST WITH GOOD VALUES") {
    test_assert_not_equal(aligned_alloc(2, 64), NULL) ;
}
TEST_CASE("tEST WITH LARGE SIZE") {
    test_assert_not_equal(aligned_alloc(64, const_size), NULL);
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION") {
    int *mem[const_size] = {NULL} ;
    uint32_t i = 0;
    for (i = 0; i < const_size; i++) {
        mem[i] = (int*)aligned_alloc(i, i*const_size);
    }
    for (i = 0; i < const_size; i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0; i < const_size; i++) {
        free(mem[i]);
    }
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION WITH LONG SIZE") {
    int *mem[const_size] = {NULL} ;
    uint32_t i = 0 ;
    for (i = 0; i<const_size;i++) {
        mem[i] = (int*)aligned_alloc(i, i*const_size);
    }
    for (i = 0; i < const_size; i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0 ; i < const_size; i++) {
        free(mem[i]);
    }
}