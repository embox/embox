/**
 * @file
 * @brief
 *
 * @date 04.07.2020
 * @author KOUAM Ledoux
 */

#include <embox/test.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>

#define CONST_TEST 0xFFF

EMBOX_TEST_SUITE("pvalloc tests suite");

TEST_CASE("CALL WITH BAD VALUE") {
    test_assert_equal(pvalloc(0), NULL);
}
TEST_CASE("TEST WITH LARGE SIZE") {
    test_assert_not_equal(pvalloc(0xF00), NULL);
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION") {
    void *mem[CONST_TEST];
    uint32_t i = 0;
    for (;i < CONST_TEST;i++) {
        mem[i] = pvalloc(0xF00);
    }
    for (i = 0;i < CONST_TEST;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0;i < CONST_TEST;i++) {
        free(mem[i]);
    }
}
TEST_CASE("TEST WITH MULTIPLE ALLOCATION WITH LONG SIZE") {
    void *mem[CONST_TEST];
    uint32_t i = 0;
    for (;i < CONST_TEST;i++) {
        mem[i] = pvalloc(i*0xF00);
    }
    for (i = 0;i < CONST_TEST;i++) {
        test_assert_not_equal(mem[i], NULL);
    }
    for (i = 0;i < CONST_TEST;i++) {
        free(mem[i]);
    }
}
